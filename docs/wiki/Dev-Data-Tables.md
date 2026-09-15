# Data Tables

This page lists every `DT_*` DataTable named in the specs, grouped by system, with its purpose and the columns the specs mention.

← [Home](Home.md)

## Conventions

- All tables live in `Content/Data/` and are named `DT_<System>_<Purpose>`.
- Every numeric balance value is a starting value in a table and is editable without a C++ rebuild.
- Each system has a catch-all rules table: a value the specs mark "tuning value" without naming a table lives in that system's rules table (`DT_Character_Races`, `DT_Progression_Rules`, `DT_Crafting_Rules`, `DT_Survival_Penalties`, `DT_World_Building`, `DT_Combat_Rules`, `DT_Inventory_Rules`, `DT_MP_Session`, `DT_Factions_Rules`, `DT_Engine_Benchmark`, `DT_EnemyAI_Rules`).
- Per-item values (weight, `Value`, dye zones, quest item flag, damage, `ArrowDamage` for arrows, insulation, armor category, `Poise`, `PoiseDamage`, bag bonus, food/drink restore values, block percentage, stat requirements, scaling stat and grade) are on item definition data assets, not DataTables.
- "Columns mentioned" lists only what the specs name. Tables will have more once implemented.

## Core (game-foundation)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Core_Save` | Save tuning | Autosave interval (5 min) |
| `DT_Core_Input` | Input thresholds | View hold time for camera toggle (0.5 s), Hammer deconstruct hold (1 s), waterskin refill hold (1 s), stick flick threshold |

## Character (character-creation)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Character_Races` | Six race rows, plus character creation tuning | Race ID, display name, description, stat bonuses, passive trait gameplay effect, active ability class, downside gameplay effect, male and female skeletal mesh sets, male and female first-person arm meshes, supported appearance options, collision capsule size; all race trait values (e.g. Versatile +10% skill XP, Fleeting Vigor ×1.15 Fatigue, Night Eyes 8 m fall exemption and 20:00–06:00 night hours, Amphibious ×3 Breath, ×1.3 swim speed, +0.25 PoisonResistance, ×0.75 Poison buildup, Water-Averse ×0.7 swim speed, ×1.5 swim stamina, ×2 Wet, Climber ×0.5 climb stamina, ×1.5 climb speed, Loud ×1.25 perception radius, Cold-Blooded +6 °C, Heat-Prone −6 °C, Thick Hide +10 Insulation and +50 MaxCarryWeight, Light Frame ×0.85 MaxEquipLoad); racial ability values and cooldowns (Pounce 8 m, 60 poise; Rally Howl 20 m, +25%, 20 s and CHA coefficients; Mauling Roar 6 m, 100 poise, 6 s taunt, Boss 3 s taunt and 25% poise); Mirror interaction range |
| `DT_Character_AppearanceOptions` | Appearance option ranges | Per-race (and per-sex where options differ) preset counts and slider ranges (e.g. body type 3, height ±8%, face 8 per race and sex, eye color 12) |

## Progression (character-progression)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Progression_Classes` | 11 class rows | Stat bonuses (starting class only), ability list, allowed armor categories, allowed weapon categories (per-class lists on [Stats and Classes](Stats-and-Classes.md)) |
| `DT_Progression_ClassAbilities` | Class abilities | Owning class, unlock class level, base XP, school (spells only), mana cost, stamina cost, cooldown, base damage (spells only), damage type (spells only), WeaponDamagePercent (non-spell damaging abilities), BaseHealing (healing abilities), PoiseDamage, RequiredStat, RequiredValue |
| `DT_Progression_SkillXPSources` | Meaningful-use XP per skill | Base XP per action (including Woodcutting XP for removing a stump), difficulty multipliers by target tier (ore tier, tree tier, enemy level relative to character level) |
| `DT_Progression_SkillXPCurve` | XP per skill level | Levels 1–100 |
| `DT_Progression_ClassXPCurve` | XP per class level | Levels 1–20 |
| `DT_Progression_CharacterXPCurve` | XP per character level | Levels 1–50 |
| `DT_Progression_SkillBonuses` | Passive per-level skill bonuses | e.g. Mining dig speed and ore yield chance, Stealth perception-radius reduction |
| `DT_Progression_SkillPerks` | Milestone perks at 25/50/75/100 | e.g. Woodcutting 50 double-log chance |
| `DT_Progression_DerivedStats` | Stat-to-derived-value coefficients | Per-stat coefficients (STR, DEX, CON, INT, WIS, CHA), base MaxStamina coefficient (CON), spell StatScaling coefficients (INT, WIS), WIS healing coefficient, base MaxPoise, MaxCarryWeight (base 150, +10 × STR mod, bag/perk/affix bonuses; racial bonus from `DT_Character_Races`), MaxEquipLoad (base 40, +3 × STR mod, +2 × CON mod; racial multiplier from `DT_Character_Races`) |
| `DT_Progression_Rules` | Progression tuning | Stat cap (30), character level cap (50), class level cap (20), skill level cap (100), stat points per level (1), class slot levels (1/10/25/40), ability bar size (6), XP share (25%), new character starting items (Plain Shirt in Chest, Plain Trousers in Legs) |

## Crafting (crafting-jobs)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Crafting_Jobs` | 9 Job rows | One row per Job |
| `DT_Crafting_Recipes` | All recipes, including dye items | Owning Job, required Job level, required station ("None" for hand-crafting), station tier (recipe tier), input items and quantities, output item and quantity, craft time (s), base XP, TierMultiplier |
| `DT_Crafting_JobXPCurve` | XP per Job level | Levels 1–100 |
| `DT_Crafting_QualityByLevel` | Rarity floors by Job level | e.g. Job 50+ never Common |
| `DT_Crafting_ConsumablePotency` | Consumable potency scaling by Job level | Potions, meals, arrows (Carpenter Job level scales `ArrowDamage`); the Job level 1 potency used by looted, bought and found consumables |
| `DT_Crafting_JobPerks` | Job milestone perks at 25/50/75/100 | e.g. Cook 50 meals last 25% longer |
| `DT_Crafting_DyeColors` | Dye palette | 48 rows: color ID, display name, linear color value, dye item definition |
| `DT_Crafting_Rules` | Crafting tuning | Attachment bind range (5 m), material pull range (10 m), craft range (10 m), queue size (10), low-level XP penalty (20 levels below → 10%), RuneMaxItemLevel per rune tier (20/30/40/50/60), Job level cap (100), Dye Station interaction range, Enchanting Altar interaction range |

## Survival (survival)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Survival_DrainRates` | Meter drain | Hunger and Thirst drain per minute, activity multipliers, Fatigue per stamina point spent |
| `DT_Survival_Temperature` | Temperature model | Insulation, Cooling and WIS coefficients for comfort range, inside-range and outside-range drift rates, heat source max contributions |
| `DT_Survival_StaminaCosts` | Stamina costs | Light attack, off-hand light attack, heavy attack, dodge roll, block per hit, sprint per second, bow shot (per aimed or quick shot), climbing, swimming |
| `DT_Survival_Movement` | Swim and climb speed | Base swim speed and base climb speed as fractions of base walk speed (0.5 each) |
| `DT_Survival_Penalties` | Survival tuning | Survival tick (1 s); starvation (1% / 5 s, regen stop); dehydration (1% / 3 s, max stamina ×0.5); temperature state penalties; Wet duration (60 s); stamina regen delay (1 s); sleep (06:00 skip, −5 Fatigue/s, 10 s combat block); swimming exhaustion (2%/s); Breath (30 s, 2 s refill, 5%/s); drinking reach; Thirst per drink |

## World (voxel-world)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_World_Climates` | 8 region rows | Base temperature, comfort range (10–26 °C), weather types, vegetation, ore types, item-level band |
| `DT_World_Structures` | Generated structures | Definitions and placement rules for ruins, capitals and towns (Quest Board positions, NPC posts) and Bandit and Beastmen camps (enemy spawn points, loot chest positions) |
| `DT_World_VoxelMaterials` | Voxel materials | Hardness, required tier, soft-material (shovel-diggable) flag, item yields per fill unit; includes Deep Snow |
| `DT_World_Trees` | Trees | Tree types, health, yields (including the Wood from removing a stump with a shovel), falling-log damage |
| `DT_World_Forage` | Forage and loose pickups | Forage types, regrow timers, per-region Loose Stick and Loose Stone density, loose pickup respawn timers |
| `DT_World_Hazards` | Hazard tuning | Lava Fire damage per second, Deep Snow movement speed reduction (30%) |
| `DT_World_Streaming` | Chunk streaming | Load radius per player, streamed-in instance budget (12,000,000, under Nanite's 16 million cap) |
| `DT_World_Building` | World tuning | Voxel resolution (25 cm), day length (30 min), dig/fill/mine radii (0.5 m), fill unit radius (0.5 m), tree regrow time (3 in-game days), destruction drop (50%), deconstruction refund (100%), blocked-zone radii (boss arena, 20 m from world spawn), dig depth limit (6 m) |

## Combat and Loot (combat-loot)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Combat_Enemies` | Enemy rows | Category (`Hostile`, `Wildlife`, `Boss`), Faction (`None`, `Bandits`, `Beastmen` or a kingdom), skeleton family (a `DT_EnemyAI_SkeletonFamilies` row), level, health, poise, resistances, damage, attack set (per attack: damage, damage type, `PoiseDamage`, `UsesWeapon` flag, optional status buildup status and amount), perception radius, FleeHealthThreshold (optional), loot table ID, Hunting yield (optional), region, KillXP, FirstKillXP (Boss rows only), AI StateTree asset |
| `DT_Combat_Movement` | Dodge tuning | I-frame window and roll recovery per equip load tier (Light / Medium / Heavy) |
| `DT_Combat_StatusEffects` | Status effects | Buildup and duration for Poison, Bleed, Burn, Frostbite; Poison buildup for drinking Poison water, Poison Water/Poison Plant contact, Lava Burn buildup, torch bash Burn buildup |
| `DT_Combat_Spawning` | Enemy spawning | Spawn density and max live enemies per streamed chunk, by region |
| `DT_Combat_PlayerScaling` | Player-count scaling | Health (+60% per extra player), damage (+10% per extra player) |
| `DT_Combat_Rules` | Combat tuning | Bleed-out (30 s, 1 s per 1% max health), revive (4 s, 2 m, 30% health), Revivify range (15 m), respawn (5 s, 50% Hunger/Thirst, Weakened 5 min, −20% damage, −20% max stamina, −10% durability), lock-on range (20 m), poise regen delay (3 s), loot eligibility (50 m), converted loot despawn (10 min after the original drop), repair cost (25%), requirement penalty (0.5), crit multiplier (player ripostes only), TwoHandingMultiplier (×1.25), two-handing STR contribution multiplier (×1.5), QuickShotDamageMultiplier (×0.5), ToolEnemyDamageMultiplier (×0.5), HeavyAttackDamageMultiplier (×1.6), HeavyAttackPoiseMultiplier (×2.0), EmptyOffHandBlockMultiplier (×0.5), BlockPoiseMultiplier (×0.5), Unarmed weapon profile (WeaponBase, STR scaling, One-Handed skill, `PoiseDamage`), ArmorConstant (100), resistance cap (0.9), spawn exclusion (25 m from buildings, 40 m from players), minimum fall height and fall damage per meter, execution range (2.5 m), forward execution angle, execution health threshold (20%), execution stamina restore (20%), execution skill XP multiplier (×3) |
| `DT_Combat_Executions` | Execution animations | Paired attacker and victim animation sets keyed by weapon category (with an Unarmed set) and skeleton family, plus a generic set per skeleton family |
| `DT_Loot_RarityWeights` | Rarity odds | Weights by item-level band, for drops and crafts (dropped jewelry excludes the Common weight; crafted Plain jewelry skips the roll) |
| `DT_Loot_Affixes` | Affix pools | Per-slot affix pools, value ranges by item level, Legendary unique powers (excluded from slots where invalid) |
| `DT_Loot_Tables` | Drop tables | Item categories and drop counts per enemy and chest type, gold ranges (Hostile enemies and chests), boss offering drop rates |

## Inventory (inventory)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Inventory_Containers` | Storage containers | Container types and weight capacities |
| `DT_Inventory_Rules` | Inventory tuning | Over-Encumbered penalties (walk speed 50%, stamina regen 50%) |

## Factions (factions-kingdoms)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Factions_Factions` | 8 faction rows | Faction ID, display name (placeholder for kingdoms), type (Kingdom or Outlaw), race ID and home region (kingdoms), camp density per region, camp respawn time (1 in-game day) and camp radius (Bandits, Beastmen) |
| `DT_Factions_NPCs` | Town and escort NPCs | NPC ID, role (Guard, Vendor, Quest Giver, Citizen, Guard Captain), kingdom, race ID, sex, appearance preset, level, health, poise, resistances, attack set (Guards; per-attack damage, damage type, `PoiseDamage`, optional status buildup status and amount), perception radius, skeleton family, AI StateTree asset, vendor stock list ID, respawn time (1 in-game day) |
| `DT_Factions_VendorStock` | Vendor stock lists | Stock list ID, item definition, quantity per restock, item level (equipment); no boss materials |
| `DT_Factions_QuestTemplates` | Board quest templates | Template ID, kingdom, quest type (Hunt, Gather, Clear Camp, Escort, Deliver), objective parameter ranges (Escort and Deliver destination town slots are rolled, not columns), gold reward range, reputation reward, character XP reward, item reward chance and list |
| `DT_Factions_Questlines` | Kingdom questlines | Kingdom, step number (1–5), quest type, objective parameters, rewards (placeholder content) |
| `DT_Factions_Raids` | Raid tuning | Base value coefficients (1 per piece, 5 per station tier), BaseChance, BaseValueCoefficient, HostilityFactor per faction, RegionMonsterWeight, raider count and level offset bands by region order and base value bracket, per-level health and damage multipliers |
| `DT_Factions_Rules` | Factions tuning | Starting reputation (+30 own kingdom, 0 others, −100 Bandits/Beastmen), attack loss (−10) and cooldown, kill loss (−25), tier price multipliers (×1.25 / ×1.00 / ×0.90 / ×0.80), rarity sell multipliers (Common ×1, Magic ×1.5, Rare ×2.5, Epic ×4, Legendary ×8), town protected radius, interaction range, cower duration, sell rate (25%), CHA price coefficient (0.03), restock and board refresh hour (06:00), board offers (3–5), quest log size (10), tracked quests (3), escort destination radius and leash, base radius (30 m) and minimum pieces (10), raid player range (100 m), raid roll interval (5 real-time minutes), raid-free first days (3 in-game days), raid duration (10 real-time minutes), retreat time (real time), raider spawn distance (100 m), raider spawn sample spacing (5 m), Hostile recovery (+2 per in-game day, ceiling −49), fine (500 gold + 10 per point below −49), Guard truce after a fine (20 m, 60 s) |

## Multiplayer (multiplayer)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_MP_SplitScreenScalability` | Rendering tiers per local viewport count | High tier (1–2 viewports: Lumen HWRT High, VSM High, 100% foliage, 100% view distance) and Split tier (3–4 viewports: Lumen Lite, VSM Medium, 50% foliage, 70% view distance), tuned from the benchmark. At runtime the user's machine-wide graphics settings are the ceiling: unchanged at 1–2 viewports, and min(user setting, Split value) per setting at 3–4 |
| `DT_MP_Session` | Session tuning | Max players (4, hard cap 4), connection timeout (20 s) |

All numbers above are starting values, tunable.

## Engine (engine-tech)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Engine_Benchmark` | Benchmark milestone run | Run length (120 s), per-character dig/fill interval (2 s), building piece count (200), AI enemy count (30) and humanoid enemy count (10), pieces destroyed in the Chaos replication evaluation (50) |

All numbers above are starting values, tunable. See [Engine and Rendering](Dev-Engine-and-Rendering.md).

## Enemy AI (enemy-ai)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_EnemyAI_Hostility` | Hostility matrix | One row per AI group (Region Monster, Wildlife, Boss, Bandits, Beastmen, Kingdom Raider, Guard, Town NPC) and one column per group including Player; each cell `Hostile`, `Neutral`, `Flee` or `Reputation` |
| `DT_EnemyAI_SkeletonFamilies` | Skeleton families | Family ID, Humanoid flag; for humanoid rows: body race ID and sex (one of the 12 bodies), appearance preset, IK Retargeter asset |
| `DT_EnemyAI_ItemScore` | Item Score weights | WeaponBaseWeight (1.0), ArmorWeight (1.0), ResistanceWeight (100), AffixWeight per affix type (1.0), LegendaryPowerScore (25) |
| `DT_EnemyAI_XP` | Enemy XP and levels | Down a player (50), kill a player (100), kill a town NPC (40), hostile enemy kill factor (×0.5 of the victim's KillXP), survival XP (5 per real-time minute), XP per gained level (100 × level), HealthPerLevel (×1.10), DamagePerLevel (×1.05), PoisePerLevel (×1.05) |
| `DT_EnemyAI_Names` | Veteran names and titles | Name lists per faction (Bandits, Beastmen) and per skeleton family (Faction None), title lists per XP source (Players, Town NPCs, Enemies, Survival); placeholder content |
| `DT_EnemyAI_Rules` | Enemy AI tuning | Home radius (15 m), leash distance (40 m), Veteran roam multiplier (×3), patrol wait (8–20 s), investigate multiplier (×1.5) and timeout (10 s), detection threat (1), target loss time (5 s), flee calm time (8 s), return timeout (30 s), AI LOD distance (50 m) and perception intervals (0.2 s / 1.0 s), loot conversion time (2 real-time minutes), loot radius (12 m), pickup reach (1.5 m) and montage (1 s), gained-level cap (10), KillXP per gained level (+10%), name plate distance (15 m), Veteran threshold (3 gained levels), caps (5 per region, 20 per world), Veteran bonus KillXP (×1.0) and minimum rarity (Rare), raid leader range (300 m) |

All numbers above are starting values, tunable. See [Enemies and AI](Enemies-and-AI.md).

## Source specs

- See the Key Files and Requirements of each spec in [Specs and Pipeline](Dev-Specs-and-Pipeline.md).
