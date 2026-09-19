#include "NamecDataTableAuthoring.h"
#include "Engine/DataTable.h"
#include "HAL/FileManager.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

bool NamecDataTableAuthoring::CreateAndSaveDataTable(const FString& PackageName, UScriptStruct* RowStruct, TFunctionRef<void(UDataTable*)> PopulateRows)
{
    UPackage* Package = CreatePackage(*PackageName);
    Package->FullyLoad();
    const FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);
    UDataTable* Table = NewObject<UDataTable>(Package, *AssetName, RF_Public | RF_Standalone);
    Table->RowStruct = RowStruct;
    PopulateRows(Table);
    Package->MarkPackageDirty();

    const FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    const bool bSaved = UPackage::SavePackage(Package, Table, *FilePath, SaveArgs);
    UE_LOG(LogTemp, Display, TEXT("  Saved %s → %s (%s)"), *PackageName, *FilePath, bSaved ? TEXT("OK") : TEXT("FAIL"));
    return bSaved;
}
