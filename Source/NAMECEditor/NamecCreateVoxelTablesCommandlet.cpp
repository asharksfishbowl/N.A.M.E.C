#include "NamecCreateVoxelTablesCommandlet.h"
#include "NamecDataTableAuthoring.h"
#include "World/NamecWorldDataTables.h"
#include "Engine/DataTable.h"

using NamecDataTableAuthoring::CreateAndSaveDataTable;

int32 UNamecCreateVoxelTablesCommandlet::Main(const FString& Params)
{
    bool bOk = CreateVoxelMaterialsTable()
             & CreateClimatesTable()
             & CreateOreVeinsTable();
    if (!bOk)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecCreateVoxelTables: one or more tables failed to save."));
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("NamecCreateVoxelTables: all tables written successfully."));
    return 0;
}

bool UNamecCreateVoxelTablesCommandlet::CreateVoxelMaterialsTable()
{
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_World_VoxelMaterials"),
        FNamecVoxelMaterialRow::StaticStruct(),
        [](UDataTable* Table)
        {
            auto AddMat = [&](FName RowName, FName MatName, bool bOre, bool bSolid)
            {
                FNamecVoxelMaterialRow Row;
                Row.MaterialName = MatName;
                Row.bIsOre       = bOre;
                Row.bIsSolid     = bSolid;
                Row.DisplayName  = FText::FromName(MatName);
                Table->AddRow(RowName, Row);
            };
            AddMat(FName("Grass"), FName("Grass"), false, true);
            AddMat(FName("Stone"), FName("Stone"), false, true);
            AddMat(FName("Dirt"),  FName("Dirt"),  false, true);
        });
}

bool UNamecCreateVoxelTablesCommandlet::CreateClimatesTable()
{
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_World_Climates"),
        FNamecClimateRow::StaticStruct(),
        [](UDataTable* Table)
        {
            FNamecClimateRow Row;
            Row.RegionName              = FName("Temperate Forest & Meadows");
            Row.CaveDensityThreshold    = 0.08f;
            Row.CaveDepthBandMinMetres  = 0.f;
            Row.CaveDepthBandMaxMetres  = 20.f;
            Row.CaveTunnelRadiusMetres  = 2.f;
            Table->AddRow(FName("Temperate"), Row);
        });
}

bool UNamecCreateVoxelTablesCommandlet::CreateOreVeinsTable()
{
    // Phase 1: empty table — no ore in the benchmark.
    return CreateAndSaveDataTable(
        TEXT("/Game/Data/DT_World_OreVeins"),
        FNamecOreVeinRow::StaticStruct(),
        [](UDataTable*) {});
}
