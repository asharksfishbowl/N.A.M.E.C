#include "Save/NamecVersionedSave.h"

// A migration fills defaults for the fields its version added and does nothing else
// (roadmap ordering rule 3). Append to a list; never reorder or remove an entry.

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
