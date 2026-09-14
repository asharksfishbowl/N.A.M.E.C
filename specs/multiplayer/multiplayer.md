# Spec: Multiplayer (Split-Screen & LAN)

## Overview
Up to 4 players share a world through any mix of local split-screen and LAN, for example 2 players on one PC plus 2 players on another PC over LAN. The host machine runs a listen server that owns all world and gameplay authority. Characters are portable: each player brings a character saved on their own machine. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Couch co-op works out of the box: plug in controllers, press Start, pick a character, play.
- LAN co-op works with no port forwarding, accounts, or internet connection.
- Split-screen and LAN combine in one session.
- A console port only needs a new platform session implementation.

## Non-Goals
- Internet or online play, NAT traversal, relay servers, or matchmaking.
- Dedicated servers.
- Host migration. When the host leaves, the session ends.
- More than 4 total players.
- Cross-platform play.

## Requirements

### Session Model
1. The session is a listen server on the host machine using Unreal's replication. All gameplay state (terrain edits, combat, loot rolls, crafting, progression XP) is server-authoritative.
2. The maximum player count is 4 total across all machines (tuning value, hard cap 4).
3. Each machine supports up to 4 local players, limited by the remaining session capacity.

### Split-Screen
4. When a controller that is not assigned to a local player presses Start during gameplay or in the lobby (the per-local-player character-select screen shown after a world is chosen and before the world loads; it is the only place local player 1 selects a character for play on the host), a join prompt appears. Confirming opens character select for that controller, and after selection the new local player spawns next to local player 1. When local player 1 is inside a sealed boss arena, the new local player spawns at their bed in this world, or at the world spawn point when no bed is set. On a client machine, the new local player registers with the host via `ServerRegisterLocalPlayers` with the same capacity, version, password, and duplicate-GUID checks as a join.
5. Viewport layouts: 1 player full screen; 2 players vertical split (side by side); 3 players one top half and two bottom quarters; 4 players quadrants.
6. Each local player has a fully independent HUD, menus, inventory screens, crafting UI, and camera mode (first/third person) within their own viewport.
7. Local player 1 may use keyboard and mouse or a controller. Local players 2–4 use controllers only.
8. A local player (other than player 1) can leave via the pause menu. Their character saves and their viewport is removed, and the layout re-flows. When local player 1 on a client machine leaves, every local player on that machine disconnects. When local player 1 on the host leaves, the session follows host exit (Requirement 21).
9. Graphics scalability auto-adjusts by local viewport count using presets in `DT_MP_SplitScreenScalability` (e.g., reduced view distance, shadow quality, and foliage density at 3–4 viewports).
10. Split-screen players on the same machine share a camera-independent world. Each viewport's streaming radius counts as a separate streaming source for voxel chunks (see voxel-world spec).

### LAN
11. The host enables LAN hosting in world settings (on by default). The session advertises on the local network via Unreal's LAN session discovery (`OnlineSubsystemNull` with `bIsLANMatch = true`).
12. The Join LAN Game screen lists discovered sessions with: world name, host name, current/max players, and game version.
13. A joining machine lists its local players (1–4) and each picks a character from that machine's saves before connecting.
14. Joining is refused with a clear message when: the session is full, the game version differs, or the joining machine has more local players than the remaining capacity.
15. An optional world password is set in world settings. When set, joining requires entering it.

### Portable Characters
16. Character data travels from the joining machine to the host on join. The host treats the received character data as that player's authoritative state for the session. Every world entry spawns the character at that character's bed in this world, or at the world spawn point when no bed is set or the bed has been destroyed (`specs/survival/survival.md` Requirement 18). A world entry is the host's local players entering on New World or Load World, a remote character joining, or a character rejoining after a disconnect or crash. The only exception to this bed or world spawn point entry is a local player added mid-session, who spawns next to local player 1 per Requirement 4. A character's last position in a world is not saved.
17. While in session, the host sends the player's character state back to the owning machine on each autosave, plus a final send on clean disconnect or exit, which writes its local `UNamecCharacterSave` on autosave, on disconnect, and on exit. When a remote client's connection drops unexpectedly (including a timeout, Requirement 22, or a crash), no final send arrives: the client keeps progress up to its last received payload, which is the last autosave, so up to 5 minutes of progress (the autosave interval in `DT_Core_Save`) is lost.
18. The same character (same character GUID) cannot be in a session twice.
19. Character inventory is part of the character and leaves with the player. World containers belong to the world.

### Disconnects
20. When a remote client disconnects, their pawn is removed from the world. On a clean disconnect, their character saves on their machine from the final send. On an unexpected drop, their character saves on their machine from the last received payload, which is the last autosave (Requirement 17).
21. When the host exits, all clients return to the main menu with "Host ended the session", and their characters save first.
22. When a remote client's connection times out (no packets for 20 seconds, tuning value), the host treats it as a disconnect.

### Tuning Data
23. Every value marked "tuning value" in this spec that names no other table lives in `DT_MP_Session`.

## Data Flow
1. Host starts or loads a world → `UNamecSessionSubsystem::HostSession(WorldSettings)` creates the LAN session and loads the map.
2. Remote machine opens Join LAN Game → `FindSessions` discovers the host → the player selects the session → local players pick characters → `JoinSession` connects.
3. On connection, the client sends `ServerRegisterLocalPlayers(Array<CharacterPayload>)` → host validates capacity, version, password, and duplicate character GUIDs → spawns pawns and initializes components from the payloads. A local player joining mid-session on a client machine sends the same RPC with one payload (Requirement 4).
4. During play, the server replicates world and gameplay state. Owner-only data (loot pickups, full inventory detail) replicates only to its owning connection. Per-player loot pickups also render only in the owning local player's viewport, because local players on one machine share a connection (see `specs/combat-loot/combat-loot.md` Requirement 28).
5. On autosave, the host sends each client `ClientSaveCharacter(CharacterPayload)` → the client writes `UNamecCharacterSave` locally.
6. On disconnect or exit, the final `ClientSaveCharacter` is sent before the connection closes. If the connection dropped unexpectedly, no final `ClientSaveCharacter` arrives, and the client saves its last received payload, which is the last autosave (up to 5 minutes of progress lost; Requirement 17).

## Edge Cases
1. When a controller disconnects (battery dies) for a local player, the game pauses that player's input, shows "Reconnect controller" in their viewport, and the character remains in the world. When the session has any remote (LAN) players, the world does not pause. When every player in the session is on the host machine, the world pauses until the controller reconnects or that local player is removed via the pause menu.
2. When any local player opens the pause menu, the world pauses only in a session with one player total. In every other session the world does not pause.
3. When a joining player's character has a `SaveVersion` older than the host build, the host runs the character migration before spawning. When it is newer, the join is refused.
4. When a player tries to join while a boss arena is sealed, the joining player spawns per Requirement 16 (bed, or world spawn point) and cannot enter the arena.
5. When two local players on the same machine select the same character, the second selection is blocked with "Character already in use".
6. When a remote player's machine crashes or their connection drops unexpectedly and they rejoin, the host treats them as a new join and uses the character save on their machine, which holds the last received payload from the last autosave (up to 5 minutes of progress lost; Requirement 17).

## Acceptance Criteria
- [ ] Four controllers on one PC can each join, pick a character, and play in a 4-way split screen.
- [ ] The 2-, 3-, and 4-player split layouts match Requirement 5.
- [ ] A second PC on the same LAN discovers the hosted session and joins without internet access.
- [ ] 2 local players on PC A + 2 local players on PC B play in one session. A fifth join attempt is refused with "Session full".
- [ ] A character from PC B joins PC A's world, gains XP and loot, disconnects, and the progress is saved on PC B.
- [ ] A client whose network cable is unplugged keeps the character state from the last autosave, and progress after that autosave is not saved.
- [ ] The same character cannot join a session twice.
- [ ] A game-version mismatch refuses the join with a clear message.
- [ ] When the host exits, clients return to the main menu with their characters saved.
- [ ] A disconnected controller shows the reconnect prompt only in that player's viewport.

## Key Files
- `Source/NAMEC/Multiplayer/NamecSessionSubsystem.h` — new; host/find/join LAN sessions, password, version check.
- `Source/NAMEC/Multiplayer/NamecLocalPlayerManager.h` — new; controller join prompt, local player add/remove, controller disconnect handling.
- `Source/NAMEC/Multiplayer/NamecCharacterPayload.h` — new; serializable character state struct used for join and save sync.
- `Source/NAMEC/Multiplayer/NamecSplitScreenLayout.h` — new; viewport layout rules for 1–4 players.
- `Source/NAMEC/Core/NamecGameMode.h` — new; player registration, capacity checks, spawning.
- `Source/NAMEC/UI/Lobby/` — new; Join LAN Game list, character select per local player.
- `Config/DefaultEngine.ini` — OnlineSubsystemNull configuration for LAN.
- `Content/Data/DT_MP_SplitScreenScalability.uasset` — new; graphics presets per viewport count.
- `Content/Data/DT_MP_Session.uasset` — new; session capacity, connection timeout, and other session tuning values.
