# Spec: Combat & Loot

## Overview
Combat is deliberate and Souls-like: stamina-gated attacks, dodge rolls with invulnerability frames, blocking, parrying, lock-on, and punishing bosses that gate each climate region. Loot is Diablo-style: each player receives their own drops, and items roll rarity tiers and random affixes. Downed players can be revived by teammates. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Combat rewards timing and positioning over button-mashing.
- Stats, class abilities, skills, and gear each visibly change combat outcomes.
- Every boss is a memorable encounter whose defeat unlocks the next tier of progression.
- Loot drops are exciting because of random rarity and stats, and co-op players never compete for drops.

## Non-Goals
- PvP modes.
- Mounted combat.
- Turn-based or dice-roll hit resolution. Hits are physical, determined by hitboxes.
- Item trading UI (players drop items on the ground to share).

## Requirements

### Core Combat
1. Melee attacks resolve via weapon hitbox traces during attack animation windows (anim notifies), on the server.
2. Actions and default stamina costs (from `DT_Survival_StaminaCosts`): light attack, heavy attack, dodge roll, block (per hit absorbed), sprint (per second). Spells are class abilities, and their mana and stamina costs come from `DT_Progression_ClassAbilities` (see character-progression spec).
3. A dodge roll grants invulnerability frames for a window defined in `DT_Combat_Movement`. The window and roll recovery time depend on the equip load tier. Equip load is computed per `specs/inventory/inventory.md` Requirement 6, and the tiers are: Light (<30%), Medium (30% to <70%), Heavy (70% to ≤100%), Overloaded (>100%: cannot dodge roll). An Over-Encumbered character also cannot dodge roll.
4. Blocking (gamepad LT, keyboard right mouse button; see `specs/game-foundation/game-foundation.md` Requirement 14) with a shield or weapon reduces incoming damage by the item's block percentage, and drains stamina per hit. When stamina hits 0 while blocking, the guard breaks and the player is staggered.
5. Parry (gamepad LB + LT, keyboard Left Alt + right mouse button; see `specs/game-foundation/game-foundation.md` Requirement 14) is available to any character holding a parry-capable item (shields, some weapons). A parry within the parry window staggers the attacker and opens a riposte (critical hit) window.
6. Lock-on targets the hostile closest to screen center among hostiles within 20 m of the player (tuning value). While locked on, the camera frames the target and movement strafes. The lock-on input (gamepad R3, keyboard Tab) toggles lock-on, and flicking the right stick (gamepad) or the mouse (keyboard) while locked on cycles to the next target in the flick direction (see `specs/game-foundation/game-foundation.md` Requirement 14).
7. Poise: characters and enemies have a Poise value. Hits deal poise damage, and at 0 Poise the target staggers. Poise regenerates after 3 seconds without taking hits (tuning value).
8. Damage formula: `Damage = (WeaponBase + StatScaling + AffixFlat) × (1 + SkillBonus + AffixPercent) × (1 − TargetResistance) × RequirementPenalty × (1 − WeakenedPenalty) × CritMultiplier`. RequirementPenalty is the product of every Requirement 36 multiplier (starting value 0.5 each) that applies: one when the attacker is below the weapon's stat requirements, one when the weapon's category is allowed by no held class, and, for a class ability, one when the caster is below its RequiredStat and RequiredValue. When none applies, RequirementPenalty is 1. WeakenedPenalty is the Requirement 17 damage-dealt penalty (starting value 0.2) while Weakened, otherwise 0. CritMultiplier applies to riposte critical hits (Requirement 5), otherwise 1, and its value lives in `DT_Combat_Rules` (tuning value). For spells, WeaponBase is the ability's base damage from `DT_Progression_ClassAbilities`. StatScaling uses the weapon's scaling stat modifier(s) and scaling grade from the item definition. For spells, StatScaling uses the INT modifier (Destruction, Arcane) or the WIS modifier (Restoration, Nature), with coefficients in `DT_Progression_DerivedStats`. SkillBonus comes from the weapon's skill (One-Handed, Two-Handed, Archery, or the spell's magic school). A non-spell class ability deals `AbilityDamage = WeaponDamagePercent × Damage`, where WeaponDamagePercent comes from the ability's row in `DT_Progression_ClassAbilities` (e.g., 1.5) and Damage is this full formula computed with the Right Hand weapon's WeaponBase, StatScaling, and SkillBonus. When the Right Hand slot is empty or holds a tool or torch, the Unarmed weapon profile in `DT_Combat_Rules` is used instead: its WeaponBase (tuning value), STR as the scaling stat, One-Handed as the skill, and Blunt as the damage type. A non-spell ability does not use the ability's base damage.
9. Damage types: Physical (Slash, Pierce, Blunt), Fire, Frost, Lightning, Poison, Holy, Shadow. Enemies and armor have resistances per type. Players hold one resistance attribute per damage type plus an Armor attribute on `UNamecAttributeSet`, and enemies hold the per-type resistances on `UNamecEnemyAttributeSet`. Each equipped item's Armor value and resistance values add to the wearer's attributes, including resistance affixes. TargetResistance in Requirement 8 is: for Slash, Pierce, or Blunt damage, `Armor ÷ (Armor + ArmorConstant)` plus the matching subtype resistance attribute; for every other type, the matching resistance attribute. Enemies have no Armor term. TargetResistance is capped at 0.9. ArmorConstant (starting value 100) and the cap are tuning values.
10. Status effects: Poison (damage over time), Bleed (burst damage when a buildup meter fills), Burn (Fire damage over time), Frostbite (stamina regen reduction). Buildup and durations live in `DT_Combat_StatusEffects`. Every status-effect buildup a player receives is multiplied by (1 − StatusResistance), using the `StatusResistance` attribute (`specs/character-progression/character-progression.md` Requirement 6). Poison, Bleed, Burn, and Frostbite buildup and active effects carry the gameplay tag `Status.Negative.Cleansable` (removable by Sauren Shed Skin, `specs/character-creation/character-creation.md` Requirement 13). Stagger is not a buildup status effect; Stagger comes only from Poise reaching 0 (Requirement 7) and from guard break (Requirement 4) or parry (Requirement 5).
11. Class abilities, including spells, are GAS abilities that consume mana and/or stamina, with cooldowns, defined per class. Mana cost, stamina cost, cooldown, base damage (spells), and WeaponDamagePercent (non-spell abilities; Requirement 8) come from `DT_Progression_ClassAbilities` (see character-progression spec).
12. Friendly fire is a world setting, off by default. When off, player attacks, spells, and area effects never damage other players. When on, they do. Player attacks, spells, and area-of-effect abilities never damage building pieces under either setting (`specs/voxel-world/voxel-world.md` Requirement 26).

### Downed & Revive
13. When a player's health reaches 0, the player enters the Downed state instead of dying: the player can crawl slowly, cannot attack or use items, and a 30-second bleed-out timer starts (tuning value). Damage taken while Downed does not reduce health. Instead, each hit reduces the remaining bleed-out timer by 1 second per 1% of max health the hit would have dealt (tuning value).
14. A teammate reviving holds the interact input for 4 seconds within 2 m of the downed player (tuning values). Taking damage interrupts the revive. The Cleric class ability "Revivify" revives instantly within 15 m (tuning value).
15. A revived player returns at 30% max health (tuning value).
16. When a downed player's bleed-out timer ends, that player dies. When every player in the session is downed or dead, all downed players die. A player counts as dead from the moment of death until respawn (the Requirement 17 respawn delay).
17. On death: the player respawns 5 seconds after death (tuning value) at their bed in this world, or at the world spawn point when no bed is set or the bed has been destroyed (`specs/survival/survival.md` Requirement 18), with full health, 50% Hunger and Thirst, BodyTemperature 37 °C, Fatigue 0, and a 5-minute "Weakened" debuff (−20% damage dealt, −20% max stamina). Equipment durability loses 10% (tuning values). No items are dropped.
18. In a single-player session (one player total), the Downed state is skipped. Health 0 means immediate death.

### Enemies
19. Enemies are defined in `DT_Combat_Enemies` with: category (`Hostile`, `Wildlife`, `Boss`), level, health, poise, resistances, damage, attack set, perception radius, loot table ID, Hunting yield (optional), region, KillXP (character XP per kill), FirstKillXP (character XP per boss first-kill, set only for `Boss` rows; see character-progression spec Requirement 8), and AI behavior tree. `Wildlife` creatures never attack and flee when damaged. Each enemy owns a `UAbilitySystemComponent` with `UNamecEnemyAttributeSet` (Health, MaxHealth, Poise, MaxPoise, resistances).
20. Enemy spawns come from the `DT_Combat_Enemies` rows whose region column matches the spawn location's region, filtered by time of day (some enemies are night-only). Spawn density and the maximum live enemies per streamed chunk come from `DT_Combat_Spawning`. Enemies never spawn within 25 m of any placed building piece or within 40 m of any player (tuning values).
21. Enemy health and damage scale with the number of players in the session per `DT_Combat_PlayerScaling` (starting values: +60% health and +10% damage per additional player).
22. Enemies detect and aggro players per Requirement 41. Enemy aggro targets the player with the highest threat. Threat comes from damage dealt, healing done, and taunt abilities. A taunt is a forced-target effect: while it lasts, the enemy targets the taunting character regardless of threat, and when it ends, the enemy's normal threat table resumes. Taunt durations are set by the taunting ability (e.g., Mauling Roar, `specs/character-creation/character-creation.md` Requirement 13).

### Bosses
23. Each region has one boss in a fixed arena location (placed by world generation). A boss is summoned by placing a region-specific offering item at the arena altar. Offering items drop from that region's `Hostile` enemies at the rate set in `DT_Loot_Tables`. Each eligible player rolls for an offering independently as part of their per-player loot.
24. Bosses have named multi-phase fights with telegraphed attacks. Each boss's phases and attack set are defined in its own Blueprint subclass of `ANamecBossBase`.
25. When a boss fight starts, the arena is enclosed by a barrier. Players inside at summon time are locked in until the boss dies or every player inside is dead. Players outside cannot enter.
26. Defeating a boss sets `BossDefeated[RegionId] = true` in `UNamecWorldSave`. Each of the 7 region bosses before the Volcanic region drops a guaranteed boss trophy and that boss's own distinct tier material (see voxel-world spec Requirement 8) for **each player present** (each player inside the arena barrier when the boss dies). The tier material is required to craft station upgrades and gear for the next tier (see crafting-jobs spec). Each player present whose character lacks that boss's first-kill flag receives FirstKillXP instead of KillXP, including in re-summoned fights (Requirement 27). Every other player present receives KillXP (see character-progression spec Requirement 8). The final (Volcanic) boss instead drops a guaranteed Legendary item plus a trophy for each player present, gates nothing, and is re-summonable as endgame content (Requirement 27).
27. Defeated bosses can be re-summoned for more loot. Re-summoning uses a new offering item.

### Loot
28. Loot is per-player instanced: when an enemy dies or a loot chest opens, each eligible player gets independently rolled drops visible and pickable only by that player. Ownership is per player (character GUID), not per connection, because split-screen players share one connection: the server replicates each per-player loot actor only to its owner's connection (`bOnlyRelevantToOwner`), the actor renders only in the owning local player's viewport (`bOnlyOwnerSee` on its primitives, owned by that player's PlayerController), so other local players on the same machine, including the host, never see it, and the server accepts a pickup only from the owning player. Loot chests are placed by world generation (see voxel-world spec Requirement 10) and are distinct from crafted storage containers. Each player can open each loot chest once, with an independent per-player roll. The opened state is tracked per character GUID in `UNamecWorldSave`. Loot chests do not refill.
29. Eligible = within 50 m of the enemy at death, or dealt damage to it (tuning value). For a `Boss` enemy, the only eligible players are the players present inside the arena barrier when the boss dies (Requirement 26), for both loot and XP, and the 50 m and dealt-damage rules do not apply. For a loot chest, the only eligible player is the player who opens it. Every player eligible for an enemy kill also receives that enemy's full KillXP (`specs/character-progression/character-progression.md` Requirement 8).
30. Rarity tiers and max affix counts: Common (0), Magic (1–2), Rare (3–4), Epic (4–5), Legendary (5 + one unique Legendary power). Rarity weights come from `DT_Loot_RarityWeights`, keyed by item-level band, for both enemy drops and crafted items, and are modified by any Magic Find affix.
31. Item level = source enemy level, or for a loot chest a uniform random integer within the item-level band of the chest's region in `DT_World_Climates`, rolled per player, or crafted item level (see crafting-jobs spec). Affix value ranges scale with item level per `DT_Loot_Affixes`.
32. Affix pool examples: +STR/DEX/CON/INT/WIS/CHA, +% damage of a type, +resistance, +max health, +stamina regen, +Insulation/Cooling, +skill XP gain for a skill, +Magic Find, life on hit. No affix appears twice on one item.
33. Loot tables (`DT_Loot_Tables`) define item categories and drop counts for each enemy and chest type. There is no gold or currency.
34. An item dropped on the ground by a player (via inventory) becomes a shared world pickup any player can see and take.
35. Equipment has durability, reduced by use. Durability 0 makes the item unusable (not destroyed). Repairing at the station of the item's crafting Job restores full durability and costs 25% of the item's recipe materials, rounded up (tuning value). Dropped items with no recipe use the recipe of their base item type. Alternatively, a Repair Kit (Blacksmith recipe, required Job level 1) repairs any item to full durability at any crafting station, with no Job level requirement, consuming one Repair Kit. The station repair path above remains the alternative for craftable items.
36. Equipment may have stat requirements (e.g., STR 16). Equipping below requirements is allowed, but the item deals 50% damage or gives 50% of its Armor and resistance values (Requirement 9) (tuning value). The same penalty value applies to a weapon or armor piece whose category no held class allows (see character-progression spec Requirements 15 and 28), and to a class ability's damage and healing when the caster is below the ability's RequiredStat and RequiredValue (see character-progression spec Requirement 16). When several of these penalties apply to the same hit or item, they multiply (e.g., 0.5 × 0.5 = 25%).

### Tuning Data
37. Every value marked "tuning value" in this spec that names no other table lives in `DT_Combat_Rules`.

### Incoming Damage, Poise Damage & Healing
38. Damage an enemy attack deals to a player: `IncomingDamage = EnemyDamage × PlayerScaling × (1 − TargetResistance) × (1 − BlockPercent)`. EnemyDamage and the damage type are the attack's values in the enemy's attack set in `DT_Combat_Enemies`. PlayerScaling is the Requirement 21 damage multiplier for the session's player count (locked at summon for bosses; Edge Case 4). TargetResistance is the player's Requirement 9 value for that damage type. BlockPercent is the blocking item's block percentage (Requirement 4) while the player is blocking, otherwise 0. Poise damage per hit comes from: the weapon's `PoiseDamage` on its item definition for weapon hits (the Unarmed weapon profile's `PoiseDamage` in `DT_Combat_Rules` when the Right Hand is empty or holds a tool or torch, tuning value); the ability's `PoiseDamage` column in `DT_Progression_ClassAbilities` for class abilities; the racial active ability's poise damage in `DT_Character_Races` for racial abilities (`specs/character-creation/character-creation.md` Requirement 12); and the attack's `PoiseDamage` in `DT_Combat_Enemies` for enemy attacks. A player's MaxPoise = base MaxPoise (tuning value in `DT_Progression_DerivedStats`) + the total `Poise` value of every equipped armor piece's item definition. An enemy's MaxPoise is the poise column of its `DT_Combat_Enemies` row.
39. Healing from a class ability: `Healing = (BaseHealing + StatScaling) × (1 + SkillBonus) × RequirementPenalty`. BaseHealing is the ability's `BaseHealing` column in `DT_Progression_ClassAbilities`. StatScaling uses the WIS modifier, with the coefficient in `DT_Progression_DerivedStats`. SkillBonus comes from the ability's magic school when it is a spell, otherwise 0. RequirementPenalty is the Requirement 36 multiplier when the caster is below the ability's RequiredStat and RequiredValue, otherwise 1. Healing never raises Health above MaxHealth.

### Fall Damage
40. A player or enemy that lands after falling a vertical distance at or above the minimum fall height takes fall damage = (fall distance − minimum fall height) × fall damage per meter, as a percentage of max health. Fall distance is measured from the highest point of the fall to the landing point. Fall damage ignores Armor, resistances, and blocking. The minimum fall height and fall damage per meter are tuning values. Racial exemptions are in `specs/character-creation/character-creation.md` Requirement 12 (Felari Night Eyes).

### Enemy Perception
41. A `Hostile` enemy detects and aggroes a player who is inside its perception radius and in its line of sight. The perception radius starts at the enemy's perception radius in `DT_Combat_Enemies` (Requirement 19). Crouching halves it, the Stealth skill reduces it further (`specs/character-progression/character-progression.md` Requirement 21), and a racial perception multiplier then multiplies the final radius (Hundari Loud ×1.25, `specs/character-creation/character-creation.md` Requirement 12).

## Data Flow
1. Player presses light attack → client predicts the animation via GAS ability `GA_LightAttack` → server validates stamina, commits cost, and plays the montage.
2. The anim notify opens the hitbox window → server traces the weapon hitbox → for each hit hostile, the server builds a damage gameplay effect spec using the Requirement 8 formula and applies it.
3. Target's attribute set subtracts health and poise → the server applies stagger if Poise ≤ 0 and awards weapon skill XP to the attacker via the progression component.
4. At enemy health 0 → server determines eligible players → awards full KillXP to each eligible player via the progression component → `UNamecLootSubsystem` rolls a drop set per eligible player → spawns per-player loot actors owned by each eligible player's PlayerController, replicated only to the owner's connection and rendered only in the owner's viewport (Requirement 28).
5. At player health 0 → server checks session player count → Downed state or death → on revive or respawn, applies the effects in Requirements 15/17.
6. Boss death → server sets the defeat flag, awards FirstKillXP to each player inside the arena barrier whose character lacks that boss's first-kill flag (including in a re-summoned fight) and sets the flag, triggers the world save, and rolls guaranteed trophies per present player.
7. Player opens a loot chest → server checks the opener's character GUID against the chest's opened set → `UNamecLootSubsystem` rolls a drop set for the opener only → server spawns the owner-only loot actors and records the GUID in the chest's opened set.

## Edge Cases
1. When a player disconnects cleanly while downed (or a host local player leaves while downed), the character saves as dead-respawn: on next join they spawn per `specs/multiplayer/multiplayer.md` Requirement 16 (bed, or world spawn point) with the death effects from Requirement 17. Any character save written while the character is Downed or dead (autosave, disconnect, or exit, including host exit) sets the dead-respawn flag instead of saving the character at 0 health, and the flag clears once the death effects are applied on the next world entry. When a remote client's connection drops unexpectedly while downed, no final save is sent, and the character keeps its last autosave state (`specs/multiplayer/multiplayer.md` Requirement 17).
2. When a player's per-player loot is left unclaimed, the loot actor despawns after 10 minutes (tuning value). It is not visible to or claimable by others.
3. When all players inside a boss arena die, the boss resets to full health and the barrier drops.
4. When a player disconnects inside a boss arena, their character is removed from the fight, and boss scaling does not change mid-fight. Player-count scaling is locked at summon.
5. When a player joins the session mid-combat, existing enemies keep their current scaling. Newly spawned enemies use the new player count.
6. When a dodge roll's invulnerability frames overlap an area-of-effect tick, the tick is ignored for that player.
7. When friendly fire is off, a falling tree log still damages players (environmental damage, not player damage).
8. When a Legendary unique power has no valid effect for the item's slot, `DT_Loot_Affixes` excludes it from that slot's pool, so an invalid roll never happens.

## Acceptance Criteria
- [ ] Dodging through an enemy attack during the invulnerability window takes 0 damage.
- [ ] Blocking at 0 stamina causes guard break and stagger.
- [ ] A successful parry staggers the enemy and allows a riposte critical hit.
- [ ] In a 2-player session, player health 0 → Downed. A teammate holding interact for 4 seconds revives at 30% health.
- [ ] In a 1-player session, health 0 → immediate death and respawn at bed 5 seconds later, with BodyTemperature 37 °C and Fatigue 0.
- [ ] A downed player hit for 10% of max health loses 10 seconds of bleed-out timer.
- [ ] A non-spell ability with WeaponDamagePercent 1.5 deals 1.5× the Right Hand weapon's light-attack damage against the same target, and uses the Unarmed weapon profile when the Right Hand holds a torch.
- [ ] A character without the Temperate boss's first-kill flag receives FirstKillXP in a re-summoned Temperate boss fight, and a character with the flag receives KillXP.
- [ ] Two players killing the same enemy each see their own independently rolled drops, and neither sees the other's.
- [ ] An item dropped from inventory is visible and pickable by all players.
- [ ] A Rare item has 3–4 unique affixes, and a Legendary item has 5 affixes plus a unique power.
- [ ] Enemy health in a 3-player session equals base × (1 + 2 × 0.6).
- [ ] Defeating the Temperate boss sets its defeat flag in the world save and gives each present player the Tier-2 material.
- [ ] With friendly fire off, a player's fireball does 0 damage to another player.
- [ ] Two players each open the same loot chest once and receive independent rolls. A second open attempt by either player yields nothing, and the opened state survives save → quit → load.
- [ ] Defeating the Volcanic boss gives each present player a Legendary item and a trophy, and the boss can be summoned again with a new offering.
- [ ] A Repair Kit restores a 0-durability item with no recipe to full durability at any crafting station.

## Key Files
- `Source/NAMEC/Combat/NamecCombatComponent.h` — new; lock-on, poise, guard, parry state.
- `Source/NAMEC/Combat/NamecDamageExecution.h` — new; GAS execution calculation implementing the damage formula (Requirement 8), incoming enemy damage and poise damage (Requirement 38), and healing (Requirement 39).
- `Source/NAMEC/Combat/Abilities/` — new; `GA_LightAttack`, `GA_HeavyAttack`, `GA_Dodge`, `GA_Block`, `GA_Parry`.
- `Source/NAMEC/Combat/NamecDownedComponent.h` — new; downed state, bleed-out, revive interaction.
- `Source/NAMEC/Combat/AI/NamecEnemyBase.h` — new; enemy base character with threat table, taunt forced-target handling (Requirement 22), and perception detection (Requirement 41).
- `Source/NAMEC/Combat/AI/NamecEnemyAttributeSet.h` — new; enemy GAS attributes (Health, MaxHealth, Poise, MaxPoise, resistances).
- `Source/NAMEC/Combat/AI/NamecBossBase.h` — new; boss base with phases, arena barrier, summon.
- `Source/NAMEC/Loot/NamecLootSubsystem.h` — new; per-player drop rolling.
- `Source/NAMEC/Loot/NamecItemInstance.h` — new; item instance with rarity, affixes, durability, item level.
- `Source/NAMEC/Loot/NamecLootPickup.h` — new; per-player pickup actor (owner-connection relevance, owner-viewport-only rendering, owner-player pickup check) and shared world pickup mode.
- `Source/NAMEC/Loot/NamecLootChest.h` — new; generated loot chest actor with per-character-GUID opened state and per-player rolls.
- `Content/Data/DT_Combat_Enemies.uasset` — new; enemy rows, including per-attack damage, damage type, and `PoiseDamage` in each attack set (Requirement 38).
- `Content/Data/DT_Combat_Movement.uasset` — new; i-frames, roll recovery by equip load.
- `Content/Data/DT_Combat_StatusEffects.uasset` — new.
- `Content/Data/DT_Combat_Spawning.uasset` — new; spawn density and per-chunk caps by region.
- `Content/Data/DT_Combat_PlayerScaling.uasset` — new.
- `Content/Data/DT_Loot_RarityWeights.uasset` — new; rarity weights by item-level band for drops and crafts.
- `Content/Data/DT_Combat_Rules.uasset` — new; downed/revive, lock-on, poise, loot eligibility and despawn, repair, crit multiplier, Unarmed weapon profile (WeaponBase, STR scaling, One-Handed skill, `PoiseDamage`), ArmorConstant, resistance cap, minimum fall height and fall damage per meter (Requirement 40), and other combat tuning values.
- `Content/Data/DT_Loot_Affixes.uasset` — new.
- `Content/Data/DT_Loot_Tables.uasset` — new.
