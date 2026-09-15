# World and Building

This page covers world sizes and seeds, the 8 regions and their boss gates, day/night and weather, digging, filling and mining, dig depth, towns and camps, trees, forage and loose pickups, hazards, and building.

← [Home](Home.md)

## The world

- A large, finite, seeded map of smooth (never blocky) voxel terrain. Terrain can be dug, filled and reshaped anywhere outside town protected areas, down to the dig depth limit.
- Surrounded by an impassable ocean edge. There is no sailing.
- A bedrock layer at a fixed depth (per world size) cannot be dug through.
- Caves generate underground in every region and hold that region's higher-tier ore. Caves are procedural only. There are no handcrafted dungeons. Every cave connects to the surface, so you can walk in without digging too deep.
- Generation also places ruins (surface structures), kingdom capitals and towns, Bandit and Beastmen camps, and loot chests in caves, ruins and camps.

## Creating a world

| Setting | Options |
|---------|---------|
| World name | Free text |
| Seed | Integer, random by default |
| Size | Small 4×4 km, Medium 8×8 km (default), Large 12×12 km |
| Friendly fire | Off (default) or on |
| LAN hosting | On (default) or off |
| Password | Optional |
| Raids | On (default) or off. See [Raids](Raids.md). |

The same seed, size and voxel resolution always generate identical terrain, regions, resources, ruins, towns, camps, loot chest positions and boss arenas on every machine. Voxel resolution is 25 cm (starting value, tunable) and is fixed per world at creation.

## Regions

The Temperate region is at the world center, which holds the world spawn point. Later regions sit farther from the center, so difficulty rises with distance.

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

- Each of the 7 boss materials is a distinct item, even when two share a tier.
- Each region has its own base temperature, comfort range, weather types, vegetation, ore types, enemies and loot item-level band.

See [Combat and Loot](Combat-and-Loot.md) for bosses and [Survival](Survival.md) for temperature.

## Day, night and weather

- One in-game day lasts 30 real-time minutes (starting value, tunable).
- Weather is chosen per region from its climate: clear, rain, storm, snow, sandstorm, ash fall.
- Some enemies spawn only at night.
- Tree regrowth, forage regrowth, loose pickup respawns, NPC and camp respawns, Vendor restocks and Quest Board refreshes run on in-game time, so time skipped by sleeping counts.
- **Exception:** the raid roll clock and raid duration run on real time, and sleeping doesn't advance them. See [Raids](Raids.md).

## Digging, filling and mining

### Shovel

- Digs or fills terrain in a sphere of radius 0.5 m (starting value, tunable), bigger with higher tool tiers.
- The first shovel is the **Stone Shovel**, hand-crafted from Stick, Stone and Fiber (Carpenter level 1). It is Tier 0. See [Crafting and Jobs](Crafting-and-Jobs.md).
- **Controls:** RB (left mouse) swings. RT (middle mouse) toggles Dig/Fill mode. D-pad Left/Right (mouse wheel) cycles the fill material: Soil, Sand, Gravel, Stone. While a shovel is equipped, those D-pad buttons don't cycle hand hotkeys. See [Co-op and Controls](Co-op-and-Controls.md).
- **Defaults:** Dig mode with Soil selected. Your mode and selected material belong to your character, apply to any shovel you hold, and are saved with your character.
- Digs only soft materials: soil, sand, gravel, snow, clay.
- **Filling** uses one item of the selected fill material (Soil, Sand, Gravel or Stone) per fill unit (the volume of a 0.5 m sphere, regardless of tool tier), rounded up. It places that material and marks it player-placed.
- When you have none of the selected fill material left, filling stops, and "No <material>" (for example "No Sand") shows in your viewport.
- When you have some but fewer items than a fill needs, the fill is blocked: nothing is placed or used, and "Not enough <material>" shows in your viewport.
- Filling is blocked where a player, enemy or building piece is standing, for those voxels only.

### Pickaxe

- Removes stone and ore in a sphere of radius 0.5 m, bigger with higher tool tiers.
- Each material has a hardness and required tier. A lower-tier pickaxe cannot damage it (for example, a Tier-1 pickaxe cannot mine Tier-3 ore).

### Yields

- Removed terrain gives items per fill unit of volume (for example Stone voxels → Stone, Iron Ore voxels → Iron Ore). Mining skill raises the yield.
- Partial yields build up per player per material, and you get an item each time the total reaches 1.
- Player-placed voxels give exactly one item per fill unit, with no skill bonus and no Mining XP. Because filling rounds cost up and digging only gives whole items, fill-and-dig is never a net gain.
- Ore never regrows.

### Dig depth

- You can dig or mine a spot only if it is at most **6 m** (starting value, tunable) below the nearest open air directly above it, as the world was originally generated.
- In practice: you can dig 6 m below the surface, and 6 m below a cave floor inside a cave.
- Only the voxels past the limit are blocked. The rest of the dig still happens, and "Too deep" shows in your viewport.
- Filling a hole and digging again doesn't change the limit, because it's measured on the original terrain.
- Anything you fill above the original ground can always be dug out.
- Every generated ore voxel is within the limit, so all ore can be reached.
- The bedrock layer still can't be dug through.

### Terrain rules

- Liquids never flow. Digging next to a lake or lava pool leaves a dry hole.
- Digging under a building piece or a tree leaves it in place.
- Digging out ground under another player makes them fall and take fall damage (see [Combat and Loot](Combat-and-Loot.md)).
- Edits are shared with every player and saved with the world.
- Terrain is built by a custom voxel mesher and lit by the rendering tier's GI method (Lumen hardware ray tracing in the High tier). See [Engine and Rendering](Dev-Engine-and-Rendering.md).

## Towns and camps

- **Towns:** each race kingdom has a capital and 2 towns in its home region. Inside a town's protected radius (tunable, covering every depth) you can't dig, fill, mine or build, and enemies don't spawn. Town buildings can't be damaged or deconstructed.
- **Camps:** Bandit and Beastmen camps generate in every region. Terrain around camps can be dug normally, but building is blocked inside a camp's radius.

See [Factions and Kingdoms](Factions-and-Kingdoms.md).

## Trees

- Each tree has health and a required axe tier.
- Axe damage scales with axe tier, STR modifier and Woodcutting skill.
- At 0 health the tree falls with physics, away from the player who landed the last hit. A falling log damages players and enemies it lands on, even with friendly fire off.
- Chop the fallen log into Wood pickups. The stump can be dug out for extra wood.
- A harvested tree regrows after 3 in-game days (starting value, tunable), unless someone placed a building piece or dug terrain within 3 m of it.

## Forage and loose pickups

- **Forage nodes:** berries, herbs, mushrooms and fiber plants. Harvest by interacting. They regrow on a timer. Fiber plants give Fiber.
- **Loose Stick and Loose Stone:** on the ground in every region. Pick up by hand with interact, no tool needed, for one Stick or one Stone. They respawn at the same spot after a timer.

Loose pickups and fiber are how a character with nothing starts crafting. See [Crafting and Jobs](Crafting-and-Jobs.md).

## Hazards

| Hazard | Where | Effect |
|--------|-------|--------|
| Poison Water | Swamp, Jungle | Each second of contact builds up Poison |
| Poison Plant | Swamp, Jungle | Each second of contact builds up Poison |
| Lava | Volcanic | Each second of contact deals Fire damage and builds up Burn |
| Deep Snow | Tundra, Snowy Mountains (surface) | Movement speed −30% (starting value, tunable) |

## Building

- Hold a **Hammer** to open the build menu and place pieces: walls, floors, roofs, stairs, doors, windows, beams, fences.
- **Hammer controls:** RT (middle mouse) opens the build menu. Choosing a piece enters placement mode: RB (left mouse) places, D-pad Left/Right (mouse wheel) rotates, B (right mouse) cancels, and holding X (F) for 1 second (starting value, tunable) on a placed piece deconstructs it. These replace the normal bindings of those buttons only while placement mode is on.
- **After placing,** you stay in placement mode with the same piece while you still have that piece's item. Placing your last one takes you back to the build menu.
- **Deconstructing works only in placement mode.** Outside it, holding X (F) uses your selected consumable.
- Pieces come from Carpenter (wooden) and Mason (stone) recipes.
- Crafting stations, station upgrade attachments, storage containers, beds, training dummies, Mirrors and other furniture are also building pieces. A Mirror lets you change your appearance (see [Races and Character Creation](Races-and-Character-Creation.md)).
- Pieces snap to other pieces' snap points, or place freely on terrain when no snap point is in range.
- There is **no structural integrity**. A piece never collapses from lack of support.
- Each piece has health and a material tier (wood, stone, reinforced, etc.).
- Only boss attacks, enemy area attacks, raider attacks and falling logs damage pieces. Player attacks, spells and area abilities never do, whatever the friendly-fire setting. Only raiders target buildings. See [Raids](Raids.md).
- A destroyed piece drops 50% of its materials (starting value, tunable).
- A Hammer can deconstruct **any** piece, placed by any player, for a 100% refund, using the hold in placement mode.
- Destroying or deconstructing a storage container drops its contents on the ground for anyone to take.
- Building is blocked inside a boss arena, within 20 m of the world spawn point, inside a town's protected radius, and inside a Bandit or Beastmen camp's radius.

## Source spec

- [Voxel World](../../specs/voxel-world/voxel-world.md)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (towns, camps, raids)
- [Game Foundation](../../specs/game-foundation/game-foundation.md) (controls)
