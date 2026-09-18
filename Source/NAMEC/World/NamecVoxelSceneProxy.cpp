#include "World/NamecVoxelSceneProxy.h"
#include "World/NamecVoxelChunkComponent.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Materials/MaterialRenderProxy.h"
#include "MeshMaterialShader.h"
#include "SceneManagement.h"
#include "Engine/Engine.h"

#if RHI_RAYTRACING
#include "RayTracingGeometry.h"
#endif

FNamecVoxelSceneProxy::FNamecVoxelSceneProxy(const UNamecVoxelChunkComponent* Component,
                                               FNamecVoxelMeshData InMeshData)
    : FPrimitiveSceneProxy(Component, NAME_None)
    , MeshData(MoveTemp(InMeshData))
    , VertexFactory(GetScene().GetFeatureLevel(), "FNamecVoxelSceneProxy")
    , Material(Component->GetMaterial(0))
{
    if (!Material)
    {
        Material = UMaterial::GetDefaultMaterial(MD_Surface);
    }

    // Opt into the Lumen scene so the chunk contributes to the surface cache.
    bVisibleInLumenScene = true;

    SetWireframeColor(FLinearColor(0.f, 0.5f, 1.f));
}

FNamecVoxelSceneProxy::~FNamecVoxelSceneProxy()
{
#if RHI_RAYTRACING
    RayTracingGeometry.ReleaseResource();
#endif
    VertexBuffers.PositionVertexBuffer.ReleaseResource();
    VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
    VertexBuffers.ColorVertexBuffer.ReleaseResource();
    IndexBuffer.ReleaseResource();
    VertexFactory.ReleaseResource();
}

void FNamecVoxelSceneProxy::CreateRenderThreadResources(FRHICommandListBase& RHICmdList)
{
    if (MeshData.IsEmpty())
    {
        return;
    }

    VertexBuffers.InitFromDynamicVertex(RHICmdList, &VertexFactory, MeshData.Vertices, 2 /*NumTexCoords*/);
    IndexBuffer.Indices = MeshData.Indices;
    IndexBuffer.InitRHI(RHICmdList);
    NumPrimitives = MeshData.Indices.Num() / 3;

#if RHI_RAYTRACING
    if (IsRayTracingEnabled())
    {
        FRayTracingGeometrySegment Segment;
        Segment.VertexBuffer       = VertexBuffers.PositionVertexBuffer.VertexBufferRHI;
        Segment.VertexBufferElementType = VET_Float3;
        Segment.VertexBufferStride = VertexBuffers.PositionVertexBuffer.GetStride();
        Segment.VertexBufferOffset = 0;
        Segment.MaxVertices        = VertexBuffers.PositionVertexBuffer.GetNumVertices();
        Segment.FirstPrimitive     = 0;
        Segment.NumPrimitives      = NumPrimitives;

        FRayTracingGeometryInitializer Initializer;
        Initializer.DebugName         = FDebugName(FName(TEXT("NamecVoxelChunk")));
        Initializer.IndexBuffer       = IndexBuffer.IndexBufferRHI;
        Initializer.IndexBufferStride = 4; // 32-bit indices
        Initializer.GeometryType      = RTGT_Triangles;
        Initializer.TotalPrimitiveCount = NumPrimitives;
        Initializer.bAllowUpdate      = true;  // mesh rebuilt on LOD change (Task 5)
        Initializer.Segments.Add(Segment);

        RayTracingGeometry.SetInitializer(Initializer);
        RayTracingGeometry.CreateRayTracingGeometry(RHICmdList, ERTAccelerationStructureBuildPriority::Normal);
    }
#endif

    // Release game-thread copy; GPU owns the data now.
    MeshData.Vertices.Empty();
    MeshData.Indices.Empty();
}

void FNamecVoxelSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
                                                    const FSceneViewFamily& ViewFamily,
                                                    uint32 VisibilityMap,
                                                    FMeshElementCollector& Collector) const
{
    if (NumPrimitives == 0 || !VertexFactory.IsInitialized())
    {
        return;
    }

    const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
    FMaterialRenderProxy* MaterialProxy = bWireframe
        ? new FColoredMaterialRenderProxy(GEngine->WireframeMaterial->GetRenderProxy(),
                                          GetWireframeColor())
        : Material->GetRenderProxy();

    for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
    {
        if (!(VisibilityMap & (1 << ViewIndex)))
        {
            continue;
        }

        FMeshBatch& Mesh = Collector.AllocateMesh();
        Mesh.bWireframe = bWireframe;
        Mesh.VertexFactory = &VertexFactory;
        Mesh.MaterialRenderProxy = MaterialProxy;
        Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
        Mesh.Type = PT_TriangleList;
        Mesh.DepthPriorityGroup = SDPG_World;
        Mesh.bCanApplyViewModeOverrides = true;

        FMeshBatchElement& Elem = Mesh.Elements[0];
        Elem.IndexBuffer = &IndexBuffer;
        Elem.FirstIndex = 0;
        Elem.NumPrimitives = NumPrimitives;
        Elem.MinVertexIndex = 0;
        Elem.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;

        FDynamicPrimitiveUniformBuffer& PrimitiveUB =
            Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
        FRHICommandList& CmdList = Collector.GetRHICommandList();
        PrimitiveUB.Set(CmdList, GetLocalToWorld(), GetLocalToWorld(),
                        GetBounds(), GetLocalBounds(),
                        true /*bReceivesDecals*/, false /*bHasPrecomputedVolumetricLightmap*/,
                        AlwaysHasVelocity());
        Elem.PrimitiveUniformBufferResource = &PrimitiveUB.UniformBuffer;

        Collector.AddMesh(ViewIndex, Mesh);
    }
}

FPrimitiveViewRelevance FNamecVoxelSceneProxy::GetViewRelevance(const FSceneView* View) const
{
    FPrimitiveViewRelevance Result;
    Result.bDrawRelevance       = IsShown(View);
    Result.bShadowRelevance     = IsShadowCast(View);
    Result.bDynamicRelevance    = true;
    Result.bRenderInMainPass    = ShouldRenderInMainPass();
    Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
    Result.bRenderCustomDepth   = ShouldRenderCustomDepth();
    return Result;
}

bool FNamecVoxelSceneProxy::CanBeOccluded() const
{
    return !ShouldRenderCustomDepth();
}

SIZE_T FNamecVoxelSceneProxy::GetTypeHash() const
{
    static size_t UniquePointer;
    return reinterpret_cast<size_t>(&UniquePointer);
}

uint32 FNamecVoxelSceneProxy::GetMemoryFootprint() const
{
    return sizeof(*this) + GetAllocatedSize();
}

#if RHI_RAYTRACING
void FNamecVoxelSceneProxy::GetDynamicRayTracingInstances(
    FRayTracingInstanceCollector& Collector)
{
    if (!RayTracingGeometry.IsValid() || NumPrimitives == 0)
    {
        return;
    }

    TArray<FMeshBatch>& MeshBatches = Collector.AllocateMeshBatchArray();
    {
        FMeshBatch& MeshBatch = MeshBatches.AddDefaulted_GetRef();
        MeshBatch.VertexFactory = &VertexFactory;
        MeshBatch.MaterialRenderProxy = Material->GetRenderProxy();
        MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
        MeshBatch.Type = PT_TriangleList;
        FMeshBatchElement& Elem = MeshBatch.Elements[0];
        Elem.IndexBuffer = &IndexBuffer;
        Elem.FirstIndex = 0;
        Elem.NumPrimitives = NumPrimitives;
        Elem.MinVertexIndex = 0;
        Elem.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
    }

    FRayTracingInstance Instance;
    Instance.Geometry = &RayTracingGeometry;
    Instance.MaterialsView = TConstArrayView<FMeshBatch>(MeshBatches);
    Instance.InstanceTransforms.Add(GetLocalToWorld());
    Instance.NumTransforms = 1;

    const TConstArrayView<const FSceneView*> Views = Collector.GetViews();
    for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
    {
        Collector.AddRayTracingInstance(ViewIndex, Instance);
    }
}
#endif
