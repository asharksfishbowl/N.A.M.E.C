# Data Tables

This page lists every `DT_*` DataTable named in the specs, grouped by system, with its purpose and the columns the specs mention.

← [Home](Home.md)

## Conventions

- All tables live in `Content/Data/` and are named `DT_<System>_<Purpose>`.
- Every numeric balance value is a starting value in a table and is editable without a C++ rebuild.
- Each system has a catch-all rules table: a value the specs mark "tuning value" without naming a table lives in that system's rules table (`DT_Character_Races`, `DT_Progression_Rules`, `DT_Crafting_Rules`, `DT_Survival_Penalties`, `DT_World_Building`, `DT_Combat_Rules`, `DT_Inventory_Rules`, `DT_MP_Session`).
- Per-item values (weight, damage, insulation, armor category, `Poise`, `PoiseDamage`, bag bonus, food/drink restore values, block percentage, stat requirements, scaling stat and grade) are on item definition data assets, not DataTables.
- "Columns mentioned" lists only what the specs name. Tables will have more once implemented.

## Core (game-foundation)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Core_Save` | Save tuning | Autosave interval (5 min) |
| `DT_Core_Input` | Input thresholds | View hold time for camera toggle (0.5 s), stick flick threshold |

## Character (character-creation)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Character_Races` | Six race rows, plus character creation tuning | Race ID, display name, description, stat bonuses, passive trait gameplay effect, active ability class, downside gameplay effect, male and female skeletal mesh sets, male and female first-person arm meshes, supported appearance options, collision capsule size; all race trait values (e.g. Versatile +10% skill XP, Fleeting Vigor ×1.15 Fatigue, Night Eyes 8 m fall exemption and 20:00–06:00 night hours, Amphibious ×3 Breath, ×1.3 swim speed, +0.25 PoisonResistance, ×0.75 Poison buildup, Water-Averse ×0.7 swim speed, ×1.5 swim stamina, ×2 Wet, Climber ×0.5 climb stamina, ×1.5 climb speed, Loud ×1.25 perception radius, Cold-Blooded +6 °C, Heat-Prone −6 °C, Thick Hide +10 Insulation and +50 MaxCarryWeight, Light Frame ×0.85 MaxEquipLoad); racial ability values and cooldowns (Pounce 8 m, 60 poise; Rally Howl 20 m, +25%, 20 s and CHA coefficients; Mauling Roar 6 m, 100 poise, 6 s taunt, Boss 3 s taunt and 25% poise); Mirror interaction range |
| `DT_Character_AppearanceOptions` | Appearance option ranges | Per-race (and per-sex where options differ) preset counts and slider ranges (e.g. body type 3, height ±8%, face 8 per race and sex, eye color 12) |

## Progression (character-progression)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Progression_Classes` | 11 class rows | Stat bonuses (starting class only), ability list, allowed armor categories, allowed weapon categories |
| `DT_Progression_ClassAbilities` | Class abilities | Owning class, unlock class level, base XP, school (spells only), mana cost, stamina cost, cooldown, base damage (spells only), WeaponDamagePercent (non-spell damaging abilities), BaseHealing (healing abilities), PoiseDamage, RequiredStat, RequiredValue |
| `DT_Progression_SkillXPSources` | Meaningful-use XP per skill | Base XP per action, difficulty multipliers by target tier (ore tier, tree tier, enemy level relative to character level) |
| `DT_Progression_SkillXPCurve` | XP per skill level | Levels 1–100 |
| `DT_Progression_ClassXPCurve` | XP per class level | Levels 1–20 |
| `DT_Progression_CharacterXPCurve` | XP per character level | Levels 1–50 |
| `DT_Progression_SkillBonuses` | Passive per-level skill bonuses | e.g. Mining dig speed and ore yield chance, Stealth perception-radius reduction |
| `DT_Progression_SkillPerks` | Milestone perks at 25/50/75/100 | e.g. Woodcutting 50 double-log chance |
| `DT_Progression_DerivedStats` | Stat-to-derived-value coefficients | Per-stat coefficients (STR, DEX, CON, INT, WIS, CHA), base MaxStamina coefficient (CON), spell StatScaling coefficients (INT, WIS), WIS healing coefficient, base MaxPoise, MaxCarryWeight (base 150, +10 × STR mod, bag/perk/affix bonuses; racial bonus from `DT_Character_Races`), MaxEquipLoad (base 40, +3 × STR mod, +2 × CON mod; racial multiplier from `DT_Character_Races`) |
| `DT_Progression_Rules` | Progression tuning | Stat cap (30), character level cap (50), class level cap (20), skill level cap (100), stat points per level (1), class slot levels (1/10/25/40), ability bar size (6), XP share (25%) |

## Crafting (crafting-jobs)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Crafting_Jobs` | 9 Job rows | One row per Job |
| `DT_Crafting_Recipes` | All recipes | Owning Job, required Job level, required station ("None" for hand-crafting), station tier (recipe tier), input items and quantities, output item and quantity, craft time (s), base XP, TierMultiplier |
| `DT_Crafting_JobXPCurve` | XP per Job level | Levels 1–100 |
| `DT_Crafting_QualityByLevel` | Rarity floors by Job level | e.g. Job 50+ never Common |
| `DT_Crafting_ConsumablePotency` | Consumable potency scaling by Job level | Potions, meals, arrows |
| `DT_Crafting_JobPerks` | Job milestone perks at 25/50/75/100 | e.g. Cook 50 meals last 25% longer |
| `DT_Crafting_Rules` | Crafting tuning | Attachment bind range (5 m), material pull range (10 m), craft range (10 m), queue size (10), low-level XP penalty (20 levels below → 10%), RuneMaxItemLevel per rune tier (20/30/40/50/60), Job level cap (100) |

## Survival (survival)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Survival_DrainRates` | Meter drain | Hunger and Thirst drain per minute, activity multipliers, Fatigue per stamina point spent |
| `DT_Survival_Temperature` | Temperature model | Insulation, Cooling and WIS coefficients for comfort range, inside-range and outside-range drift rates, heat source max contributions |
| `DT_Survival_StaminaCosts` | Stamina costs | Light attack, heavy attack, dodge roll, block per hit, sprint per second, climbing, swimming |
| `DT_Survival_Movement` | Swim and climb speed | Base swim speed and base climb speed as fractions of base walk speed (0.5 each) |
| `DT_Survival_Penalties` | Survival tuning | Survival tick (1 s); starvation (1% / 5 s, regen stop); dehydration (1% / 3 s, max stamina ×0.5); temperature state penalties; Wet duration (60 s); stamina regen delay (1 s); sleep (06:00 skip, −5 Fatigue/s, 10 s combat block); swimming exhaustion (2%/s); Breath (30 s, 2 s refill, 5%/s) |

## World (voxel-world)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_World_Climates` | 8 region rows | Base temperature, comfort range (10–26 °C), weather types, vegetation, ore types, item-level band |
| `DT_World_Structures` | Ruins | Ruin (generated surface structure) definitions and placement rules |
| `DT_World_VoxelMaterials` | Voxel materials | Hardness, required tier, soft-material (shovel-diggable) flag, item yields per fill unit; includes Deep Snow |
| `DT_World_Trees` | Trees | Tree types, health, yields, falling-log damage |
| `DT_World_Forage` | Forage and loose pickups | Forage types, regrow timers, per-region Loose Stick and Loose Stone density, loose pickup respawn timers |
| `DT_World_Hazards` | Hazard tuning | Lava Fire damage per second, Deep Snow movement speed reduction (30%) |
| `DT_World_Streaming` | Chunk streaming | Load radius per player |
| `DT_World_Building` | World tuning | Voxel resolution (25 cm), day length (30 min), dig/fill/mine radii (0.5 m), fill unit radius (0.5 m), tree regrow time (3 in-game days), destruction drop (50%), deconstruction refund (100%), blocked-zone radii (boss arena, 20 m from world spawn) |

## Combat and Loot (combat-loot)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Combat_Enemies` | Enemy rows | Category (`Hostile`, `Wildlife`, `Boss`), level, health, poise, resistances, damage, attack set (per attack: damage, damage type, `PoiseDamage`), perception radius, loot table ID, Hunting yield (optional), region, KillXP, FirstKillXP (Boss rows only), AI behavior tree |
| `DT_Combat_Movement` | Dodge tuning | I-frame window and roll recovery per equip load tier (Light / Medium / Heavy) |
| `DT_Combat_StatusEffects` | Status effects | Buildup and duration for Poison, Bleed, Burn, Frostbite; Poison buildup for drinking Poison water, Poison Water/Poison Plant contact, Lava Burn buildup |
| `DT_Combat_Spawning` | Enemy spawning | Spawn density and max live enemies per streamed chunk, by region |
| `DT_Combat_PlayerScaling` | Player-count scaling | Health (+60% per extra player), damage (+10% per extra player) |
| `DT_Combat_Rules` | Combat tuning | Bleed-out (30 s, 1 s per 1% max health), revive (4 s, 2 m, 30% health), Revivify range (15 m), respawn (5 s, 50% Hunger/Thirst, Weakened 5 min, −20% damage, −20% max stamina, −10% durability), lock-on range (20 m), poise regen delay (3 s), loot eligibility (50 m), loot despawn (10 min), repair cost (25%), requirement penalty (0.5), crit multiplier, Unarmed weapon profile (WeaponBase, STR scaling, One-Handed skill, `PoiseDamage`), ArmorConstant (100), resistance cap (0.9), spawn exclusion (25 m from buildings, 40 m from players), minimum fall height and fall damage per meter |
| `DT_Loot_RarityWeights` | Rarity odds | Weights by item-level band, for drops and crafts |
| `DT_Loot_Affixes` | Affix pools | Per-slot affix pools, value ranges by item level, Legendary unique powers (excluded from slots where invalid) |
| `DT_Loot_Tables` | Drop tables | Item categories and drop counts per enemy and chest type, boss offering drop rates |

## Inventory (inventory)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_Inventory_Containers` | Storage containers | Container types and weight capacities |
| `DT_Inventory_Rules` | Inventory tuning | Over-Encumbered penalties (walk speed 50%, stamina regen 50%) |

## Multiplayer (multiplayer)

| Table | Purpose | Columns / values mentioned |
|-------|---------|----------------------------|
| `DT_MP_SplitScreenScalability` | Graphics presets per local viewport count | View distance, shadow quality, foliage density |
| `DT_MP_Session` | Session tuning | Max players (4, hard cap 4), connection timeout (20 s) |

All numbers above are starting values, tunable.

## Source specs

- See the Key Files and Requirements of each spec in [Specs and Pipeline](Dev-Specs-and-Pipeline.md).
