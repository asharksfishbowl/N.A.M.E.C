#include "Save/NamecVersionedSave.h"
#include "Save/NamecWorldSave.h"
#include "World/Map/NamecMapSettings.h"
#include "World/Map/NamecVoxelMapAsset.h"

DEFINE_LOG_CATEGORY_STATIC(LogNamecSaveMigration, Log, All);

// A migration fills defaults for the fields its version added and does nothing else
// (roadmap ordering rule 3). Append to a list; never reorder or remove an entry.

namespace
{
    // World 1 -> 2: map identity from the shipped map asset; both new collections stay empty
    // (authored-map Requirement 24).
    void MigrateWorldSaveFrom1(UNamecVersionedSave& Save)
    {
        const UNamecVoxelMapAsset* ShippedMapAsset = GetDefault<UNamecMapSettings>()->ShippedMapAsset.LoadSynchronous();
        if (!ShippedMapAsset)
        {
            UE_LOG(LogNamecSaveMigration, Warning, TEXT("No shipped map asset to take a version 1 world's map identity from"));
            return;
        }
        CastChecked<UNamecWorldSave>(&Save)->StampMapIdentity(*ShippedMapAsset);
    }

    const FNamecSaveMigration WorldSaveMigrations[] = { &MigrateWorldSaveFrom1 };
}

TConstArrayView<FNamecSaveMigration> UNamecWorldSave::GetMigrations() const
{
    return WorldSaveMigrations;
}

TConstArrayView<FNamecSaveMigration> UNamecVersionedSave::GetMigrations() const
{
    return TConstArrayView<FNamecSaveMigration>();
}

int32 UNamecVersionedSave::GetCurrentSaveVersion() const
{
    return GetMigrations().Num() + 1;
}

bool UNamecVersionedSave::MigrateToCurrent()
{
    const TConstArrayView<FNamecSaveMigration> Migrations = GetMigrations();
    if (SaveVersion < 1 || SaveVersion > Migrations.Num() + 1)
    {
        return false;
    }

    for (int32 Index = SaveVersion - 1; Index < Migrations.Num(); ++Index)
    {
        Migrations[Index](*this);
    }
    SaveVersion = Migrations.Num() + 1;
    return true;
}
