#include "Multiplayer/NamecScalabilitySubsystem.h"
#include "Core/NamecDataTableRows.h"
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

    ScalabilityTable = LoadTierTable();

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

void UNamecScalabilitySubsystem::ApplyTierForViewportCount(int32 ViewportCount)
{
    if (!ScalabilityTable) return;

    TArray<FNamecScalabilityTierRow*> Rows;
    ScalabilityTable->GetAllRows<FNamecScalabilityTierRow>(TEXT("ApplyTierForViewportCount"), Rows);

    const FNamecScalabilityTierRow* MatchedRow = nullptr;
    for (const FNamecScalabilityTierRow* Row : Rows)
    {
        if (ViewportCount >= Row->MinViewports && ViewportCount <= Row->MaxViewports)
        {
            MatchedRow = Row;
            ActiveTierName = Row->TierName;
            break;
        }
    }

    if (!MatchedRow) return;
    ActiveTierRow = *MatchedRow;

    auto SetCVar = [](const TCHAR* Name, int32 Value)
    {
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
            CVar->Set(Value, ECVF_SetByScalability);
    };

    SetCVar(TEXT("r.Lumen.HardwareRayTracing.MaxViews"), MatchedRow->LumenMaxViews);
    SetCVar(TEXT("sg.GlobalIlluminationQuality"), MatchedRow->GIQuality);
    SetCVar(TEXT("sg.ShadowQuality"), MatchedRow->ShadowQuality);
    SetCVar(TEXT("sg.FoliageQuality"), MatchedRow->FoliageQuality);
    SetCVar(TEXT("sg.PostProcessQuality"), MatchedRow->PostProcessQuality);
    SetCVar(TEXT("r.DynamicRes.OperationMode"), MatchedRow->DynamicResOperationMode);
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
