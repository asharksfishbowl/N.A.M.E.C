# Co-op and Controls

This page covers split-screen layouts, joining, LAN play, portable characters, disconnect behaviour, and the full default control table.

← [Home](Home.md)

## Player count

- Up to 4 players total across all machines (starting value, tunable; hard cap 4).
- Each machine supports up to 4 local players, limited by the remaining capacity.
- Split-screen and LAN combine in one session, for example 2 players on PC A plus 2 on PC B.
- No online play, dedicated servers, host migration or cross-platform play. When the host leaves, the session ends.

## Split-screen

### Layouts

| Local players | Layout |
|---------------|--------|
| 1 | Full screen |
| 2 | Vertical split (side by side) |
| 3 | One on the top half, two in the bottom quarters |
| 4 | Quadrants |

- Each local player has their own HUD, menus, inventory, crafting UI and camera mode (first/third person).
- Local player 1 can use keyboard and mouse or a controller. Players 2–4 use controllers only.
- Graphics settings scale down automatically at 3–4 viewports on your machine (lighting quality, shadows, foliage density, view distance). See [Engine and Rendering](Dev-Engine-and-Rendering.md).

### Joining locally

- **Lobby:** after choosing New World or Load World, local player 1 picks a character. Others press Start on an unassigned controller to join and pick theirs.
- **Create New:** every character select (lobby, mid-game join and LAN join) also has a "Create New" option that runs character creation inside that player's viewport. See [Races and Character Creation](Races-and-Character-Creation.md).
- **Mid-game:** press Start on an unassigned controller, confirm the prompt, pick or create a character. The world keeps running while you create one, and cancelling returns you to character select. The new player spawns next to local player 1. If player 1 is inside a sealed boss arena, the new player spawns at their bed in this world, or the world spawn point.
- Two local players cannot pick the same character ("Character already in use").

### Leaving and pausing

- Local players 2–4 can leave from the pause menu. Their character saves and the layout re-flows.
- If local player 1 on a client machine leaves, every local player on that machine disconnects. If local player 1 on the host leaves, the session ends.
- The pause menu only pauses the world when there is exactly one player in the session.
- **Controller disconnect** (for example a dead battery): that player's input pauses and "Reconnect controller" shows in their viewport only. Their character stays in the world. The world pauses only if every player is on the host machine. A character being created at the time is kept until the controller reconnects.

## LAN

- The host turns on LAN hosting in world settings (on by default). No port forwarding, accounts or internet needed.
- **Join LAN Game** lists sessions with world name, host name, current/max players and game version.
- The joining machine lists its local players (1–4), and each picks a character (or creates one with "Create New") before connecting.
- An optional world password must be entered to join.
- A join is refused with a clear message when:
  - the session is full ("Session full")
  - the game version differs
  - the joining machine has more local players than the remaining capacity
  - the character is already in the session
  - the character's save is from a newer game version

## Portable characters

- A character is saved on its owner's machine and can play in any world.
- On join, the character travels to the host, which uses it for the session.
- Your inventory (including dye colors), gold, reputation and quests are part of your character and leave with you. World containers, Vendor stock, Quest Board offers and camp and NPC states stay with the world.
- **Every world entry spawns you at your bed in that world, or at the world spawn point** if you have no bed there or it was destroyed. Your last position is not saved. The only exception is a local player added mid-session (spawns next to local player 1).
- Joining while a boss arena is sealed spawns you at your bed or the world spawn point, and you cannot enter the arena.
- Each character can be in a session only once.

## Saving and disconnects

- The host sends your character back to your machine on every autosave (every 5 minutes, starting value, tunable) and once more on a clean disconnect or exit.
- **Clean disconnect:** your character saves on your machine with all progress.
- **Unexpected drop** (cable pulled, crash, or no packets for 20 seconds): no final save arrives. You keep the last autosave, so up to 5 minutes of progress can be lost. Rejoining counts as a new join.
- **Host exits:** everyone returns to the main menu with "Host ended the session", and characters save first.
- A remote player who disconnects has their character removed from the world.

## Controls

All bindings are defaults and can be remapped per local player in Settings. Chorded bindings (LB held + a button, Left Alt + RMB) take priority, so the plain action never fires while its chord modifier is held. While an execution prompt is showing in your viewport, Y (or R) executes instead of toggling two-handing, and LB + Y still casts ability 4. Every menu and the HUD work with a gamepad alone.

| Action | Gamepad | Keyboard & mouse (local player 1 only) |
|--------|---------|----------------------------------------|
| Move | Left stick | W A S D |
| Look | Right stick | Mouse |
| Jump | A (when no interactable is targeted) | Space |
| Interact (including revive hold, fishing bite, Vendors, Quest Givers, Guard Captains, Quest Boards and the Dye Station) | A (when an interactable is targeted) | E |
| Climb (hold against a surface steeper than 60°) | Hold A | Hold Space |
| Dodge roll | B (tap) | Left Shift (tap) |
| Sprint | B (hold) | Left Shift (hold) |
| Use selected consumable | X | F |
| Toggle two-handing the Right Hand weapon | Y (when no execution prompt is shown) | R (when no execution prompt is shown) |
| Execute (see [Combat and Loot](Combat-and-Loot.md)) | Y (only while an execution prompt is shown) | R (only while an execution prompt is shown) |
| Light attack | RB | Left mouse button |
| Heavy attack | RT | Middle mouse button |
| Block | LT | Right mouse button |
| Parry | LB + LT | Left Alt + right mouse button |
| Abilities 1–4 | LB + A / B / X / Y | Z / X / C / V |
| Abilities 5–6 (one bar slot always holds your racial ability) | LB + RB / LB + RT | B / N |
| Lock-on toggle | R3 | Tab |
| Cycle lock-on target (while locked on) | Flick right stick | Flick mouse |
| Crouch | L3 | Left Ctrl |
| Camera first/third-person toggle | View (hold 0.5 s) | H |
| Inventory and Character menu | View (tap) | I |
| Quest log (see [Factions and Kingdoms](Factions-and-Kingdoms.md)) | View (tap), then the Quests tab. The gamepad has no free button, so the quest log is a tab of this menu. | J (opens the menu on the Quests tab) |
| Favorites quick menu | D-pad Up | Q |
| Cycle right-hand hotkeyed items | D-pad Left | — |
| Cycle left-hand hotkeyed items | D-pad Right | — |
| Cycle hotkeyed consumables | D-pad Down | — |
| Activate hotkey slots 1–8 | — (use the D-pad cycles) | 1–8 |
| Pause menu | Menu | Esc |
| Join as a new local player | Start (Menu) on a controller not assigned to a local player | — |

Some controls are not decided yet (tools, shovel mode, build menu, bow, fishing cast, leaving a bed). See [Open Decisions](Open-Decisions.md).

## Source spec

- [Multiplayer](../../specs/multiplayer/multiplayer.md)
- [Game Foundation](../../specs/game-foundation/game-foundation.md) (controls)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (what travels with a character)
- [Character Creation](../../specs/character-creation/character-creation.md) ("Create New")
