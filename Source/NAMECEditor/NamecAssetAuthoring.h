#pragma once

#include "CoreMinimal.h"

class UPackage;

// How a commandlet writes any asset to Content/: create the package, fill the object, save.
namespace NamecAssetAuthoring
{
    template<typename AssetType>
    AssetType* NewAssetInPackage(const FString& PackageName);

    bool SaveAsset(UObject& Asset);

    UPackage* CreateFullyLoadedPackage(const FString& PackageName);
    FString GetAssetName(const FString& PackageName);
}

template<typename AssetType>
AssetType* NamecAssetAuthoring::NewAssetInPackage(const FString& PackageName)
{
    return NewObject<AssetType>(CreateFullyLoadedPackage(PackageName), *GetAssetName(PackageName), RF_Public | RF_Standalone);
}
