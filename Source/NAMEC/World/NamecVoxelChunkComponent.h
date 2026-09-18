#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Async/Future.h"
#include "World/NamecVoxelTypes.h"
#include "World/NamecVoxelSceneProxy.h"

#include "NamecVoxelChunkComponent.generated.h"

// Transition face bitmask — which faces border a coarser LOD chunk.
// Bit 0 = +X, 1 = -X, 2 = +Y, 3 = -Y, 4 = +Z, 5 = -Z.
enum ENamecTransitionFace : uint8
{
    NTF_PosX = (1 << 0),
    NTF_NegX = (1 << 1),
    NTF_PosY = (1 << 2),
    NTF_NegY = (1 << 3),
    NTF_PosZ = (1 << 4),
    NTF_NegZ = (1 << 5),
};

// Renders one voxel chunk using a custom scene proxy.
UCLASS(ClassGroup="NAMEC", meta=(BlueprintSpawnableComponent))
class NAMEC_API UNamecVoxelChunkComponent : public UPrimitiveComponent
{
    GENERATED_BODY()

public:
    UNamecVoxelChunkComponent();

    // Update the voxel data and trigger a synchronous re-mesh (game thread).
    void SetVoxelData(const FNamecVoxelData& InData);

    // Called by the chunk manager after neighbors are known; enables LOD ticking.
    void InitLOD(FIntPoint InChunkCoord);

    // Update neighbor transition bitmask and re-mesh if it changed.
    void SetTransitionFaces(uint8 InFaces);

    // UActorComponent interface
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // UPrimitiveComponent interface
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
    virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* InMaterial) override;
    virtual int32 GetNumMaterials() const override { return 1; }

    // Build a marching-cubes mesh at the given LOD stride with optional transition faces.
    // LODLevel 0 = stride 1 (full res), 1 = stride 2, 2 = stride 4.
    // TransitionFaces: ENamecTransitionFace bitmask — faces needing transition cell seaming.
    // Static so the mesh-export commandlet can call it without a component instance.
    static FNamecVoxelMeshData BuildMesh(const FNamecVoxelData& VoxelData,
                                         int32 LODLevel = 0,
                                         uint8 TransitionFaces = 0);

private:
    void RequestReMesh(int32 NewLOD, uint8 NewTransitionFaces);
    void PollPendingMesh();
    int32 ComputeLODFromCamera() const;

    FNamecVoxelData VoxelData;
    FBoxSphereBounds LocalBounds;

    FIntPoint ChunkCoord;
    int32 CurrentLOD = 0;
    uint8 CurrentTransitionFaces = 0;
    bool bLODEnabled = false;

    // Async mesh build: launched on the thread pool, result polled each tick.
    TFuture<FNamecVoxelMeshData> PendingMesh;
    bool bHasPendingMesh = false;
    int32 PendingLOD = 0;
    uint8 PendingTransitionFaces = 0;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> Material;
};
