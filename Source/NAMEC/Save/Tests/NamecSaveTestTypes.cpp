#include "Save/Tests/NamecSaveTestTypes.h"

namespace
{
    void MigrateTestSaveFrom1(UNamecVersionedSave& Save)
    {
        CastChecked<UNamecMigrationTestSave>(&Save)->AddedInVersion2 = 7;
    }

    void MigrateTestSaveFrom2(UNamecVersionedSave& Save)
    {
        CastChecked<UNamecMigrationTestSave>(&Save)->AddedInVersion3 = TEXT("FilledByMigration");
    }

    const FNamecSaveMigration TestSaveMigrations[] = { &MigrateTestSaveFrom1, &MigrateTestSaveFrom2 };
}

TConstArrayView<FNamecSaveMigration> UNamecMigrationTestSave::GetMigrations() const
{
    return TestSaveMigrations;
}
