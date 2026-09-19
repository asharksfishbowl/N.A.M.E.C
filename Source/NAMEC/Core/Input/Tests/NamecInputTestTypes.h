#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedPlayerInput.h"
#include "NamecInputTestTypes.generated.h"

// Stands in for UEnhancedInputLocalPlayerSubsystem so a test needs no local player or viewport,
// the way Epic's InputTestFramework does. UHT does not allow a UCLASS inside
// WITH_DEV_AUTOMATION_TESTS, so this compiles in every build.
UCLASS()
class UNamecMockEnhancedInputSubsystem : public UObject, public IEnhancedInputSubsystemInterface
{
    GENERATED_BODY()

public:
    TWeakObjectPtr<UEnhancedPlayerInput> PlayerInput;

    virtual UEnhancedPlayerInput* GetPlayerInput() const override { return PlayerInput.Get(); }

protected:
    virtual TMap<TObjectPtr<const UInputAction>, FInjectedInput>& GetContinuouslyInjectedInputs() override { return ContinuouslyInjectedInputs; }

    UPROPERTY(Transient)
    TMap<TObjectPtr<const UInputAction>, FInjectedInput> ContinuouslyInjectedInputs;
};
