#include "World/NamecPCGHelpers.h"

int32 UNamecPCGHelpers::GetChunkSeed(int32 AuthoringSeed, int32 ChunkX, int32 ChunkY)
{
    // Combine three integers into one deterministic seed using UE's standard hash combine.
    // HashCombine is commutative only in pairs, so order (AuthoringSeed, X, Y) is fixed.
    uint32 Seed = static_cast<uint32>(AuthoringSeed);
    Seed = HashCombine(Seed, static_cast<uint32>(ChunkX));
    Seed = HashCombine(Seed, static_cast<uint32>(ChunkY));
    return static_cast<int32>(Seed);
}
