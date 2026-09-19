#include "Character/NamecPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "Core/Input/NamecBaseInputBindings.h"
#include "InputAction.h"
#include "World/NamecVoxelWorld.h"

ANamecPlayerCharacter::ANamecPlayerCharacter()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.f;
    CameraBoom->bUsePawnControlRotation = true;

    ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    ThirdPersonCamera->bUsePawnControlRotation = false;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh());
    FirstPersonCamera->bUsePawnControlRotation = true;
    FirstPersonCamera->SetActive(false);

    ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
    ArmMesh->SetupAttachment(FirstPersonCamera);
    ArmMesh->SetOnlyOwnerSee(true);
    ArmMesh->SetVisibility(false);

    MutableBody = CreateDefaultSubobject<UCustomizableSkeletalComponent>(TEXT("MutableBody"));
}

void ANamecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // The action is the base contexts' own asset, so it is not a per-Blueprint property.
        if (const UInputAction* CameraToggleAction = NamecBaseInput::LoadAction(NamecBaseInput::CameraToggleActionName))
        {
            EIC->BindAction(CameraToggleAction, ETriggerEvent::Triggered, this, &ANamecPlayerCharacter::ToggleCamera);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_CameraToggle is missing. Run -run=NamecCreateInputAssets"));
        }
    }
}

void ANamecPlayerCharacter::ToggleCamera()
{
    bFirstPerson = !bFirstPerson;
    ThirdPersonCamera->SetActive(!bFirstPerson);
    FirstPersonCamera->SetActive(bFirstPerson);
    ArmMesh->SetVisibility(bFirstPerson);
    GetMesh()->SetOwnerNoSee(bFirstPerson);
}

void ANamecPlayerCharacter::ServerApplyTerrainEdit_Implementation(
    FVector CentreMetres, float RadiusMetres,
    ENamecEditMode Mode, FName MaterialRow, int32 ToolTier)
{
    UWorld* W = GetWorld();
    UNamecVoxelWorld* VoxelWorld = W ? W->GetSubsystem<UNamecVoxelWorld>() : nullptr;
    if (!VoxelWorld) return;

    TArray<FIntPoint>               ChunkCoords;
    TArray<TArray<FNamecVoxelDelta>> PerChunkDeltas;
    VoxelWorld->ApplyTerrainEdit(CentreMetres, RadiusMetres, Mode, MaterialRow, ToolTier,
                                  ChunkCoords, PerChunkDeltas);

    for (int32 i = 0; i < ChunkCoords.Num(); ++i)
        Multicast_TerrainDelta(ChunkCoords[i], PerChunkDeltas[i]);
}

void ANamecPlayerCharacter::Multicast_TerrainDelta_Implementation(
    FIntPoint ChunkCoord, const TArray<FNamecVoxelDelta>& Deltas)
{
    UWorld* W = GetWorld();
    UNamecVoxelWorld* VoxelWorld = W ? W->GetSubsystem<UNamecVoxelWorld>() : nullptr;
    if (VoxelWorld)
        VoxelWorld->ApplyDelta(ChunkCoord, Deltas);
}
