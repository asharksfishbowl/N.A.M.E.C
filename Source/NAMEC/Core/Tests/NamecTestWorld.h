#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Engine/Engine.h"
#include "Engine/World.h"

// A throwaway game world for a test that needs actors or a player controller.
class FNamecScopedTestWorld
{
public:
    FNamecScopedTestWorld()
        : World(UWorld::CreateWorld(EWorldType::Game, false, TEXT("NamecTestWorld")))
    {
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    }

    ~FNamecScopedTestWorld()
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    }

    UWorld* operator->() const { return World; }
    UWorld* Get() const { return World; }

private:
    UWorld* World;
};

#endif
