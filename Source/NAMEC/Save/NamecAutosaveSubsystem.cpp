#include "Save/NamecAutosaveSubsystem.h"
#include "Save/NamecSaveFileService.h"
#include "Save/NamecVersionedSave.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "NamecAutosave"

DEFINE_LOG_CATEGORY_STATIC(LogNamecAutosave, Log, All);

const TCHAR* const UNamecAutosaveSubsystem::CoreSaveTablePackageName = TEXT("/Game/Data/DT_Core_Save");
const FName UNamecAutosaveSubsystem::CoreSaveRowName(TEXT("Default"));

namespace
{
    const TCHAR* LexToString(ENamecSaveReason Reason)
    {
        switch (Reason)
        {
        case ENamecSaveReason::Autosave: return TEXT("Autosave");
        case ENamecSaveReason::HostExit: return TEXT("HostExit");
        case ENamecSaveReason::Disconnect: return TEXT("Disconnect");
        }
        return TEXT("Unknown");
    }
}

void UNamecAutosaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UNamecSaveFileService* SaveFileService = Collection.InitializeDependency<UNamecSaveFileService>();
    check(SaveFileService);
    UseSaveFiles(*SaveFileService);

    if (const UDataTable* CoreSaveTable = LoadObject<UDataTable>(nullptr, CoreSaveTablePackageName))
    {
        StartAutosaveTimer(*CoreSaveTable);
    }
    else
    {
        UE_LOG(LogNamecAutosave, Error, TEXT("%s is missing; the autosave timer is off. Run -run=NamecCreateFoundationTables"), CoreSaveTablePackageName);
    }
}

void UNamecAutosaveSubsystem::Deinitialize()
{
    SaveAllNow(ENamecSaveReason::HostExit);
    GetGameInstance()->GetTimerManager().ClearTimer(AutosaveTimer);
    Super::Deinitialize();
}

void UNamecAutosaveSubsystem::UseSaveFiles(UNamecSaveFileService& InSaveFiles)
{
    SaveFiles = &InSaveFiles;
}

void UNamecAutosaveSubsystem::StartAutosaveTimer(const UDataTable& CoreSaveTable)
{
    const FNamecCoreSaveRow* Row = CoreSaveTable.FindRow<FNamecCoreSaveRow>(CoreSaveRowName, TEXT("StartAutosaveTimer"));
    if (!Row || Row->AutosaveIntervalSeconds <= 0.f)
    {
        UE_LOG(LogNamecAutosave, Error, TEXT("DT_Core_Save has no usable '%s' row; the autosave timer is off"), *CoreSaveRowName.ToString());
        return;
    }

    GetGameInstance()->GetTimerManager().SetTimer(AutosaveTimer, this, &UNamecAutosaveSubsystem::RunAutosave, Row->AutosaveIntervalSeconds, true);
}

void UNamecAutosaveSubsystem::RegisterSave(UNamecVersionedSave& Save, const FString& FileName)
{
    RegisteredSaves.RemoveAll([&Save, &FileName](const FNamecRegisteredSave& Registered)
    {
        return Registered.Save == &Save || Registered.FileName == FileName;
    });
    RegisteredSaves.Add({ &Save, FileName });
}

void UNamecAutosaveSubsystem::UnregisterSave(const UNamecVersionedSave& Save)
{
    RegisteredSaves.RemoveAll([&Save](const FNamecRegisteredSave& Registered) { return Registered.Save == &Save; });
}

void UNamecAutosaveSubsystem::RunAutosave()
{
    SaveAllNow(ENamecSaveReason::Autosave);
}

void UNamecAutosaveSubsystem::SaveAllNow(ENamecSaveReason Reason)
{
    check(SaveFiles);
    UE_LOG(LogNamecAutosave, Log, TEXT("Saving %d registered save(s): %s"), RegisteredSaves.Num(), LexToString(Reason));

    bool bDiskFull = false;
    for (const FNamecRegisteredSave& Registered : RegisteredSaves)
    {
        const ENamecSaveWriteResult Result = SaveFiles->Write(*Registered.Save, Registered.FileName);
        bDiskFull |= Result == ENamecSaveWriteResult::DiskFull;
        if (Result != ENamecSaveWriteResult::Ok)
        {
            UE_LOG(LogNamecAutosave, Warning, TEXT("'%s' was not written (%s)"), *Registered.FileName, LexToString(Result));
        }
    }

    // The text promises no retry: only an Autosave has a next tick, and HostExit has none.
    if (bDiskFull)
    {
        OnSaveWarning.Broadcast(LOCTEXT("DiskFull", "Not enough disk space to save."));
    }
}

float UNamecAutosaveSubsystem::GetAutosaveTimerRateSeconds() const
{
    return GetGameInstance()->GetTimerManager().GetTimerRate(AutosaveTimer);
}

#undef LOCTEXT_NAMESPACE
