#include "Multiplayer/NamecScalabilitySubsystem.h"
#include "Core/NamecDataTableRows.h"
#include "Save/NamecSettingsService.h"
#include "RHIGlobals.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"

static const TCHAR* GTablePath = TEXT("/Game/Data/DT_MP_SplitScreenScalability.DT_MP_SplitScreenScalability");

const FName UNamecScalabilitySubsystem::HighTierRowName(TEXT("High"));

UDataTable* UNamecScalabilitySubsystem::LoadTierTable()
{
    return LoadObject<UDataTable>(nullptr, GTablePath);
}

TOptional<FNamecScalabilityTierRow> UNamecScalabilitySubsystem::FindTierRow(FName RowName)
{
    return NamecDataTableRows::LoadRow<FNamecScalabilityTierRow>(GTablePath, RowName);
}

void UNamecScalabilitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LogActiveTierCmd.Emplace(TEXT("NamecScalability.LogActiveTier"),
        TEXT("Log the currently active scalability tier"),
        FConsoleCommandDelegate::CreateUObject(this, &UNamecScalabilitySubsystem::LogActiveTier));

    UNamecSettingsService* SettingsService = Collection.InitializeDependency<UNamecSettingsService>();
    check(SettingsService);
    UseSettings(*SettingsService);

    if (UGameInstance* GI = GetGameInstance())
    {
        GI->OnLocalPlayerAddedEvent.AddUObject(this, &UNamecScalabilitySubsystem::OnLocalPlayerCountChanged);
        GI->OnLocalPlayerRemovedEvent.AddUObject(this, &UNamecScalabilitySubsystem::OnLocalPlayerCountChanged);
        ApplyTierForViewportCount(GI->GetNumLocalPlayers());
    }
}

void UNamecScalabilitySubsystem::Deinitialize()
{
    LogActiveTierCmd.Reset();
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }
    if (UGameInstance* GI = GetGameInstance())
    {
        GI->OnLocalPlayerAddedEvent.RemoveAll(this);
        GI->OnLocalPlayerRemovedEvent.RemoveAll(this);
    }
    Super::Deinitialize();
}

void UNamecScalabilitySubsystem::OnLocalPlayerCountChanged(ULocalPlayer* LocalPlayer)
{
    if (UGameInstance* GI = GetGameInstance())
    {
        ApplyTierForViewportCount(GI->GetNumLocalPlayers());
    }
}

void UNamecScalabilitySubsystem::UseSettings(UNamecSettingsService& InSettings)
{
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }
    Settings = &InSettings;
    Settings->OnSettingsApplied.AddUObject(this, &UNamecScalabilitySubsystem::OnSettingsApplied);

    if (!ScalabilityTable)
    {
        ScalabilityTable = LoadTierTable();
    }
}

void UNamecScalabilitySubsystem::ApplyTierForViewportCount(int32 ViewportCount)
{
    if (!ScalabilityTable) return;

    TArray<FNamecScalabilityTierRow*> Rows;
    ScalabilityTable->GetAllRows<FNamecScalabilityTierRow>(TEXT("ApplyTierForViewportCount"), Rows);

    for (const FNamecScalabilityTierRow* Row : Rows)
    {
        if (ViewportCount >= Row->MinViewports && ViewportCount <= Row->MaxViewports)
        {
            ActiveTierName = Row->TierName;
            ActiveTierRow = *Row;
            RecomputeAppliedSettings();
            return;
        }
    }
}

void UNamecScalabilitySubsystem::OnSettingsApplied(const FNamecSettings& AppliedSettings)
{
    RecomputeAppliedSettings();
}

FNamecAppliedGraphics UNamecScalabilitySubsystem::ComputeAppliedSettings(const FNamecMachineSettings& UserSettings, const FNamecScalabilityTierRow& TierRow, bool bSupportsHWRT)
{
    FNamecAppliedGraphics Applied;
    Applied.GIQuality = UserSettings.GIQuality;
    Applied.ShadowQuality = UserSettings.ShadowQuality;
    Applied.FoliageDensityPercent = UserSettings.FoliageDensityPercent;
    Applied.ViewDistancePercent = UserSettings.ViewDistancePercent;

    if (TierRow.TierName != HighTierRowName)
    {
        Applied.GIQuality = FMath::Min(Applied.GIQuality, TierRow.GIQuality);
        Applied.ShadowQuality = FMath::Min(Applied.ShadowQuality, TierRow.ShadowQuality);
        Applied.FoliageDensityPercent = FMath::Min(Applied.FoliageDensityPercent, TierRow.FoliageDensityPercent);
        Applied.ViewDistancePercent = FMath::Min(Applied.ViewDistancePercent, TierRow.ViewDistancePercent);
    }

    // Dynamic-mesh terrain has no distance fields for software ray tracing (Requirement 10).
    if (!bSupportsHWRT)
    {
        Applied.GIQuality = 0;
    }
    return Applied;
}

void UNamecScalabilitySubsystem::RecomputeAppliedSettings()
{
    // The user's saved settings are only read here, never written: that is what restores them
    // when the viewport count drops back to the High tier (Edge Case 9).
    check(Settings);
    ApplyToEngine(ComputeAppliedSettings(Settings->GetSettings().Machine, ActiveTierRow, GRHISupportsRayTracing));
}

void UNamecScalabilitySubsystem::ApplyToEngine(const FNamecAppliedGraphics& Applied)
{
    AppliedGraphics = Applied;

    auto SetCVar = [](const TCHAR* Name, auto Value)
    {
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
            CVar->Set(Value, ECVF_SetByScalability);
    };

    SetCVar(TEXT("sg.GlobalIlluminationQuality"), Applied.GIQuality);
    SetCVar(TEXT("sg.ShadowQuality"), Applied.ShadowQuality);
    SetCVar(TEXT("r.ViewDistanceScale"), Applied.ViewDistancePercent / 100.f);
    // FoliageDensityPercent has no engine variable: cosmetic PCG graphs read it from Phase 4 on.

    SetCVar(TEXT("r.Lumen.HardwareRayTracing.MaxViews"), ActiveTierRow.LumenMaxViews);
    SetCVar(TEXT("sg.FoliageQuality"), ActiveTierRow.FoliageQuality);
    SetCVar(TEXT("sg.PostProcessQuality"), ActiveTierRow.PostProcessQuality);
    SetCVar(TEXT("r.DynamicRes.OperationMode"), ActiveTierRow.DynamicResOperationMode);
}

void UNamecScalabilitySubsystem::GetActiveLODDistances(float& Out0, float& Out1, float& Out2) const
{
    Out0 = ActiveTierRow.LODDistance0Metres;
    Out1 = ActiveTierRow.LODDistance1Metres;
    Out2 = ActiveTierRow.LODDistance2Metres;
}

void UNamecScalabilitySubsystem::LogActiveTier() const
{
    UE_LOG(LogTemp, Log, TEXT("NamecScalability: active tier = %s"), *ActiveTierName.ToString());
}
