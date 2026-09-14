# Save System

This page covers the two save types and their full contents, SaveVersion and migration, atomic writes, autosave cadence, and corrupt or newer-save handling.

← [Home](Home.md)

## Two independent save types

| Save | Class | One per | Stored on |
|------|-------|---------|-----------|
| Character save | `UNamecCharacterSave` (`Source/NAMEC/Save/NamecCharacterSave.h`) | Character | The local user's save directory on the machine that owns the character |
| World save | `UNamecWorldSave` (`Source/NAMEC/Save/NamecWorldSave.h`) | World | The host machine |

A character is portable: the same character save loads into any world. Save file paths go through the platform abstraction (`INamecPlatform`).

## `UNamecCharacterSave` contents

| Area | Contents | Owning spec |
|------|----------|-------------|
| Identity | Character GUID and name | game-foundation |
| Progression | Stats, spent and unspent stat points, classes, class levels, skill XP, boss first-kill flags, ability bar | character-progression |
| Jobs | Job levels and XP | crafting-jobs |
| Survival | Survival meter values (Breath is not saved) | survival |
| Inventory | Every item instance (definition ID, quantity, rarity, affixes, durability, item level), equipped slots, favorites, hotkeys, sort choice | inventory |
| Death | Dead-respawn flag | combat-loot |
| Timed effects | Remaining duration of each active timed effect: Weakened, meal buff, Wet, Salty | game-foundation |

### Dead-respawn flag

- Any character save written while the character is Downed or dead (autosave, disconnect, or exit, including host exit) sets the dead-respawn flag instead of saving the character at 0 health.
- On the next world entry the character spawns at its bed or the world spawn point with the death effects (full health, 50% Hunger and Thirst, BodyTemperature 37 °C, Fatigue 0, Weakened, −10% durability; starting values, tunable), and the flag clears.
- If a remote client's connection drops unexpectedly while Downed, no final save is sent, and the character keeps its last autosave state.

## `UNamecWorldSave` contents

| Area | Contents |
|------|----------|
| Identity and generation | World name, seed, size, voxel resolution |
| World settings | Friendly fire, LAN hosting, password |
| Time and weather | Current in-game time of day, each region's current weather |
| Terrain | Per-chunk edit deltas over the generated base, including each changed voxel's player-placed flag |
| Building | Placed building pieces, including crafting stations with bound attachments and tier, and storage containers with their contents |
| Pickups | Shared world pickups (per-player loot actors are not saved) |
| Gathering | Tree harvest states, forage timers, Loose Stick and Loose Stone collected states and respawn timers |
| Respawn | Bed respawn points per character GUID |
| Loot chests | Opened state per character GUID |
| Bosses | Boss defeat flags (`BossDefeated[RegionId]`) |

Not saved: character positions (every world entry spawns at the bed or world spawn point).

### Notes

- Voxel resolution is written at world creation. Generation and edit-delta replay read it from the save, never from the tuning table.
- When a chunk's edit delta list exceeds a size threshold, the server re-bakes it into a compressed full-chunk snapshot in the save.
- Defeating a boss triggers a world save.

## SaveVersion and migration

- Every save type carries an integer `SaveVersion`.
- Loading an **older** version runs a migration function (`Source/NAMEC/Save/NamecSaveMigrations.cpp`).
- Loading a **newer** version than the build supports is refused with a user-facing message.
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
| Corrupt save (fails deserialization) | Show "Save could not be loaded", keep the file on disk untouched, return to the menu. Never overwrite it. |
| Crash mid-save | Previous save remains valid (temp file + atomic rename). |
| Newer `SaveVersion` than the build | Refused with a message. Characters show "Requires newer version". |
| Insufficient disk space | Warning on the host HUD, retry on the next autosave. In-memory state is kept. |
| Mid-craft disconnect or quit | Current craft is cancelled and reserved materials return to the inventory before the character saves. |

## Source specs

- [Game Foundation](../../specs/game-foundation/game-foundation.md) (Requirements 6–8, Edge Cases 1–4)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (Edge Case 1)
- [Multiplayer](../../specs/multiplayer/multiplayer.md) (Requirements 16–17)
