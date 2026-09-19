#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "NamecUILayerSubsystem.generated.h"

class APlayerController;
class UNamecAutosaveSubsystem;
class UNamecSaveFileService;
class UNamecInputContextSubsystem;
class UNamecSettingsService;
class UNamecUIRootLayout;
struct FNamecWorldLoadDecision;

// What a root layout's screens work through. All four are required.
struct FNamecUILayerServices
{
    UNamecSaveFileService* SaveFiles = nullptr;
    UNamecAutosaveSubsystem* Autosave = nullptr;
    UNamecSettingsService* Settings = nullptr;
    UNamecInputContextSubsystem* InputContexts = nullptr;
};

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
    UNamecUIRootLayout* CreateRootLayout(UWorld& World, APlayerController* OwningPlayer, int32 InSlotNumber, const FNamecUILayerServices& Services);

    void RemoveRootLayout();

    // authored-map Requirement 25.2: "World updated to map revision N" shows on the host's local
    // player 1 only. Any other slot, and any other outcome, shows nothing. Call once per load.
    void ShowWorldLoadNotice(const FNamecWorldLoadDecision& Decision);

    UNamecUIRootLayout* GetRootLayout() const { return RootLayout; }

private:
    void OnPlayerControllerChanged(APlayerController* NewPlayerController);
    void ShowSaveWarning(const FText& Warning);

    UPROPERTY()
    TObjectPtr<UNamecUIRootLayout> RootLayout;

    UPROPERTY()
    TObjectPtr<UNamecAutosaveSubsystem> WarningSource;

    int32 SlotNumber = 0;
};
