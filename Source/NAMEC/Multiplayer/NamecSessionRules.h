#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NamecSessionRules.generated.h"

// DT_MP_Session: every multiplayer tuning value that names no other table (Requirement 23).
USTRUCT(BlueprintType)
struct NAMEC_API FNamecSessionRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxPlayers = 4;

    // Stored for multiplayer Requirement 22, which uses it in Phase 3.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ConnectionTimeoutSeconds = 20.f;
};

namespace NamecSessionRules
{
    NAMEC_API extern const TCHAR* const TablePackageName;
    NAMEC_API extern const FName RowName;

    // The spec's hard cap, and the one session constant allowed in code (multiplayer Requirement 2).
    inline constexpr int32 HardMaxPlayers = 4;

    // Unset when the table or its row is missing.
    NAMEC_API TOptional<FNamecSessionRow> LoadRow();

    NAMEC_API int32 GetMaxPlayers(const FNamecSessionRow& Row);

    NAMEC_API int32 GetMaxLocalPlayers(const FNamecSessionRow& Row, int32 PlayersInSession);
}
