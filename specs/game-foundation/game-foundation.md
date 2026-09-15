# Spec: Game Foundation

## Overview
N.A.M.E.C is a first/third-person 3D fantasy survival RPG for up to 4 players in local split-screen and LAN co-op, built in Unreal Engine 5.8. Players explore a seeded, smooth-voxel world of distinct climate regions, dig and build freely, level skills by using them, craft through independent Jobs, trade and take quests in the towns of six race kingdoms, defend their bases against raids, and fight Souls-like enemies and bosses that gate progression. This spec is the umbrella: it defines project-wide architecture, shared terms, save layout, and the index of system specs.

## System Spec Index
| Spec | Covers |
|------|--------|
| `specs/game-foundation/game-foundation.md` | This file — project setup, glossary, save layout, data-driven tuning, platform rules |
| `specs/character-creation/character-creation.md` | Character creation, six races with traits, sex, appearance customization, Mirror |
| `specs/character-progression/character-progression.md` | D&D stats, character level, classes, use-based skills |
| `specs/crafting-jobs/crafting-jobs.md` | Jobs, crafting stations, recipes, crafted item quality |
| `specs/survival/survival.md` | Hunger, thirst, temperature, stamina, fatigue |
| `specs/voxel-world/voxel-world.md` | World generation, climates, terrain dig/fill, building, gathering |
| `specs/combat-loot/combat-loot.md` | Souls-like combat, downed/revive, enemies, bosses, randomized loot |
| `specs/inventory/inventory.md` | Skyrim-style weight inventory, equipment slots, favorites, containers |
| `specs/multiplayer/multiplayer.md` | Split-screen, LAN, authority, portable characters |
| `specs/factions-kingdoms/factions-kingdoms.md` | Factions, reputation, kingdoms and towns, NPCs, gold and vendors, quests, raids on player bases |
| `specs/engine-tech/engine-tech.md` | Adopted, avoided, and evaluated Unreal Engine 5.8 features, the Nanite plan, split-screen rendering scalability tiers, the benchmark milestone, console readiness |
| `specs/enemy-ai/enemy-ai.md` | Enemy and town NPC AI behaviour (StateTree states, AI LOD), faction hostility, enemy loot pickup and equipment, enemy XP and levels, Veterans |

## Glossary
- **Character** — a player's persistent avatar (name, race, sex, appearance, stats, classes, skills, jobs, inventory). Saved on the owning player's machine, portable between worlds.
- **Race** — one of six playable peoples (Human, Felari, Hundari, Sauren, Vanari, Ursan), each with stat bonuses, a passive, an active ability, and a downside (see character-creation spec).
- **World** — a seeded generated map plus all terrain edits, placed structures, containers, and boss-defeat state. Saved on the host machine.
- **Host** — the machine running the listen server. In pure split-screen play, the only machine.
- **Local player** — one of up to 4 controllers/keyboard users on a single machine.
- **Region** — a contiguous area of the world belonging to one climate type (see voxel-world spec).
- **Stat** — one of the six ability scores: STR, DEX, CON, INT, WIS, CHA.
- **Modifier** — derived from a stat: `floor((score - 10) / 2)`.
- **Character level** — overall level, 1–50. Grants stat points and class slots.
- **Class** — a combat archetype granting an ability set (e.g., Warrior, Mage).
- **Skill** — a use-based proficiency, levels 1–100, raised by performing the associated action (e.g., Mining, Archery).
- **Job** — a crafting profession, levels 1–100, raised by crafting (e.g., Blacksmith).
- **Rarity** — item tier: Common, Magic, Rare, Epic, Legendary.
- **Affix** — a randomized stat bonus rolled onto an item.
- **Faction** — one of eight groups in `DT_Factions_Factions`: the six race kingdoms (Human, Felari, Hundari, Sauren, Vanari, Ursan) and the always-hostile Bandits and Beastmen. Region monsters belong to no faction (see factions-kingdoms spec).
- **Reputation** — a character's standing with one faction, from −100 to +100, grouped into the tiers Hostile, Unfriendly, Neutral, Friendly, and Honored. Saved with the character. Hostile standing with a kingdom recovers daily and can be ended by paying a Guard Captain's fine (`specs/factions-kingdoms/factions-kingdoms.md` Requirements 63–65).
- **Gold** — the per-character currency counter. Gold is not an inventory item and has no weight.
- **Base** — for raids, a group of at least 10 player-placed building pieces within 30 m of a crafting station or bed (starting values; `specs/factions-kingdoms/factions-kingdoms.md` Requirement 51).
- **Execution** — a paired kill animation a player can start on a weakened or riposte-open `Hostile` enemy (`specs/combat-loot/combat-loot.md` Requirement 44).
- **Veteran** — an enemy that gained 3 or more levels (starting value) and was promoted: it has a name and title, is saved in `UNamecWorldSave` until killed, and can lead raids (`specs/enemy-ai/enemy-ai.md` Requirements 43–54).
- **Item Score** — the number a humanoid enemy uses to decide whether a picked-up item replaces the item in that equipment slot, computed from damage, armor, resistances, and affix values (`specs/enemy-ai/enemy-ai.md` Requirement 28).
- **Tuning table** — an Unreal DataTable holding numeric balance values. All numbers in the system specs are starting values that live in tuning tables (or, for per-item values, item definition data assets), not constants in code.

## Goals
- A single Unreal Engine 5 C++ project with gameplay systems split into the modules listed in Requirements.
- Every numeric balance value is data-driven and editable without recompiling.
- The game is fully playable with a gamepad only, so a console port needs no gameplay rework.
- Characters and worlds save and load reliably, and a character can move between worlds.

## Non-Goals
- Online (internet) matchmaking, dedicated servers, or cross-platform play.
- Console builds in this spec — only console-readiness constraints.
- Mod support.
- Anti-cheat. Characters are saved locally and trusted.
- A story campaign. Quests are limited to town Quest Board quests and the per-kingdom questlines in `specs/factions-kingdoms/factions-kingdoms.md`.

## Requirements
1. The project uses Unreal Engine 5.8, pinned at 5.8.2, and takes later 5.8 hotfix releases as they ship. The version is recorded in `NAMEC.uproject` and updated with each hotfix. The pin is fixed by this spec: moving to another minor or major version (5.7, UE6) is a spec change. The engine features used, not used, and under evaluation are listed in `specs/engine-tech/engine-tech.md`.
2. Gameplay code is C++. Blueprints are used for content (enemy variants, item assets, UI layout), not for core system logic.
3. Stats, abilities, damage, and status effects are built on Unreal's Gameplay Ability System (GAS), per `specs/engine-tech/engine-tech.md` Requirement 3. Each character owns one `UAbilitySystemComponent`. Each enemy also owns one `UAbilitySystemComponent` with an enemy attribute set (see combat-loot spec).
4. The game module `Source/NAMEC/` is organized into subfolders: `Core/`, `Character/`, `Progression/`, `Crafting/`, `Survival/`, `World/`, `Combat/`, `Loot/`, `Inventory/`, `Multiplayer/`, `Factions/`, `EnemyAI/`, `UI/`, `Save/`. Editor-only code (data validators) lives in a separate editor module `Source/NAMECEditor/`.
5. All balance numbers live in DataTables under `Content/Data/`, one or more tables per system, and are named `DT_<System>_<Purpose>` (e.g., `DT_Progression_SkillXPCurve`). Per-item values (weight, damage, insulation) live in item definition data assets (see `specs/inventory/inventory.md`). Both are editable without recompiling.
6. Save data is split into three independent save types:
   - `UNamecCharacterSave` — one per character, stored in the local user's save directory on the machine that owns the character. Contents: character GUID and name; race, sex, and `FNamecAppearance` (character-creation spec); stats and spent/unspent stat points, classes, class levels, skill XP, boss first-kill flags, and ability bar (character-progression spec); Job levels and XP (crafting-jobs spec); survival meter values (survival spec; Breath is not saved); current Health and Mana (Stamina is not saved); every item instance, equipped slots, favorites, hotkeys, and sort choice (inventory spec), including each item instance's dye colors (`specs/inventory/inventory.md` Requirement 23) each consumable's potency value (`specs/crafting-jobs/crafting-jobs.md` Requirement 11), each rune's tier (`specs/crafting-jobs/crafting-jobs.md` Requirement 12), and each waterskin's remaining drinks (`specs/survival/survival.md` Requirement 6); the shovel Dig/Fill mode and selected fill material (`specs/voxel-world/voxel-world.md` Requirement 42); two-handing state is not saved, and every character loads one-handed (`specs/combat-loot/combat-loot.md` Requirement 57); gold, reputation per faction, and quest state (active quests with objective parameters, progress, and tracked flags, and completed questline steps per kingdom) (`specs/factions-kingdoms/factions-kingdoms.md` Requirements 11, 28, and 48); the dead-respawn flag, set by any save written while the character is Downed or dead (combat-loot spec Edge Case 1); and the remaining duration of each active timed effect (Weakened, meal buff, Wet, Salty). On world entry, the saved Health and Mana are restored after the character's equipment and effects are applied, clamped to its current MaxHealth and MaxMana, and Stamina starts full. A character whose save has the dead-respawn flag loads with the death respawn state, including full Health and full Mana (combat-loot spec Requirement 17 and Edge Case 1), not its saved Health and Mana.
   - `UNamecWorldSave` — one per world, stored on the host machine. Contents: world name, seed, size, voxel resolution (voxel-world spec Requirement 3), and world settings (friendly fire, LAN hosting, password, Raids); current in-game time of day, the world's total elapsed in-game time, and each region's current weather; terrain chunk edit deltas; placed building pieces, including crafting stations with their bound attachments and tier, storage containers with their contents, and doors with their open or closed state (`specs/voxel-world/voxel-world.md` Requirement 45); shared world pickups, including per-player loot converted into shared world pickups with each one's remaining despawn time (`specs/enemy-ai/enemy-ai.md` Requirement 20) and each live non-Veteran enemy's picked-up items and gold written as shared world pickups at its position (`specs/enemy-ai/enemy-ai.md` Requirement 34) (unconverted per-player loot actors are not saved); one record per living Veteran with its Veteran GUID, enemy row ID, faction, current level, XP, current health without player-count scaling, equipped and carried item instances, gold, home position, name, title, kill counts, and camp and spawn point for a camp Veteran (`specs/enemy-ai/enemy-ai.md` Requirement 46); tree harvest states and forage timers; Loose Stick and Loose Stone pickup collected states and respawn timers; bed respawn points per character GUID; loot chest opened state per character GUID; boss defeat flags; town NPC respawn timers, Vendor stock quantities, Quest Board offers with each offer's accepted character GUID set, and Bandit and Beastmen camp states (per-spawn-point dead states, cleared state, and respawn timer) (`specs/factions-kingdoms/factions-kingdoms.md` Requirements 19, 25, 34, 37, and 42). Gold dropped by players is saved as a shared world pickup. Active raids and the real-time raid roll clock are not saved: an autosave does not change an active raid, and when a world loads no raid is active and raiders from before the host exited are not restored (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 59). Non-Veteran enemies are not saved (`specs/enemy-ai/enemy-ai.md` Requirement 34). Character positions are not saved (multiplayer spec Requirement 16).
   - `UNamecSettingsSave` — one per machine, stored in the local user's save directory on that machine and never sent to other machines. Contents: one section per local player slot index (1–4) and one machine-wide section. A local player slot's section holds that slot's input remaps (Requirement 14), the Night Eyes toggle (`specs/character-creation/character-creation.md` Requirement 13), the "Execution prompts" option (`specs/combat-loot/combat-loot.md` Requirement 45), and every other per-local-player Setting. Per-local-player Settings belong to the slot index, not to a character or a controller: whoever joins as local player 2 uses slot 2's section. The machine-wide section holds global settings (graphics and audio). The settings save is written whenever a Settings change is applied, with the same temp-file-and-rename rule as the other saves (Edge Case 2).
7. Every save type (`UNamecCharacterSave`, `UNamecWorldSave`, and `UNamecSettingsSave`) carries an integer `SaveVersion`. Loading a save with an older `SaveVersion` runs a migration function; loading a newer `SaveVersion` than the build supports is refused with a user-facing message. Exception: a `UNamecSettingsSave` with a newer `SaveVersion` is left on disk untouched, and default settings are used for that session without being written back to it (Edge Case 1 has the corrupt settings save exception).
8. The world autosaves every 5 minutes (tuning value, in `DT_Core_Save`) and on host exit. Character saves write on autosave, on disconnect, and on exit.
9. All menus and HUD are navigable with a gamepad alone, built on the unified Enhanced Input + Common UI system (`specs/engine-tech/engine-tech.md` Requirement 3). Keyboard and mouse is supported for local player 1 only. Each local player's Settings menu edits that local player slot's section of `UNamecSettingsSave`, and global graphics and audio settings edit its machine-wide section (Requirement 6).
10. Platform-specific calls (file paths, user accounts, controller pairing, on-screen text entry keyboard) go through an interface in `Source/NAMEC/Core/Platform/` so a console port swaps one implementation.
11. The main menu offers: New World, Load World, Join LAN Game, Characters (create/delete/view), Settings, Quit.
12. Camera supports first-person and third-person, toggled per local player with the camera toggle input (Requirement 14). The default is third-person. Cameras use PlayerCameraManager + SpringArm per `specs/engine-tech/engine-tech.md` Requirement 3.
13. Performance targets on the reference PC (NVIDIA RTX 3060-class GPU, 6-core CPU, 16 GB RAM) at 1080p: 60 fps with 1–2 local viewports, 30 fps with 3–4 local viewports, using the High and Split rendering scalability tiers in `DT_MP_SplitScreenScalability` (`specs/engine-tech/engine-tech.md` Requirement 6). These targets are first measured by the benchmark milestone (`specs/engine-tech/engine-tech.md` Requirement 11).
14. All input uses Enhanced Input. Every binding below is the default and is remappable per local player in Settings, and remaps are saved per local player slot in `UNamecSettingsSave` (Requirement 6). Chorded bindings (LB held + a button, Left Alt + RMB) take priority over the unchorded binding of the same button, so an unchorded action never fires while its chord modifier is held. While an execution prompt is shown in a local player's viewport (`specs/combat-loot/combat-loot.md` Requirement 45), that player's unchorded Y (gamepad) or R (keyboard) executes instead of toggling two-handing; LB + Y still casts ability 4. Context bindings replace the unchorded default binding of the same button only while their context is active, and chord priority still applies inside every context:
    - **Tool in the Right Hand** (axe, pickaxe, shovel, Hammer, or Fishing Rod; `specs/combat-loot/combat-loot.md` Requirement 56): RB (left mouse button) swings the tool instead of a light attack, except that a Fishing Rod casts (Fishing Rod context below) (tool hits on enemies follow `specs/combat-loot/combat-loot.md` Requirement 56). With an axe or pickaxe in the Right Hand, RT (middle mouse button) does nothing instead of a heavy attack (`specs/combat-loot/combat-loot.md` Requirement 58).
    - **Shovel in the Right Hand:** RT (middle mouse button) toggles Dig/Fill mode instead of a heavy attack (`specs/voxel-world/voxel-world.md` Requirement 40). D-pad Left and D-pad Right (mouse wheel) cycle the fill material instead of the right-hand and left-hand hotkey cycles. This override applies only while a shovel is equipped in the Right Hand.
    - **Hammer in the Right Hand:** RT (middle mouse button) opens the build menu instead of a heavy attack (`specs/voxel-world/voxel-world.md` Requirement 41).
    - **Hammer placement mode:** RB (left mouse button) places the piece, D-pad Left and D-pad Right (mouse wheel) rotate it, B (right mouse button) cancels placement, and holding X (holding F) for 1 s (tuning value in `DT_Core_Input`) while aiming at a placed piece deconstructs it. These bindings override the hotkey cycles, consumable use, and every other unchorded binding they share (the tool swing, dodge roll and sprint, and block) only while placement mode is active. Deconstruction works only in placement mode: outside it, X (F) uses the selected consumable. Placement mode stays active after placing while the player still has that piece's item, and returns to the build menu when none is left (`specs/voxel-world/voxel-world.md` Requirement 43).
    - **Bow in the Right Hand** (a bow is a two-handed item occupying both hand slots): a bow can't block, so LT (right mouse button) aims instead of blocking, RB (left mouse button) fires an aimed shot while aiming and a quick shot at 50% of an aimed shot's damage (tuning value in `DT_Combat_Rules`) while not aiming, and RT (middle mouse button) does nothing instead of a heavy attack (`specs/combat-loot/combat-loot.md` Requirements 53 and 55). Parry (LB + LT, Left Alt + right mouse button) is not available with a bow.
    - **Fishing Rod in the Right Hand:** RB (left mouse button) casts (`specs/character-progression/character-progression.md` Requirement 21), and RT (middle mouse button) does nothing instead of a heavy attack (`specs/combat-loot/combat-loot.md` Requirement 58). During the bite window, A (E) reels in, even when no interactable is targeted. This is the one exception to A's jump and interact context rule: during the bite window, A reels in instead of jumping or interacting.
    - **Torch in the Right Hand:** RB (left mouse button) makes a torch bash light attack (`specs/combat-loot/combat-loot.md` Requirement 60), and RT (middle mouse button) does nothing instead of a heavy attack (`specs/combat-loot/combat-loot.md` Requirement 58).
    - **One-handed weapon in the Left Hand** (`specs/combat-loot/combat-loot.md` Requirement 59): LT (right mouse button) makes an off-hand light attack with the Left Hand weapon instead of blocking. LB + LT (Left Alt + right mouse button) parries only when the Left Hand weapon is parry-capable.
    - **Torch in the Left Hand** (`specs/combat-loot/combat-loot.md` Requirement 59): LT (right mouse button) does nothing instead of blocking. LB + LT (Left Alt + right mouse button) parries with the Right Hand weapon when it is parry-capable.
    - The bow context and two-handing take precedence over both Left Hand contexts: while the Right Hand holds a bow, LT aims, and while the Right Hand weapon is two-handed, LT blocks with it (`specs/combat-loot/combat-loot.md` Requirement 59).
    - **In bed** (`specs/survival/survival.md` Requirement 18): B (Space) leaves the bed instead of dodge rolling, sprinting, or jumping.
    - **Aiming at water** (`specs/survival/survival.md` Requirement 6): a water volume's surface within drinking reach counts as an interactable target only when no other interactable is targeted and the character is not swimming. While swimming, no water surface is targetable, so A (Space) keeps its swim and jump behavior. While a Fresh water surface is targeted, pressing A (E) and releasing it before 1 s (tuning value in `DT_Core_Input`) drinks once instead of jumping, and holding A (E) for 1 s refills every carried waterskin instead of drinking. While a Salt or Poison water surface is targeted, pressing A (E) drinks once instead of jumping whether it is released before or after 1 s, and holding it does nothing more: no refill and no second drink.

    | Action | Gamepad | Keyboard & mouse (local player 1 only) |
    |--------|---------|----------------------------------------|
    | Move | Left stick | W A S D |
    | Look | Right stick | Mouse |
    | Jump | A (when no interactable is targeted) | Space |
    | Interact (including revive hold, Vendors, Quest Givers, Guard Captains, Quest Boards, the Dye Station, doors (`specs/voxel-world/voxel-world.md` Requirement 45), and water volume surfaces when no other interactable is targeted) | A (when an interactable is targeted) | E |
    | Drink from water (`specs/survival/survival.md` Requirement 6; not while swimming) | A, released before 1 s (Fresh water surface targeted), or A held any length (Salt or Poison water surface targeted) | E, released before 1 s (Fresh water surface targeted), or E held any length (Salt or Poison water surface targeted) |
    | Refill every carried waterskin (`specs/survival/survival.md` Requirement 6) | Hold A for 1 s (Fresh water surface targeted) | Hold E for 1 s (Fresh water surface targeted) |
    | Climb (hold against a surface steeper than 60°; `specs/character-progression/character-progression.md` Requirement 21) | Hold A | Hold Space |
    | Dodge roll | B (tap) | Left Shift (tap) |
    | Sprint | B (hold) | Left Shift (hold) |
    | Use selected consumable (`specs/inventory/inventory.md` Requirement 16) | X | F |
    | Toggle two-handing the Right Hand weapon (`specs/combat-loot/combat-loot.md` Requirement 54) | Y (when no execution prompt is shown) | R (when no execution prompt is shown) |
    | Execute (`specs/combat-loot/combat-loot.md` Requirement 46) | Y (only while an execution prompt is shown) | R (only while an execution prompt is shown) |
    | Light attack | RB | Left mouse button |
    | Heavy attack (`specs/combat-loot/combat-loot.md` Requirement 58; does nothing with a bow, axe, pickaxe, Fishing Rod, or torch in the Right Hand) | RT | Middle mouse button |
    | Block (`specs/combat-loot/combat-loot.md` Requirements 4 and 59; not with a bow, and not with a weapon or torch in the Left Hand) | LT | Right mouse button |
    | Off-hand light attack (`specs/combat-loot/combat-loot.md` Requirement 59) | LT (one-handed weapon in the Left Hand) | Right mouse button (one-handed weapon in the Left Hand) |
    | Parry (`specs/combat-loot/combat-loot.md` Requirements 5 and 59; not with a bow) | LB + LT | Left Alt + right mouse button |
    | Swing the Right Hand tool (axe, pickaxe, shovel, Hammer) | RB (tool in the Right Hand) | Left mouse button (tool in the Right Hand) |
    | Toggle shovel Dig/Fill mode (`specs/voxel-world/voxel-world.md` Requirement 40) | RT (shovel in the Right Hand) | Middle mouse button (shovel in the Right Hand) |
    | Cycle shovel fill material (Soil, Sand, Gravel, Stone) | D-pad Left / D-pad Right (shovel in the Right Hand) | Mouse wheel (shovel in the Right Hand) |
    | Open the build menu (`specs/voxel-world/voxel-world.md` Requirement 41) | RT (Hammer in the Right Hand) | Middle mouse button (Hammer in the Right Hand) |
    | Place piece | RB (placement mode) | Left mouse button (placement mode) |
    | Rotate piece | D-pad Left / D-pad Right (placement mode) | Mouse wheel (placement mode) |
    | Cancel placement | B (placement mode) | Right mouse button (placement mode) |
    | Deconstruct the aimed-at piece | Hold X for 1 s (placement mode) | Hold F for 1 s (placement mode) |
    | Aim bow (`specs/combat-loot/combat-loot.md` Requirement 53) | LT (bow in the Right Hand) | Right mouse button (bow in the Right Hand) |
    | Fire bow, aimed shot | RB (while aiming) | Left mouse button (while aiming) |
    | Fire bow, quick shot (`specs/combat-loot/combat-loot.md` Requirement 55) | RB (bow in the Right Hand, not aiming) | Left mouse button (bow in the Right Hand, not aiming) |
    | Cast fishing rod | RB (Fishing Rod in the Right Hand) | Left mouse button (Fishing Rod in the Right Hand) |
    | Reel in (fishing bite window) | A (during the bite window, whether or not an interactable is targeted) | E (during the bite window) |
    | Leave bed (`specs/survival/survival.md` Requirement 18) | B (in bed) | Space (in bed) |
    | Abilities 1–4 (`specs/character-progression/character-progression.md` Requirement 18) | LB + A / B / X / Y | Z / X / C / V |
    | Abilities 5–6 | LB + RB / LB + RT | B / N |
    | Lock-on toggle (`specs/combat-loot/combat-loot.md` Requirement 6) | R3 | Tab |
    | Cycle lock-on target (while locked on) | Flick right stick | Flick mouse |
    | Crouch | L3 | Left Ctrl |
    | Camera first/third-person toggle (Requirement 12) | View (hold 0.5 s, tuning value in `DT_Core_Input`) | H |
    | Inventory and Character menu | View (tap) | I |
    | Quest log (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 48) | View (tap), then select the Quests tab. The gamepad has no free button, so the quest log is a tab of the Inventory and Character menu, not a separate binding. | J (opens the Inventory and Character menu on the Quests tab) |
    | Favorites quick menu (`specs/inventory/inventory.md` Requirement 15) | D-pad Up | Q |
    | Cycle right-hand hotkeyed items | D-pad Left | — |
    | Cycle left-hand hotkeyed items | D-pad Right | — |
    | Cycle hotkeyed consumables | D-pad Down | — |
    | Activate hotkey slots 1–8 | — (use the D-pad cycles) | 1–8 |
    | Pause menu | Menu | Esc |
    | Join as a new local player (`specs/multiplayer/multiplayer.md` Requirement 4) | Start (Menu) on a controller not assigned to a local player | — |

## Data Flow
1. Player launches game → main menu. The Characters screen only creates, deletes, and views characters. It does not select a character for play.
2. Player selects New World (enters world name, seed or random, world size, and world settings per `specs/voxel-world/voxel-world.md` Requirement 5) or Load World → the lobby opens (`specs/multiplayer/multiplayer.md` Requirement 4). Local player 1 picks a character from this machine's saves or creates one with "Create New" (`specs/character-creation/character-creation.md` Requirement 1), and additional local players press Start to join and each pick or create a character → each chosen `UNamecCharacterSave` loads into memory → host loads `UNamecWorldSave` and generates terrain from the seed, then applies saved terrain edits.
3. Local players can also join after the world loads by pressing Start (see multiplayer spec).
4. Remote players choose Join LAN Game, discover the host, and connect, bringing their own characters.
5. During play, systems read tuning from `Content/Data/` DataTables and mutate character state (server-authoritative) and world state.
6. On autosave, the host writes `UNamecWorldSave`, and each machine writes its own characters' `UNamecCharacterSave`.

## Edge Cases
1. When a save file is corrupt (fails deserialization), the game shows "Save could not be loaded", keeps the file on disk untouched, and returns to the menu. It never overwrites the file. This applies to `UNamecCharacterSave` and `UNamecWorldSave`. Exception: a corrupt `UNamecSettingsSave` is renamed with a `.bak` extension, default settings are used, and no message blocks the menu (Requirement 7 has the newer `SaveVersion` settings save exception).
2. When the game crashes mid-save, the previous save remains valid: saves write to a temp file, then atomically rename over the old file.
3. When a character save's `SaveVersion` is newer than the build, that character is shown greyed out in the character list with "Requires newer version".
4. When disk space is insufficient on save, the game shows a warning on the host HUD and retries on the next autosave interval. The in-memory state is kept.

## Acceptance Criteria
- [ ] Project opens in Unreal Engine 5.8.2 (or a later 5.8 hotfix) and builds with no errors.
- [ ] `Source/NAMEC/` contains the subfolders listed in Requirement 4.
- [ ] Changing a value in any `DT_*` table changes in-game behavior without a C++ rebuild.
- [ ] A character created in World A can be loaded into World B with race, sex, appearance, stats, classes, skills, jobs, inventory (including dye colors), gold, reputation, and quest state intact.
- [ ] Killing the process during a save leaves the previous save loadable, for character, world, and settings saves.
- [ ] Turning off Night Eyes and "Execution prompts" for local player slot 2 persists after restart for slot 2 and leaves slot 1 unchanged, and a graphics setting change persists machine-wide for every slot.
- [ ] A corrupt settings save is renamed to `.bak` and the game starts with default settings, and a settings save with a newer `SaveVersion` stays unchanged on disk while the session uses default settings and writes nothing back to it.
- [ ] Every menu screen can be completed start-to-finish with a gamepad only.
- [ ] Camera toggles between first- and third-person per local player.
- [ ] Every default binding in Requirement 14 performs its action, a remapped binding persists after restart for the same local player slot without changing any other slot's bindings, pressing LB + A casts ability 1 without jumping, and pressing Y while an execution prompt is shown starts an execution without toggling two-handing.
- [ ] With a shovel in the Right Hand, D-pad Right cycles the fill material without cycling left-hand hotkeyed items, and after unequipping the shovel D-pad Right cycles left-hand hotkeyed items again.
- [ ] In Hammer placement mode, B cancels placement without a dodge roll, and holding X for 1 s on a placed piece deconstructs it without using the selected consumable.
- [ ] With a bow in the Right Hand, LT aims instead of blocking, LB + LT does not parry, RB without aiming fires a quick shot, and RT does nothing.
- [ ] With an axe, pickaxe, Fishing Rod, or torch in the Right Hand, RT does nothing.
- [ ] With a one-handed weapon in the Left Hand, LT makes an off-hand light attack and never blocks. With a torch in the Left Hand, LT does nothing. While two-handing the Right Hand weapon, LT blocks with it.
- [ ] A character saved with Health and Mana below their maximums loads with the same Health and Mana, clamped to its current MaxHealth and MaxMana, and with full Stamina.
- [ ] Outside Hammer placement mode, holding X uses the selected consumable and never deconstructs a piece.
- [ ] A character's shovel Dig/Fill mode and selected fill material survive save → quit → load, and a character saved while two-handing loads one-handed.
- [ ] During a fishing bite window with no interactable targeted, A reels in without jumping.
- [ ] In bed, B leaves the bed without a dodge roll.
- [ ] Aiming at a Fresh water surface within drinking reach with no other interactable targeted, tapping A drinks once without jumping, and holding A for 1 s refills every carried waterskin without drinking. Holding A for 1 s at Salt or Poison water drinks exactly once and refills nothing, and while swimming A never drinks or refills.

## Key Files
- `NAMEC.uproject` — new; project descriptor with the pinned engine version (5.8, Requirement 1) and enabled plugins (GameplayAbilities, EnhancedInput, OnlineSubsystem, OnlineSubsystemNull, and the plugin of every adopted feature in `specs/engine-tech/engine-tech.md` Requirement 3 that ships as a plugin).
- `Source/NAMEC/NAMEC.Build.cs` — new; module dependencies.
- `Source/NAMEC/Core/NamecGameInstance.h` — new; owns save loading, local player management, session lifecycle.
- `Source/NAMEC/Core/Platform/INamecPlatform.h` — new; platform abstraction interface.
- `Source/NAMEC/Save/NamecCharacterSave.h` — new; character save object with `SaveVersion` and the contents listed in Requirement 6.
- `Source/NAMEC/Save/NamecWorldSave.h` — new; world save object with `SaveVersion` and the contents listed in Requirement 6, including Veteran records (`specs/enemy-ai/enemy-ai.md` Requirement 46).
- `Source/NAMEC/Save/NamecSettingsSave.h` — new; machine-local settings save object with `SaveVersion`, one section per local player slot index (1–4) and one machine-wide section (Requirement 6), with the `.bak` rename for a corrupt file and session-only defaults for a newer `SaveVersion` (Requirement 7, Edge Case 1).
- `Source/NAMEC/Save/NamecSaveMigrations.cpp` — new; version migration functions for all three save types.
- `Source/NAMEC/Character/NamecCameraComponent.h` — new; first/third-person toggle.
- `Content/Data/` — new; all `DT_*` tuning tables.
- `Content/Data/DT_Core_Save.uasset` — new; autosave interval and other save tuning values.
- `Content/Data/DT_Core_Input.uasset` — new; hold and tap thresholds (View hold for camera toggle, Hammer deconstruct hold, waterskin refill hold, stick flick threshold) and other input tuning values.
- `Content/Input/IMC_Gamepad.uasset` — new; Enhanced Input mapping context with the Requirement 14 gamepad defaults.
- `Content/Input/IMC_KeyboardMouse.uasset` — new; Enhanced Input mapping context with the Requirement 14 keyboard and mouse defaults.
- `Content/Input/Contexts/` — new; one higher-priority Enhanced Input mapping context per Requirement 14 context (tool, shovel, Hammer, placement mode, bow, Fishing Rod, fishing bite window, torch, Left Hand weapon, Left Hand torch, bed, aiming at water), each for gamepad and keyboard and mouse, added while its context is active and removed when it ends.
- `Content/Input/Actions/` — new; one `UInputAction` asset per Requirement 14 action.
- `Source/NAMEC/UI/Settings/InputRemapScreen/` — new; per-local-player binding remap screen in Settings, saving to that local player slot's section of `UNamecSettingsSave` (Requirement 6).
