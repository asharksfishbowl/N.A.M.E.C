# Networking

This page covers listen-server authority, the host/find/join session flow, character payload sync, per-player loot relevance, capacity/version/password checks, and disconnect handling.

← [Home](Home.md)

## Authority model

- The session is a **listen server** on the host machine using Unreal's legacy replication (Iris is only being evaluated; see [Engine and Rendering](Dev-Engine-and-Rendering.md)). In pure split-screen play the host is the only machine.
- All gameplay state is server-authoritative: terrain edits, combat, loot rolls, crafting, progression XP.
- Characters are saved locally and trusted. There is no anti-cheat.
- No internet play, NAT traversal, relay, matchmaking, dedicated servers, host migration or cross-platform play.
- Max 4 players total across all machines (starting value, tunable; hard cap 4). Each machine supports up to 4 local players, limited by remaining capacity.

## Session flow

1. **Host:** New World or Load World opens the lobby. `UNamecSessionSubsystem::HostSession(WorldSettings)` creates the LAN session and loads the map. The host loads `UNamecWorldSave`, generates terrain from the seed, then applies saved edits.
2. **Discovery:** LAN session discovery via `OnlineSubsystemNull` with `bIsLANMatch = true` (configured in `Config/DefaultEngine.ini`). LAN hosting is a world setting, on by default.
3. **Find:** the remote machine opens Join LAN Game. `FindSessions` lists sessions with world name, host name, current/max players and game version.
4. **Select:** the joining machine lists its local players (1–4), and each picks a character from that machine's saves or creates one with "Create New" (character creation runs locally, before connecting).
5. **Join:** `JoinSession` connects.
6. **Register:** the client sends `ServerRegisterLocalPlayers(Array<CharacterPayload>)`. The host validates, then spawns pawns and initializes components from the payloads.

A local player added mid-session on a client machine sends the same RPC with one payload and goes through the same checks.

## Join validation

The host refuses a join with a clear message when:

| Check | Failure |
|-------|---------|
| Capacity | Session full, or the joining machine has more local players than the remaining capacity |
| Version | Game version differs |
| Password | Optional world password set and not matched |
| Duplicate GUID | The same character GUID is already in the session |
| Save version | Character `SaveVersion` newer than the host build (older saves are migrated before spawning) |

Two local players on one machine selecting the same character are blocked locally with "Character already in use".

## Character payload sync

- `NamecCharacterPayload` is the serializable character state used for join and save sync.
- On join, the host treats the received payload as the player's authoritative state for the session.
- On every autosave, the host sends `ClientSaveCharacter(CharacterPayload)` to each client, which writes `UNamecCharacterSave` locally.
- On clean disconnect or exit, a final `ClientSaveCharacter` is sent before the connection closes.
- On an unexpected drop, no final send arrives. The client saves its last received payload, which is the last autosave (up to 5 minutes, starting value, tunable, of progress lost).
- Character inventory (including dye colors), gold, reputation per faction and quest state are part of the payload and leave with the player. World containers, Vendor stock, Quest Board offers, NPC respawn timers and camp states stay in the world save.
- The payload carries race, sex and `FNamecAppearance`. On spawn, the server applies the racial passive and downside effects, grants the racial active ability, and replicates race, sex and appearance to all clients.

## Spawning

- Every world entry spawns the character at its bed in this world, or at the world spawn point if there is no bed or it was destroyed. World entries: the host's local players on New World or Load World, a remote character joining, a character rejoining after a disconnect or crash.
- Exception: a local player added mid-session spawns next to local player 1, or at bed/world spawn if local player 1 is inside a sealed boss arena.
- Joining while a boss arena is sealed spawns per the rule above, and the player cannot enter the arena.
- Positions are not saved.

## Replication and relevance

- The server replicates world and gameplay state to all clients.
- Inventory contents, gold, reputation and the quest log replicate only to the owning connection. Equipped item visuals, including dye colors, replicate to all clients.
- Vendor stock quantities and Quest Board offers are world state, replicated to every client and shared by every player.
- Terrain edits are applied on the server in receive order and replicated to all clients, which re-mesh affected chunks. Late joiners receive edit deltas for chunks within their streaming radius, then more as chunks stream in.
- Each local viewport's streaming radius is a separate chunk streaming source.
- Progression state replicates to the owning client. Survival attributes replicate to the owning client.

### Appearance replication

- Race, sex and appearance replicate to all clients (`NamecAppearanceComponent`). Each client builds the character's visual locally from race, sex, appearance and equipped items, picking the matching race × sex variant of every equipped item.
- **Mirror:** the client sends `ServerSetAppearance(FNamecAppearance)`. The server checks that the race and sex match the character, every option index is in range, and the player is within the Mirror's interaction range, then applies and replicates. A request with a different race or sex, or out-of-range indexes, is rejected and the appearance is unchanged.
- **Local-only racial visuals:** Felari Night Eyes (post-process) and Hundari Keen Nose outlines render only in the owning local player's viewport. Keen Nose finds outlined actors with local queries, with no replication.

### Per-player loot relevance

Split-screen players share one connection, so loot ownership is per player (character GUID), not per connection:

- Each per-player loot actor (`NamecLootPickup`) is owned by the eligible player's PlayerController.
- `bOnlyRelevantToOwner`: the server replicates it only to the owner's connection.
- `bOnlyOwnerSee` on its primitives: it renders only in the owning local player's viewport, so other local players on the same machine, including the host, never see it.
- The server accepts a pickup (`ServerPickup(PickupId)`) only from the owning player.
- Items dropped from an inventory use the shared world pickup mode, visible and pickable by everyone.
- Loot chests roll only for the opener and record the opener's GUID in the chest's opened set.
- Gold from kills and chests is rolled per eligible player and carried in that player's own loot actor. Gold dropped with Drop Gold uses the shared world pickup mode.

### Owning-viewport UI

- The execution prompt, dye preview, vendor, quest board and quest giver screens, the Quests tab and the HUD quest tracker all render only in the owning local player's viewport.

### Server-validated requests

| RPC | Server checks |
|-----|---------------|
| `ServerRequestExecution(TargetId)` | Prompt conditions (including attacker not Downed, executing, airborne, climbing, swimming, or in a menu), range, target not already being executed and not airborne, capsule sweep for clear space. First request received wins. |
| `ServerBuyItem(VendorId, StockRowId, Quantity)` / `ServerSellItem(VendorId, ItemInstanceId, Quantity)` | Range, reputation tier, gold, stock quantity, item ownership. Requests processed in receive order. |
| `ServerAcceptQuest(BoardId, OfferId)` | Reputation tier, quest log size, character GUID not in the offer's accepted character GUID set (then adds it) |
| `ServerTurnInQuest(QuestGiverId, QuestId)` | Location, reputation tier, completed objectives |
| `ServerPayFine(GuardCaptainId)` | Range, Hostile tier with the Guard Captain's kingdom, gold at least the fine |
| `ServerApplyDye(DyeStationId, ItemInstanceId, Zone, ColorId)` / `ServerClearDyeZone(DyeStationId, ItemInstanceId, Zone)` | Range, item ownership, zone defined on the item, dye item held (Apply) |

## Disconnects

| Event | Handling |
|-------|----------|
| Remote client clean disconnect | Pawn removed. Character saves on its machine from the final send. |
| Remote client unexpected drop or crash | Pawn removed. Character keeps the last received payload (last autosave). Rejoin is a new join. |
| Connection timeout | No packets for 20 seconds (starting value, tunable) is treated as a disconnect. |
| Host exits | All clients return to the main menu with "Host ended the session". Characters save first. Session ends. |
| Local player 1 on a client leaves | Every local player on that machine disconnects. |
| Local player 1 on the host leaves | Treated as host exit. |
| Other local player leaves via pause menu | Character saves, viewport removed, layout re-flows. |
| Controller disconnect | That player's input pauses with "Reconnect controller" in their viewport. World pauses only if every player is on the host machine. |
| Disconnect inside a boss arena | Character removed from the fight. Boss scaling stays locked from summon. |
| Disconnect mid-craft | Current craft cancelled, reserved materials returned before saving. |
| Clean disconnect while Downed | Character saves with the dead-respawn flag. |

Pause menu: the world pauses only in a session with one player total.

## Source specs

- [Multiplayer](../../specs/multiplayer/multiplayer.md)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (Requirements 28 and 44–51, Data Flow 8, Edge Cases 1 and 4)
- [Voxel World](../../specs/voxel-world/voxel-world.md) (Requirement 15, Edge Cases 6–7)
- [Character Creation](../../specs/character-creation/character-creation.md) (Requirements 13 and 24, Data Flow 4–6, Edge Case 3)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (Data Flow 3–6)
- [Crafting Jobs](../../specs/crafting-jobs/crafting-jobs.md) (Data Flow 8)
