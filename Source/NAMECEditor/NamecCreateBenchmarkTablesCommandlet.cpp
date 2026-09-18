#include "NamecCreateBenchmarkTablesCommandlet.h"
#include "Settings/NamecScalabilitySubsystem.h"
#include "Engine/DataTable.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"

int32 UNamecCreateBenchmarkTablesCommandlet::Main(const FString& Params)
{
    bool bOk = CreateScalabilityTable();
    if (!bOk)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecCreateBenchmarkTables: one or more tables failed to save."));
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("NamecCreateBenchmarkTables: all tables written successfully."));
    return 0;
}

bool UNamecCreateBenchmarkTablesCommandlet::CreateAndSaveDataTable(
    const FString& PackageName, UScriptStruct* RowStruct,
    TFunctionRef<void(UDataTable*)> PopulateRows)
{
    UPackage* Package = CreatePackage(*PackageName);
    Package->FullyLoad();
    FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);
    UDataTable* Table = NewObject<UDataTable>(Package, *AssetName, RF_Public | RF_Standalone);
    Table->RowStruct = RowStruct;
    PopulateRows(Table);
    Package->MarkPackageDirty();

    FString FilePath = FPackageName::LongPackageNameToFilename(PackageName,
        FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    bool bSaved = UPackage::SavePackage(Package, Table, *FilePath, SaveArgs);
    UE_LOG(LogTemp, Display, TEXT("  Saved %s → %s (%s)"),
        *PackageName, *FilePath, bSaved ? TEXT("OK") : TEXT("FAIL"));
    return bSaved;
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
            Table->AddRow(FName("High"), High);

            FNamecScalabilityTierRow Split;
            Split.TierName             = TEXT("Split");
            Split.MinViewports         = 3; Split.MaxViewports = 4;
            Split.LumenMaxViews        = 4; Split.GIQuality    = 2;
            Split.ShadowQuality        = 2; Split.FoliageQuality = 1;
            Split.PostProcessQuality   = 2; Split.DynamicResOperationMode = 2;
            Table->AddRow(FName("Split"), Split);
        }
    );
}
