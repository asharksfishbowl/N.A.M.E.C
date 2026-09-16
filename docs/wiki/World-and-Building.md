# World and Building

This page covers the one authored map and how it is patched, the 8 regions and their boss gates, day/night and weather, digging, filling and mining, dig depth, towns and camps, trees, forage and loose pickups, hazards, and building.

← [Home](Home.md)

## The world

- One large, finite, hand-authored map of smooth (never blocky) voxel terrain. Every world is a play-through of the same map. Terrain can be dug, filled and reshaped anywhere outside town protected areas, down to the dig depth limit.
- Surrounded by an impassable ocean edge. There is no sailing.
- A bedrock layer at a fixed depth cannot be dug through.
- Caves exist underground in every region and hold that region's higher-tier ore. They are carved when the map is baked, from the map's authoring seed and per-region cave settings, and the author can add or reshape them by hand. There are no handcrafted dungeon levels. Every cave connects to the surface, so you can walk in without digging too deep.
- Ruins, kingdom capitals and towns, Bandit and Beastmen camps, boss arenas and loot chests are placed by hand in the editor. A validator refuses to save the map if a rule is broken (one capital and two towns per kingdom in its home region, no towns in Snowy Mountains or Volcanic, no overlapping protected areas, one boss arena per region).
- Trees, forage, loose pickups and rocks are scattered at runtime from the map's authoring seed, so every machine sees them in the same places, and never inside towns, camps, arenas or reserved regions.

## How the map is made

The authored inputs are a heightfield, region and material masks, and a text log of sculpt strokes, all versioned in git. A bake tool on a developer PC turns them into the map asset the game ships. Baking the same inputs twice gives a byte-identical asset. See `specs/authored-map/authored-map.md`.

## Creating a world

| Setting | Options |
|---------|---------|
| World name | Free text |
| Friendly fire | Off (default) or on |
| LAN hosting | On (default) or off |
| Password | Optional |
| Raids | On (default) or off. See [Raids](Raids.md). |

There is no seed and no size choice. Voxel resolution is 25 cm (starting value, tunable) and is fixed per world at creation.

## Map updates and your world

- A world save remembers which map and map revision it was created on. When a patch changes the map, your world still loads and every dig, fill and building you made is kept.
- Where the ground under one of your edits changed in the patch, a "terrain here was updated" marker appears. Interact with it to dismiss it. Markers survive save and load until dismissed.
- A save from a different map, or from a newer map revision than your build, is refused with a message and left untouched.
- Reserved regions for future content are fenced off: you cannot walk in, dig in, or build in them, and nothing spawns there. A patch opens them.
- In LAN play, everyone must be on the same map build. A client with a different map is refused with "Host is running a different map build".

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

- You can dig or mine a spot only if it is at most **6 m** (starting value, tunable) below the nearest open air directly above it, as the map was originally baked (your own edits are ignored).
- In practice: you can dig 6 m below the surface, and 6 m below a cave floor inside a cave.
- Only the voxels past the limit are blocked. The rest of the dig still happens, and "Too deep" shows in your viewport.
- Filling a hole and digging again doesn't change the limit, because it's measured on the original terrain.
- Anything you fill above the original ground can always be dug out.
- Every ore voxel in the baked map is within the limit, so all ore can be reached. The bake tool refuses a map where one is not.
- The bedrock layer still can't be dug through.

### Terrain rules

- Liquids never flow. Digging next to a lake or lava pool leaves a dry hole.
- Digging under a building piece or a tree leaves it in place.
- Digging out ground under another player makes them fall and take fall damage (see [Combat and Loot](Combat-and-Loot.md)).
- Edits are shared with every player and saved with the world.
- Terrain is built by a custom voxel mesher and lit by the rendering tier's GI method (Lumen hardware ray tracing in the High tier). See [Engine and Rendering](Dev-Engine-and-Rendering.md).

## Towns and camps

- **Towns:** each race kingdom has a capital and 2 towns in its home region. Inside a town's protected radius (tunable, covering every depth) you can't dig, fill, mine or build, and enemies don't spawn. Town buildings can't be damaged or deconstructed.
- **Camps:** Bandit and Beastmen camps are placed by hand across the regions. Terrain around camps can be dug normally, but building is blocked inside a camp's radius.

See [Factions and Kingdoms](Factions-and-Kingdoms.md).

## Trees

- Each tree has health and a required axe tier.
- Axe damage scales with axe tier, STR modifier and Woodcutting skill.
- At 0 health the tree falls with physics, away from the player who landed the last hit. A falling log damages players and enemies it lands on, even with friendly fire off.
- Chop the fallen log into Wood pickups. A stump is left behind.
- **Stumps:** hit a stump with a shovel (in either Dig or Fill mode) to remove it. It drops Wood (amount per tree type, starting value, tunable) and gives Woodcutting XP. Removing a stump isn't digging: no terrain changes, and it doesn't stop the tree regrowing. A removed stump stays removed after saving.
- A harvested tree regrows after 3 in-game days (starting value, tunable) where its stump stands, unless someone placed a building piece or dug terrain within 3 m of it. A stump still there at that point is replaced by the new tree.

## Forage and loose pickups

- **Forage nodes:** berries, herbs, mushrooms and fiber plants. Harvest by interacting. They regrow on a timer. Fiber plants give Fiber.
- **Loose Stick and Loose Stone:** on the ground in every region. Pick up by hand with interact, no tool needed, for one Stick or one Stone. They respawn at the same spot after a timer.

Loose pickups and fiber are how a new character, who has only its starting clothes, starts crafting. See [Crafting and Jobs](Crafting-and-Jobs.md).

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
- **Training dummies** are the one piece that reacts to your hits: every player hit plays its hit reaction but never lowers its health and gives no skill XP or class XP. Boss, enemy area, raider and falling-log damage still hurt it like any other piece.
- **Doors:** a newly placed door starts closed. Interact with a door to open or close it. Every player sees the same state, and it's saved with the world. Enemies, raiders and NPCs can't open doors: a closed door blocks their path, so raiders have to destroy it. A closed door counts as a wall for shelter, an open one doesn't (see [Survival](Survival.md)).
- A destroyed piece drops 50% of its materials (starting value, tunable).
- A Hammer can deconstruct **any** piece, placed by any player, for a 100% refund, using the hold in placement mode.
- The materials refunded or dropped are those of the recipe that made the piece's item (so a Workbench made from Wood gives back Wood). A piece nobody crafted uses the first recipe for that item, and a piece with no recipe at all gives back nothing. See [Crafting and Jobs](Crafting-and-Jobs.md).
- Destroying or deconstructing a storage container drops its contents on the ground for anyone to take.
- Building is blocked inside a boss arena, within 20 m of the world spawn point, inside a town's protected radius, and inside a Bandit or Beastmen camp's radius.

## Source spec

- [Voxel World](../../specs/voxel-world/voxel-world.md)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (towns, camps, raids)
- [Game Foundation](../../specs/game-foundation/game-foundation.md) (controls)
