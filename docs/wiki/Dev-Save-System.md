# Save System

This page covers the three save types and their full contents, SaveVersion and migration, atomic writes, autosave cadence, and corrupt or newer-save handling.

← [Home](Home.md)

## Three independent save types

| Save | Class | One per | Stored on |
|------|-------|---------|-----------|
| Character save | `UNamecCharacterSave` (`Source/NAMEC/Save/NamecCharacterSave.h`) | Character | The local user's save directory on the machine that owns the character |
| World save | `UNamecWorldSave` (`Source/NAMEC/Save/NamecWorldSave.h`) | World | The host machine |
| Settings save | `UNamecSettingsSave` (`Source/NAMEC/Save/NamecSettingsSave.h`) | Machine | The local user's save directory on that machine (never sent to other machines) |

A character is portable: the same character save loads into any world. Save file paths go through the platform abstraction (`INamecPlatform`).

## `UNamecCharacterSave` contents

| Area | Contents | Owning spec |
|------|----------|-------------|
| Identity | Character GUID and name | game-foundation |
| Race and appearance | Race, sex, and `FNamecAppearance` (race ID, sex, one value per appearance option) | character-creation |
| Progression | Stats, spent and unspent stat points, classes, class levels, skill XP, boss first-kill flags, ability bar | character-progression |
| Jobs | Job levels and XP | crafting-jobs |
| Survival | Survival meter values (Breath is not saved) | survival |
| Health and Mana | Current Health and Mana, restored on world entry after equipment and effects apply, clamped to current MaxHealth and MaxMana. Stamina is not saved and starts full. | game-foundation |
| Inventory | Every item instance (definition ID, quantity, rarity, affixes, durability (none for jewelry), item level, dye colors, consumable potency value, rune tier, waterskin remaining drinks, the recipe row ID of a crafted item, and a weapon's poison or oil coating with its status, buildup amount and remaining hits), equipped slots, favorites, hotkeys, sort choice | inventory |
| Shovel | Dig/Fill mode and selected fill material (default Dig and Soil), per character | voxel-world |
| Factions | Gold, reputation per faction, active quests (objective parameters, progress, tracked flag), completed questline steps per kingdom | factions-kingdoms |
| Death | Dead-respawn flag | combat-loot |
| Timed effects | Remaining duration of each active timed effect: Weakened, meal buff, Wet, Salty (racial passives and downsides are not saved; they are reapplied on spawn) | game-foundation |

Two-handing state is not saved: every character loads holding its Right Hand weapon one-handed (combat-loot).

### Race and appearance

- Confirming character creation writes a new `UNamecCharacterSave` with a new character GUID and the starting items (Plain Shirt and Plain Trousers) equipped.
- Ability bar in the save includes the racial ability's slot.
- An appearance change at a Mirror applies immediately and is written on the next character save.
- If a loaded save has an appearance option index outside the current range in `DT_Character_AppearanceOptions`, that option resets to the race default and the character still loads.
- If a save references a race ID missing from `DT_Character_Races`, the character shows greyed out with "Race data missing" and cannot be selected.

### Gold, reputation, quests and dyes

- A character save from before these existed migrates to the starting reputation values (+30 own kingdom, 0 other kingdoms, −100 Bandits and Beastmen), 0 gold and an empty quest log.
- Quests reference only kingdoms, town slots (capital, town 1, town 2), enemy rows, item definitions and faction/region pairs, so an active quest stays valid in every world. An Escort quest whose objective is not complete fails when the character leaves the world, and one still incomplete in a loaded character save fails on world entry (escort NPCs are not saved). A completed Escort quest stays saved until turn-in or abandon.
- If an item stores a dye color ID no longer in `DT_Crafting_DyeColors`, that zone shows its default color and the item still loads.
- If an item stores a recipe row ID no longer in `DT_Crafting_Recipes`, the item still loads and uses the first recipe row for its item definition, or none.
- Cancelling creation, or removing the local player mid-creation, writes no save.

### Dead-respawn flag

- Any character save written while the character is Downed or dead (autosave, disconnect, or exit, including host exit) sets the dead-respawn flag instead of saving the character at 0 health. The flag takes precedence over the saved Health and Mana: the character respawns with full Health and full Mana.
- On the next world entry the character spawns at its bed or the world spawn point with the death effects (full health, full Mana, 50% Hunger and Thirst, BodyTemperature 37 °C, Fatigue 0, Weakened, −10% durability; starting values, tunable), and the flag clears.
- If a remote client's connection drops unexpectedly while Downed, no final save is sent, and the character keeps its last autosave state.

## `UNamecWorldSave` contents

| Area | Contents |
|------|----------|
| Identity and map | World name, map id, map revision, map hash, per-edited-chunk base hashes, pending terrain markers, voxel resolution |
| World settings | Friendly fire, LAN hosting, password, Raids |
| Time and weather | Current in-game time of day, the world's total elapsed in-game time, each region's current weather |
| Terrain | Per-chunk edit deltas over the baked base, including each changed voxel's player-placed flag |
| Building | Placed building pieces, including each piece's recipe row ID (decides its deconstruct refund and destroyed drop), crafting stations with bound attachments and tier, storage containers with their contents, and doors with their open or closed state |
| Pickups | Shared world pickups, including dropped gold, per-player loot that has converted into shared pickups (with each one's remaining despawn time), and the picked-up items and gold of every live non-Veteran enemy, written as pickups where it stands. Unconverted per-player loot actors are not saved. |
| Veterans | One record per living Veteran: Veteran GUID, enemy row ID, faction, current level, XP, current health (saved without player-count scaling), equipped and carried items, gold, home position, name, title, kill counts, and camp and spawn point for a camp Veteran |
| Gathering | Tree harvest states (including whether the stump was removed), forage timers, Loose Stick and Loose Stone collected states and respawn timers |
| Respawn | Bed respawn points per character GUID |
| Loot chests | Opened state per character GUID |
| Bosses | Boss defeat flags (`BossDefeated[RegionId]`) |
| Towns | Town NPC respawn timers, Vendor stock quantities, Quest Board offers with each offer's accepted character GUID set |
| Camps | Bandit and Beastmen camp per-spawn-point dead states, cleared state, respawn timer |

Not saved: character positions (every world entry spawns at the bed or world spawn point); NPC health (living NPCs load at full health); escort NPCs; non-Veteran enemies; active raids. An autosave does not change an active raid, and when a world loads no raid is active and raiders from before the host exited are not restored. Pieces destroyed during a raid stay destroyed.

### Notes

- Voxel resolution is written at world creation, copied from the map asset. Edit-delta application reads it from the save, never from the tuning table.
- Loading a world on a newer map revision keeps every edit: deltas apply per chunk on stream, and chunks whose base changed get a pending terrain marker. A save from a different map id, or a newer revision than the build, is refused and left untouched (`specs/authored-map/authored-map.md` Requirement 25).
- When a chunk's edit delta list exceeds a size threshold, the server re-bakes it into a compressed full-chunk snapshot in the save.
- Defeating a boss triggers a world save.
- A killed Veteran is removed at once and never written again. A Veteran record whose enemy row no longer exists is skipped on load with a log entry and dropped from the next save, and a Veteran item whose definition no longer exists is skipped with a log entry.
- On load, a Veteran whose home is inside a base area or a town's protected radius is re-homed to the nearest valid point outside it. A Veteran that led a raid when the host exited is saved at its home.
- A world save during an enemy's pickup saves the item as still on the ground.

## `UNamecSettingsSave` contents

| Section | Contents |
|---------|----------|
| One per local player slot (1–4) | Input remaps, Night Eyes toggle, "Execution prompts" option, and every other per-local-player Setting |
| Machine-wide | Global settings: audio, and graphics (GI method, VSM quality, foliage density, view distance). The graphics settings are the ceiling for the 3–4 viewport Split tier (see [Engine and Rendering](Dev-Engine-and-Rendering.md)). |

- Per-local-player settings belong to the slot, not to a character or a controller: whoever joins as local player 2 uses slot 2's settings.
- The settings save is written whenever a Settings change is applied, using the same atomic write as the other saves.

## SaveVersion and migration

- Every save type (character, world and settings) carries an integer `SaveVersion`.
- Loading an **older** version runs a migration function (`Source/NAMEC/Save/NamecSaveMigrations.cpp`).
- Loading a **newer** version than the build supports is refused with a user-facing message. Exception: a newer settings save is left untouched, and defaults are used for the session without being written back.
- A character save newer than the build shows greyed out in the character list with "Requires newer version".
- When a joining player's character is older than the host build, the host runs the character migration before spawning. When it is newer, the join is refused.

## Atomic writes

Saves write to a temp file, then atomically rename over the old file. If the game crashes mid-save, the previous save stays valid.

## Autosave cadence

| Trigger | World save (host) | Character saves |
|---------|-------------------|-----------------|
| Autosave, every 5 minutes (starting value, tunable, in `DT_Core_Save`) | Yes | Yes, each machine writes its own characters |
| Host exit | Yes | Yes, clients save before returning to the menu |
| Disconnect | — | Yes |
| Exit | — | Yes |
| Local player leaves via pause menu | — | Yes |
| Boss defeat | Yes | — |

For remote players, the host sends the character payload on each autosave and a final send on clean disconnect or exit. An unexpected drop loses up to one autosave interval of progress. See [Networking](Dev-Networking.md).

## Failure handling

| Case | Behaviour |
|------|-----------|
| Corrupt save (fails deserialization) | Character and world saves: show "Save could not be loaded", keep the file on disk untouched, return to the menu. Never overwrite it. Settings save: rename it to `.bak` and use default settings. |
| Crash mid-save | Previous save remains valid (temp file + atomic rename). |
| Newer `SaveVersion` than the build | Character and world saves: refused with a message. Characters show "Requires newer version". Settings save: left untouched, and defaults are used for the session without being written back. |
| Insufficient disk space | Warning on the host HUD, retry on the next autosave. In-memory state is kept. |
| Mid-craft disconnect or quit | Current craft is cancelled and reserved materials return to the inventory before the character saves. |

## Source specs

- [Game Foundation](../../specs/game-foundation/game-foundation.md) (Requirements 6–8, Edge Cases 1–4)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (Requirements 45, 57 and 61, Edge Case 1)
- [Voxel World](../../specs/voxel-world/voxel-world.md) (Requirements 20, 24, 42 and 45)
- [Engine Tech](../../specs/engine-tech/engine-tech.md) (Requirement 7)
- [Inventory](../../specs/inventory/inventory.md) (Data Flow 4)
- [Multiplayer](../../specs/multiplayer/multiplayer.md) (Requirements 16–17)
- [Character Creation](../../specs/character-creation/character-creation.md) (Requirements 6, 13, 16, 24 and 28, Data Flow 3, Edge Cases 1, 2, 10 and 11)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (Requirements 11, 19, 25, 28, 34, 37, 40, 48 and 59, Edge Case 1)
- [Crafting Jobs](../../specs/crafting-jobs/crafting-jobs.md) (Requirements 11 and 24, Edge Cases 9 and 13)
- [Enemy AI](../../specs/enemy-ai/enemy-ai.md) (Requirements 20, 34, 46 and 53, Edge Cases 4, 6, 7, 8 and 18)
