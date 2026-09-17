#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NamecVoxelMapAsset.generated.h"

// UHT does not support TArray<uint8> as a TMap value — wrap it.
USTRUCT()
struct FNamecChunkData
{
    GENERATED_BODY()
    UPROPERTY()
    TArray<uint8> Bytes;
};

UCLASS(BlueprintType)
class NAMEC_API UNamecVoxelMapAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGuid MapId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MapRevision = 0;

    // SHA-256 per chunk over uncompressed base voxels
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FIntPoint, FString> BaseChunkHashes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString MapHash;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WorldExtentMetres = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BedrockDepthMetres = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float VoxelResolutionCm = 25.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AuthoringSeed = 0;

    // Compressed per-chunk base voxel data — not Blueprint-accessible (FIntPoint key unsupported)
    UPROPERTY(EditAnywhere)
    TMap<FIntPoint, FNamecChunkData> BakedChunks;
};
