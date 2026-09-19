#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "NamecUILayerSubsystem.generated.h"

class APlayerController;
class UNamecAutosaveSubsystem;
class UNamecSaveFileService;
class UNamecUIRootLayout;

// Gives each local player its own UNamecUIRootLayout on its own screen, so every viewport has an
// independent menu stack and HUD layer (multiplayer Requirement 6).
UCLASS()
class NAMEC_API UNamecUILayerSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Creates and wires the layout without putting it on a screen. Public because a test has no
    // viewport. SlotNumber is 1 to 4; only slot 1 shows the autosave warning (Edge Case 4).
    UNamecUIRootLayout* CreateRootLayout(UWorld& World, APlayerController* OwningPlayer, int32 SlotNumber, UNamecSaveFileService& SaveFiles, UNamecAutosaveSubsystem& Autosave);

    void RemoveRootLayout();

    UNamecUIRootLayout* GetRootLayout() const { return RootLayout; }

private:
    void OnPlayerControllerChanged(APlayerController* NewPlayerController);
    void ShowSaveWarning(const FText& Warning);

    UPROPERTY()
    TObjectPtr<UNamecUIRootLayout> RootLayout;

    UPROPERTY()
    TObjectPtr<UNamecAutosaveSubsystem> WarningSource;
};
