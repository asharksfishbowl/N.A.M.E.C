#include "Character/NamecPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"

ANamecPlayerCharacter::ANamecPlayerCharacter()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.f;
    CameraBoom->bUsePawnControlRotation = true;

    ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    ThirdPersonCamera->bUsePawnControlRotation = false;

    // First-person camera. Blueprint subclass attaches to head socket via SetupAttachment override.
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
        if (CameraToggleAction)
            EIC->BindAction(CameraToggleAction, ETriggerEvent::Triggered, this,
                &ANamecPlayerCharacter::ToggleCamera);
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
