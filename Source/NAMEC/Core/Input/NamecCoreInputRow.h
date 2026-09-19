#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NamecCoreInputRow.generated.h"

// DT_Core_Input. Every threshold here reaches a player's mappings at runtime, never the IMC
// asset, so a table edit needs no rebuild and no re-run of NamecCreateInputAssets.
USTRUCT(BlueprintType)
struct NAMEC_API FNamecCoreInputRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float CameraToggleHoldSeconds = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DeconstructHoldSeconds = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float RefillHoldSeconds = 1.f;

    // Released sooner than this is a dodge roll; held longer is a sprint.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DodgeTapSeconds = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float StickFlickThreshold = 0.5f;
};

namespace NamecCoreInput
{
    NAMEC_API extern const TCHAR* const TablePackageName;
    NAMEC_API extern const FName RowName;

    // Unset when the table or its row is missing.
    NAMEC_API TOptional<FNamecCoreInputRow> LoadRow();
}
