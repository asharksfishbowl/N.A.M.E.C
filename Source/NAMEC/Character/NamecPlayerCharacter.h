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

    // The one interact RPC. The server ignores a target that is not an INamecInteractable. No reach
    // rule yet: no spec gives a distance.
    UFUNCTION(Server, Reliable)
    void ServerInteract(AActor* Target);

    // What IA_Interact acts on. The trace and targeting rules that choose it belong to the phases
    // that add doors and pickups.
    void SetInteractTarget(AActor* Target) { InteractTarget = Target; }

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

    void InteractWithTarget();
    TWeakObjectPtr<AActor> InteractTarget;
};
