#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NamecAutosaveSubsystem.generated.h"

class UNamecSaveFileService;
class UNamecVersionedSave;

USTRUCT(BlueprintType)
struct NAMEC_API FNamecCoreSaveRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float AutosaveIntervalSeconds = 300.f;
};

USTRUCT()
struct FNamecRegisteredSave
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UNamecVersionedSave> Save;

    UPROPERTY()
    FString FileName;
};

enum class ENamecSaveReason : uint8
{
    Autosave,
    HostExit,
    Disconnect,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FNamecOnSaveWarning, const FText&);

UCLASS()
class NAMEC_API UNamecAutosaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static const TCHAR* const CoreSaveTablePackageName;
    static const FName CoreSaveRowName;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Both public because a test has no subsystem collection to run Initialize for it. They are
    // separate so a missing table turns off the timer and never the host-exit save.
    void UseSaveFiles(UNamecSaveFileService& InSaveFiles);
    void StartAutosaveTimer(const UDataTable& CoreSaveTable);

    // A world or character registers while it is loaded, and is kept alive while registered.
    // Registering a file name again replaces its entry, so one file never has two writers.
    void RegisterSave(UNamecVersionedSave& Save, const FString& FileName);
    void UnregisterSave(const UNamecVersionedSave& Save);

    void RunAutosave();

    void SaveAllNow(ENamecSaveReason Reason);

    float GetAutosaveTimerRateSeconds() const;

    // Edge Case 4: fired on DiskFull. The save stays in memory and the next timer tick is the retry.
    FNamecOnSaveWarning OnSaveWarning;

private:
    UPROPERTY()
    TObjectPtr<UNamecSaveFileService> SaveFiles;

    UPROPERTY()
    TArray<FNamecRegisteredSave> RegisteredSaves;

    FTimerHandle AutosaveTimer;
};
