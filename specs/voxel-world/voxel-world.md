# Spec: Voxel World

## Overview
The world is a large, finite, seeded map rendered as smooth (non-blocky) voxel terrain, so it looks like natural fantasy landscape while remaining fully diggable. It is divided into eight climate regions, filled with trees, ore, and resources players can gather, and supports Valheim-style snap building. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- A beautiful, natural-looking fantasy world. Terrain never looks like cubes.
- Every piece of terrain can be dug, filled, and reshaped.
- Trees can be chopped down and ore mined, feeding crafting.
- Players build bases from snapping building pieces.
- Region difficulty and boss gates create a clear progression path.

## Non-Goals
- Infinite world generation.
- Minecraft-style blocky cubes.
- Liquid simulation (water and lava do not flow when terrain is dug; see Edge Cases).
- Farming and animal husbandry.
- Underground dungeons as handcrafted levels. Caves are procedurally generated only.

## Requirements

### Voxel Technology
1. Terrain is stored as a signed-density voxel field with a material ID and a player-placed flag per voxel, rendered as a smooth mesh (marching cubes, dual contouring, or equivalent surface extraction).
2. The Researcher evaluates **Voxel Plugin** (commercial Unreal plugin) against a **custom implementation** and records the decision with rationale in the roadmap before any world code is written. Decision criteria: split-screen performance with 4 viewports, network replication of edits, save size, license cost, and console portability.
3. Voxel resolution is 25 cm (tuning value, fixed per world at creation). The resolution is stored in `UNamecWorldSave` when the world is created, and generation and edit-delta replay for that world read it from the save, never from the tuning table.
4. The world streams in chunks. Chunks near any player (local or remote) are meshed and have collision. The load radius per player comes from `DT_World_Streaming`.

### World Generation
5. World creation takes: seed (integer, random by default), size (Small 4×4 km, Medium 8×8 km (default), Large 12×12 km), world name, and world settings (friendly fire off/on — see combat-loot spec; LAN hosting on/off, on by default, and an optional password — see multiplayer spec).
6. Given the same seed, size, and voxel resolution, generation is fully deterministic: identical terrain, regions, resource placement, ruins, loot chest positions, and boss arena locations on every machine.
7. The world is surrounded by an impassable ocean edge. Sailing is out of scope. Players cannot pass the world boundary. Water is a static liquid volume placed by generation and flagged Fresh (lakes, rivers), Salt (the ocean), or Poison (Swamp and Jungle hazard water; Requirement 31).
8. Eight climate regions exist, each a row in `DT_World_Climates` with base temperature, comfort range (see survival spec), weather types, vegetation, ore types, and item-level band (used for loot chests; see combat-loot spec). Enemies and bosses map to regions through the region column of `DT_Combat_Enemies` (see combat-loot spec):
   | Order | Region | Pressure | Boss gate |
   |-------|--------|----------|-----------|
   | 1 | Temperate Forest & Meadows | none (starting region) | Boss 1 → Temperate boss material (Tier-2) |
   | 2 | Swamp | poison, wet | Boss 2 → Swamp boss material (Tier-3) |
   | 3 | Savanna | heat, open terrain | Boss 3 → Savanna boss material (Tier-3) |
   | 4 | Desert | severe heat, thirst | Boss 4 → Desert boss material (Tier-4) |
   | 5 | Jungle | disease/poison, dense | Boss 5 → Jungle boss material (Tier-4) |
   | 6 | Tundra | cold | Boss 6 → Tundra boss material (Tier-5) |
   | 7 | Snowy Mountains | severe cold, altitude | Boss 7 → Snowy Mountains boss material (Tier-5) |
   | 8 | Volcanic | extreme heat, lava | Final boss → gates nothing (guaranteed Legendary item and trophy; re-summonable endgame content) |

   Each of the 7 boss materials is a distinct item. Two materials at the same tier are different items, and each recipe names the specific boss material it requires.
9. Region placement: the Temperate region is at the world center, which holds the world spawn point (characters with no bed in this world spawn there; see `specs/multiplayer/multiplayer.md` Requirement 16). Later regions sit at increasing distance from the center, so difficulty rises with distance.
10. Caves generate underground in every region and contain higher-tier ore for that region. Generation also places ruins (generated surface structure placements defined in `DT_World_Structures`) and places loot chests deterministically in caves and ruins. Loot chests do not refill (see combat-loot spec).
11. A day/night cycle runs at 30 real-time minutes per in-game day (tuning value). Weather (clear, rain, storm, snow, sandstorm, ash fall) is chosen per region from its climate row. Tree regrow, forage regrow, and loose pickup respawn (Requirement 30) timers are stored and measured in in-game time, so time skipped by sleeping (see survival spec Requirement 19) counts toward regrowth.

### Digging & Terrain Manipulation
12. A shovel removes (digs) or adds (fills) terrain in a sphere of radius 0.5 m (tuning value, scaled by tool tier). A shovel digs only soft materials (soil, sand, gravel, snow, clay) as flagged in `DT_World_VoxelMaterials`. A **fill unit** is the volume of a sphere with the base radius 0.5 m (tuning value), independent of tool tier. Filling consumes one Soil, Sand, Gravel, or Stone item per fill unit of volume added, rounded up, places that material, and flags the placed voxels player-placed. A voxel counts as added when its density crosses the surface threshold from empty to solid, and as removed when it crosses from solid to empty.
13. A pickaxe removes stone and ore voxels in a sphere of radius 0.5 m (tuning value, scaled by tool tier). Each voxel material has a hardness, and a pickaxe tier below the material's required tier cannot damage the material (e.g., a Tier-1 pickaxe cannot mine Tier-3 ore).
14. Removing voxels yields items per `DT_World_VoxelMaterials`, which defines items per fill unit of removed volume for each material (e.g., Stone voxels → Stone, Iron Ore voxels → Iron Ore). Yield scales with Mining skill. Fractional yield accumulates per player per material, and one item is granted each time the accumulated amount reaches 1. Voxels flagged player-placed yield exactly one item per fill unit removed, with no Mining skill scaling. Because filling rounds item cost up and removal grants only whole items, filling and re-digging is never net positive.
15. Terrain edits are server-authoritative, replicated to all clients, and stored in `UNamecWorldSave` as per-chunk edit deltas over the generated base. Edit deltas include each changed voxel's player-placed flag.
16. Terrain cannot be dug through the world floor (bedrock layer at a fixed depth defined per world size).

### Trees & Gathering
17. Trees are actors placed deterministically by generation, not voxels. Each tree has health and a required axe tier.
18. Hitting a tree with an axe deals damage scaled by axe tier, STR modifier, and Woodcutting skill. At 0 health the tree falls with physics in the direction away from the player who landed the final hit.
19. A fallen tree log damages players and enemies it lands on, per `DT_World_Trees`.
20. A fallen log is chopped further into Wood items, dropped as world pickups. A stump remains, which can be dug out for extra wood.
21. Harvested trees regrow after 3 in-game days (tuning value), unless a player has placed a building piece or dug terrain within 3 m of the tree's origin.
22. Foraging nodes (berries, herbs, mushrooms, fiber plants) are placed by generation, harvested by interaction, and regrow on a timer per `DT_World_Forage`. Fiber plant nodes yield Fiber.
23. Ore generation is embedded in terrain voxels and never regrows.

### Building
24. Building pieces (walls, floors, roofs, stairs, doors, windows, beams, fences) come from crafting (Carpenter/Mason recipes) and are placed from a build menu while holding a Hammer. Crafting stations, station upgrade attachments, storage containers, beds, training dummies, and other furniture are also building pieces: each is an item placed from the Hammer build menu, has health and a material tier (Requirement 26), and is deconstructed with a Hammer with a material refund (Requirement 27). Deconstructing or destroying a storage container drops its contents per `specs/inventory/inventory.md` Edge Case 7, and deconstructing or destroying a crafting station or attachment follows `specs/crafting-jobs/crafting-jobs.md` Requirement 8 and Edge Case 5.
25. Pieces snap to other pieces' snap points. When no snap point is in range, a piece places freely on terrain.
26. Placed pieces are actors with health and a material tier (wood, stone, reinforced, etc.), not terrain voxels. There is no structural integrity: a piece never collapses from lack of support. Piece health is reduced only by boss attacks, enemy area-of-effect attacks, and falling tree logs. Player attacks, spells, and area-of-effect abilities never damage building pieces, regardless of the friendly-fire setting (`specs/combat-loot/combat-loot.md` Requirement 12). Normal enemies do not target buildings. At 0 health a piece is destroyed and drops 50% of its materials (tuning value).
27. A Hammer deconstructs any placed piece in the world, placed by any player, and refunds 100% of materials (tuning value).
28. Building is blocked within a boss arena's radius and within 20 m of the world spawn point (tuning values).

### Tuning Data
29. Every value marked "tuning value" in this spec that names no other table lives in `DT_World_Building`.

### Loose Pickups
30. Loose Stick and Loose Stone pickups are placed deterministically on the ground by generation in every region, at the per-region density in `DT_World_Forage`. A player collects one by hand with the interact input, with no tool, and receives one Stick or one Stone. A collected loose pickup respawns at its generated location after a respawn timer per `DT_World_Forage`, measured in in-game time like forage regrow timers (Requirement 11). Each loose pickup's collected state and respawn timer are stored in `UNamecWorldSave` alongside forage timers. Loose pickups are the tool-free source of Stick and Stone for the hand-crafting recipes in `specs/crafting-jobs/crafting-jobs.md` Requirement 16.

### Hazards
31. Swamp and Jungle regions contain Poison Water volumes (water volumes flagged Poison by generation; Requirement 7) and Poison Plant hazard actors placed deterministically by generation. Each second of contact with either applies Poison buildup at the rate in `DT_Combat_StatusEffects` (`specs/combat-loot/combat-loot.md` Requirement 10).
32. The Volcanic region contains Lava volumes (static liquid volumes placed by generation). Each second of contact with a Lava volume deals Fire damage (tuning value in `DT_World_Hazards`) and applies Burn buildup at the rate in `DT_Combat_StatusEffects`.
33. Deep Snow is a voxel material in `DT_World_VoxelMaterials`, generated on the surface of the Tundra and Snowy Mountains regions. A character walking on Deep Snow has movement speed reduced by 30% (tuning value in `DT_World_Hazards`).

## Data Flow
1. Host creates a world → `UNamecWorldGenerator` builds region layout, heightfield, caves, ruins, resource placement, loot chest positions, and boss arena positions from the seed → the world save is created with the seed, size, voxel resolution, and an empty edit list.
2. As players move, `UNamecVoxelWorld` streams chunks around each player, generating base voxel data from the seed and then applying stored edit deltas for that chunk.
3. A player swings a pickaxe → client requests `ServerApplyTerrainEdit(Center, Radius, ToolTier, Mode)` → server validates tool tier against voxel materials and range, applies the edit, records the delta, spawns item drops, awards Mining XP via progression, and replicates the edit to all clients.
4. Clients re-mesh affected chunks on receiving the replicated edit.
5. A player chops a tree → server applies damage → on 0 health the server spawns the falling log physics actor (replicated), marks the tree harvested with a regrow timestamp, and awards Woodcutting XP.
6. A player places a building piece → server validates snap position, collision, and blocked zones, consumes the item, spawns the piece actor, and records it in the world save.
7. On save, the host writes every `UNamecWorldSave` field listed in `specs/game-foundation/game-foundation.md` Requirement 6: world settings, time of day and weather, chunk edit deltas, placed pieces (including stations with tiers and containers with contents), shared world pickups, tree harvest states, forage timers, loose pickup collected states and respawn timers, bed respawn points per character GUID, loot chest opened state per character GUID, and boss defeat flags.

## Edge Cases
1. When a player digs beneath a placed building piece, the piece stays in place. Building pieces do not require terrain support.
2. When a player digs next to a lake or lava pool, the liquid does not flow into the hole. Liquids are static volumes defined at generation.
3. When a player digs the terrain out from under another player, that player falls normally under physics and takes fall damage (`specs/combat-loot/combat-loot.md` Requirement 40).
4. When a player digs out the terrain beneath a tree, the tree stays standing. Trees are anchored at their origin.
5. When a player tries to fill terrain in a space occupied by a player, enemy, or building piece, the fill is blocked for the occupied voxels only.
6. When two players edit the same voxel in the same frame, the server applies edits in receive order. Both clients converge to the server state via replication.
7. When a late-joining LAN client connects, the server sends the edit deltas for chunks within that client's streaming radius, and further chunks as they stream in.
8. When the edit delta list for a chunk exceeds a size threshold, the server re-bakes that chunk's delta into a compressed full-chunk snapshot in the save.

## Acceptance Criteria
- [ ] Two worlds created with the same seed and size generate identical terrain, regions, and tree positions.
- [ ] Terrain renders smooth with no visible cube faces.
- [ ] Digging with a shovel removes terrain, and filling with Soil adds it back.
- [ ] A Tier-1 pickaxe cannot mine Tier-3 ore, and a Tier-3 pickaxe can.
- [ ] Chopping a tree to 0 health makes it fall and awards Woodcutting XP, and chopping the fallen log drops Wood.
- [ ] A harvested tree regrows after 3 in-game days.
- [ ] Building pieces snap to each other and refund materials on deconstruction.
- [ ] With friendly fire on, a player's melee attack, spell, and area-of-effect ability each deal 0 damage to a building piece.
- [ ] A character with no tools collects a Loose Stick and a Loose Stone by hand in every region.
- [ ] Standing in Swamp Poison Water builds up Poison, touching Lava deals Fire damage and builds up Burn, and walking on Deep Snow reduces movement speed by 30%.
- [ ] Terrain edits and placed buildings persist across save → quit → load.
- [ ] Terrain edits made by one LAN player appear for all other players.
- [ ] The Temperate region is at the world center, and the Volcanic region is farthest from it.
- [ ] On the reference PC in `specs/game-foundation/game-foundation.md` Requirement 13, the temperate region holds 60 fps with 2 split-screen viewports and 30 fps with 4.

## Key Files
- `Source/NAMEC/World/NamecWorldGenerator.h` — new; seeded region layout, heightfield, caves, placement.
- `Source/NAMEC/World/NamecVoxelWorld.h` — new; voxel chunk storage, streaming, meshing (or wrapper around Voxel Plugin per Requirement 2).
- `Source/NAMEC/World/NamecTerrainEditComponent.h` — new; dig/fill/mine requests and validation.
- `Source/NAMEC/World/NamecClimateSubsystem.h` — new; region lookup, day/night, weather.
- `Source/NAMEC/World/NamecTreeActor.h` — new; tree health, felling, regrowth.
- `Source/NAMEC/World/NamecForageNode.h` — new; harvestable plants.
- `Source/NAMEC/World/Building/NamecBuildPiece.h` — new; placeable piece actor with snap points and health.
- `Source/NAMEC/World/Building/NamecBuildComponent.h` — new; placement preview, snap, deconstruction.
- `Content/Data/DT_World_Climates.uasset` — new; 8 region rows, including comfort range and item-level band.
- `Content/Data/DT_World_Structures.uasset` — new; ruin (generated surface structure) definitions and placement rules.
- `Content/Data/DT_World_VoxelMaterials.uasset` — new; hardness, tier, yields per fill unit.
- `Content/Data/DT_World_Trees.uasset` — new; tree types, health, yields.
- `Content/Data/DT_World_Forage.uasset` — new; forage types, regrow timers, per-region Loose Stick and Loose Stone density, and loose pickup respawn timers.
- `Content/Data/DT_World_Hazards.uasset` — new; Lava Fire damage per second, Deep Snow movement speed reduction, and other hazard tuning values.
- `Source/NAMEC/World/Hazards/NamecHazardVolume.h` — new; Poison Water and Lava contact effects (Poison buildup, Fire damage, Burn buildup).
- `Source/NAMEC/World/Hazards/NamecPoisonPlant.h` — new; Poison Plant hazard actor applying Poison buildup on contact.
- `Content/Data/DT_World_Streaming.uasset` — new; streaming radii.
- `Content/Data/DT_World_Building.uasset` — new; building blocked-zone radii, refund and destruction percentages, dig/fill radii, fill unit radius, and other world tuning values.
