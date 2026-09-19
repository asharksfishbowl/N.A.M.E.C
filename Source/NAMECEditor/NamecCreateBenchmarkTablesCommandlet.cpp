#include "NamecCreateBenchmarkTablesCommandlet.h"
#include "NamecDataTableAuthoring.h"
#include "Multiplayer/NamecScalabilitySubsystem.h"
#include "World/NamecBuildingPieceActor.h"
#include "EnemyAI/NamecEnemyEquipComponent.h"
#include "Engine/DataTable.h"

using NamecDataTableAuthoring::CreateAndSaveDataTable;

int32 UNamecCreateBenchmarkTablesCommandlet::Main(const FString& Params)
{
    bool bOk = CreateScalabilityTable();
    bOk &= CreateCharacterRacesTable();
    bOk &= CreateBuildingPiecesTable();
    bOk &= CreateEnemyAIRulesTable();
    bOk &= CreateBenchmarkWearablesTable();
    if (!bOk)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecCreateBenchmarkTables: one or more tables failed to save."));
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("NamecCreateBenchmarkTables: all tables written successfully."));
    return 0;
}

bool UNamecCreateBenchmarkTablesCommandlet::CreateScalabilityTable()
{
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_MP_SplitScreenScalability"),
        FNamecScalabilityTierRow::StaticStruct(),
        [](UDataTable* Table)
        {
            FNamecScalabilityTierRow High;
            High.TierName              = TEXT("High");
            High.MinViewports          = 1; High.MaxViewports = 2;
            High.LumenMaxViews         = 2; High.GIQuality    = 3;
            High.ShadowQuality         = 3; High.FoliageQuality = 2;
            High.PostProcessQuality    = 3; High.DynamicResOperationMode = 2;
            High.FoliageDensityPercent = 100.f; High.ViewDistancePercent = 100.f;
            Table->AddRow(FName("High"), High);

            FNamecScalabilityTierRow Split;
            Split.TierName             = TEXT("Split");
            Split.MinViewports         = 3; Split.MaxViewports = 4;
            Split.LumenMaxViews        = 4; Split.GIQuality    = 2;
            Split.ShadowQuality        = 2; Split.FoliageQuality = 1;
            Split.PostProcessQuality   = 2; Split.DynamicResOperationMode = 2;
            Split.FoliageDensityPercent = 50.f; Split.ViewDistancePercent = 70.f;
            Table->AddRow(FName("Split"), Split);
        }
    );
}
bool UNamecCreateBenchmarkTablesCommandlet::CreateCharacterRacesTable()
{
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_Character_Races"),
        FNamecCharacterRaceRow::StaticStruct(),
        [](UDataTable* T)
        {
            FNamecCharacterRaceRow Human;
            Human.RaceId      = TEXT("Human");
            Human.DisplayName = FText::FromString(TEXT("Human"));
            Human.BaseCO      = FSoftObjectPath(TEXT("/Game/Benchmark/Char/CO_NamecBody.CO_NamecBody"));
            Human.AnimSetRow  = TEXT("Human");
            T->AddRow(FName("Human"), Human);

            FNamecCharacterRaceRow Elf;
            Elf.RaceId      = TEXT("Elf");
            Elf.DisplayName = FText::FromString(TEXT("Elf"));
            Elf.BaseCO      = FSoftObjectPath(TEXT("/Game/Benchmark/Char/CO_NamecBody.CO_NamecBody"));
            Elf.AnimSetRow  = TEXT("Elf");
            T->AddRow(FName("Elf"), Elf);
        });
}

bool UNamecCreateBenchmarkTablesCommandlet::CreateBuildingPiecesTable()
{
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_Benchmark_BuildingPieces"),
        FNamecBuildingPieceRow::StaticStruct(),
        [](UDataTable* T)
        {
            FNamecBuildingPieceRow Wall;
            Wall.GeometryCollection = FSoftObjectPath(TEXT("/Game/Placeholder/Building/GC_BenchmarkWall.GC_BenchmarkWall"));
            Wall.DestroyedMesh      = FSoftObjectPath(TEXT("/Game/Placeholder/Building/SM_BenchmarkWall_Destroyed.SM_BenchmarkWall_Destroyed"));
            Wall.MaxHealth          = 100.f;
            T->AddRow(FName("WallSegment"), Wall);
        });
}

bool UNamecCreateBenchmarkTablesCommandlet::CreateEnemyAIRulesTable()
{
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_EnemyAI_Rules"),
        FNamecEnemyAIRulesRow::StaticStruct(),
        [](UDataTable* T)
        {
            FNamecEnemyAIRulesRow Row;
            Row.MaxConcurrentMutableUpdates = 2;
            Row.MutableDeferDistanceMetres  = 50.f;
            T->AddRow(FName("Default"), Row);
        });
}

bool UNamecCreateBenchmarkTablesCommandlet::CreateBenchmarkWearablesTable()
{
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_EnemyAI_BenchmarkWearables"),
        FNamecBenchmarkWearableRow::StaticStruct(),
        [](UDataTable* T)
        {
            FNamecBenchmarkWearableRow Male;
            Male.BodyVariant = 0;
            Male.MeshVariant = FSoftObjectPath(TEXT("/Game/Placeholder/Armor/SKM_LeatherArmor_Male.SKM_LeatherArmor_Male"));
            T->AddRow(FName("HumanMale"), Male);

            FNamecBenchmarkWearableRow Female;
            Female.BodyVariant = 1;
            Female.MeshVariant = FSoftObjectPath(TEXT("/Game/Placeholder/Armor/SKM_LeatherArmor_Female.SKM_LeatherArmor_Female"));
            T->AddRow(FName("HumanFemale"), Female);
        });
}
