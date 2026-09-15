# Stats and Classes

This page covers the six stats, character level and stat points, class slots, the 11 classes and their allowed armor and weapon categories, armor categories, weapon categories, class levels and abilities, and the ability bar.

← [Home](Home.md)

Characters grow along three independent tracks: stats (from character level), classes (combat abilities), and use-based skills (see [Skills](Skills.md)). Crafting Jobs are a fourth, separate track (see [Crafting and Jobs](Crafting-and-Jobs.md)). There is no respec or reset, and there are no backgrounds. Races are covered in [Races and Character Creation](Races-and-Character-Creation.md).

## The six stats

Every stat starts at 10. At creation, your race and your starting class each add their stat bonuses, and both are permanent:

```
Starting stat = 10 + race bonus + starting class bonus
```

For example, a Human (+1 to all six stats) Warrior (+2 STR, +1 CON) starts with STR 13, CON 12 and every other stat 11. Stat scores cap at 30 (starting value, tunable).

| Stat | Affects |
|------|---------|
| STR | Melee damage, max carry weight, max equip load, heavy-weapon requirements |
| DEX | Ranged damage, attack speed, dodge-roll recovery, light-weapon requirements |
| CON | Max health, health regen, base max stamina, max equip load, resistance to hunger/thirst drain |
| INT | Destruction and Arcane spell power, max mana, Destruction and Arcane spell requirements |
| WIS | Restoration and Nature spell power, mana regen, status-effect resistance, temperature tolerance, Restoration and Nature spell requirements, healing from class abilities |
| CHA | Strength and radius of party buffs and auras (Bard, Paladin, Cleric abilities, and the Hundari's Rally Howl), and Vendor prices |

### CHA and Vendor prices

Each point of CHA modifier cuts Vendor buy prices by 3% (starting value, tunable). A negative modifier raises them. Your reputation tier with the Vendor's kingdom multiplies on top:

```
Price = Value × (1 − CHA modifier × 0.03) × reputation tier multiplier
```

For example, CHA modifier +3 pays 9% less than CHA modifier 0 at the same tier. Selling prices are not affected by CHA. See [Factions and Kingdoms](Factions-and-Kingdoms.md).

## Modifier formula

```
Modifier = floor((score - 10) / 2)
```

| Score | 8 | 10 | 11 | 12 | 14 | 16 | 20 | 30 |
|-------|---|----|----|----|----|----|----|----|
| Modifier | -1 | 0 | 0 | +1 | +2 | +3 | +5 | +10 |

## Character level and stat points

- Character level runs 1–50 (starting value, tunable).
- Character XP comes from five sources:
  - 25% (starting value, tunable) of all skill XP earned
  - the same 25% of all class XP earned
  - enemy kills: every eligible player gets the full kill XP, not a split
  - boss first-kills: the first time a character defeats a given region's boss, it gets first-kill XP instead of kill XP. This also applies in a re-summoned fight if the character has not beaten that boss before.
  - quest rewards: the quest's full character XP reward on turn-in (see [Factions and Kingdoms](Factions-and-Kingdoms.md))
- Each character level-up grants 1 unspent stat point (starting value, tunable). Spend it in the Character screen for +1 to one stat. Spent points are permanent. Unspent points are kept forever.
- The spend button is disabled for a stat already at 30.
- At max character level, character XP stops and the XP bar shows "MAX".
- Crafting Job XP does not feed character XP.

## Class slots

| Character level | Class slot |
|-----------------|------------|
| 1 | 1st (starting class, picked at creation) |
| 10 | 2nd |
| 25 | 3rd |
| 40 | 4th |

(starting values, tunable)

- When a slot unlocks, the player may choose any class not already held, in the Character screen, at any time afterward. An unfilled slot shows a notification badge.
- All held classes are active at the same time. The player can use any ability, weapon category or armor category allowed by any held class.
- Starting-class stat bonuses apply only to the starting class, not to classes added later.

## The 11 classes

| Class | Allowed armor categories | Allowed weapon categories |
|-------|--------------------------|---------------------------|
| Warrior | Cloth, Leather, Metal | One-Handed Sword, One-Handed Axe, One-Handed Mace, Two-Handed Sword, Two-Handed Axe, Shield |
| Knight | Cloth, Leather, Metal | One-Handed Sword, One-Handed Mace, Spear, Shield |
| Paladin | Cloth, Leather, Metal | One-Handed Sword, One-Handed Mace, Two-Handed Hammer, Shield |
| Barbarian | Cloth, Leather | Two-Handed Axe, Two-Handed Hammer, Two-Handed Sword, One-Handed Axe |
| Ranger | Cloth, Leather | Bow, One-Handed Sword, Dagger |
| Rogue | Cloth, Leather | Dagger, One-Handed Sword, Bow |
| Druid | Cloth, Leather | Staff, Spear, Dagger |
| Bard | Cloth, Leather | One-Handed Sword, Dagger, Bow |
| Mage | Cloth | Staff, Dagger |
| Cleric | Cloth | One-Handed Mace, Staff, Shield |
| Warlock | Cloth | Staff, Dagger, One-Handed Sword |

Each class also has stat bonuses (starting class only) and an ability list. The specs give Warrior's bonus (+2 STR, +1 CON) as an example and name one ability, the Cleric's "Revivify" (instant revive within 15 m). Other bonuses and ability names are not yet listed in the specs.

## Armor categories

Every armor piece has exactly one category.

| Category | Made by | Trains skill |
|----------|---------|--------------|
| Cloth | Tailor (cloth armor and all clothing) | none |
| Leather | Leatherworker | Light Armor |
| Metal | Armorsmith | Heavy Armor |

Capes (Cloak slot) and jewelry (Neck and Ring slots) are not armor. They have no armor category, take no category penalty, train no armor skill, and don't count toward the 3 pieces an armor skill needs. See [Inventory](Inventory.md).

## Weapon categories

Every weapon and shield has exactly one category.

| Category | Hands | Trains skill | Made by |
|----------|-------|--------------|---------|
| One-Handed Sword | One | One-Handed | Blacksmith |
| One-Handed Axe | One | One-Handed | Blacksmith |
| One-Handed Mace | One | One-Handed | Blacksmith |
| Dagger | One | One-Handed | Blacksmith |
| Spear | One | One-Handed | Blacksmith (metal spear head) |
| Two-Handed Sword | Both hand slots | Two-Handed | Blacksmith |
| Two-Handed Axe | Both hand slots | Two-Handed | Blacksmith |
| Two-Handed Hammer | Both hand slots | Two-Handed | Blacksmith |
| Staff | One (Right Hand) | One-Handed | Carpenter |
| Bow | Both hand slots | Archery | Carpenter |
| Shield | Left Hand | Block | Armorsmith |

- A spear is wielded one-handed, so you can carry a shield with it.
- A staff is a melee focus in your Right Hand. Your Left Hand can hold a shield or torch with it. Its melee hits train One-Handed. Spells still come only from class abilities.
- Tools (axes, pickaxes, shovels, Hammers, Fishing Rods) and torches have no weapon category. See [Combat and Loot](Combat-and-Loot.md) for tool hits on enemies.
## Wearing or wielding outside your classes

- Any character can equip any weapon or armor.
- A weapon of a category no held class allows deals 50% damage (starting value, tunable). Example: a Mage with a One-Handed Sword deals 50% damage, a Warlock with the same sword deals full damage.
- A shield of a category no held class allows has its block percentage halved.
- An armor piece of a category no held class allows gives 50% of its Armor and resistance values.
- Gear below its stat requirements (for example STR 16) takes the same 50% penalty.
- Penalties multiply. An armor piece both outside your classes and below requirements gives 25%.

Example: a Mage wearing Leather gets 50% of its Armor. A Ranger wearing the same pieces gets 100% and trains Light Armor.

## Class levels and abilities

- Each held class has its own class level, 1–20 (starting value, tunable). A newly added class starts at class level 1 with its level-1 abilities.
- Each class has 8 abilities (starting count, tunable). Each ability unlocks at a set class level.
- Class XP is earned each time one of that class's abilities hits a target or completes its effect.
- Abilities cost mana and/or stamina and have cooldowns.
- **Spells** are class abilities tagged with one magic school: Destruction, Restoration, Nature or Arcane. Spells are not items.
- Spell damage uses the spell's base damage. Spell power comes from INT (Destruction, Arcane) or WIS (Restoration, Nature).
- A non-spell damaging ability deals a percentage of your Right Hand weapon's damage (for example 150%). With an empty Right Hand, or a tool or torch in it, the unarmed profile is used.
- Healing abilities scale with WIS and, for spells, the school's skill. Healing never goes above max health.
- Each ability has a required stat and value. Below it, the ability deals 50% damage and healing.

## Ability bar

Up to 6 abilities (starting value, tunable), drawn from any held classes plus your racial ability.

- Your racial ability always takes one slot and cannot be removed, so you can equip up to 5 class abilities. It starts in the last slot (slot 6).
- Moving an ability onto an occupied slot swaps the two.
- Racial abilities cost no stamina or mana, have no stat requirement and give no class XP. See [Races and Character Creation](Races-and-Character-Creation.md).

| Slot | Gamepad | Keyboard |
|------|---------|----------|
| 1 | LB + A | Z |
| 2 | LB + B | X |
| 3 | LB + X | C |
| 4 | LB + Y | V |
| 5 | LB + RB | B |
| 6 | LB + RT | N |

The ability bar is separate from inventory hotkeys. Full controls are on [Co-op and Controls](Co-op-and-Controls.md).

## Level-up feedback

A level-up of any skill, class or character level shows a toast on the owning player's viewport only.

## Source spec

- [Character Progression](../../specs/character-progression/character-progression.md) (including weapon categories)
- [Character Creation](../../specs/character-creation/character-creation.md) (racial stat bonuses and the racial ability slot)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (Vendor prices and quest XP)
