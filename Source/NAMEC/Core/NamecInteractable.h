#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NamecInteractable.generated.h"

UINTERFACE()
class NAMEC_API UNamecInteractable : public UInterface
{
    GENERATED_BODY()
};

// What the Interact input acts on: the terrain-updated marker now; doors, Quest Boards, NPCs,
// containers and pickups in later phases (game-foundation Requirement 14). One server RPC,
// ANamecPlayerCharacter::ServerInteract, reaches all of them.
class NAMEC_API INamecInteractable
{
    GENERATED_BODY()

public:
    // Server only.
    virtual void OnInteract(AActor* Interactor) = 0;
};
