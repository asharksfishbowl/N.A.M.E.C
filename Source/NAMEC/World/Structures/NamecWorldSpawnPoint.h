#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "NamecWorldSpawnPoint.generated.h"

// ATargetPoint is Unreal's zero-overhead location marker with the correct editor glyph.
UCLASS()
class NAMEC_API ANamecWorldSpawnPoint : public ATargetPoint
{
    GENERATED_BODY()
};
