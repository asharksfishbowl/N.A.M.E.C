#pragma once

#include "CoreMinimal.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"
#include "StaticMeshResources.h"
#include "DynamicMeshBuilder.h"

#if RHI_RAYTRACING
#include "RayTracingGeometry.h"
#endif

// Mesh data produced by the voxel mesher on the game thread and consumed by the scene proxy
// on the render thread. Moved at proxy construction time to avoid a copy.
struct NAMEC_API FNamecVoxelMeshData
{
    TArray<FDynamicMeshVertex> Vertices;
    TArray<uint32>             Indices;
    bool IsEmpty() const { return Indices.IsEmpty(); }
};

// ──────────────────────────────────────────────────────────────────────────────
// Render-thread scene proxy for one voxel chunk.
//
// Verified against UE 5.8.2 engine source:
//   CreateRenderThreadResources(FRHICommandListBase&) — Engine/PrimitiveSceneProxy.h:L~450
//   GetDynamicRayTracingInstances(FRayTracingInstanceCollector&) — same file:L458
//   bVisibleInLumenScene — same file; drives whether proxy enters the Lumen scene
//   FRayTracingGeometry — RenderCore/RayTracingGeometry.h; SetInitializer+CreateRayTracingGeometry
//   FRayTracingInstanceCollector::AddRayTracingInstance(ViewIndex, Instance)
//     — Engine/SceneManagement.h:L~1537 class decl
//
// Sitting-2 Lumen diagnostic: if the surface cache does not cover this chunk, check
//   bVisibleInLumenScene (set below) and compare against
//   FPrimitiveSceneProxy::IsVisibleInLumenScene() return. If the surface cache
//   footprint API was renamed, grep engine source for SetupLumenGIData.
// ──────────────────────────────────────────────────────────────────────────────
class FNamecVoxelSceneProxy final : public FPrimitiveSceneProxy
{
public:
    explicit FNamecVoxelSceneProxy(const class UNamecVoxelChunkComponent* Component,
                                    FNamecVoxelMeshData InMeshData);
    virtual ~FNamecVoxelSceneProxy() override;

    // FPrimitiveSceneProxy interface
    virtual void CreateRenderThreadResources(FRHICommandListBase& RHICmdList) override;
    virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
                                        const FSceneViewFamily& ViewFamily,
                                        uint32 VisibilityMap,
                                        FMeshElementCollector& Collector) const override;
    virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
    virtual bool CanBeOccluded() const override;
    virtual SIZE_T GetTypeHash() const override;
    virtual uint32 GetMemoryFootprint() const override;

#if RHI_RAYTRACING
    virtual bool IsRayTracingRelevant() const override       { return true; }
    virtual bool IsRayTracingStaticRelevant() const override { return false; }
    virtual void GetDynamicRayTracingInstances(
        class FRayTracingInstanceCollector& Collector) override;
#endif

private:
    FNamecVoxelMeshData MeshData; // consumed in CreateRenderThreadResources, then emptied

    FStaticMeshVertexBuffers VertexBuffers;
    FDynamicMeshIndexBuffer32 IndexBuffer;
    FLocalVertexFactory VertexFactory;

#if RHI_RAYTRACING
    FRayTracingGeometry RayTracingGeometry;
#endif

    UMaterialInterface* Material = nullptr;
    int32 NumPrimitives = 0;
};
