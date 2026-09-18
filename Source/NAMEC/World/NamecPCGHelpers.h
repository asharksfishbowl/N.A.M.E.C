#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NamecPCGHelpers.generated.h"

// PCG utility functions exposed to Blueprints so PCG graphs can call them as graph nodes.
UCLASS()
class NAMEC_API UNamecPCGHelpers : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Returns a deterministic seed for a chunk's PCG graph from the map's AuthoringSeed
    // and the chunk's grid coordinates. Identical on server and every client.
    UFUNCTION(BlueprintCallable, Category="NAMEC|PCG")
    static int32 GetChunkSeed(int32 AuthoringSeed, int32 ChunkX, int32 ChunkY);
};
