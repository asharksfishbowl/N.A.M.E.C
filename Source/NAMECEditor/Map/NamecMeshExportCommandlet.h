#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NamecMeshExportCommandlet.generated.h"

// Sitting-1 tool: extract one chunk from VMA_Benchmark, run the voxel mesher, save as
// UStaticMesh at Content/Benchmark/TestChunk.uasset for Lumen lighting sanity check.
//
// Run: UnrealEditor-Cmd.exe NAMEC.uproject -run=NamecMeshExport -Chunk=0,0
UCLASS()
class UNamecMeshExportCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
};
