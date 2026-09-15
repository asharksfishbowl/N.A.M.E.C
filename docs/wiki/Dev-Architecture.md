# Architecture

This page covers the engine and GAS approach, module layout, the C++ vs Blueprint rule, platform abstraction, performance targets, and every file named in the specs' Key Files.

← [Home](Home.md)

## Engine

- One Unreal Engine 5 C++ project, on the latest stable UE5 release at project creation. The Researcher pins the exact version in the roadmap, and it is recorded in `NAMEC.uproject`.
- Enabled plugins: GameplayAbilities, EnhancedInput, OnlineSubsystem.
- All input uses Enhanced Input. Bindings are remappable per local player. See [Co-op and Controls](Co-op-and-Controls.md).
- Terrain technology (Voxel Plugin vs custom implementation) is a Researcher decision recorded in the roadmap before any world code is written. Criteria: split-screen performance with 4 viewports, network replication of edits, save size, license cost, console portability.

## Gameplay Ability System (GAS)

- Stats, abilities, damage and status effects are built on GAS.
- Each character owns one `UAbilitySystemComponent`. Each enemy also owns one, with `UNamecEnemyAttributeSet`.
- Player attributes are split across two sets:
  - `UNamecAttributeSet`: STR, DEX, CON, INT, WIS, CHA, Health, MaxHealth, HealthRegen, Mana, MaxMana, ManaRegen, MaxCarryWeight, MaxEquipLoad, Poise, MaxPoise, Armor, StatusResistance, and SlashResistance, PierceResistance, BluntResistance, FireResistance, FrostResistance, LightningResistance, PoisonResistance, HolyResistance, ShadowResistance.
  - `UNamecSurvivalAttributeSet`: Hunger, Thirst, BodyTemperature, Fatigue, Stamina, MaxStamina, StaminaRegen, Breath.
- Derived values recalculate via attribute-change callbacks, never polled per tick.
- The survival tick is a GAS periodic gameplay effect (1 s, starting value, tunable), not actor Tick.
- Class abilities are `UGameplayAbility` subclasses. Core combat abilities: `GA_LightAttack`, `GA_HeavyAttack`, `GA_Dodge`, `GA_Block`, `GA_Parry`.
- Racial active abilities are also `UGameplayAbility` subclasses (`GA_Race_SecondWind`, `GA_Race_Pounce`, `GA_Race_RallyHowl`, `GA_Race_ShedSkin`, `GA_Race_SwingLeap`, `GA_Race_MaulingRoar`), granted by the server on spawn. They are not rows in `DT_Progression_ClassAbilities`: they cost no stamina or mana, have no RequiredStat and award no class XP.
- Racial passives and downsides are infinite-duration gameplay effects applied by the server on spawn and never removed.
- Poison, Bleed, Burn and Frostbite buildup and active effects carry the gameplay tag `Status.Negative.Cleansable` (removed by Sauren Shed Skin).
- Damage, incoming damage, poise damage and healing are implemented in one GAS execution calculation (`NamecDamageExecution`).
- Gameplay effects named in the specs: `GE_Freezing`, `GE_Cold`, `GE_Hot`, `GE_Overheating`, `GE_Wet`, `GE_Salty`, starvation and dehydration effects, `GE_OverEncumbered`.

## C++ vs Blueprint

- Gameplay code and core system logic are C++.
- Blueprints are for content only: enemy variants, item assets, UI layout. Each boss's phases and attack set live in its own Blueprint subclass of `ANamecBossBase`.

## Data-driven tuning

- Every balance number lives in a DataTable under `Content/Data/`, named `DT_<System>_<Purpose>`, editable without recompiling. See [Data Tables](Dev-Data-Tables.md).
- Per-item values (weight, damage, insulation, armor category, `Poise`, `PoiseDamage`) live in item definition data assets.
- All numbers in the specs are starting values.

## Module folder layout

`Source/NAMEC/` has these subfolders: `Core/`, `Character/`, `Progression/`, `Crafting/`, `Survival/`, `World/`, `Combat/`, `Loot/`, `Inventory/`, `Multiplayer/`, `UI/`, `Save/`.

Editor-only code (data validators) lives in a separate editor module, `Source/NAMECEditor/`.

## Platform abstraction

- Platform-specific calls (file paths, user accounts, controller pairing, on-screen text entry keyboard) go through `Source/NAMEC/Core/Platform/INamecPlatform.h`, so a console port swaps one implementation.
- Gamepad name entry uses an on-screen keyboard rendered inside the owning local player's viewport, through `INamecPlatform`, so consoles can swap in the platform keyboard. The UI font covers at least Latin Extended-A.
- The game is fully playable with a gamepad only. Every menu can be completed with a gamepad.
- A console port should need only a new platform session implementation. Console builds themselves are out of scope.

## Performance targets

Reference PC: NVIDIA RTX 3060-class GPU, 6-core CPU, 16 GB RAM, at 1080p.

| Local viewports | Target |
|-----------------|--------|
| 1–2 | 60 fps |
| 3–4 | 30 fps |

Scalability presets per viewport count come from `DT_MP_SplitScreenScalability`. The voxel-world acceptance test measures this in the Temperate region.

## Key files

### Project root and config

| File | Purpose |
|------|---------|
| `NAMEC.uproject` | Project descriptor with pinned engine version and enabled plugins |
| `Config/DefaultEngine.ini` | OnlineSubsystemNull configuration for LAN |
| `Source/NAMEC/NAMEC.Build.cs` | Module dependencies |

### `Source/NAMEC/Core/`

| File | Purpose |
|------|---------|
| `NamecGameInstance.h` | Owns save loading, local player management, session lifecycle |
| `NamecGameMode.h` | Player registration, capacity checks, spawning |
| `Platform/INamecPlatform.h` | Platform abstraction interface |

### `Source/NAMEC/Character/`

| File | Purpose |
|------|---------|
| `NamecCameraComponent.h` | First/third-person toggle |
| `Creation/NamecCharacterCreationWidget.h` | Creation step screens and navigation |
| `Creation/NamecCharacterDraft.h` | In-progress creation state struct (`FNamecCharacterDraft`) |
| `Creation/NamecCharacterFactory.h` | Name and option validation, starting stats, GUID, racial ability placement, save creation |
| `Creation/NamecCharacterPreview.h` | Preview actor with rotate, zoom and preview outfits |
| `Appearance/NamecAppearance.h` | `FNamecAppearance` struct (race ID, sex, one value per option) |
| `Appearance/NamecAppearanceComponent.h` | Builds body and equipment visuals from race, sex and appearance; replicates appearance |
| `Races/NamecRaceDefinition.h` | `DT_Character_Races` row struct |
| `Races/NamecNightEyesComponent.h` | Owning-viewport night vision post-process, night and underground activation, Settings toggle |
| `Races/Abilities/` | `GA_Race_SecondWind`, `GA_Race_Pounce`, `GA_Race_RallyHowl`, `GA_Race_ShedSkin`, `GA_Race_SwingLeap`, `GA_Race_MaulingRoar` |

### `Source/NAMEC/Progression/`

| File | Purpose |
|------|---------|
| `NamecAttributeSet.h` | GAS attributes for the six stats, derived values, Armor, StatusResistance, per-damage-type resistances |
| `NamecProgressionComponent.h` | XP awarding, level-ups, stat points, class slots, broadcasts |
| `NamecClassDefinition.h` | DataTable row struct for classes |
| `NamecSkillTypes.h` | Skill IDs, XP source row structs |

### `Source/NAMEC/Crafting/`

| File | Purpose |
|------|---------|
| `NamecJobComponent.h` | Per-character Job XP, levels, perks |
| `NamecCraftingStation.h` | Station actor (subclass of `ANamecBuildPiece`), attachment binding and tier, craft queues, material reservation |
| `NamecRecipeTypes.h` | Recipe and Job DataTable row structs |
| `NamecEnchantingService.h` | Affix add/reroll logic |

### `Source/NAMEC/Survival/`

| File | Purpose |
|------|---------|
| `NamecSurvivalAttributeSet.h` | Hunger, Thirst, BodyTemperature, Fatigue, Stamina, MaxStamina, StaminaRegen, Breath |
| `NamecSurvivalComponent.h` | Per-tick survival computation and status effects |
| `NamecShelterQuery.h` | Shelter detection against voxel data |
| `NamecHeatSourceComponent.h` | Attachable heat source for fires, forges, torches |
| `NamecSleepSubsystem.h` | Bed tracking and time skip |

### `Source/NAMEC/World/`

| File | Purpose |
|------|---------|
| `NamecWorldGenerator.h` | Seeded region layout, heightfield, caves, placement |
| `NamecVoxelWorld.h` | Voxel chunk storage, streaming, meshing (or wrapper around Voxel Plugin) |
| `NamecTerrainEditComponent.h` | Dig/fill/mine requests and validation |
| `NamecClimateSubsystem.h` | Region lookup, day/night, weather |
| `NamecTreeActor.h` | Tree health, felling, regrowth |
| `NamecForageNode.h` | Harvestable plants |
| `Building/NamecBuildPiece.h` | Placeable piece actor with snap points and health |
| `Building/NamecBuildComponent.h` | Placement preview, snap, deconstruction |
| `Building/NamecMirrorPiece.h` | Mirror build piece that opens the appearance editor |
| `Hazards/NamecHazardVolume.h` | Poison Water and Lava contact effects (Poison buildup, Fire damage, Burn buildup) |
| `Hazards/NamecPoisonPlant.h` | Poison Plant hazard actor applying Poison buildup on contact |

### `Source/NAMEC/Combat/`

| File | Purpose |
|------|---------|
| `NamecCombatComponent.h` | Lock-on, poise, guard, parry state |
| `NamecDamageExecution.h` | GAS execution calculation: outgoing damage, incoming enemy damage and poise damage, healing |
| `Abilities/` | `GA_LightAttack`, `GA_HeavyAttack`, `GA_Dodge`, `GA_Block`, `GA_Parry` |
| `NamecDownedComponent.h` | Downed state, bleed-out, revive interaction |
| `AI/NamecEnemyBase.h` | Enemy base character with threat table, taunt forced-target handling, and perception detection |
| `AI/NamecEnemyAttributeSet.h` | Enemy GAS attributes (Health, MaxHealth, Poise, MaxPoise, resistances) |
| `AI/NamecBossBase.h` | Boss base with phases, arena barrier, summon |

### `Source/NAMEC/Loot/`

| File | Purpose |
|------|---------|
| `NamecLootSubsystem.h` | Per-player drop rolling |
| `NamecItemInstance.h` | Item instance with rarity, affixes, durability, item level |
| `NamecLootPickup.h` | Per-player pickup actor (owner-only relevance, rendering and pickup) and shared world pickup mode |
| `NamecLootChest.h` | Generated loot chest with per-character-GUID opened state and per-player rolls |

### `Source/NAMEC/Inventory/`

| File | Purpose |
|------|---------|
| `NamecInventoryComponent.h` | Item storage, weight, equip slots, favorites, hotkeys |
| `NamecItemDefinition.h` | Item definition data asset (weight, category, slot, armor category, `Poise` for armor, `PoiseDamage` for weapons, effects) |
| `NamecContainerActor.h` | Placeable storage container (subclass of `ANamecBuildPiece`) with weight capacity |

### `Source/NAMEC/Multiplayer/`

| File | Purpose |
|------|---------|
| `NamecSessionSubsystem.h` | Host/find/join LAN sessions, password, version check |
| `NamecLocalPlayerManager.h` | Controller join prompt, local player add/remove, controller disconnect handling |
| `NamecCharacterPayload.h` | Serializable character state struct for join and save sync, including race, sex and `FNamecAppearance` |
| `NamecSplitScreenLayout.h` | Viewport layout rules for 1–4 players |

### `Source/NAMEC/Save/`

| File | Purpose |
|------|---------|
| `NamecCharacterSave.h` | Character save object with `SaveVersion` |
| `NamecWorldSave.h` | World save object with `SaveVersion` |
| `NamecSaveMigrations.cpp` | Version migration functions |

### `Source/NAMEC/UI/`

| Folder | Purpose |
|--------|---------|
| `CharacterScreen/` | Stat point spending, class selection, ability bar |
| `Crafting/` | Station UI, queue display, hand-crafting menu |
| `Inventory/` | Inventory list, detail panel, transfer view, Favorites quick menu |
| `Lobby/` | Join LAN Game list, character select per local player |
| `Settings/InputRemapScreen/` | Per-local-player binding remap screen |
| `OnScreenKeyboard/NamecOnScreenKeyboardWidget.h` | Per-viewport gamepad on-screen keyboard for name entry, used through `INamecPlatform` |

### `Source/NAMECEditor/` (editor-only module)

| File | Purpose |
|------|---------|
| `NamecWearableVariantValidator.h` | Data validator: fails the content build when a wearable item is missing any of the 12 race × sex body variants, a required ear/frill/crest/mane or tail visibility setting, or (hand armor) any of the 12 first-person variants |

### Content

| Path | Purpose |
|------|---------|
| `Content/Data/` | All `DT_*` tuning tables (see [Data Tables](Dev-Data-Tables.md)) |
| `Content/Input/IMC_Gamepad.uasset` | Enhanced Input mapping context with gamepad defaults |
| `Content/Input/IMC_KeyboardMouse.uasset` | Enhanced Input mapping context with keyboard and mouse defaults |
| `Content/Input/Actions/` | One `UInputAction` asset per input action |
| `Content/Survival/Effects/` | `GE_Freezing`, `GE_Cold`, `GE_Hot`, `GE_Overheating`, `GE_Wet`, `GE_Salty`, starvation and dehydration effects |
| `Content/Inventory/Effects/GE_OverEncumbered.uasset` | Over-Encumbered gameplay effect |
| `Content/Character/Races/Effects/` | Passive and downside gameplay effects for all six races |

## Source specs

- [Game Foundation](../../specs/game-foundation/game-foundation.md) and the Key Files section of every system spec (see [Specs and Pipeline](Dev-Specs-and-Pipeline.md))
