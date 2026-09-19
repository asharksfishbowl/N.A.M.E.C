#include "Save/NamecSaveFileService.h"
#include "Save/NamecSaveEnvelope.h"
#include "Save/NamecVersionedSave.h"
#include "Core/Platform/INamecPlatform.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogNamecSave, Log, All);

namespace
{
    // A save name is a bare file name: a path separator would let a caller leave the save directory.
    // That is a caller's bug, never a disk condition, so it is logged as an error.
    bool ValidateSaveFileName(const FString& FileName)
    {
        const bool bBare = !FileName.IsEmpty() && FPaths::GetCleanFilename(FileName) == FileName;
        if (!bBare)
        {
            UE_LOG(LogNamecSave, Error, TEXT("'%s' is not a bare save file name"), *FileName);
        }
        return bBare;
    }

    bool WriteFramedFile(const FString& FilePath, const TArray<uint8>& Payload)
    {
        const TUniquePtr<FArchive> Writer(IFileManager::Get().CreateFileWriter(*FilePath));
        if (!Writer)
        {
            return false;
        }
        NamecSaveEnvelope::WriteFramed(*Writer, Payload);
        return Writer->Close() && !Writer->IsError();
    }
}

const TCHAR* LexToString(ENamecSaveWriteResult Result)
{
    switch (Result)
    {
    case ENamecSaveWriteResult::Ok: return TEXT("Ok");
    case ENamecSaveWriteResult::DiskFull: return TEXT("DiskFull");
    case ENamecSaveWriteResult::WriteFailed: return TEXT("WriteFailed");
    }
    return TEXT("Unknown");
}

FString UNamecSaveFileService::GetSaveFilePath(const FString& FileName)
{
    return FNamecPlatform::Get().GetSaveDirectory() / FileName;
}

FString UNamecSaveFileService::GetTempFilePath(const FString& FileName)
{
    return GetSaveFilePath(FileName) + TEXT(".tmp");
}

TArray<FString> UNamecSaveFileService::FindSaveFiles(const FString& Prefix) const
{
    TArray<FString> FileNames;
    IFileManager::Get().FindFiles(FileNames, *(FNamecPlatform::Get().GetSaveDirectory() / (Prefix + TEXT("*.sav"))), true, false);
    FileNames.Sort();
    return FileNames;
}

FString UNamecSaveFileService::GetBakFilePath(const FString& FileName)
{
    return GetSaveFilePath(FileName) + TEXT(".bak");
}

bool UNamecSaveFileService::MoveToBak(const FString& FileName) const
{
    return ValidateSaveFileName(FileName)
        && IFileManager::Get().Move(*GetBakFilePath(FileName), *GetSaveFilePath(FileName), true, true);
}

ENamecSaveWriteResult UNamecSaveFileService::Write(UNamecVersionedSave& Save, const FString& FileName) const
{
    if (!ValidateSaveFileName(FileName))
    {
        return ENamecSaveWriteResult::WriteFailed;
    }

    Save.OnBeforeWrite();
    Save.SaveVersion = Save.GetCurrentSaveVersion();

    TArray<uint8> Payload;
    if (!UGameplayStatics::SaveGameToMemory(&Save, Payload))
    {
        return ENamecSaveWriteResult::WriteFailed;
    }

    INamecPlatform& Platform = FNamecPlatform::Get();
    const FString SaveDirectory = Platform.GetSaveDirectory();
    IFileManager& FileManager = IFileManager::Get();
    if (!FileManager.MakeDirectory(*SaveDirectory, true))
    {
        return ENamecSaveWriteResult::WriteFailed;
    }

    const TOptional<uint64> FreeBytes = Platform.GetFreeDiskSpaceBytes(SaveDirectory);
    if (FreeBytes.IsSet() && FreeBytes.GetValue() < static_cast<uint64>(NamecSaveEnvelope::GetFramedSize(Payload.Num())))
    {
        return ENamecSaveWriteResult::DiskFull;
    }

    const FString TempPath = GetTempFilePath(FileName);
    const bool bWritten = WriteFramedFile(TempPath, Payload)
        && FileManager.Move(*GetSaveFilePath(FileName), *TempPath, true, true);
    if (!bWritten)
    {
        FileManager.Delete(*TempPath, false, true, true);
        return ENamecSaveWriteResult::WriteFailed;
    }
    return ENamecSaveWriteResult::Ok;
}

FNamecSaveLoadOutcome UNamecSaveFileService::Load(const FString& FileName, TSubclassOf<UNamecVersionedSave> SaveClass) const
{
    check(SaveClass);

    FNamecSaveLoadOutcome Outcome;
    if (!ValidateSaveFileName(FileName) || !IFileManager::Get().FileExists(*GetSaveFilePath(FileName)))
    {
        Outcome.Result = ENamecSaveLoadResult::Missing;
        return Outcome;
    }

    Outcome.Result = ENamecSaveLoadResult::Corrupt;

    TArray<uint8> Bytes;
    if (!FFileHelper::LoadFileToArray(Bytes, *GetSaveFilePath(FileName)) || !NamecSaveEnvelope::StripFrame(Bytes))
    {
        return Outcome;
    }

    UNamecVersionedSave* Loaded = Cast<UNamecVersionedSave>(UGameplayStatics::LoadGameFromMemory(Bytes));
    if (!Loaded || !Loaded->IsA(SaveClass))
    {
        return Outcome;
    }

    if (Loaded->SaveVersion > Loaded->GetCurrentSaveVersion())
    {
        Outcome.Result = ENamecSaveLoadResult::NewerVersion;
        return Outcome;
    }

    if (!Loaded->MigrateToCurrent())
    {
        return Outcome;
    }

    Outcome.Result = ENamecSaveLoadResult::Ok;
    Outcome.Save = Loaded;
    return Outcome;
}
