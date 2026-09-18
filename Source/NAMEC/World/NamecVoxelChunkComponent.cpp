#include "World/NamecVoxelChunkComponent.h"
#include "World/NamecVoxelSceneProxy.h"
#include "Settings/NamecScalabilitySubsystem.h"
#include "Materials/Material.h"
#include "Engine/Engine.h"
#include "Async/Async.h"
#include "Engine/GameInstance.h"
#include "PhysicsEngine/BodySetup.h"

// ─────────────────────────────────────────────────────────────────────────────
// Marching-cubes lookup tables (Paul Bourke / Lorensen-Cline, public domain).
// edgeTable: 12-bit bitmask of which edges are cut for each of 256 configurations.
// triTable:  up to 5 triangles per config encoded as edge indices; -1 = end.
// ─────────────────────────────────────────────────────────────────────────────
namespace NamecMC
{
static const int32 EdgeTable[256] = {
    0x000,0x109,0x203,0x30a,0x406,0x50f,0x605,0x70c,0x80c,0x905,0xa0f,0xb06,0xc0a,0xd03,0xe09,0xf00,
    0x190,0x099,0x393,0x29a,0x596,0x49f,0x795,0x69c,0x99c,0x895,0xb9f,0xa96,0xd9a,0xc93,0xf99,0xe90,
    0x230,0x339,0x033,0x13a,0x636,0x73f,0x435,0x53c,0xa3c,0xb35,0x83f,0x936,0xe3a,0xf33,0xc39,0xd30,
    0x3a0,0x2a9,0x1a3,0x0aa,0x7a6,0x6af,0x5a5,0x4ac,0xbac,0xaa5,0x9af,0x8a6,0xfaa,0xea3,0xda9,0xca0,
    0x460,0x569,0x663,0x76a,0x066,0x16f,0x265,0x36c,0xc6c,0xd65,0xe6f,0xf66,0x86a,0x963,0xa69,0xb60,
    0x5f0,0x4f9,0x7f3,0x6fa,0x1f6,0x0ff,0x3f5,0x2fc,0xdfc,0xcf5,0xfff,0xef6,0x9fa,0x8f3,0xbf9,0xaf0,
    0x650,0x759,0x453,0x55a,0x256,0x35f,0x055,0x15c,0xe5c,0xf55,0xc5f,0xd56,0xa5a,0xb53,0x859,0x950,
    0x7c0,0x6c9,0x5c3,0x4ca,0x3c6,0x2cf,0x1c5,0x0cc,0xfcc,0xec5,0xdcf,0xcc6,0xbca,0xac3,0x9c9,0x8c0,
    0x8c0,0x9c9,0xac3,0xbca,0xcc6,0xdcf,0xec5,0xfcc,0x0cc,0x1c5,0x2cf,0x3c6,0x4ca,0x5c3,0x6c9,0x7c0,
    0x950,0x859,0xb53,0xa5a,0xd56,0xc5f,0xf55,0xe5c,0x15c,0x055,0x35f,0x256,0x55a,0x453,0x759,0x650,
    0xaf0,0xbf9,0x8f3,0x9fa,0xef6,0xfff,0xcf5,0xdfc,0x2fc,0x3f5,0x0ff,0x1f6,0x6fa,0x7f3,0x4f9,0x5f0,
    0xb60,0xa69,0x963,0x86a,0xf66,0xe6f,0xd65,0xc6c,0x36c,0x265,0x16f,0x066,0x76a,0x663,0x569,0x460,
    0xca0,0xda9,0xea3,0xfaa,0x8a6,0x9af,0xaa5,0xbac,0x4ac,0x5a5,0x6af,0x7a6,0x0aa,0x1a3,0x2a9,0x3a0,
    0xd30,0xc39,0xf33,0xe3a,0x936,0x83f,0xb35,0xa3c,0x53c,0x435,0x73f,0x636,0x13a,0x033,0x339,0x230,
    0xe90,0xf99,0xc93,0xd9a,0xa96,0xb9f,0x895,0x99c,0x69c,0x795,0x49f,0x596,0x29a,0x393,0x099,0x190,
    0xf00,0xe09,0xd03,0xc0a,0xb06,0xa0f,0x905,0x80c,0x70c,0x605,0x50f,0x406,0x30a,0x203,0x109,0x000
};

// triTable[256][16]: edge indices 0-11, rows terminated by -1.
static const int8 TriTable[256][16] = {
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 1, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 8, 3, 9, 8, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 1, 2,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 2,10, 0, 2, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 2, 8, 3, 2,10, 8,10, 9, 8,-1,-1,-1,-1,-1,-1,-1},
    { 3,11, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0,11, 2, 8,11, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 9, 0, 2, 3,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1,11, 2, 1, 9,11, 9, 8,11,-1,-1,-1,-1,-1,-1,-1},
    { 3,10, 1,11,10, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0,10, 1, 0, 8,10, 8,11,10,-1,-1,-1,-1,-1,-1,-1},
    { 3, 9, 0, 3,11, 9,11,10, 9,-1,-1,-1,-1,-1,-1,-1},
    { 9, 8,10,10, 8,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 7, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 3, 0, 7, 3, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 1, 9, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 1, 9, 4, 7, 1, 7, 3, 1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 4, 7, 3, 0, 4, 1, 2,10,-1,-1,-1,-1,-1,-1,-1},
    { 9, 2,10, 9, 0, 2, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    { 2,10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4,-1,-1,-1,-1},
    { 8, 4, 7, 3,11, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 4, 7,11, 2, 4, 2, 0, 4,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 1, 8, 4, 7, 2, 3,11,-1,-1,-1,-1,-1,-1,-1},
    { 4, 7,11, 9, 4,11, 9,11, 2, 9, 2, 1,-1,-1,-1,-1},
    { 3,10, 1, 3,11,10, 7, 8, 4,-1,-1,-1,-1,-1,-1,-1},
    { 1,11,10, 1, 4,11, 1, 0, 4, 7,11, 4,-1,-1,-1,-1},
    { 4, 7, 8, 9, 0,11, 9,11,10,11, 0, 3,-1,-1,-1,-1},
    { 4, 7,11, 4,11, 9, 9,11,10,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 4, 0, 8, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 5, 4, 1, 5, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 5, 4, 8, 3, 5, 3, 1, 5,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 8, 1, 2,10, 4, 9, 5,-1,-1,-1,-1,-1,-1,-1},
    { 5, 2,10, 5, 4, 2, 4, 0, 2,-1,-1,-1,-1,-1,-1,-1},
    { 2,10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8,-1,-1,-1,-1},
    { 9, 5, 4, 2, 3,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0,11, 2, 0, 8,11, 4, 9, 5,-1,-1,-1,-1,-1,-1,-1},
    { 0, 5, 4, 0, 1, 5, 2, 3,11,-1,-1,-1,-1,-1,-1,-1},
    { 2, 1, 5, 2, 5, 8, 2, 8,11, 4, 8, 5,-1,-1,-1,-1},
    {10, 3,11,10, 1, 3, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1},
    { 4, 9, 5, 0, 8, 1, 8,10, 1, 8,11,10,-1,-1,-1,-1},
    { 5, 4, 0, 5, 0,11, 5,11,10,11, 0, 3,-1,-1,-1,-1},
    { 5, 4, 8, 5, 8,10,10, 8,11,-1,-1,-1,-1,-1,-1,-1},
    { 9, 7, 8, 5, 7, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 3, 0, 9, 5, 3, 5, 7, 3,-1,-1,-1,-1,-1,-1,-1},
    { 0, 7, 8, 0, 1, 7, 1, 5, 7,-1,-1,-1,-1,-1,-1,-1},
    { 1, 5, 3, 3, 5, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 7, 8, 9, 5, 7,10, 1, 2,-1,-1,-1,-1,-1,-1,-1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3,-1,-1,-1,-1},
    { 8, 0, 2, 8, 2, 5, 8, 5, 7,10, 5, 2,-1,-1,-1,-1},
    { 2,10, 5, 2, 5, 3, 3, 5, 7,-1,-1,-1,-1,-1,-1,-1},
    { 7, 9, 5, 7, 8, 9, 3,11, 2,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7,11,-1,-1,-1,-1},
    { 2, 3,11, 0, 1, 8, 1, 7, 8, 1, 5, 7,-1,-1,-1,-1},
    {11, 2, 1,11, 1, 7, 7, 1, 5,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 8, 8, 5, 7,10, 1, 3,10, 3,11,-1,-1,-1,-1},
    { 5, 7, 0, 5, 0, 9, 7,11, 0, 1, 0,10,11,10, 0,-1},
    {11,10, 0,11, 0, 3,10, 5, 0, 8, 0, 7, 5, 7, 0,-1},
    {11,10, 5, 7,11, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 5,10, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 1, 5,10, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 8, 3, 1, 9, 8, 5,10, 6,-1,-1,-1,-1,-1,-1,-1},
    { 1, 6, 5, 2, 6, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 6, 5, 1, 2, 6, 3, 0, 8,-1,-1,-1,-1,-1,-1,-1},
    { 9, 6, 5, 9, 0, 6, 0, 2, 6,-1,-1,-1,-1,-1,-1,-1},
    { 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8,-1,-1,-1,-1},
    { 2, 3,11,10, 6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 0, 8,11, 2, 0,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 0, 1, 9, 2, 3,11, 5,10, 6,-1,-1,-1,-1,-1,-1,-1},
    { 5,10, 6, 1, 9, 2, 9,11, 2, 9, 8,11,-1,-1,-1,-1},
    { 6, 3,11, 6, 5, 3, 5, 1, 3,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8,11, 0,11, 5, 0, 5, 1, 5,11, 6,-1,-1,-1,-1},
    { 3,11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9,-1,-1,-1,-1},
    { 6, 5, 9, 6, 9,11,11, 9, 8,-1,-1,-1,-1,-1,-1,-1},
    { 5,10, 6, 4, 7, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 3, 0, 4, 7, 3, 6, 5,10,-1,-1,-1,-1,-1,-1,-1},
    { 1, 9, 0, 5,10, 6, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4,-1,-1,-1,-1},
    { 6, 1, 2, 6, 5, 1, 4, 7, 8,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2, 5, 5, 2, 6, 3, 4, 0, 3, 7, 4,-1,-1,-1,-1},
    { 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6,-1,-1,-1,-1},
    { 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9,-1},
    { 3,11, 2, 7, 8, 4,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 5,10, 6, 4, 7, 2, 4, 2, 0, 2, 7,11,-1,-1,-1,-1},
    { 0, 1, 9, 4, 7, 8, 2, 3,11, 5,10, 6,-1,-1,-1,-1},
    { 9, 2, 1, 9,11, 2, 9, 4,11, 7,11, 4, 5,10, 6,-1},
    { 8, 4, 7, 3,11, 5, 3, 5, 1, 5,11, 6,-1,-1,-1,-1},
    { 5, 1,11, 5,11, 6, 1, 0,11, 7,11, 4, 0, 4,11,-1},
    { 0, 5, 9, 0, 6, 5, 0, 3, 6,11, 6, 3, 8, 4, 7,-1},
    { 6, 5, 9, 6, 9,11, 4, 7, 9, 7,11, 9,-1,-1,-1,-1},
    {10, 4, 9, 6, 4,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4,10, 6, 4, 9,10, 0, 8, 3,-1,-1,-1,-1,-1,-1,-1},
    {10, 0, 1,10, 6, 0, 6, 4, 0,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1,10,-1,-1,-1,-1},
    { 1, 4, 9, 1, 2, 4, 2, 6, 4,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4,-1,-1,-1,-1},
    { 0, 2, 4, 4, 2, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 2, 8, 2, 4, 4, 2, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 4, 9,10, 6, 4,11, 2, 3,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 2, 2, 8,11, 4, 9,10, 4,10, 6,-1,-1,-1,-1},
    { 3,11, 2, 0, 1, 6, 0, 6, 4, 6, 1,10,-1,-1,-1,-1},
    { 6, 4, 1, 6, 1,10, 4, 8, 1, 2, 1,11, 8,11, 1,-1},
    { 9, 6, 4, 9, 3, 6, 9, 1, 3,11, 6, 3,-1,-1,-1,-1},
    { 8,11, 1, 8, 1, 0,11, 6, 1, 9, 1, 4, 6, 4, 1,-1},
    { 3,11, 6, 3, 6, 0, 0, 6, 4,-1,-1,-1,-1,-1,-1,-1},
    { 6, 4, 8,11, 6, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 7,10, 6, 7, 8,10, 8, 9,10,-1,-1,-1,-1,-1,-1,-1},
    { 0, 7, 3, 0,10, 7, 0, 9,10, 6, 7,10,-1,-1,-1,-1},
    {10, 6, 7, 1,10, 7, 1, 7, 8, 1, 8, 0,-1,-1,-1,-1},
    {10, 6, 7,10, 7, 1, 1, 7, 3,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7,-1,-1,-1,-1},
    { 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9,-1},
    { 7, 8, 0, 7, 0, 6, 6, 0, 2,-1,-1,-1,-1,-1,-1,-1},
    { 7, 3, 2, 6, 7, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 2, 3,11,10, 6, 8,10, 8, 9, 8, 6, 7,-1,-1,-1,-1},
    { 2, 0, 7, 2, 7,11, 0, 9, 7, 6, 7,10, 9,10, 7,-1},
    { 1, 8, 0, 1, 7, 8, 1,10, 7, 6, 7,10, 2, 3,11,-1},
    {11, 2, 1,11, 1, 7,10, 6, 1, 6, 7, 1,-1,-1,-1,-1},
    { 8, 9, 6, 8, 6, 7, 9, 1, 6,11, 6, 3, 1, 3, 6,-1},
    { 0, 9, 1,11, 6, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 7, 8, 0, 7, 0, 6, 3,11, 0,11, 6, 0,-1,-1,-1,-1},
    { 7,11, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 7, 6,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 8,11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 1, 9,11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 1, 9, 8, 3, 1,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 1, 2, 6,11, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10, 3, 0, 8, 6,11, 7,-1,-1,-1,-1,-1,-1,-1},
    { 2, 9, 0, 2,10, 9, 6,11, 7,-1,-1,-1,-1,-1,-1,-1},
    { 6,11, 7, 2,10, 3,10, 8, 3,10, 9, 8,-1,-1,-1,-1},
    { 7, 2, 3, 6, 2, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 7, 0, 8, 7, 6, 0, 6, 2, 0,-1,-1,-1,-1,-1,-1,-1},
    { 2, 7, 6, 2, 3, 7, 0, 1, 9,-1,-1,-1,-1,-1,-1,-1},
    { 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6,-1,-1,-1,-1},
    {10, 7, 6,10, 1, 7, 1, 3, 7,-1,-1,-1,-1,-1,-1,-1},
    {10, 7, 6, 1, 7,10, 1, 8, 7, 1, 0, 8,-1,-1,-1,-1},
    { 0, 3, 7, 0, 7,10, 0,10, 9, 6,10, 7,-1,-1,-1,-1},
    { 7, 6,10, 7,10, 8, 8,10, 9,-1,-1,-1,-1,-1,-1,-1},
    { 6, 8, 4,11, 8, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 6,11, 3, 0, 6, 0, 4, 6,-1,-1,-1,-1,-1,-1,-1},
    { 8, 6,11, 8, 4, 6, 9, 0, 1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 4, 6, 9, 6, 3, 9, 3, 1,11, 3, 6,-1,-1,-1,-1},
    { 6, 8, 4, 6,11, 8, 2,10, 1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10, 3, 0,11, 0, 6,11, 0, 4, 6,-1,-1,-1,-1},
    { 4,11, 8, 4, 6,11, 0, 2, 9, 2,10, 9,-1,-1,-1,-1},
    {10, 9, 3,10, 3, 2, 9, 4, 3,11, 3, 6, 4, 6, 3,-1},
    { 8, 2, 3, 8, 4, 2, 4, 6, 2,-1,-1,-1,-1,-1,-1,-1},
    { 0, 4, 2, 4, 6, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8,-1,-1,-1,-1},
    { 1, 9, 4, 1, 4, 2, 2, 4, 6,-1,-1,-1,-1,-1,-1,-1},
    { 8, 1, 3, 8, 6, 1, 8, 4, 6, 6,10, 1,-1,-1,-1,-1},
    {10, 1, 0,10, 0, 6, 6, 0, 4,-1,-1,-1,-1,-1,-1,-1},
    { 4, 6, 3, 4, 3, 8, 6,10, 3, 0, 3, 9,10, 9, 3,-1},
    {10, 9, 4, 6,10, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 9, 5, 7, 6,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 4, 9, 5,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    { 5, 0, 1, 5, 4, 0, 7, 6,11,-1,-1,-1,-1,-1,-1,-1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5,-1,-1,-1,-1},
    { 9, 5, 4,10, 1, 2, 7, 6,11,-1,-1,-1,-1,-1,-1,-1},
    { 6,11, 7, 1, 2,10, 0, 8, 3, 4, 9, 5,-1,-1,-1,-1},
    { 7, 6,11, 5, 4,10, 4, 2,10, 4, 0, 2,-1,-1,-1,-1},
    { 3, 4, 8, 3, 5, 4, 3, 2, 5,10, 5, 2,11, 7, 6,-1},
    { 7, 2, 3, 7, 6, 2, 5, 4, 9,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7,-1,-1,-1,-1},
    { 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0,-1,-1,-1,-1},
    { 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8,-1},
    { 9, 5, 4,10, 1, 6, 1, 7, 6, 1, 3, 7,-1,-1,-1,-1},
    { 1, 6,10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4,-1},
    { 4, 0,10, 4,10, 5, 0, 3,10, 6,10, 7, 3, 7,10,-1},
    { 7, 6,10, 7,10, 8, 5, 4,10, 4, 8,10,-1,-1,-1,-1},
    { 6, 9, 5, 6,11, 9,11, 8, 9,-1,-1,-1,-1,-1,-1,-1},
    { 3, 6,11, 0, 6, 3, 0, 5, 6, 0, 9, 5,-1,-1,-1,-1},
    { 0,11, 8, 0, 5,11, 0, 1, 5, 5, 6,11,-1,-1,-1,-1},
    { 6,11, 3, 6, 3, 5, 5, 3, 1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10, 9, 5,11, 9,11, 8,11, 5, 6,-1,-1,-1,-1},
    { 0,11, 3, 0, 6,11, 0, 9, 6, 5, 6, 9, 1, 2,10,-1},
    {11, 8, 5,11, 5, 6, 8, 0, 5,10, 5, 2, 0, 2, 5,-1},
    { 6,11, 3, 6, 3, 5, 2,10, 3,10, 5, 3,-1,-1,-1,-1},
    { 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2,-1,-1,-1,-1},
    { 9, 5, 6, 9, 6, 0, 0, 6, 2,-1,-1,-1,-1,-1,-1,-1},
    { 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8,-1},
    { 1, 5, 6, 2, 1, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 3, 6, 1, 6,10, 3, 8, 6, 5, 6, 9, 8, 9, 6,-1},
    {10, 1, 0,10, 0, 6, 9, 5, 0, 5, 6, 0,-1,-1,-1,-1},
    { 0, 3, 8, 5, 6,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {10, 5, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 5,10, 7, 5,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 5,10,11, 7, 5, 8, 3, 0,-1,-1,-1,-1,-1,-1,-1},
    { 5,11, 7, 5,10,11, 1, 9, 0,-1,-1,-1,-1,-1,-1,-1},
    {10, 7, 5,10,11, 7, 9, 8, 1, 8, 3, 1,-1,-1,-1,-1},
    {11, 1, 2,11, 7, 1, 7, 5, 1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2,11,-1,-1,-1,-1},
    { 9, 7, 5, 9, 2, 7, 9, 0, 2, 2,11, 7,-1,-1,-1,-1},
    { 7, 5, 2, 7, 2,11, 5, 9, 2, 3, 2, 8, 9, 8, 2,-1},
    { 2, 5,10, 2, 3, 5, 3, 7, 5,-1,-1,-1,-1,-1,-1,-1},
    { 8, 2, 0, 8, 5, 2, 8, 7, 5,10, 2, 5,-1,-1,-1,-1},
    { 9, 0, 1, 5,10, 3, 5, 3, 7, 3,10, 2,-1,-1,-1,-1},
    { 9, 8, 2, 9, 2, 1, 8, 7, 2,10, 2, 5, 7, 5, 2,-1},
    { 1, 3, 5, 3, 7, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 7, 0, 7, 1, 1, 7, 5,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 3, 9, 3, 5, 5, 3, 7,-1,-1,-1,-1,-1,-1,-1},
    { 9, 8, 7, 5, 9, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 5, 8, 4, 5,10, 8,10,11, 8,-1,-1,-1,-1,-1,-1,-1},
    { 5, 0, 4, 5,11, 0, 5,10,11,11, 3, 0,-1,-1,-1,-1},
    { 0, 1, 9, 8, 4,10, 8,10,11,10, 4, 5,-1,-1,-1,-1},
    {10,11, 4,10, 4, 5,11, 3, 4, 9, 4, 1, 3, 1, 4,-1},
    { 2, 5, 1, 2, 8, 5, 2,11, 8, 4, 5, 8,-1,-1,-1,-1},
    { 0, 4,11, 0,11, 3, 4, 5,11, 2,11, 1, 5, 1,11,-1},
    { 0, 2, 5, 0, 5, 9, 2,11, 5, 4, 5, 8,11, 8, 5,-1},
    { 9, 4, 5, 2,11, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 2, 5,10, 3, 5, 2, 3, 4, 5, 3, 8, 4,-1,-1,-1,-1},
    { 5,10, 2, 5, 2, 4, 4, 2, 0,-1,-1,-1,-1,-1,-1,-1},
    { 3,10, 2, 3, 5,10, 3, 8, 5, 4, 5, 8, 0, 1, 9,-1},
    { 5,10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2,-1,-1,-1,-1},
    { 8, 4, 5, 8, 5, 3, 3, 5, 1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 4, 5, 1, 0, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5,-1,-1,-1,-1},
    { 9, 4, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4,11, 7, 4, 9,11, 9,10,11,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 4, 9, 7, 9,11, 7, 9,10,11,-1,-1,-1,-1},
    { 1,10,11, 1,11, 4, 1, 4, 0, 7, 4,11,-1,-1,-1,-1},
    { 3, 1, 4, 3, 4, 8, 1,10, 4, 7, 4,11,10,11, 4,-1},
    { 4,11, 7, 9,11, 4, 9, 2,11, 9, 1, 2,-1,-1,-1,-1},
    { 9, 7, 4, 9,11, 7, 9, 1,11, 2,11, 1, 0, 8, 3,-1},
    {11, 7, 4,11, 4, 2, 2, 4, 0,-1,-1,-1,-1,-1,-1,-1},
    {11, 7, 4,11, 4, 2, 8, 3, 4, 3, 2, 4,-1,-1,-1,-1},
    { 2, 9,10, 2, 7, 9, 2, 3, 7, 7, 4, 9,-1,-1,-1,-1},
    { 9,10, 7, 9, 7, 4,10, 2, 7, 8, 7, 0, 2, 0, 7,-1},
    { 3, 7,10, 3,10, 2, 7, 4,10, 1,10, 0, 4, 0,10,-1},
    { 1,10, 2, 8, 7, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 9, 1, 4, 1, 7, 7, 1, 3,-1,-1,-1,-1,-1,-1,-1},
    { 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1,-1,-1,-1,-1},
    { 4, 0, 3, 7, 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 8, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9,10, 8,10,11, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 9, 3, 9,11,11, 9,10,-1,-1,-1,-1,-1,-1,-1},
    { 0, 1,10, 0,10, 8, 8,10,11,-1,-1,-1,-1,-1,-1,-1},
    { 3, 1,10,11, 3,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,11, 1,11, 9, 9,11, 8,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 9, 3, 9,11, 1, 2, 9, 2,11, 9,-1,-1,-1,-1},
    { 0, 2,11, 8, 0,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 2,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 2, 3, 8, 2, 8,10,10, 8, 9,-1,-1,-1,-1,-1,-1,-1},
    { 9,10, 2, 0, 9, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 2, 3, 8, 2, 8,10, 0, 1, 8, 1,10, 8,-1,-1,-1,-1},
    { 1,10, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 1, 3, 8, 9, 1, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 9, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 3, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

// Corner offsets for the 8 marching-cubes cube corners (X,Y,Z in voxel units).
static const FIntVector CornerOffset[8] = {
    {0,0,0},{1,0,0},{1,1,0},{0,1,0},
    {0,0,1},{1,0,1},{1,1,1},{0,1,1}
};

// Edge endpoint pairs (two corners per edge, 12 edges total).
static const int32 EdgeCorners[12][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}
};

// Interpolate between two corners — binary density so midpoint is always 0.5.
static FVector3f LerpEdge(FVector3f P0, FVector3f P1) { return (P0 + P1) * 0.5f; }

} // namespace NamecMC

// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
// 2D marching-squares table for transition-face seam healing.
// 4 corners (A=bit0, C=bit1, G=bit2, I=bit3) → two face-edge indices per segment.
// Edge indices: 0=bottom(A-C), 1=right(C-I), 2=top(G-I), 3=left(A-G). -1=none.
// ─────────────────────────────────────────────────────────────────────────────
namespace NamecTF
{
// Each case: up to 2 edge segments (4 values). -1 = terminate.
static const int8 MSTable[16][5] = {
    {-1,-1,-1,-1,-1}, // 0000 — none inside
    { 0, 3,-1,-1,-1}, // 0001 — A
    { 0, 1,-1,-1,-1}, // 0010 — C
    { 3, 1,-1,-1,-1}, // 0011 — AC
    { 3, 2,-1,-1,-1}, // 0100 — G
    { 0, 2,-1,-1,-1}, // 0101 — AG  (saddle: prefer A,G split)
    { 0, 3, 1, 2,-1}, // 0110 — CG  (two segments)
    { 1, 2,-1,-1,-1}, // 0111 — ACG
    { 1, 2,-1,-1,-1}, // 1000 — I
    { 0, 3, 1, 2,-1}, // 1001 — AI  (two segments)
    { 0, 2,-1,-1,-1}, // 1010 — CI  (saddle: prefer C,I split)
    { 3, 2,-1,-1,-1}, // 1011 — ACI
    { 3, 1,-1,-1,-1}, // 1100 — GI
    { 0, 1,-1,-1,-1}, // 1101 — AGI
    { 0, 3,-1,-1,-1}, // 1110 — CGI
    {-1,-1,-1,-1,-1}, // 1111 — all inside
};
} // namespace NamecTF
// ─────────────────────────────────────────────────────────────────────────────

FNamecVoxelMeshData UNamecVoxelChunkComponent::BuildMesh(const FNamecVoxelData& Vd,
                                                          int32 LODLevel,
                                                          uint8 TransitionFaces)
{
    using namespace NamecMC;
    using namespace NamecTF;

    FNamecVoxelMeshData Out;
    if (Vd.SizeX < 2 || Vd.SizeY < 2 || Vd.SizeZ < 2) return Out;

    const int32 Stride   = 1 << FMath::Clamp(LODLevel, 0, 4); // voxel step per cell
    const int32 NumCellX = (Vd.SizeX - 1) / Stride;
    const int32 NumCellY = (Vd.SizeY - 1) / Stride;
    const int32 NumCellZ = (Vd.SizeZ - 1) / Stride;

    if (NumCellX < 1 || NumCellY < 1 || NumCellZ < 1) return Out;

    const float Vs     = Vd.VoxelSizeMetres * 100.f;          // voxel size in UU (cm)
    const FVector3f Origin(Vd.WorldOriginMetres * 100.f);

    Out.Vertices.Reserve(NumCellX * NumCellY * NumCellZ * 3);
    Out.Indices.Reserve(Out.Vertices.GetSlack() * 3);
    TArray<FVector3f> NormalAccum;

    // Vertex cache: key encodes (CellX, CellY, CellZ, EdgeIdx) in cell-space.
    TMap<uint64, int32> EdgeVertexMap;
    EdgeVertexMap.Reserve(NumCellX * NumCellY * 4);

    // ── helpers ──────────────────────────────────────────────────────────────

    auto GetDensity = [&](int32 X, int32 Y, int32 Z) -> float
    {
        if (X < 0 || Y < 0 || Z < 0 || X >= Vd.SizeX || Y >= Vd.SizeY || Z >= Vd.SizeZ)
            return 0.f;
        return Vd.MaterialIndex[Vd.LinearIdx(X, Y, Z)] > 0 ? 1.f : 0.f;
    };

    // CellX/Y/Z are in cell-space; multiply by Stride for voxel-space coords.
    auto EncodeEdge = [](int32 Cx, int32 Cy, int32 Cz, int32 Edge) -> uint64
    {
        return ((uint64)(uint16)Cx) | ((uint64)(uint16)Cy << 16) |
               ((uint64)(uint16)Cz << 32) | ((uint64)(uint8)Edge << 48);
    };

    auto AddVertex = [&](FVector3f Pos) -> int32
    {
        FDynamicMeshVertex V;
        V.Position = Pos;
        V.TextureCoordinate[0] = FVector2f(Pos.X / (Vs * 4.f), Pos.Y / (Vs * 4.f));
        V.Color = FColor::White;
        V.SetTangents(FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector3f(0, 0, 1));
        NormalAccum.AddZeroed();
        return Out.Vertices.Add(V);
    };

    auto GetOrCreateVertex = [&](int32 Cx, int32 Cy, int32 Cz, int32 EdgeIdx) -> int32
    {
        uint64 Key = EncodeEdge(Cx, Cy, Cz, EdgeIdx);
        if (int32* Ex = EdgeVertexMap.Find(Key)) return *Ex;

        int32 C0 = EdgeCorners[EdgeIdx][0], C1 = EdgeCorners[EdgeIdx][1];
        FIntVector O0 = FIntVector(Cx, Cy, Cz) * Stride + CornerOffset[C0] * Stride;
        FIntVector O1 = FIntVector(Cx, Cy, Cz) * Stride + CornerOffset[C1] * Stride;
        FVector3f Pos = LerpEdge(Origin + FVector3f(O0.X, O0.Y, O0.Z) * Vs,
                                  Origin + FVector3f(O1.X, O1.Y, O1.Z) * Vs);
        int32 Idx = AddVertex(Pos);
        EdgeVertexMap.Add(Key, Idx);
        return Idx;
    };

    auto EmitTriangle = [&](int32 I0, int32 I1, int32 I2)
    {
        FVector3f P0 = Out.Vertices[I0].Position;
        FVector3f P1 = Out.Vertices[I1].Position;
        FVector3f P2 = Out.Vertices[I2].Position;
        FVector3f N  = ((P1 - P0) ^ (P2 - P0)).GetSafeNormal();
        NormalAccum[I0] += N; NormalAccum[I1] += N; NormalAccum[I2] += N;
        Out.Indices.Add(I0); Out.Indices.Add(I1); Out.Indices.Add(I2);
    };

    // ── regular marching-cubes at LOD stride ─────────────────────────────────
    // Cells on the last row/col of a transition face are skipped here and handled
    // by the 2D marching-squares pass below.

    auto IsTransitionCell = [&](int32 Cx, int32 Cy, int32 Cz) -> bool
    {
        if ((TransitionFaces & NTF_PosX) && Cx == NumCellX - 1) return true;
        if ((TransitionFaces & NTF_NegX) && Cx == 0)            return true;
        if ((TransitionFaces & NTF_PosY) && Cy == NumCellY - 1) return true;
        if ((TransitionFaces & NTF_NegY) && Cy == 0)            return true;
        if ((TransitionFaces & NTF_PosZ) && Cz == NumCellZ - 1) return true;
        if ((TransitionFaces & NTF_NegZ) && Cz == 0)            return true;
        return false;
    };

    for (int32 Cz = 0; Cz < NumCellZ; ++Cz)
    for (int32 Cy = 0; Cy < NumCellY; ++Cy)
    for (int32 Cx = 0; Cx < NumCellX; ++Cx)
    {
        if (IsTransitionCell(Cx, Cy, Cz)) continue;

        uint8 Config = 0;
        for (int32 c = 0; c < 8; ++c)
        {
            FIntVector Off = FIntVector(Cx, Cy, Cz) * Stride + CornerOffset[c] * Stride;
            if (GetDensity(Off.X, Off.Y, Off.Z) > 0.5f) Config |= (1 << c);
        }
        if (Config == 0 || Config == 255 || EdgeTable[Config] == 0) continue;

        for (int32 t = 0; TriTable[Config][t] != -1; t += 3)
            EmitTriangle(GetOrCreateVertex(Cx, Cy, Cz, TriTable[Config][t]),
                         GetOrCreateVertex(Cx, Cy, Cz, TriTable[Config][t + 1]),
                         GetOrCreateVertex(Cx, Cy, Cz, TriTable[Config][t + 2]));
    }

    // ── transition-face 2D marching-squares seam cells ────────────────────────
    // For each enabled transition face, run 2D marching squares using only the 4
    // low-res corner samples (A,C,G,I). Face-edge crossing vertices (B,D,F,H) are
    // at positions the coarser neighbor also produces, so no T-junctions arise.
    // Each 2D face segment is connected to an interior backing vertex (one stride
    // inward) to form a closed 3D triangle.
    //
    // Local 2D coordinate conventions per face axis:
    //   +X face: face axes are (Y,Z); inner axis is -X
    //   -X face: face axes are (Y,Z); inner axis is +X
    //   +Y face: face axes are (X,Z); inner axis is -Y
    //   -Y face: face axes are (X,Z); inner axis is +Y
    //   +Z face: face axes are (X,Y); inner axis is -Z
    //   -Z face: face axes are (X,Y); inner axis is +Z

    // Transition-face segment emission for one strip cell.
    // Va/Vc/Vg/Vi: world positions of the 4 low-res face corners.
    // Vbk: interior backing vertex position (one stride into the chunk).
    // Avoid 'PC'/'PI' — Windows macros conflict with those names.
    auto EmitTransitionFaceCell = [&](FVector3f Va, FVector3f Vc,
                                       FVector3f Vg, FVector3f Vi,
                                       FVector3f Vbk,
                                       float dA, float dC,
                                       float dG, float dI)
    {
        FVector3f PB = LerpEdge(Va, Vc); // bottom edge midpoint
        FVector3f PF = LerpEdge(Vc, Vi); // right edge midpoint
        FVector3f PH = LerpEdge(Vg, Vi); // top edge midpoint
        FVector3f PD = LerpEdge(Va, Vg); // left edge midpoint

        uint8 MSCase = ((dA > 0.5f) ? 1 : 0) | ((dC > 0.5f) ? 2 : 0) |
                       ((dG > 0.5f) ? 4 : 0) | ((dI > 0.5f) ? 8 : 0);

        // Resolve each face-segment pair from MSTable into a triangle with PBack.
        const int8* Row = MSTable[MSCase];
        for (int32 s = 0; s < 4 && Row[s] != -1; s += 2)
        {
            // Get the two edge crossing positions for this segment.
            FVector3f FaceEdgeMid[4] = { PB, PF, PH, PD };
            FVector3f EP0 = FaceEdgeMid[Row[s]];
            FVector3f EP1 = FaceEdgeMid[Row[s + 1]];

            // Add face and backing vertices (no key-based sharing — transition
            // face vertices don't need to share with regular cells since the
            // boundary strip was skipped above).
            int32 VI0  = AddVertex(EP0);
            int32 VI1  = AddVertex(EP1);
            int32 VIBk = AddVertex(Vbk);
            EmitTriangle(VI0, VI1, VIBk);
        }
    };

    // +X face
    if (TransitionFaces & NTF_PosX)
    {
        int32 FVx = (NumCellX - 1) * Stride;
        for (int32 Cz = 0; Cz < NumCellZ; ++Cz)
        for (int32 Cy = 0; Cy < NumCellY; ++Cy)
        {
            int32 Vy0 = Cy * Stride, Vy1 = Vy0 + Stride;
            int32 Vz0 = Cz * Stride, Vz1 = Vz0 + Stride;
            EmitTransitionFaceCell(
                Origin + FVector3f(FVx, Vy0, Vz0) * Vs,
                Origin + FVector3f(FVx, Vy1, Vz0) * Vs,
                Origin + FVector3f(FVx, Vy0, Vz1) * Vs,
                Origin + FVector3f(FVx, Vy1, Vz1) * Vs,
                Origin + FVector3f(FVx - Stride, (Vy0 + Vy1) / 2, (Vz0 + Vz1) / 2) * Vs,
                GetDensity(FVx, Vy0, Vz0), GetDensity(FVx, Vy1, Vz0),
                GetDensity(FVx, Vy0, Vz1), GetDensity(FVx, Vy1, Vz1));
        }
    }

    // -X face
    if (TransitionFaces & NTF_NegX)
    {
        for (int32 Cz = 0; Cz < NumCellZ; ++Cz)
        for (int32 Cy = 0; Cy < NumCellY; ++Cy)
        {
            int32 Vy0 = Cy * Stride, Vy1 = Vy0 + Stride;
            int32 Vz0 = Cz * Stride, Vz1 = Vz0 + Stride;
            EmitTransitionFaceCell(
                Origin + FVector3f(0, Vy0, Vz0) * Vs,
                Origin + FVector3f(0, Vy1, Vz0) * Vs,
                Origin + FVector3f(0, Vy0, Vz1) * Vs,
                Origin + FVector3f(0, Vy1, Vz1) * Vs,
                Origin + FVector3f(Stride, (Vy0 + Vy1) / 2, (Vz0 + Vz1) / 2) * Vs,
                GetDensity(0, Vy0, Vz0), GetDensity(0, Vy1, Vz0),
                GetDensity(0, Vy0, Vz1), GetDensity(0, Vy1, Vz1));
        }
    }

    // +Y face
    if (TransitionFaces & NTF_PosY)
    {
        int32 FVy = (NumCellY - 1) * Stride;
        for (int32 Cz = 0; Cz < NumCellZ; ++Cz)
        for (int32 Cx = 0; Cx < NumCellX; ++Cx)
        {
            int32 Vx0 = Cx * Stride, Vx1 = Vx0 + Stride;
            int32 Vz0 = Cz * Stride, Vz1 = Vz0 + Stride;
            EmitTransitionFaceCell(
                Origin + FVector3f(Vx0, FVy, Vz0) * Vs,
                Origin + FVector3f(Vx1, FVy, Vz0) * Vs,
                Origin + FVector3f(Vx0, FVy, Vz1) * Vs,
                Origin + FVector3f(Vx1, FVy, Vz1) * Vs,
                Origin + FVector3f((Vx0 + Vx1) / 2, FVy - Stride, (Vz0 + Vz1) / 2) * Vs,
                GetDensity(Vx0, FVy, Vz0), GetDensity(Vx1, FVy, Vz0),
                GetDensity(Vx0, FVy, Vz1), GetDensity(Vx1, FVy, Vz1));
        }
    }

    // -Y face
    if (TransitionFaces & NTF_NegY)
    {
        for (int32 Cz = 0; Cz < NumCellZ; ++Cz)
        for (int32 Cx = 0; Cx < NumCellX; ++Cx)
        {
            int32 Vx0 = Cx * Stride, Vx1 = Vx0 + Stride;
            int32 Vz0 = Cz * Stride, Vz1 = Vz0 + Stride;
            EmitTransitionFaceCell(
                Origin + FVector3f(Vx0, 0, Vz0) * Vs,
                Origin + FVector3f(Vx1, 0, Vz0) * Vs,
                Origin + FVector3f(Vx0, 0, Vz1) * Vs,
                Origin + FVector3f(Vx1, 0, Vz1) * Vs,
                Origin + FVector3f((Vx0 + Vx1) / 2, Stride, (Vz0 + Vz1) / 2) * Vs,
                GetDensity(Vx0, 0, Vz0), GetDensity(Vx1, 0, Vz0),
                GetDensity(Vx0, 0, Vz1), GetDensity(Vx1, 0, Vz1));
        }
    }

    // +Z face
    if (TransitionFaces & NTF_PosZ)
    {
        int32 FVz = (NumCellZ - 1) * Stride;
        for (int32 Cy = 0; Cy < NumCellY; ++Cy)
        for (int32 Cx = 0; Cx < NumCellX; ++Cx)
        {
            int32 Vx0 = Cx * Stride, Vx1 = Vx0 + Stride;
            int32 Vy0 = Cy * Stride, Vy1 = Vy0 + Stride;
            EmitTransitionFaceCell(
                Origin + FVector3f(Vx0, Vy0, FVz) * Vs,
                Origin + FVector3f(Vx1, Vy0, FVz) * Vs,
                Origin + FVector3f(Vx0, Vy1, FVz) * Vs,
                Origin + FVector3f(Vx1, Vy1, FVz) * Vs,
                Origin + FVector3f((Vx0 + Vx1) / 2, (Vy0 + Vy1) / 2, FVz - Stride) * Vs,
                GetDensity(Vx0, Vy0, FVz), GetDensity(Vx1, Vy0, FVz),
                GetDensity(Vx0, Vy1, FVz), GetDensity(Vx1, Vy1, FVz));
        }
    }

    // -Z face
    if (TransitionFaces & NTF_NegZ)
    {
        for (int32 Cy = 0; Cy < NumCellY; ++Cy)
        for (int32 Cx = 0; Cx < NumCellX; ++Cx)
        {
            int32 Vx0 = Cx * Stride, Vx1 = Vx0 + Stride;
            int32 Vy0 = Cy * Stride, Vy1 = Vy0 + Stride;
            EmitTransitionFaceCell(
                Origin + FVector3f(Vx0, Vy0, 0) * Vs,
                Origin + FVector3f(Vx1, Vy0, 0) * Vs,
                Origin + FVector3f(Vx0, Vy1, 0) * Vs,
                Origin + FVector3f(Vx1, Vy1, 0) * Vs,
                Origin + FVector3f((Vx0 + Vx1) / 2, (Vy0 + Vy1) / 2, Stride) * Vs,
                GetDensity(Vx0, Vy0, 0), GetDensity(Vx1, Vy0, 0),
                GetDensity(Vx0, Vy1, 0), GetDensity(Vx1, Vy1, 0));
        }
    }

    // ── normalise accumulated normals ─────────────────────────────────────────
    for (int32 i = 0; i < Out.Vertices.Num(); ++i)
    {
        FVector3f N = NormalAccum[i].GetSafeNormal();
        FVector3f T = FMath::Abs(N.Z) < 0.9f ? FVector3f(0, 0, 1) ^ N : FVector3f(1, 0, 0) ^ N;
        T = T.GetSafeNormal();
        FVector3f B = N ^ T;
        Out.Vertices[i].SetTangents(T, B, N);
    }

    return Out;
}

// ─────────────────────────────────────────────────────────────────────────────

UNamecVoxelChunkComponent::UNamecVoxelChunkComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false; // enabled in InitLOD
    LocalBounds = FBoxSphereBounds(FVector::ZeroVector, FVector(50, 50, 50), 86.6f);
}

void UNamecVoxelChunkComponent::SetVoxelData(const FNamecVoxelData& InData)
{
    VoxelData = InData;

    const float VsUU = InData.VoxelSizeMetres * 100.f;
    const FVector Ext(InData.SizeX * VsUU * 0.5f,
                      InData.SizeY * VsUU * 0.5f,
                      InData.SizeZ * VsUU * 0.5f);
    const FVector Centre = FVector(InData.WorldOriginMetres * 100.f) + Ext;
    LocalBounds = FBoxSphereBounds(Centre, Ext, Ext.Size());

    MarkRenderStateDirty();
}

void UNamecVoxelChunkComponent::InitLOD(FIntPoint InChunkCoord)
{
    ChunkCoord    = InChunkCoord;
    bLODEnabled   = true;
    SetComponentTickEnabled(true);
}

void UNamecVoxelChunkComponent::SetTransitionFaces(uint8 InFaces)
{
    if (CurrentTransitionFaces == InFaces) return;
    CurrentTransitionFaces = InFaces;
    if (!bHasPendingMesh)
        RequestReMesh(CurrentLOD, CurrentTransitionFaces);
}

void UNamecVoxelChunkComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    PollPendingMesh();

    if (!bLODEnabled || bHasPendingMesh) return;

    int32 NewLOD = ComputeLODFromCamera();
    if (NewLOD != CurrentLOD)
        RequestReMesh(NewLOD, CurrentTransitionFaces);
}

void UNamecVoxelChunkComponent::RequestReMesh(int32 NewLOD, uint8 NewTransitionFaces)
{
    if (VoxelData.SizeX == 0) return;

    bHasPendingMesh       = true;
    PendingLOD            = NewLOD;
    PendingTransitionFaces = NewTransitionFaces;

    // Capture a copy for the worker thread; VoxelData may be large but is owned here.
    FNamecVoxelData SnapData = VoxelData;
    PendingMesh = Async(EAsyncExecution::ThreadPool,
        [SnapData, NewLOD, NewTransitionFaces]() mutable
        {
            return BuildMesh(SnapData, NewLOD, NewTransitionFaces);
        });
}

void UNamecVoxelChunkComponent::PollPendingMesh()
{
    if (!bHasPendingMesh || !PendingMesh.IsReady()) return;

    FNamecVoxelMeshData Result = PendingMesh.Get();
    bHasPendingMesh = false;
    CurrentLOD             = PendingLOD;
    CurrentTransitionFaces = PendingTransitionFaces;

    if (!Result.IsEmpty())
    {
        PhysicsMesh = MoveTemp(Result);
        MarkRenderStateDirty();

        // Cook collision on the game thread; chunk meshes are small so this is fast.
        UBodySetup* BS = GetBodySetup();
        BS->InvalidatePhysicsData();
        BS->CreatePhysicsMeshes();
        RecreatePhysicsState();
    }
}

int32 UNamecVoxelChunkComponent::ComputeLODFromCamera() const
{
    float D0 = 4000.f, D1 = 10000.f, D2 = 20000.f; // defaults in UU (cm)

    if (UWorld* W = GetWorld())
    {
        if (UGameInstance* GI = W->GetGameInstance())
        {
            if (UNamecScalabilitySubsystem* SS =
                    GI->GetSubsystem<UNamecScalabilitySubsystem>())
            {
                float M0, M1, M2;
                SS->GetActiveLODDistances(M0, M1, M2);
                D0 = M0 * 100.f;
                D1 = M1 * 100.f;
                D2 = M2 * 100.f;
            }
        }
    }

    FVector Centre = LocalBounds.Origin;
    float MinDist  = FLT_MAX;
    if (UWorld* W = GetWorld())
    {
        for (FConstPlayerControllerIterator It = W->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
            {
                FVector Loc; FRotator Rot;
                PC->GetPlayerViewPoint(Loc, Rot);
                MinDist = FMath::Min(MinDist, FVector::Dist(Loc, Centre));
            }
        }
    }

    if (MinDist <= D0) return 0;
    if (MinDist <= D1) return 1;
    if (MinDist <= D2) return 2;
    return 3;
}

FPrimitiveSceneProxy* UNamecVoxelChunkComponent::CreateSceneProxy()
{
    if (VoxelData.SizeX == 0) return nullptr;
    // If an async mesh just completed, the result is already in CurrentLOD/TransitionFaces.
    // If no async task is pending, build synchronously so the first frame is never blank.
    FNamecVoxelMeshData Mesh;
    if (bHasPendingMesh && PendingMesh.IsReady())
    {
        Mesh = PendingMesh.Get();
        bHasPendingMesh = false;
        CurrentLOD = PendingLOD;
        CurrentTransitionFaces = PendingTransitionFaces;
    }
    else
    {
        Mesh = BuildMesh(VoxelData, CurrentLOD, CurrentTransitionFaces);
    }
    if (Mesh.IsEmpty()) return nullptr;
    return new FNamecVoxelSceneProxy(this, MoveTemp(Mesh));
}

FBoxSphereBounds UNamecVoxelChunkComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    return LocalBounds.TransformBy(LocalToWorld);
}

UMaterialInterface* UNamecVoxelChunkComponent::GetMaterial(int32 ElementIndex) const
{
    return ElementIndex == 0 ? Material.Get() : nullptr;
}

void UNamecVoxelChunkComponent::SetMaterial(int32 ElementIndex, UMaterialInterface* InMaterial)
{
    if (ElementIndex == 0)
    {
        Material = InMaterial;
        MarkRenderStateDirty();
    }
}

UBodySetup* UNamecVoxelChunkComponent::GetBodySetup()
{
    if (!BodySetup)
    {
        BodySetup = NewObject<UBodySetup>(this, TEXT("ChunkBodySetup"),
                                          RF_Transient | RF_DuplicateTransient);
        BodySetup->bMeshCollideAll = true;
        BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
    }
    return BodySetup;
}

bool UNamecVoxelChunkComponent::GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData, bool)
{
    if (!CollisionData || PhysicsMesh.Indices.IsEmpty()) return false;

    CollisionData->Vertices.Reserve(PhysicsMesh.Vertices.Num());
    for (const FDynamicMeshVertex& V : PhysicsMesh.Vertices)
        CollisionData->Vertices.Add(V.Position);

    const int32 NumTri = PhysicsMesh.Indices.Num() / 3;
    CollisionData->Indices.Reserve(NumTri);
    for (int32 i = 0; i < NumTri; ++i)
    {
        FTriIndices Tri;
        Tri.v0 = PhysicsMesh.Indices[i * 3 + 0];
        Tri.v1 = PhysicsMesh.Indices[i * 3 + 1];
        Tri.v2 = PhysicsMesh.Indices[i * 3 + 2];
        CollisionData->Indices.Add(Tri);
    }
    CollisionData->bDeformableMesh = true;
    CollisionData->bFastCook     = true;
    return true;
}
