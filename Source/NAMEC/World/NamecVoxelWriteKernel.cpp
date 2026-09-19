#include "World/NamecVoxelWriteKernel.h"

// Material-name → index mapping used during bake only. Runtime uses stored indices.
static uint8 MaterialNameToIndex(FName Name)
{
    if (Name.IsNone())          return 0; // air
    if (Name == TEXT("Grass"))  return 1;
    if (Name == TEXT("Stone"))  return 2;
    if (Name == TEXT("Dirt"))   return 3;
    if (Name == TEXT("Bedrock"))return 4;
    return 2; // unknown → stone
}

void FNamecVoxelWriteKernel::Apply(FNamecVoxelData& Data, FVector CentreMetres,
    float RadiusMetres, ENamecEditMode Mode, FName MaterialRow, int32 ToolTier)
{
    if (Data.SizeX == 0 || Data.MaterialIndex.IsEmpty()) return;

    const float InvVox = 1.f / Data.VoxelSizeMetres;
    const FVector LocalCentre = (CentreMetres - Data.WorldOriginMetres) * InvVox;
    const float RadV = RadiusMetres * InvVox;
    const float RadV2 = RadV * RadV;

    const int32 MinX = FMath::Max(0, FMath::FloorToInt(LocalCentre.X - RadV));
    const int32 MaxX = FMath::Min(Data.SizeX - 1, FMath::CeilToInt(LocalCentre.X + RadV));
    const int32 MinY = FMath::Max(0, FMath::FloorToInt(LocalCentre.Y - RadV));
    const int32 MaxY = FMath::Min(Data.SizeY - 1, FMath::CeilToInt(LocalCentre.Y + RadV));
    const int32 MinZ = FMath::Max(0, FMath::FloorToInt(LocalCentre.Z - RadV));
    const int32 MaxZ = FMath::Min(Data.SizeZ - 1, FMath::CeilToInt(LocalCentre.Z + RadV));

    const uint8 FillMat = (Mode == ENamecEditMode::Dig) ? 0 : MaterialNameToIndex(MaterialRow);

    for (int32 X = MinX; X <= MaxX; ++X)
    {
        const float dX = X - LocalCentre.X;
        for (int32 Y = MinY; Y <= MaxY; ++Y)
        {
            const float dY = Y - LocalCentre.Y;
            for (int32 Z = MinZ; Z <= MaxZ; ++Z)
            {
                const float dZ = Z - LocalCentre.Z;
                if (dX*dX + dY*dY + dZ*dZ <= RadV2)
                    Data.MaterialIndex[Data.LinearIdx(X, Y, Z)] = FillMat;
            }
        }
    }
}
