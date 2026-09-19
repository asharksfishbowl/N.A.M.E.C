#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Commandlets/Commandlet.h"
#include "NamecCreateBenchmarkPCGCommandlet.generated.h"

USTRUCT(BlueprintType)
struct FNamecPCGMeshRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) TSoftObjectPtr<UStaticMesh> Mesh;
    UPROPERTY(EditAnywhere) float Density = 1.0f;
};

// Commandlet that programmatically creates PCG graph skeletons and mesh DataTable assets.
// Run via: UnrealEditor-Cmd.exe NAMEC.uproject -run=NamecCreateBenchmarkPCG
UCLASS()
class UNamecCreateBenchmarkPCGCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
private:
    bool CreatePCGMeshTable(const FString& PackageName, const FString& MeshPath, float Density);
    bool CreatePCGGraph(const FString& PackageName, const FString& MeshPath);
};
