# Architecture

This page covers the engine and GAS approach, module layout, the C++ vs Blueprint rule, platform abstraction, performance targets, and every file named in the specs' Key Files.

← [Home](Home.md)

## Engine

- One Unreal Engine C++ project on **Unreal Engine 5.8, pinned at 5.8.2**, taking later 5.8 hotfixes. The version is recorded in `NAMEC.uproject`. Changing to another minor or major version is a spec change.
- Enabled plugins: GameplayAbilities, EnhancedInput, OnlineSubsystem, OnlineSubsystemNull, and the plugin of every adopted engine feature that ships as one (for example Common UI, PCG, Mutable, StateTree, Smart Objects, Motion Warping, MetaSounds). No Experimental feature ships in a core system. See [Engine and Rendering](Dev-Engine-and-Rendering.md).
- All input uses Enhanced Input, unified with Common UI. Bindings are remappable per local player. See [Co-op and Controls](Co-op-and-Controls.md).
- Terrain uses a **custom C++ voxel mesher** with non-Nanite chunk meshes lit by the rendering tier's GI method (Lumen Hardware Ray Tracing in the High tier). Voxel Plugin is not used. The mesh backend (Geometry Script / Dynamic Mesh or a procedural mesh component) is a Researcher implementation choice. See [Engine and Rendering](Dev-Engine-and-Rendering.md).

## Gameplay Ability System (GAS)

- Stats, abilities, damage and status effects are built on GAS.
- Each character owns one `UAbilitySystemComponent`. Each enemy also owns one, with `UNamecEnemyAttributeSet` (Health, MaxHealth, Poise, MaxPoise, Armor, resistances; Armor and extra resistances and Poise come from items a humanoid enemy has equipped).
- Player attributes are split across two sets:
  - `UNamecAttributeSet`: STR, DEX, CON, INT, WIS, CHA, Health, MaxHealth, HealthRegen, Mana, MaxMana, ManaRegen, MaxCarryWeight, MaxEquipLoad, Poise, MaxPoise, Armor, StatusResistance, and SlashResistance, PierceResistance, BluntResistance, FireResistance, FrostResistance, LightningResistance, PoisonResistance, HolyResistance, ShadowResistance.
  - `UNamecSurvivalAttributeSet`: Hunger, Thirst, BodyTemperature, Fatigue, Stamina, MaxStamina, StaminaRegen, Breath.
- Derived values recalculate via attribute-change callbacks, never polled per tick.
- The survival tick is a GAS periodic gameplay effect (1 s, starting value, tunable), not actor Tick.
- Class abilities are `UGameplayAbility` subclasses. Core combat abilities: `GA_LightAttack`, `GA_OffHandAttack`, `GA_HeavyAttack`, `GA_Dodge`, `GA_Block`, `GA_Parry`, `GA_Execute`.
- Town NPCs (`ANamecTownNPC`) subclass `ANamecEnemyBase` and use `UNamecEnemyAttributeSet`, so they share enemy GAS combat.
- Racial active abilities are also `UGameplayAbility` subclasses (`GA_Race_SecondWind`, `GA_Race_Pounce`, `GA_Race_RallyHowl`, `GA_Race_ShedSkin`, `GA_Race_SwingLeap`, `GA_Race_MaulingRoar`), granted by the server on spawn. They are not rows in `DT_Progression_ClassAbilities`: they cost no stamina or mana, have no RequiredStat and award no class XP.
- Racial passives and downsides are infinite-duration gameplay effects applied by the server on spawn and never removed.
- Poison, Bleed, Burn and Frostbite buildup and active effects carry the gameplay tag `Status.Negative.Cleansable` (removed by Sauren Shed Skin).
- Damage, incoming damage, poise damage, status buildup from attacks, weapons and weapon coatings, and healing are implemented in one GAS execution calculation (`NamecDamageExecution`).
- Gameplay effects named in the specs: `GE_Freezing`, `GE_Cold`, `GE_Hot`, `GE_Overheating`, `GE_Wet`, `GE_Salty`, starvation and dehydration effects, `GE_OverEncumbered`.

## C++ vs Blueprint

- Gameplay code and core system logic are C++.
- Blueprints are for content only: enemy variants, item assets, UI layout. Each boss's phases and attack set live in its own Blueprint subclass of `ANamecBossBase`.

## Data-driven tuning

- Every balance number lives in a DataTable under `Content/Data/`, named `DT_<System>_<Purpose>`, editable without recompiling. See [Data Tables](Dev-Data-Tables.md).
- Per-item values (weight, `Value`, damage, insulation, armor category, `Poise`, `PoiseDamage`, dye zones, quest item flag) live in item definition data assets.
- All numbers in the specs are starting values.

## Module folder layout

`Source/NAMEC/` has these subfolders: `Core/`, `Character/`, `Progression/`, `Crafting/`, `Survival/`, `World/`, `Combat/`, `Loot/`, `Inventory/`, `Multiplayer/`, `Factions/`, `EnemyAI/`, `UI/`, `Save/`.

Editor-only code (data validators) lives in a separate editor module, `Source/NAMECEditor/`.

## Enemy AI

- Every enemy and town NPC runs its row's StateTree **on the host only**. Clients get replicated movement, animation, AI state, level, Veteran name and title, and equipped gear visuals.
- States: Idle/Patrol, Investigate, Combat, Loot, Flee, Return, Raid. AI LOD uses StateTree scheduled ticking and a slower perception interval beyond 50 m from every player (starting value), and nothing simulates in unstreamed chunks.
- Hostility between groups comes from the `DT_EnemyAI_Hostility` matrix. Enemies pick up lootable shared world pickups through Smart Object claims, humanoid enemies equip by Item Score, and Veterans are saved by `UNamecVeteranSubsystem`. See [Enemies and AI](Enemies-and-AI.md).

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

Each machine uses the High tier (1–2 viewports) or Split tier (3–4 viewports) from `DT_MP_SplitScreenScalability`, capped by the user's machine-wide graphics settings (unchanged at 1–2 viewports, the lower of the user's setting and the Split value at 3–4). The targets are first measured by the benchmark milestone, which must pass before content production, and the voxel-world acceptance test measures them in the Temperate region. See [Engine and Rendering](Dev-Engine-and-Rendering.md).

## Key files

### Project root and config

| File | Purpose |
|------|---------|
| `NAMEC.uproject` | Project descriptor with the pinned engine version (5.8) and enabled plugins |
| `Config/DefaultEngine.ini` | OnlineSubsystemNull configuration for LAN; renderer settings for Lumen with hardware ray tracing, Virtual Shadow Maps, Substrate and Nanite |
| `Source/NAMEC/NAMEC.Build.cs` | Module dependencies |

### `Source/NAMEC/Core/`

| File | Purpose |
|------|---------|
| `NamecGameInstance.h` | Owns save loading, local player management, session lifecycle |
| `NamecGameMode.h` | Player registration, capacity checks, spawning |
| `Platform/INamecPlatform.h` | Platform abstraction interface |
| `Benchmark/NamecBenchmarkDirector.h` | Scripted split-screen benchmark run at 1–4 viewports and measurement capture |

### `Source/NAMEC/Character/`

| File | Purpose |
|------|---------|
| `NamecCameraComponent.h` | First/third-person toggle |
| `Creation/NamecCharacterCreationWidget.h` | Creation step screens and navigation |
| `Creation/NamecCharacterDraft.h` | In-progress creation state struct (`FNamecCharacterDraft`) |
| `Creation/NamecCharacterFactory.h` | Name and option validation, starting stats, GUID, racial ability placement, equipped starting items, save creation |
| `Creation/NamecCharacterPreview.h` | Preview actor with rotate, zoom and preview outfits |
| `Appearance/NamecAppearance.h` | `FNamecAppearance` struct (race ID, sex, one value per option) |
| `Appearance/NamecAppearanceComponent.h` | Builds body and equipment visuals from race, sex and appearance; replicates appearance |
| `Races/NamecRaceDefinition.h` | `DT_Character_Races` row struct |
| `Races/NamecNightEyesComponent.h` | Owning-viewport night vision post-process, night and underground activation, Settings toggle (saved per local player slot) |
| `Races/Abilities/` | `GA_Race_SecondWind`, `GA_Race_Pounce`, `GA_Race_RallyHowl`, `GA_Race_ShedSkin`, `GA_Race_SwingLeap`, `GA_Race_MaulingRoar` |

### `Source/NAMEC/Progression/`

| File | Purpose |
|------|---------|
| `NamecAttributeSet.h` | GAS attributes for the six stats, derived values, Armor, StatusResistance, per-damage-type resistances |
| `NamecProgressionComponent.h` | XP awarding, level-ups, stat points, class slots, broadcasts |
| `NamecClassDefinition.h` | DataTable row struct for classes, with allowed armor and weapon categories |
| `NamecSkillTypes.h` | Skill IDs, XP source row structs |

### `Source/NAMEC/Crafting/`

| File | Purpose |
|------|---------|
| `NamecJobComponent.h` | Per-character Job XP, levels, perks |
| `NamecCraftingStation.h` | Station actor (subclass of `ANamecBuildPiece`), attachment binding and tier, craft queues, material reservation |
| `NamecRecipeTypes.h` | Recipe and Job DataTable row structs |
| `NamecEnchantingService.h` | Affix add/reroll logic, including adding an affix to Common jewelry (which makes it Magic), and `ServerApplyRune` validation, rune consumption and no Job XP |
| `NamecRecipeProvenance.h` | Stores the recipe row ID on crafted items and resolves an item's or placed piece's provenance recipe (stored row, first recipe row for the item, or none) for refunds, destroyed-piece drops and repair |
| `NamecDyeTypes.h` | Dye zone enum (Primary, Secondary, Accent, Trim) and `DT_Crafting_DyeColors` row struct |

### `Source/NAMEC/Survival/`

| File | Purpose |
|------|---------|
| `NamecSurvivalAttributeSet.h` | Hunger, Thirst, BodyTemperature, Fatigue, Stamina, MaxStamina, StaminaRegen, Breath |
| `NamecSurvivalComponent.h` | Per-tick survival computation and status effects, drinking from water, waterskin refill and Use |
| `NamecShelterQuery.h` | Shelter detection against voxel data |
| `NamecHeatSourceComponent.h` | Attachable heat source for fires, forges, torches |
| `NamecSleepSubsystem.h` | Bed tracking, entering and leaving bed, and time skip |

### `Source/NAMEC/World/`

| File | Purpose |
|------|---------|
| `Map/NamecVoxelMapAsset.h` | The baked map asset: map id, revision, per-chunk base hashes, extent, bedrock depth, authoring seed, compressed base voxel data |
| `NamecVoxelWriteKernel.h` | The one sphere-edit writer, shared by runtime dig/fill and the bake's stroke replay |
| `NamecMapFunctionLibrary.h` | Per-chunk PCG seed and authored exclusion volumes for PCG graphs |
| `Structures/` | Authored structure actors: town, camp, ruin, boss arena, spawn point, chest point, water volume, expansion region, and the structure id component |
| `Save/NamecWorldLoader.h` | Map id, revision and hash load rule; fills pending terrain markers |
| `NAMECEditor/Map/` | Bake commandlet and stateless bake passes, sculpt brush editor mode, map validator |
| `NamecVoxelWorld.h` | Voxel chunk storage, streaming, and the custom C++ mesher producing non-Nanite chunk meshes with tight bounds |
| `NamecTerrainEditComponent.h` | Dig/fill/mine requests and validation, shovel Dig/Fill mode and selected fill material (Dig and Soil defaults, saved per character, "No <material>" and "Not enough <material>") |
| `NamecDigDepthQuery.h` | Per-column lookup of the nearest base air voxel in the baked map, used for the dig depth limit |
| `NamecClimateSubsystem.h` | Region lookup, day/night, weather |
| `NamecTreeActor.h` | Tree health, felling (swap to a simulating actor with the same Nanite mesh), the stump actor and its shovel removal (Wood and Woodcutting XP, not a terrain edit), regrowth at the stump's origin |
| `NamecForageNode.h` | Harvestable plants |
| `Building/NamecBuildPiece.h` | Placeable piece actor with snap points, health, and its provenance recipe row ID (used for deconstruct refunds and destroyed-piece drops) |
| `Building/NamecBuildComponent.h` | Placement mode and preview, rotation, snap, deconstruction hold (placement mode only), staying in placement mode while the piece's item remains |
| `Building/NamecDoorPiece.h` | Door build piece: Interact open/close toggle, replicated and saved open state, closed-door navmesh area impassable for enemies, raiders and NPCs |
| `Building/NamecTrainingDummyPiece.h` | Training dummy build piece that registers player hits without losing piece health |
| `Building/NamecMirrorPiece.h` | Mirror build piece that opens the appearance editor |
| `Building/NamecDyeStationPiece.h` | Dye Station build piece that opens the dye screen and validates dye requests |
| `Hazards/NamecHazardVolume.h` | Poison Water and Lava contact effects (Poison buildup, Fire damage, Burn buildup) |
| `Hazards/NamecPoisonPlant.h` | Poison Plant hazard actor applying Poison buildup on contact |

### `Source/NAMEC/Combat/`

| File | Purpose |
|------|---------|
| `NamecCombatComponent.h` | Lock-on, poise, guard, parry state, block and parry item selection and LT action per hand configuration, two-handing state with the stowed Left Hand item (ends on a Right Hand change or Left Hand equip; not saved) |
| `NamecDamageExecution.h` | GAS execution calculation: outgoing damage (including two-handing and arrow damage), incoming enemy damage and poise damage (including the block poise multiplier), status buildup from enemy and Guard attacks, weapons and weapon coatings, healing |
| `Abilities/GA_BowAim.h`, `Abilities/GA_BowFire.h` | Bow aim and fire (aimed and quick shots), bow shot stamina cost, arrow consumption, "No arrows" |
| `NamecProjectile.h` | Server-spawned replicated projectile that resolves bow hits, plus the client's cosmetic predicted projectile |
| `Abilities/` | `GA_LightAttack`, `GA_OffHandAttack`, `GA_HeavyAttack`, `GA_Dodge`, `GA_Block`, `GA_Parry` |
| `Abilities/GA_Execute.h` | Plays the paired execution montages and applies the kill at the kill notify |
| `NamecExecutionComponent.h` | Execution candidate detection, owning-viewport prompt, server validation, invulnerability, rewards |
| `NamecExecutionCamera.h` | Third-person execution camera used for first-person executions |
| `NamecDownedComponent.h` | Downed state, bleed-out, revive interaction |
| `AI/NamecEnemyBase.h` | Enemy base character with threat table, taunt forced-target handling, and perception detection; hosts the enemy AI, loot, equipment and progression components |
| `AI/NamecEnemyAttributeSet.h` | Enemy GAS attributes (Health, MaxHealth, Poise, MaxPoise, Armor, resistances) |
| `AI/NamecBossBase.h` | Boss base with phases, arena barrier, summon |

### `Source/NAMEC/Loot/`

| File | Purpose |
|------|---------|
| `NamecLootSubsystem.h` | Per-player drop rolling, including jewelry rolling Magic or above |
| `NamecItemInstance.h` | Item instance with rarity, affixes, durability (none for jewelry), item level, dye colors, consumable potency value, rune tier, waterskin remaining drinks, provenance recipe row ID, weapon coating (status, buildup amount, remaining hits) |
| `NamecLootPickup.h` | Per-player pickup actor (owner-only relevance, rendering and pickup), shared world pickup mode, gold amount, lootable flag and Smart Object slot, and conversion of per-player loot into shared pickups after 2 real-time minutes |
| `NamecLootChest.h` | Generated loot chest with per-character-GUID opened state and per-player rolls |

### `Source/NAMEC/Inventory/`

| File | Purpose |
|------|---------|
| `NamecInventoryComponent.h` | Item storage (one row per item definition plus potency value, rune tier, waterskin remaining drinks and provenance recipe row ID), poison and weapon oil Use coating the Right Hand weapon, weight, equip slots, favorites, hotkeys |
| `NamecItemCategories.h` | Armor category enum and weapon category enum (hand use, trained skill) |
| `NamecItemDefinition.h` | Item definition data asset (weight, `Value`, category, slot, armor category, weapon category, `Poise` for armor, `PoiseDamage`, damage type and optional status buildup for weapons, coating buildup and coating hit count for poisons and weapon oils, block percentage and parry-capable flag for weapons and shields, `WeaponBase` for weapons and every tool except the Fishing Rod, `ArrowDamage` for arrows, Insulation or Cooling for capes, `DrinkCapacity` for waterskins, jewelry's no-durability flag, dye zones and mask channels, quest item flag, effects) |
| `NamecContainerActor.h` | Placeable storage container (subclass of `ANamecBuildPiece`) with weight capacity |

### `Source/NAMEC/Multiplayer/`

| File | Purpose |
|------|---------|
| `NamecSessionSubsystem.h` | Host/find/join LAN sessions, password, version check |
| `NamecLocalPlayerManager.h` | Controller join prompt, local player add/remove, controller disconnect handling |
| `NamecCharacterPayload.h` | Serializable character state struct for join and save sync, including race, sex, `FNamecAppearance`, item dye colors, current Health and Mana, gold, reputation and quest state |
| `NamecSplitScreenLayout.h` | Viewport layout rules for 1–4 players |
| `NamecScalabilitySubsystem.h` | Picks the High or Split rendering tier from the machine's local viewport count and applies the user's graphics settings unchanged (High) or min(user setting, Split value) per setting (Split), recomputing on a settings change |

### `Source/NAMEC/Factions/`

| File | Purpose |
|------|---------|
| `NamecFactionTypes.h` | Faction IDs, reputation tiers, and the `DT_Factions_Factions`, `DT_Factions_NPCs` and `DT_Factions_Rules` row structs |
| `NamecReputationComponent.h` | Per-character reputation values, tier computation, NPC harm losses and cooldowns, daily Hostile recovery, fine payment |
| `NamecCurrencyComponent.h` | Per-character gold counter, clamping, Drop Gold |
| `NamecTownSubsystem.h` | Town and camp registry, town protected radius and camp radius queries, NPC spawning and respawn timers, Vendor stock and restock, Quest Board refresh |
| `NamecTownNPC.h` | `ANamecTownNPC` (subclass of `ANamecEnemyBase`) with Guard, Vendor, Quest Giver, Citizen and Guard Captain behavior, and the post-fine Guard truce |
| `NamecVendorComponent.h` | Buy and sell validation and price calculation |
| `NamecQuestBoard.h` | `ANamecQuestBoard` interactable with per-world offers |
| `NamecQuestComponent.h` | Per-character quest log, progress, tracked flags, questline steps, abandon, Escort failure and completion rules |
| `NamecQuestSubsystem.h` | Offer rolling, kill and camp credit, turn-in rewards |
| `NamecCampActor.h` | Bandit and Beastmen camp spawn points, cleared state, respawn timer |
| `NamecRaidSubsystem.h` | Base detection, base value, real-time raid roll clock and roll, raider source and band selection, spawning, the raid's building piece snapshot and merges, retreat and end |

### `Source/NAMEC/EnemyAI/`

| File | Purpose |
|------|---------|
| `NamecEnemyAIComponent.h` | StateTree host, home position, home radius and leash, the seven AI states (including a raider attacking the first building piece blocking its path to an unreachable target), replicated AI state, AI LOD perception intervals |
| `NamecHostilityQuery.h` | Hostility group lookup, `DT_EnemyAI_Hostility` cell lookup, hostile-target and damage filter |
| `NamecEnemyLootComponent.h` | Lootable pickup perception, Smart Object claims, pickup requests, carried items and gold, drops on death, despawn and world save |
| `NamecEnemyEquipmentComponent.h` | Humanoid equipment slots, Item Score, equip decision, attribute changes, the wielded weapon's own status buildup on weapon attacks, Mutable-built equipped visuals |
| `NamecEnemyProgressionComponent.h` | Enemy XP sources, gained levels, per-level multipliers, current level, level-up event, name plate data |
| `NamecVeteranSubsystem.h` | Veteran promotion, caps, names and titles, records and save/load, home spawning and home validity, camp Veterans, raid leader selection |
| `StateTree/` | StateTree tasks, conditions and evaluators for the AI states |

### `Source/NAMEC/Save/`

| File | Purpose |
|------|---------|
| `NamecCharacterSave.h` | Character save object with `SaveVersion` |
| `NamecWorldSave.h` | World save object with `SaveVersion` |
| `NamecSettingsSave.h` | Machine-local settings save object with `SaveVersion`: one section per local player slot (1–4) and one machine-wide section |
| `NamecSaveMigrations.cpp` | Version migration functions for all three save types |

### `Source/NAMEC/UI/`

| Folder | Purpose |
|--------|---------|
| `CharacterScreen/` | Stat point spending, class selection, ability bar |
| `Crafting/` | Station UI, queue display, hand-crafting menu, dye screen with character preview, Enchanting Altar enchanting screen |
| `Inventory/` | Inventory list, detail panel, transfer view, Favorites quick menu, footer gold display and Drop Gold action |
| `Building/` | Hammer build menu |
| `Vendor/` | Vendor screen |
| `GuardCaptain/` | Guard Captain fine screen |
| `Quests/` | Quest board screen, quest giver screen, Quests tab with Reputation section, HUD quest tracker |
| `HUD/NamecExecutionPromptWidget.h` | Per-viewport execution prompt |
| `HUD/NamecEnemyNamePlateWidget.h` | Per-viewport enemy name plate with level, and Veteran name and title |
| `Lobby/` | Join LAN Game list, character select per local player |
| `Settings/InputRemapScreen/` | Per-local-player binding remap screen, saving to that local player slot's section of the settings save |
| `OnScreenKeyboard/NamecOnScreenKeyboardWidget.h` | Per-viewport gamepad on-screen keyboard for name entry, used through `INamecPlatform` |

### `Source/NAMECEditor/` (editor-only module)

| File | Purpose |
|------|---------|
| `NamecWearableVariantValidator.h` | Data validator: fails the content build when a wearable item is missing any of the 12 race × sex body variants, a required ear/frill/crest/mane/hair or tail visibility setting, or (hand armor) any of the 12 first-person variants, or when a defined dye zone is missing from any variant's material mask, or when a humanoid `DT_EnemyAI_SkeletonFamilies` row has no valid race × sex body, an out-of-range appearance preset, or no IK Retargeter asset |
| `NamecHostilityValidator.h` | Data validator: fails the content build when `DT_EnemyAI_Hostility` is missing a group, is not symmetric for Hostile cells between fighting groups, or misuses Flee, Reputation or the Town NPC row |
| `NamecVendorStockValidator.h` | Data validator: fails the content build when a `DT_Factions_VendorStock` row names a boss material |
| `NamecCampEnemyValidator.h` | Data validator: fails the content build when a Bandits or Beastmen camp density above 0 has no matching enemy row for that region |
| `NamecNaniteAuthoringValidator.h` | Data validator: fails the content build when a Nanite mesh uses translucency, Lighting Channels, unclamped WPO or Nanite Tessellation |

### Content

| Path | Purpose |
|------|---------|
| `Content/Data/` | All `DT_*` tuning tables (see [Data Tables](Dev-Data-Tables.md)) |
| `Content/Input/IMC_Gamepad.uasset` | Enhanced Input mapping context with gamepad defaults |
| `Content/Input/IMC_KeyboardMouse.uasset` | Enhanced Input mapping context with keyboard and mouse defaults |
| `Content/Input/Actions/` | One `UInputAction` asset per input action |
| `Content/Input/Contexts/` | Higher-priority mapping contexts for tool, shovel, Hammer, placement mode, bow, Fishing Rod, bite window, torch, Left Hand weapon, Left Hand torch, bed and aiming-at-water bindings, added only while each context is active |
| `Content/Survival/Effects/` | `GE_Freezing`, `GE_Cold`, `GE_Hot`, `GE_Overheating`, `GE_Wet`, `GE_Salty`, starvation and dehydration effects |
| `Content/Inventory/Effects/GE_OverEncumbered.uasset` | Over-Encumbered gameplay effect |
| `Content/Character/Races/Effects/` | Passive and downside gameplay effects for all six races |
| `Content/Character/Mutable/` | Mutable assets for race bodies, appearance options, the 12 body variants of every wearable, and materials that keep dye zones as runtime parameters |
| `Content/AI/Enemies/` | StateTree assets for enemies, bosses, Bandits, Beastmen and raiders |
| `Content/AI/TownNPCs/` | StateTree and Smart Object assets for town and escort NPCs |
| `Content/AI/SmartObjects/` | Smart Object definition for lootable pickups |
| `Content/World/PCG/` | Runtime PCG graphs for placement, seeded per chunk from the map's authoring seed |
| `Content/Map/Source/` | Versioned map inputs: heightfield tiles, masks, stroke log, bake settings |
| `Content/Map/Baked/` | Gitignored bake output (the map assets) |
| `Content/Placeholder/` | Gitignored third-party placeholder art |
| `Content/Maps/Benchmark/L_Benchmark_SplitScreen.umap` | Benchmark milestone scene |

## Source specs

- [Game Foundation](../../specs/game-foundation/game-foundation.md), [Engine Tech](../../specs/engine-tech/engine-tech.md), [Enemy AI](../../specs/enemy-ai/enemy-ai.md), and the Key Files section of every system spec (see [Specs and Pipeline](Dev-Specs-and-Pipeline.md))
