#pragma once

#include "CoreMinimal.h"
#include "Save/NamecVersionedSave.h"
#include "NamecSaveTestTypes.generated.h"

// A save type at version 3, so the migration chain can be proved before a real type has one.
// UHT does not allow a UCLASS inside WITH_DEV_AUTOMATION_TESTS, so this compiles in every build.
UCLASS()
class NAMEC_API UNamecMigrationTestSave : public UNamecVersionedSave
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString PresentSinceVersion1;

    UPROPERTY()
    int32 AddedInVersion2 = 0;

    UPROPERTY()
    FString AddedInVersion3;

    virtual TConstArrayView<FNamecSaveMigration> GetMigrations() const override;
};
