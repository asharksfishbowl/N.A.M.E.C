#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "World/NamecVoxelTypes.h"
#include "World/NamecVoxelSceneProxy.h"

#include "NamecVoxelChunkComponent.generated.h"

// Renders one voxel chunk using a custom scene proxy.
// Task 5 adds LOD and transition cells on top; this is Phase 1 (regular cells only).
UCLASS(ClassGroup="NAMEC", meta=(BlueprintSpawnableComponent))
class NAMEC_API UNamecVoxelChunkComponent : public UPrimitiveComponent
{
    GENERATED_BODY()

public:
    UNamecVoxelChunkComponent();

    // Update the voxel data and trigger a re-mesh (game thread).
    void SetVoxelData(const FNamecVoxelData& InData);

    // UPrimitiveComponent interface
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
    virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* InMaterial) override;
    virtual int32 GetNumMaterials() const override { return 1; }

    // Build a marching-cubes mesh from voxel data.
    // Static so the mesh-export commandlet can call it without a component instance.
    static FNamecVoxelMeshData BuildMesh(const FNamecVoxelData& VoxelData);

private:
    FNamecVoxelData VoxelData;
    FBoxSphereBounds LocalBounds;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> Material;
};
