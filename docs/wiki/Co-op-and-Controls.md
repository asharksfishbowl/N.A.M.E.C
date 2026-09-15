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
- Graphics scale down automatically at 3–4 viewports on your machine (lighting quality, shadows, foliage density, view distance), but never above your own graphics settings: each one uses the lower of your setting and the 3–4 viewport value. At 1–2 viewports your settings apply unchanged. See [Engine and Rendering](Dev-Engine-and-Rendering.md).

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

All bindings are defaults and can be remapped per local player in Settings. Remaps are saved per local player slot on this machine (see [Save System](Dev-Save-System.md)). Chorded bindings (LB held + a button, Left Alt + RMB) take priority, so the plain action never fires while its chord modifier is held. While an execution prompt is showing in your viewport, Y (or R) executes instead of toggling two-handing, and LB + Y still casts ability 4. Every menu and the HUD work with a gamepad alone.

Some bindings change with what you're holding or doing. Each one replaces the normal binding of that button only while its situation lasts, and chords still take priority:

- **Tool in your Right Hand** (axe, pickaxe, shovel, Hammer, Fishing Rod): RB (left mouse) swings the tool instead of a light attack, except that a Fishing Rod casts (see below). With an axe or pickaxe, RT does nothing.
- **Shovel:** RT (middle mouse) toggles Dig/Fill mode. D-pad Left/Right (mouse wheel) cycles the fill material: Soil, Sand, Gravel, Stone. While a shovel is equipped, the D-pad Left/Right hand hotkey cycles don't work.
- **Hammer:** RT (middle mouse) opens the build menu. Choosing a piece enters placement mode.
- **Placement mode:** RB (left mouse) places, D-pad Left/Right (mouse wheel) rotates, B (right mouse) cancels, and holding X (F) for 1 second (starting value, tunable) on a placed piece deconstructs it. While placement mode is on, these replace the hotkey cycles, using a consumable, dodge roll and sprint, and block. Deconstructing works only in placement mode. After placing, you stay in placement mode while you still have that piece's item.
- **Bow** (a two-handed item): a bow can't block, so LT (right mouse) aims, RB (left mouse) fires an aimed shot while aiming, and RB without aiming fires a quick shot at 50% damage. RT does nothing. There is no parry with a bow.
- **Fishing Rod:** RB (left mouse) casts, and RT does nothing. During the bite window, A (E) reels in, even if you aren't looking at anything you can interact with. This is the one time A doesn't jump or interact.
- **Torch in your Right Hand:** RB (left mouse) makes a torch bash that builds up Burn, and RT does nothing.
- **One-handed weapon in your Left Hand:** LT (right mouse) makes an off-hand light attack instead of blocking. LB + LT parries only if that weapon is parry-capable.
- **Torch in your Left Hand:** LT (right mouse) does nothing. LB + LT parries with your Right Hand weapon if it's parry-capable.
- A bow in your Right Hand, or two-handing your Right Hand weapon, overrides both Left Hand rules: LT aims with a bow, and blocks with the two-handed weapon. See [Combat and Loot](Combat-and-Loot.md).
- **In bed:** B (Space) gets you out of bed.
- **Aiming at water** (only when nothing else interactable is targeted, and never while swimming, so A still swims and jumps in water): at Fresh water, tapping A (E) drinks once instead of jumping, and holding it for 1 second (starting value, tunable) refills all your waterskins instead. At Salt or Poison water, pressing A (E) drinks once however long you hold it, with no refill and no second drink. See [Survival](Survival.md).

| Action | Gamepad | Keyboard & mouse (local player 1 only) |
|--------|---------|----------------------------------------|
| Move | Left stick | W A S D |
| Look | Right stick | Mouse |
| Jump | A (when no interactable is targeted) | Space |
| Interact (including revive hold, Vendors, Quest Givers, Guard Captains, Quest Boards, the Dye Station, doors, and water when nothing else is targeted) | A (when an interactable is targeted) | E |
| Drink from water (see [Survival](Survival.md); not while swimming) | A, released before 1 s (Fresh water), or held any length (Salt or Poison water) | E, released before 1 s (Fresh water), or held any length (Salt or Poison water) |
| Refill all waterskins | Hold A for 1 s (Fresh water targeted) | Hold E for 1 s (Fresh water targeted) |
| Climb (hold against a surface steeper than 60°) | Hold A | Hold Space |
| Dodge roll | B (tap) | Left Shift (tap) |
| Sprint | B (hold) | Left Shift (hold) |
| Use selected consumable | X | F |
| Toggle two-handing the Right Hand weapon (see [Combat and Loot](Combat-and-Loot.md)) | Y (when no execution prompt is shown) | R (when no execution prompt is shown) |
| Execute (see [Combat and Loot](Combat-and-Loot.md)) | Y (only while an execution prompt is shown) | R (only while an execution prompt is shown) |
| Light attack | RB | Left mouse button |
| Heavy attack (see [Combat and Loot](Combat-and-Loot.md); does nothing with a bow, axe, pickaxe, Fishing Rod or torch) | RT | Middle mouse button |
| Block (not with a bow, or with a weapon or torch in the Left Hand) | LT | Right mouse button |
| Off-hand light attack (see [Combat and Loot](Combat-and-Loot.md)) | LT (one-handed weapon in the Left Hand) | Right mouse button (one-handed weapon in the Left Hand) |
| Parry (not with a bow) | LB + LT | Left Alt + right mouse button |
| Swing the Right Hand tool (axe, pickaxe, shovel, Hammer) | RB (tool in hand) | Left mouse button (tool in hand) |
| Toggle shovel Dig/Fill mode (see [World and Building](World-and-Building.md)) | RT (shovel in hand) | Middle mouse button (shovel in hand) |
| Cycle shovel fill material (Soil, Sand, Gravel, Stone) | D-pad Left / Right (shovel in hand) | Mouse wheel (shovel in hand) |
| Open the build menu | RT (Hammer in hand) | Middle mouse button (Hammer in hand) |
| Place piece | RB (placement mode) | Left mouse button (placement mode) |
| Rotate piece | D-pad Left / Right (placement mode) | Mouse wheel (placement mode) |
| Cancel placement | B (placement mode) | Right mouse button (placement mode) |
| Deconstruct the piece you're aiming at | Hold X for 1 s (placement mode) | Hold F for 1 s (placement mode) |
| Aim bow | LT (bow in hand) | Right mouse button (bow in hand) |
| Fire bow, aimed shot | RB (while aiming) | Left mouse button (while aiming) |
| Fire bow, quick shot (50% damage) | RB (bow in hand, not aiming) | Left mouse button (bow in hand, not aiming) |
| Cast fishing rod (see [Skills](Skills.md)) | RB (Fishing Rod in hand) | Left mouse button (Fishing Rod in hand) |
| Reel in (fishing bite window) | A (during the bite window) | E (during the bite window) |
| Leave bed (see [Survival](Survival.md)) | B (in bed) | Space (in bed) |
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

## Source spec

- [Multiplayer](../../specs/multiplayer/multiplayer.md)
- [Game Foundation](../../specs/game-foundation/game-foundation.md) (controls)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (what travels with a character)
- [Voxel World](../../specs/voxel-world/voxel-world.md) (shovel and Hammer controls)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (bow and two-handing)
- [Character Creation](../../specs/character-creation/character-creation.md) ("Create New")
