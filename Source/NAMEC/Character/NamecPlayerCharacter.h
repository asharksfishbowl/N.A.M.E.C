#pragma once

#include "CoreMinimal.h"
#include "Character/NamecCharacterBase.h"
#include "MuCO/CustomizableSkeletalComponent.h"
#include "World/NamecVoxelTypes.h"
#include "NamecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;

UCLASS()
class NAMEC_API ANamecPlayerCharacter : public ANamecCharacterBase
{
    GENERATED_BODY()

public:
    ANamecPlayerCharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void ToggleCamera();
    bool IsFirstPerson() const { return bFirstPerson; }

    // Called from input on the owning client; forwards to server for authority.
    UFUNCTION(Server, Reliable)
    void ServerApplyTerrainEdit(FVector CentreMetres, float RadiusMetres,
                                ENamecEditMode Mode, FName MaterialRow, int32 ToolTier);

    // Server-to-all replication of a single chunk's voxel changes.
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_TerrainDelta(FIntPoint ChunkCoord,
                                const TArray<FNamecVoxelDelta>& Deltas);

private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent>    ThirdPersonCamera;

    UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent>       FirstPersonCamera;
    UPROPERTY(VisibleAnywhere) TObjectPtr<USkeletalMeshComponent> ArmMesh;

    UPROPERTY(VisibleAnywhere) TObjectPtr<UCustomizableSkeletalComponent> MutableBody;

    bool bFirstPerson = false;
};
