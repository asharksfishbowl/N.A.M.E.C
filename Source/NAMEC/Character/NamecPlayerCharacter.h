#pragma once

#include "CoreMinimal.h"
#include "Character/NamecCharacterBase.h"
#include "MuCO/CustomizableObjectInstanceComponent.h"
#include "NamecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputAction;

UCLASS()
class NAMEC_API ANamecPlayerCharacter : public ANamecCharacterBase
{
    GENERATED_BODY()

public:
    ANamecPlayerCharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputAction> CameraToggleAction;

private:
    // Third-person spring arm + camera.
    UPROPERTY(VisibleAnywhere) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent>    ThirdPersonCamera;

    // First-person camera attached to head socket; arm mesh renders only for local player.
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent>       FirstPersonCamera;
    UPROPERTY(VisibleAnywhere) TObjectPtr<USkeletalMeshComponent> ArmMesh;

    // Mutable body customization instance. Blueprint assigns CO_NamecBody via CustomizableObject.
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCustomizableObjectInstanceComponent> MutableBody;

    bool bFirstPerson = false;
    void ToggleCamera();
};
