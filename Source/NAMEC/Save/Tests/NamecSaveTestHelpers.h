#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Save/NamecSaveEnvelope.h"
#include "Save/NamecSaveFileService.h"
#include "Save/NamecSettingsService.h"
#include "Save/NamecVersionedSave.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"

namespace NamecSaveTestHelpers
{
    // A test has no running game, so it gives each subsystem a game instance of its own as outer.
    inline UGameInstance* NewGameInstance()
    {
        return NewObject<UGameInstance>(GEngine);
    }

    inline UNamecSaveFileService* NewSaveFileService()
    {
        return NewObject<UNamecSaveFileService>(NewGameInstance());
    }

    // One start of the game: a fresh settings service reading whatever the save directory holds.
    inline UNamecSettingsService* StartSettingsService()
    {
        UGameInstance* GameInstance = NewGameInstance();
        UNamecSettingsService* Service = NewObject<UNamecSettingsService>(GameInstance);
        Service->LoadSettings(*NewObject<UNamecSaveFileService>(GameInstance));
        return Service;
    }

    inline TArray<uint8> ReadFileBytes(const FString& FilePath)
    {
        TArray<uint8> Bytes;
        FFileHelper::LoadFileToArray(Bytes, *FilePath);
        return Bytes;
    }

    // Writes a file the service itself never would: a well-formed save at a version the test chooses.
    inline void WriteSaveFileAtVersion(UNamecVersionedSave& Save, int32 SaveVersion, const FString& FileName)
    {
        Save.SaveVersion = SaveVersion;
        TArray<uint8> Payload;
        UGameplayStatics::SaveGameToMemory(&Save, Payload);
        const TUniquePtr<FArchive> Writer(IFileManager::Get().CreateFileWriter(*UNamecSaveFileService::GetSaveFilePath(FileName)));
        NamecSaveEnvelope::WriteFramed(*Writer, Payload);
    }
}

#endif
