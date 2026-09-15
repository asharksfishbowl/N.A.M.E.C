# Skills

This page covers use-based skill leveling, every skill and how it earns XP, milestone perks, and the anti-farming rules.

← [Home](Home.md)

## How skills level

- Skills level 1–100 (starting value, tunable) by performing the matching action.
- XP is earned only on a **meaningful use** (listed below).
- XP per use = base XP × difficulty multiplier × skill XP gain multiplier. The difficulty multiplier comes from the target's tier: ore tier, tree tier, or enemy level relative to your character level.
- Skill XP gain multiplier = 1 + racial bonus (Human Versatile +10%, otherwise 0) + any "+skill XP gain" affix for that skill. The bonuses add together.
- Each skill level gives a passive bonus to its action (for example, Mining raises dig speed and ore yield chance).
- 25% (starting value, tunable) of all skill XP (after the multipliers) also goes to character XP. See [Stats and Classes](Stats-and-Classes.md).
- A skill already at max level earns no more skill XP, but still gives the character XP share.
- If one action gives enough XP for several levels, every level-up and perk applies in order. One toast shows the final level.

"Hostile enemy" below means a creature in the Hostile or Boss category. This includes Bandits, Beastmen and raiders, but not town NPCs (see [Factions and Kingdoms](Factions-and-Kingdoms.md)).

## Gathering

| Skill | Earns XP for |
|-------|--------------|
| Woodcutting | Each hit that damages a tree, plus a bonus for felling it |
| Mining | Each voxel of stone or ore removed with a pickaxe |
| Foraging | Each plant harvested |
| Fishing | Each fish caught |
| Hunting | Skinning a Wildlife or Hostile creature that has a Hunting yield |

**Fishing:** equip a Fishing Rod (Carpenter recipe), cast into water, and wait for a bite (random 5–20 s, starting value, tunable; shortened by Fishing skill). Press interact within the 1-second bite window to catch. A miss catches nothing.

**Hunting (skinning):** interact with the creature's corpse. Each player can skin a given corpse once and gets their own yield items and their own Hunting XP.

## Weapon and armor

| Skill | Earns XP for |
|-------|--------------|
| One-Handed | Each hit that damages a hostile enemy (including unarmed hits and the Felari's Pounce) |
| Two-Handed | Each hit that damages a hostile enemy |
| Archery | Each hit that damages a hostile enemy |
| Block | Each blocked hit that would have dealt damage |
| Light Armor | Each hit taken from a hostile enemy while wearing 3 or more Leather pieces |
| Heavy Armor | Each hit taken from a hostile enemy while wearing 3 or more Metal pieces |

Cloth pieces train no armor skill.

## Movement and utility

| Skill | Earns XP for |
|-------|--------------|
| Sprinting | Each full second sprinting while stamina drains |
| Climbing | Each meter climbed |
| Swimming | Each meter swum |
| Stealth | Each second crouched, undetected, inside a hostile enemy's perception radius |

- **Climbing:** any terrain or building surface steeper than 60° can be climbed by holding jump against it. Climbing drains stamina per second, and at 0 stamina you fall. Climb speed is covered in [Survival](Survival.md).
- **Stealth:** each enemy has its own perception radius. Crouching halves it, and Stealth skill shrinks it further. A Hundari's Loud trait then makes the final radius 25% larger. "Undetected" means the enemy has not perceived or aggroed you. A Hostile enemy detects you when you are inside that radius and in its line of sight (see [Combat and Loot](Combat-and-Loot.md)).

## Magic schools

| Skill | Earns XP for |
|-------|--------------|
| Destruction | Each cast of a Destruction spell that hits a target or completes its effect |
| Restoration | Same, for Restoration spells |
| Nature | Same, for Nature spells |
| Arcane | Same, for Arcane spells |

## Perks

Each skill unlocks a perk at levels 25, 50, 75 and 100. Example from the spec: Woodcutting 50 gives a chance for double logs.

## Anti-farming rules

- Hitting a **training dummy** (a Carpenter-crafted target), **another player**, or a **Wildlife** creature (passive animals that never attack) gives no weapon or magic skill XP. Skinning Wildlife still gives Hunting XP.
- Digging and refilling the same spot: Mining XP comes only from stone or ore that is not player-placed. Voxels you filled in give no XP.
- Filling and re-digging never produces a net gain of items either. See [World and Building](World-and-Building.md).
- Armor skills need 3 or more matching pieces, and only hits from hostile enemies count.
- Sprinting only counts while stamina is actually draining.

## Source spec

- [Character Progression](../../specs/character-progression/character-progression.md)
