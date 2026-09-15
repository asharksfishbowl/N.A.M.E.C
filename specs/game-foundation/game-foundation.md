# Spec: Game Foundation

## Overview
N.A.M.E.C is a first/third-person 3D fantasy survival RPG for up to 4 players in local split-screen and LAN co-op, built in Unreal Engine 5. Players explore a seeded, smooth-voxel world of distinct climate regions, dig and build freely, level skills by using them, craft through independent Jobs, and fight Souls-like enemies and bosses that gate progression. This spec is the umbrella: it defines project-wide architecture, shared terms, save layout, and the index of system specs.

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
- A story campaign or quest system beyond boss progression.
- NPCs, towns, vendors, and currency. No gold exists.
- Enemy raids on player bases.

## Requirements
1. The project uses the latest stable Unreal Engine 5 release at project creation. The Researcher pins the exact version in the roadmap, and the version is recorded in `NAMEC.uproject`.
2. Gameplay code is C++. Blueprints are used for content (enemy variants, item assets, UI layout), not for core system logic.
3. Stats, abilities, damage, and status effects are built on Unreal's Gameplay Ability System (GAS). Each character owns one `UAbilitySystemComponent`. Each enemy also owns one `UAbilitySystemComponent` with an enemy attribute set (see combat-loot spec).
4. The game module `Source/NAMEC/` is organized into subfolders: `Core/`, `Character/`, `Progression/`, `Crafting/`, `Survival/`, `World/`, `Combat/`, `Loot/`, `Inventory/`, `Multiplayer/`, `UI/`, `Save/`. Editor-only code (data validators) lives in a separate editor module `Source/NAMECEditor/`.
5. All balance numbers live in DataTables under `Content/Data/`, one or more tables per system, and are named `DT_<System>_<Purpose>` (e.g., `DT_Progression_SkillXPCurve`). Per-item values (weight, damage, insulation) live in item definition data assets (see `specs/inventory/inventory.md`). Both are editable without recompiling.
6. Save data is split into two independent save types:
   - `UNamecCharacterSave` — one per character, stored in the local user's save directory on the machine that owns the character. Contents: character GUID and name; race, sex, and `FNamecAppearance` (character-creation spec); stats and spent/unspent stat points, classes, class levels, skill XP, boss first-kill flags, and ability bar (character-progression spec); Job levels and XP (crafting-jobs spec); survival meter values (survival spec; Breath is not saved); every item instance, equipped slots, favorites, hotkeys, and sort choice (inventory spec); the dead-respawn flag, set by any save written while the character is Downed or dead (combat-loot spec Edge Case 1); and the remaining duration of each active timed effect (Weakened, meal buff, Wet, Salty).
   - `UNamecWorldSave` — one per world, stored on the host machine. Contents: world name, seed, size, voxel resolution (voxel-world spec Requirement 3), and world settings (friendly fire, LAN hosting, password); current in-game time of day and each region's current weather; terrain chunk edit deltas; placed building pieces, including crafting stations with their bound attachments and tier, and storage containers with their contents; shared world pickups (per-player loot actors are not saved); tree harvest states and forage timers; Loose Stick and Loose Stone pickup collected states and respawn timers; bed respawn points per character GUID; loot chest opened state per character GUID; and boss defeat flags. Character positions are not saved (multiplayer spec Requirement 16).
7. Every save type carries an integer `SaveVersion`. Loading a save with an older `SaveVersion` runs a migration function; loading a newer `SaveVersion` than the build supports is refused with a user-facing message.
8. The world autosaves every 5 minutes (tuning value, in `DT_Core_Save`) and on host exit. Character saves write on autosave, on disconnect, and on exit.
9. All menus and HUD are navigable with a gamepad alone. Keyboard and mouse is supported for local player 1 only.
10. Platform-specific calls (file paths, user accounts, controller pairing, on-screen text entry keyboard) go through an interface in `Source/NAMEC/Core/Platform/` so a console port swaps one implementation.
11. The main menu offers: New World, Load World, Join LAN Game, Characters (create/delete/view), Settings, Quit.
12. Camera supports first-person and third-person, toggled per local player with the camera toggle input (Requirement 14). The default is third-person.
13. Performance targets on the reference PC (NVIDIA RTX 3060-class GPU, 6-core CPU, 16 GB RAM) at 1080p: 60 fps with 1–2 local viewports, 30 fps with 3–4 local viewports, using the scalability presets in `DT_MP_SplitScreenScalability`.
14. All input uses Enhanced Input. Every binding below is the default and is remappable per local player in Settings. Chorded bindings (LB held + a button, Left Alt + RMB) take priority over the unchorded binding of the same button, so an unchorded action never fires while its chord modifier is held.

    | Action | Gamepad | Keyboard & mouse (local player 1 only) |
    |--------|---------|----------------------------------------|
    | Move | Left stick | W A S D |
    | Look | Right stick | Mouse |
    | Jump | A (when no interactable is targeted) | Space |
    | Interact (including revive hold and fishing bite) | A (when an interactable is targeted) | E |
    | Climb (hold against a surface steeper than 60°; `specs/character-progression/character-progression.md` Requirement 21) | Hold A | Hold Space |
    | Dodge roll | B (tap) | Left Shift (tap) |
    | Sprint | B (hold) | Left Shift (hold) |
    | Use selected consumable (`specs/inventory/inventory.md` Requirement 16) | X | F |
    | Toggle two-handing the Right Hand weapon | Y | R |
    | Light attack | RB | Left mouse button |
    | Heavy attack | RT | Middle mouse button |
    | Block (`specs/combat-loot/combat-loot.md` Requirement 4) | LT | Right mouse button |
    | Parry (`specs/combat-loot/combat-loot.md` Requirement 5) | LB + LT | Left Alt + right mouse button |
    | Abilities 1–4 (`specs/character-progression/character-progression.md` Requirement 18) | LB + A / B / X / Y | Z / X / C / V |
    | Abilities 5–6 | LB + RB / LB + RT | B / N |
    | Lock-on toggle (`specs/combat-loot/combat-loot.md` Requirement 6) | R3 | Tab |
    | Cycle lock-on target (while locked on) | Flick right stick | Flick mouse |
    | Crouch | L3 | Left Ctrl |
    | Camera first/third-person toggle (Requirement 12) | View (hold 0.5 s, tuning value in `DT_Core_Input`) | H |
    | Inventory and Character menu | View (tap) | I |
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
1. When a save file is corrupt (fails deserialization), the game shows "Save could not be loaded", keeps the file on disk untouched, and returns to the menu. It never overwrites the file.
2. When the game crashes mid-save, the previous save remains valid: saves write to a temp file, then atomically rename over the old file.
3. When a character save's `SaveVersion` is newer than the build, that character is shown greyed out in the character list with "Requires newer version".
4. When disk space is insufficient on save, the game shows a warning on the host HUD and retries on the next autosave interval. The in-memory state is kept.

## Acceptance Criteria
- [ ] Project opens in the pinned Unreal Engine 5 version and builds with no errors.
- [ ] `Source/NAMEC/` contains the subfolders listed in Requirement 4.
- [ ] Changing a value in any `DT_*` table changes in-game behavior without a C++ rebuild.
- [ ] A character created in World A can be loaded into World B with race, sex, appearance, stats, classes, skills, jobs, and inventory intact.
- [ ] Killing the process during a save leaves the previous save loadable.
- [ ] Every menu screen can be completed start-to-finish with a gamepad only.
- [ ] Camera toggles between first- and third-person per local player.
- [ ] Every default binding in Requirement 14 performs its action, a remapped binding persists after restart, and pressing LB + A casts ability 1 without jumping.

## Key Files
- `NAMEC.uproject` — new; project descriptor with pinned engine version and enabled plugins (GameplayAbilities, EnhancedInput, OnlineSubsystem).
- `Source/NAMEC/NAMEC.Build.cs` — new; module dependencies.
- `Source/NAMEC/Core/NamecGameInstance.h` — new; owns save loading, local player management, session lifecycle.
- `Source/NAMEC/Core/Platform/INamecPlatform.h` — new; platform abstraction interface.
- `Source/NAMEC/Save/NamecCharacterSave.h` — new; character save object with `SaveVersion` and the contents listed in Requirement 6.
- `Source/NAMEC/Save/NamecWorldSave.h` — new; world save object with `SaveVersion` and the contents listed in Requirement 6.
- `Source/NAMEC/Save/NamecSaveMigrations.cpp` — new; version migration functions.
- `Source/NAMEC/Character/NamecCameraComponent.h` — new; first/third-person toggle.
- `Content/Data/` — new; all `DT_*` tuning tables.
- `Content/Data/DT_Core_Save.uasset` — new; autosave interval and other save tuning values.
- `Content/Data/DT_Core_Input.uasset` — new; hold and tap thresholds (View hold for camera toggle, stick flick threshold) and other input tuning values.
- `Content/Input/IMC_Gamepad.uasset` — new; Enhanced Input mapping context with the Requirement 14 gamepad defaults.
- `Content/Input/IMC_KeyboardMouse.uasset` — new; Enhanced Input mapping context with the Requirement 14 keyboard and mouse defaults.
- `Content/Input/Actions/` — new; one `UInputAction` asset per Requirement 14 action.
- `Source/NAMEC/UI/Settings/InputRemapScreen/` — new; per-local-player binding remap screen in Settings.
