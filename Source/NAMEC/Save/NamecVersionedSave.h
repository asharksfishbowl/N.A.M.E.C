#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NamecVersionedSave.generated.h"

class UNamecVersionedSave;

using FNamecSaveMigration = void (*)(UNamecVersionedSave& Save);

UCLASS(Abstract)
class NAMEC_API UNamecVersionedSave : public USaveGame
{
    GENERATED_BODY()

public:
    // Must stay 1. A property equal to the class default is not serialised, so a version 1 file
    // carries no SaveVersion at all and reads back as whatever this default is.
    UPROPERTY()
    int32 SaveVersion = 1;

    // Entry N upgrades a save from version N + 1, so the list's length fixes the current version.
    // A type with migrations overrides this in NamecSaveMigrations.cpp.
    virtual TConstArrayView<FNamecSaveMigration> GetMigrations() const;

    int32 GetCurrentSaveVersion() const;

    // Runs at the start of every UNamecSaveFileService::Write, for fields that must be current on disk.
    virtual void OnBeforeWrite() {}

    // Returns false, changing nothing, when SaveVersion is outside 1..current.
    bool MigrateToCurrent();
};
