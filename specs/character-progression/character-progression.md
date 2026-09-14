# Spec: Character Progression

## Overview
Characters grow along three independent tracks: D&D-style stats raised with points from character level, classes that grant combat abilities and unlock at level milestones, and use-based skills that level up the more the player performs them. Skill use also feeds character XP, so doing anything makes the character stronger. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Every action a player repeats (swinging an axe, casting a fireball, sprinting) visibly improves that action over time.
- Stats feel like D&D: six familiar scores with modifiers that drive derived values.
- Players start as one class and can combine up to four classes by level 40.

## Non-Goals
- Crafting Job leveling (see `specs/crafting-jobs/crafting-jobs.md`).
- Combat damage formulas beyond which stat and skill feed them (see `specs/combat-loot/combat-loot.md`).
- Respec/reset mechanics.
- Races or backgrounds.

## Requirements

### Stats
1. Each character has six stats: STR, DEX, CON, INT, WIS, CHA. Each starts at 10.
2. At creation, the chosen starting class applies its stat bonuses from `DT_Progression_Classes` (e.g., Warrior +2 STR, +1 CON).
3. Stat modifier = `floor((score - 10) / 2)`.
4. Stat scores are capped at 30 (tuning value).
5. Stats drive these derived values (coefficients live in `DT_Progression_DerivedStats`):
   - STR — melee damage, MaxCarryWeight, MaxEquipLoad, heavy-weapon requirements.
   - DEX — ranged damage, attack speed, dodge-roll recovery, light-weapon requirements.
   - CON — max health, health regen, base MaxStamina, MaxEquipLoad, resistance to hunger/thirst drain.
   - INT — Destruction and Arcane spell power, max mana, Destruction and Arcane spell requirements.
   - WIS — Restoration and Nature spell power, mana regen, status-effect resistance, temperature tolerance, Restoration and Nature spell requirements.
   - CHA — strength and radius of party buffs and auras (Bard, Paladin, Cleric abilities).
6. Stats are implemented as GAS attributes on `UNamecAttributeSet`, together with Health, MaxHealth, HealthRegen, Mana, MaxMana, ManaRegen, MaxCarryWeight, MaxEquipLoad, Poise, MaxPoise, Armor, StatusResistance, and one resistance attribute per damage type (SlashResistance, PierceResistance, BluntResistance, FireResistance, FrostResistance, LightningResistance, PoisonResistance, HolyResistance, ShadowResistance; see `specs/combat-loot/combat-loot.md` Requirement 9). Stamina, MaxStamina, and StaminaRegen live in `UNamecSurvivalAttributeSet` (see survival spec). Derived values are recalculated via GAS attribute-change callbacks, never polled per tick.

### Character Level
7. Character level ranges 1–50 (tuning value).
8. Character XP comes from four sources: a fixed percentage of all skill XP earned (starting value 25%), the same percentage of all class XP earned (Requirement 17), enemy kills, and boss first-kills. XP per enemy kill and per boss first-kill comes from the XP columns of `DT_Combat_Enemies`. Kill XP is awarded in full, not split, to every player loot-eligible for that kill (`specs/combat-loot/combat-loot.md` Requirement 29). A boss first-kill is the first time a given character defeats a given region's boss, tracked per character in `UNamecCharacterSave`. When a boss dies, every player inside the arena barrier whose character lacks that boss's first-kill flag receives first-kill XP instead of kill XP, and the flag is set, so each character receives first-kill XP once per boss. This includes re-summoned boss fights (`specs/combat-loot/combat-loot.md` Requirement 27). Characters that already hold that boss's first-kill flag receive normal kill XP only.
9. The XP required for each level comes from `DT_Progression_CharacterXPCurve`.
10. Each character level-up grants 1 unspent stat point (tuning value). The player spends points in the Character screen, and each point adds +1 to one stat. Spent points are permanent.
11. Unspent stat points persist in the character save indefinitely.

### Classes
12. Eleven classes exist: Warrior, Knight, Barbarian, Ranger, Rogue, Mage, Cleric, Druid, Paladin, Warlock, Bard. Each class is a row in `DT_Progression_Classes` with: stat bonuses (applied only for the starting class), an ability list, allowed armor categories (Requirement 28), and allowed weapon categories.
13. At creation the player picks exactly one starting class.
14. Class slots unlock at character levels 1, 10, 25, and 40 (tuning values). When a slot unlocks, the player may choose any class not already held, via the Character screen, at any time afterward.
15. All held classes are active simultaneously. The player can equip and use any ability, weapon, or armor category allowed by any held class. Any character can also equip a weapon or armor piece of a category no held class allows. Such a weapon deals 50% damage, and such an armor piece gives 50% of its Armor and resistance values (Requirement 28), using the penalty value of `specs/combat-loot/combat-loot.md` Requirement 36. This category penalty multiplies with any stat-requirement penalty on the same item.
16. Each class defines 8 abilities (starting count). Abilities are GAS `UGameplayAbility` subclasses. Spells are class abilities tagged with one magic school (Destruction, Restoration, Nature, or Arcane). Spells are not items. Each ability row in `DT_Progression_ClassAbilities` has: owning class, unlock class level, base XP, school (optional; set only for spells), mana cost, stamina cost, cooldown, base damage (set only for spells), WeaponDamagePercent (set only for non-spell abilities that deal damage; e.g., 1.5), BaseHealing (set only for abilities that heal), PoiseDamage, RequiredStat, and RequiredValue. Healing uses the formula in `specs/combat-loot/combat-loot.md` Requirement 39, and poise damage follows `specs/combat-loot/combat-loot.md` Requirement 38. A spell's damage uses its base damage, and a non-spell ability's damage is WeaponDamagePercent × the weapon damage of the Right Hand weapon, or of the Unarmed weapon profile when the Right Hand is empty or holds a tool or torch (`specs/combat-loot/combat-loot.md` Requirement 8). When the caster's RequiredStat score is below RequiredValue, the ability deals 50% damage and healing, using the same penalty value as `specs/combat-loot/combat-loot.md` Requirement 36.
17. Each held class has its own class level (1–20, tuning value). Class XP is awarded each time one of that class's abilities hits a target or completes its effect, with base XP per ability in `DT_Progression_ClassAbilities` and required XP per class level in `DT_Progression_ClassXPCurve`. Class XP feeds character XP at the same 25% share as skill XP. Each ability unlocks at the class level listed in `DT_Progression_ClassAbilities`, and a newly held class starts at class level 1 with its level-1 abilities.
18. The player equips up to 6 abilities on an ability bar (tuning value), drawn from any held classes. Ability bar inputs:
    - Gamepad: hold LB and press A, B, X, or Y for abilities 1–4, and hold LB and press RB or RT for abilities 5–6. Block is on LT.
    - Keyboard: Z, X, C, V, B, N for abilities 1–6.
    
    The full default binding table is `specs/game-foundation/game-foundation.md` Requirement 14.

### Skills
19. Skills level 1–100 (tuning value). XP required per level comes from `DT_Progression_SkillXPCurve`.
20. Skill families and skills:
    - **Gathering:** Woodcutting, Mining, Foraging, Fishing, Hunting (includes skinning).
    - **Weapon & armor:** One-Handed, Two-Handed, Archery, Block, Light Armor, Heavy Armor.
    - **Movement & utility:** Sprinting, Climbing, Swimming, Stealth.
    - **Magic schools:** Destruction, Restoration, Nature, Arcane.
21. Skill XP is awarded only on a **meaningful use**, defined per skill in `DT_Progression_SkillXPSources`. In this spec, a hostile enemy is any creature in the `Hostile` or `Boss` category of `DT_Combat_Enemies`.
    - Woodcutting — each hit that damages a tree; bonus on felling.
    - Mining — each voxel of ore or stone removed with a pickaxe.
    - Foraging — each plant harvested.
    - Fishing — each fish caught. Fishing: equip a Fishing Rod (Carpenter recipe), cast into a water volume, wait for a bite (random 5–20 s, shortened by Fishing skill), and press interact within a 1-second bite window to catch. A miss catches nothing.
    - Hunting — each creature in the `Wildlife` or `Hostile` category of `DT_Combat_Enemies` that has a Hunting yield, awarded when skinned. Skinning is an interact on the creature's corpse that yields its Hunting yield items. Each player can skin a given corpse once, and each skinning player receives their own Hunting yield items and their own Hunting XP.
    - One-Handed / Two-Handed / Archery — each hit that deals damage to a hostile enemy.
    - Block — each blocked hit that would have dealt damage.
    - Light Armor / Heavy Armor — each hit taken from a hostile enemy while wearing ≥3 pieces of the matching armor category: Leather for Light Armor, Metal for Heavy Armor (Requirement 27). Cloth pieces train no armor skill.
    - Sprinting — each full second spent sprinting while stamina is draining.
    - Climbing — each meter climbed. Any terrain or building surface steeper than 60° is climbable by holding the jump input (gamepad A, keyboard Space; `specs/game-foundation/game-foundation.md` Requirement 14) against it, draining stamina per second. At 0 stamina the character falls.
    - Swimming — each meter swum.
    - Stealth — each second spent crouched, undetected (the enemy has not perceived or aggroed the character), and inside the perception radius of a hostile enemy. Enemy perception radius is defined per enemy in `DT_Combat_Enemies`. Crouching halves it, and Stealth skill reduces it further per `DT_Progression_SkillBonuses`.
    - Magic schools — each cast of a spell (a class ability tagged with that school, Requirement 16) that hits a target or completes its effect.
22. XP per use scales with target difficulty: `baseXP × difficultyMultiplier`, where the difficulty multiplier comes from the target's tier (ore tier, tree tier, enemy level relative to the player's character level) in `DT_Progression_SkillXPSources`.
23. Each skill level gives a passive bonus to its action (e.g., Mining level raises dig speed and ore yield chance). Per-level bonuses live in `DT_Progression_SkillBonuses`.
24. Skills at milestone levels (25, 50, 75, 100) unlock a perk defined in `DT_Progression_SkillPerks` (e.g., Woodcutting 50: chance for double logs).
25. A level-up of any skill, class level, or character level shows a HUD toast on the owning local player's viewport only.

### Tuning Data
26. Every value marked "tuning value" in this spec that names no other table lives in `DT_Progression_Rules`.

### Armor Categories
27. Every armor piece has exactly one armor category, set on its item definition: Cloth, Leather, or Metal. Tailor outputs (cloth armor and all clothing) are Cloth, Leatherworker armor is Leather, and Armorsmith armor is Metal. Cloth trains no armor skill, Leather trains Light Armor, and Metal trains Heavy Armor (Requirement 21).
28. Allowed armor categories per class, stored in `DT_Progression_Classes`:
    - Warrior, Knight, Paladin — Cloth, Leather, Metal.
    - Barbarian, Ranger, Rogue, Druid, Bard — Cloth, Leather.
    - Mage, Cleric, Warlock — Cloth.
    
    Any character can equip any armor category. An equipped armor piece whose category no held class allows gives 50% of its Armor and resistance values, using the same penalty value as `specs/combat-loot/combat-loot.md` Requirement 36. When the piece is also below its stat requirements, both penalties multiply (25% of its values at starting values).

## Data Flow
1. Player performs an action (e.g., a pickaxe removes an ore voxel).
2. The system handling the action (e.g., `UNamecTerrainEditComponent`) calls `UNamecProgressionComponent::AwardSkillXP(SkillId, SourceId, Difficulty)` on the server.
3. `UNamecProgressionComponent` looks up base XP in `DT_Progression_SkillXPSources`, applies the difficulty multiplier, adds the XP to the skill, and checks the skill XP curve for level-up.
4. The same call adds `skillXP × 25%` to character XP and checks the character XP curve.
5. On level-up, the component updates GAS attributes and passive bonuses, grants a stat point if character level changed, unlocks a class slot if a milestone was reached, and broadcasts `OnSkillLevelUp` / `OnCharacterLevelUp`.
6. Progression state replicates to the owning client. The HUD listens to the broadcasts and shows a toast.
7. On save, `UNamecProgressionComponent` serializes stats, spent/unspent points, classes, class levels, skill XP, boss first-kill flags, and equipped ability bar into `UNamecCharacterSave`.

## Edge Cases
1. When an action would award XP to a skill already at max level, no skill XP is added, but the character XP share is still awarded.
2. When a character is at max character level, character XP stops accruing, and the XP bar shows "MAX".
3. When a single action levels a skill more than once (large XP gain), each level-up applies in order and each perk unlocks. One toast shows the final level.
4. When a player spends a stat point that would push a stat above 30, the spend button is disabled for that stat.
5. When a player hits a training dummy (a Carpenter-crafted damageable target actor that is not an enemy), another player, or a creature in the `Wildlife` category of `DT_Combat_Enemies` (passive animals that never attack), weapon and magic skills get no XP. Skinning Wildlife still awards Hunting XP.
6. When a player repeatedly digs and refills the same voxel, Mining awards XP only for stone or ore voxels that are not flagged player-placed (see `specs/voxel-world/voxel-world.md` Requirements 1 and 12). Player-placed voxels award no XP.
7. When a player unlocks a class slot but has not chosen a class, the slot stays open and the Character screen shows a notification badge until filled.

## Acceptance Criteria
- [ ] A new character's stats are 10 across the board plus the starting class's bonuses.
- [ ] Mining 20 stone voxels raises Mining XP by the amount defined in `DT_Progression_SkillXPSources`, and raises character XP by 25% of that.
- [ ] Reaching character level 10 opens a second class slot, and choosing a class lets the player equip that class's abilities.
- [ ] Each character level-up adds exactly 1 unspent stat point, and spending it increases the chosen stat by 1.
- [ ] A skill reaching level 25 unlocks its level-25 perk.
- [ ] Hitting a training dummy awards 0 One-Handed XP.
- [ ] Refilling and re-digging a player-placed voxel awards 0 Mining XP.
- [ ] All progression state survives save → quit → load.
- [ ] A Mage wearing 3 Leather pieces gets 50% of their Armor values, and a Ranger wearing the same pieces gets 100% and earns Light Armor XP when hit by a hostile enemy.
- [ ] Two eligible players killing the same enemy each receive its full kill XP.

## Key Files
- `Source/NAMEC/Progression/NamecAttributeSet.h` — new; GAS attributes for the six stats, derived values, Armor, StatusResistance, and per-damage-type resistances.
- `Source/NAMEC/Progression/NamecProgressionComponent.h` — new; XP awarding, level-ups, stat points, class slots, broadcasts.
- `Source/NAMEC/Progression/NamecClassDefinition.h` — new; DataTable row struct for classes.
- `Source/NAMEC/Progression/NamecSkillTypes.h` — new; skill IDs, XP source row structs.
- `Source/NAMEC/UI/CharacterScreen/` — new; stat point spending, class selection, ability bar.
- `Content/Data/DT_Progression_Classes.uasset` — new; 11 class rows, including allowed armor categories (Requirement 28).
- `Content/Data/DT_Progression_ClassAbilities.uasset` — new; per-ability unlock level, base XP, school, mana cost, stamina cost, cooldown, base damage (spells), WeaponDamagePercent (non-spell abilities), BaseHealing (healing abilities), PoiseDamage, RequiredStat, and RequiredValue.
- `Content/Data/DT_Progression_SkillXPSources.uasset` — new; base XP per action and difficulty multipliers.
- `Content/Data/DT_Progression_SkillXPCurve.uasset` — new; XP per skill level.
- `Content/Data/DT_Progression_ClassXPCurve.uasset` — new; XP per class level.
- `Content/Data/DT_Progression_CharacterXPCurve.uasset` — new; XP per character level.
- `Content/Data/DT_Progression_SkillBonuses.uasset` — new; passive per-level bonuses.
- `Content/Data/DT_Progression_SkillPerks.uasset` — new; milestone perks.
- `Content/Data/DT_Progression_DerivedStats.uasset` — new; stat-to-derived-value coefficients, including the WIS healing coefficient and base MaxPoise (`specs/combat-loot/combat-loot.md` Requirements 38 and 39).
- `Content/Data/DT_Progression_Rules.uasset` — new; level caps, class slot levels, ability bar size, XP share, and other progression tuning values.
