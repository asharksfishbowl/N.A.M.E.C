#include "NamecAssetAuthoring.h"
#include "HAL/FileManager.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

UPackage* NamecAssetAuthoring::CreateFullyLoadedPackage(const FString& PackageName)
{
    UPackage* Package = CreatePackage(*PackageName);
    Package->FullyLoad();
    return Package;
}

FString NamecAssetAuthoring::GetAssetName(const FString& PackageName)
{
    return FPackageName::GetLongPackageAssetName(PackageName);
}

bool NamecAssetAuthoring::SaveAsset(UObject& Asset)
{
    UPackage* Package = Asset.GetPackage();
    Package->MarkPackageDirty();

    const FString FilePath = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    const bool bSaved = UPackage::SavePackage(Package, &Asset, *FilePath, SaveArgs);
    UE_LOG(LogTemp, Display, TEXT("  Saved %s → %s (%s)"), *Package->GetName(), *FilePath, bSaved ? TEXT("OK") : TEXT("FAIL"));
    return bSaved;
}
