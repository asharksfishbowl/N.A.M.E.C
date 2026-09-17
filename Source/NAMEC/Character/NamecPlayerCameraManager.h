#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "NamecPlayerCameraManager.generated.h"

// Minimal camera manager. SpringArm on ANamecPlayerCharacter drives all view positioning.
UCLASS()
class NAMEC_API ANamecPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()
};
