# Combat and Loot

This page covers Souls-style combat, damage types and status effects, fall damage, downed/revive/death, enemies, perception and taunts, scaling, bosses and summoning, per-player loot, rarity and affixes, chests, and durability.

← [Home](Home.md)

Combat is deliberate: stamina-gated attacks, dodge rolls, blocking, parrying and lock-on. Hits are physical, decided by weapon hitboxes during attack animations, not dice rolls. There is no PvP and no mounted combat.

## Stamina costs

Light attacks, heavy attacks, dodge rolls, blocking (per hit absorbed) and sprinting (per second) cost stamina. Class abilities cost mana and/or stamina set per ability. See [Survival](Survival.md) for stamina regen.

## Dodge roll

- Grants invulnerability frames. An area-of-effect tick during those frames is ignored.
- The i-frame window and roll recovery depend on your **equip load** (see [Inventory](Inventory.md)):

| Equip load tier | Equip load |
|-----------------|------------|
| Light | under 30% |
| Medium | 30% to under 70% |
| Heavy | 70% to 100% |
| Overloaded | over 100%: cannot dodge roll |

An Over-Encumbered character (carrying too much weight) also cannot dodge roll.

## Block and parry

- **Block** with a shield or weapon to cut incoming damage by the item's block percentage. Each blocked hit drains stamina. If stamina hits 0 while blocking, your guard breaks and you are staggered.
- **Parry** needs a parry-capable item (shields, some weapons). A parry inside the parry window staggers the attacker and opens a riposte window. A riposte is a critical hit.

## Lock-on

- Targets the hostile closest to screen center within 20 m (starting value, tunable).
- While locked on, the camera frames the target and movement strafes.
- Flick the right stick (or mouse) to switch to the next target in that direction.

## Poise and stagger

- Players and enemies have Poise. Hits deal poise damage. At 0 Poise the target staggers.
- Poise regenerates after 3 seconds (starting value, tunable) without taking hits.
- A player's max Poise = a base value + the Poise of every equipped armor piece.
- Poise damage comes from:
  - the weapon (or the unarmed profile) for weapon hits
  - each class ability's own poise damage
  - racial abilities: Felari Pounce 60, Ursan Mauling Roar 100 (a Boss takes 25% of Mauling Roar's)
  - each enemy attack's poise damage
- Stagger comes only from Poise reaching 0, a guard break, or a parry.

## Damage

Outgoing damage:

```
Damage = (WeaponBase + StatScaling + AffixFlat)
       × (1 + SkillBonus + AffixPercent)
       × (1 − TargetResistance)
       × RequirementPenalty × (1 − WeakenedPenalty) × CritMultiplier
```

- **StatScaling:** the weapon's scaling stat(s) and grade. For spells, INT (Destruction, Arcane) or WIS (Restoration, Nature).
- **SkillBonus:** the weapon's skill (One-Handed, Two-Handed, Archery) or the spell's school.
- **RequirementPenalty:** ×0.5 for each that applies: below the weapon's stat requirements, weapon category allowed by no held class, below a class ability's required stat. They multiply.
- **WeakenedPenalty:** 0.2 while Weakened, otherwise 0.
- **CritMultiplier:** applies only to riposte critical hits.
- **Unarmed:** with an empty Right Hand, or a tool or torch in it, the unarmed profile is used (Blunt damage, STR scaling, One-Handed skill).
- **Non-spell class abilities** deal a percentage of this damage (for example 150%) using the Right Hand weapon, or the unarmed profile.
- **Felari Pounce** always uses the unarmed profile at 100%, with every penalty above applied. See [Races and Character Creation](Races-and-Character-Creation.md).

Incoming enemy damage:

```
IncomingDamage = EnemyDamage × PlayerScaling × (1 − TargetResistance) × (1 − BlockPercent)
```

### Damage types and resistance

| Group | Types |
|-------|-------|
| Physical | Slash, Pierce, Blunt |
| Elemental and other | Fire, Frost, Lightning, Poison, Holy, Shadow |

- Players have an Armor value plus one resistance per damage type. Equipped items (and resistance affixes) add to them.
- Against Slash, Pierce or Blunt: resistance = Armor ÷ (Armor + 100) + the matching subtype resistance. The constant 100 is a starting value, tunable.
- Against other types: the matching resistance only.
- Enemies have per-type resistances but no Armor.
- Resistance is capped at 90%.

### Status effects

| Effect | What it does |
|--------|--------------|
| Poison | Damage over time |
| Bleed | Burst damage when its buildup meter fills |
| Burn | Fire damage over time |
| Frostbite | Reduces stamina regen |

- Effects build up and last for set durations.
- Every buildup you receive is multiplied by (1 − your status-effect resistance). WIS raises status-effect resistance.
- A Sauren also takes 25% less Poison buildup, and has +25% Poison resistance.
- The Sauren's Shed Skin clears all four of these effects, both the buildup and the active effect. It does not clear Weakened, Wet, Salty or survival states.

### Fall damage

- Landing after a fall at or above a minimum height deals fall damage: (fall distance − minimum height) × damage per meter, as a percentage of max health.
- Fall distance is measured from the highest point of the fall.
- Armor, resistances and blocking do not reduce it. The minimum height and damage per meter are starting values, tunable.
- It applies to players and enemies.
- A Felari takes no fall damage from falls under 8 m. Falls of 8 m or more deal normal damage for the full distance.

### Friendly fire

A world setting, off by default. When off, player attacks, spells and area effects never hurt other players. When on, they do. Player attacks never damage building pieces either way. Falling tree logs always damage players.

## Downed, revive and death

**Downed** (sessions with 2+ players):

- At 0 health you are Downed instead of dead. You can crawl slowly but cannot attack, use items or open your inventory.
- A 30-second (starting value, tunable) bleed-out timer starts.
- Hits while Downed don't reduce health. Each hit removes 1 second of bleed-out per 1% of max health it would have dealt.
- Survival meters pause while Downed.

**Revive:**

- A teammate holds interact for 4 seconds within 2 m. Taking damage interrupts the revive.
- The Cleric ability "Revivify" revives instantly within 15 m.
- You return at 30% max health.

**Death:**

- You die when bleed-out ends. If every player is downed or dead at once, all downed players die.
- In a single-player session there is no Downed state. 0 health means death.
- You respawn 5 seconds later at your bed in this world, or at the world spawn point if you have no bed or it was destroyed.
- On respawn: full health, 50% Hunger and Thirst, body temperature 37 °C, Fatigue 0, and **Weakened** for 5 minutes (−20% damage dealt, −20% max stamina).
- Equipped gear loses 10% durability. **No items are dropped.**
- Leaving cleanly while Downed (or any save written while Downed or dead) counts as a death: you respawn with the death effects on your next world entry. If your connection drops unexpectedly instead, your character keeps its last autosave.

## Enemies

| Category | Behaviour |
|----------|-----------|
| Hostile | Attacks players. Drops loot and boss offerings. |
| Wildlife | Never attacks, flees when damaged. Gives no weapon or magic XP. Can be skinned. |
| Boss | One per region, summoned at an arena altar. |

- Each enemy has a level, health, poise, resistances, damage, attacks, perception radius, loot table, optional hunting yield, region and XP rewards.
- Enemies spawn from their region's list, filtered by time of day. Spawn density and the maximum live enemies per streamed chunk are set per region.
- Enemies never spawn within 25 m of a placed building piece or within 40 m of a player (starting values, tunable).

### Perception and aggro

- A Hostile enemy notices and attacks a player who is inside its perception radius **and** in its line of sight.
- Each enemy has its own perception radius. How the radius is worked out:
  1. Crouching halves it.
  2. Stealth skill shrinks it further (see [Skills](Skills.md)).
  3. A Hundari's Loud trait then makes the final radius 25% larger.

### Threat and taunts

- Enemies target the player with the highest threat, built from damage dealt, healing done and taunt abilities.
- A **taunt** forces the enemy to target the taunting player for its duration. When it ends, the enemy goes back to normal threat targeting.
- Ursan Mauling Roar taunts Hostile enemies for 6 seconds and Bosses for 3 seconds (starting values, tunable).

### Player-count scaling

Each additional player adds +60% enemy health and +10% enemy damage (starting values, tunable).

| Players | Enemy health | Enemy damage |
|---------|--------------|--------------|
| 1 | ×1.0 | ×1.0 |
| 2 | ×1.6 | ×1.1 |
| 3 | ×2.2 | ×1.2 |
| 4 | ×2.8 | ×1.3 |

- Existing enemies keep their scaling when someone joins. New spawns use the new count.
- A boss's scaling is locked at summon.

## Bosses

- Each region has one boss with a fixed arena, placed by world generation.
- **Summoning:** place that region's offering item at the arena altar. Offerings drop from the region's Hostile enemies. Each eligible player rolls for one separately.
- Bosses have named, multi-phase fights with telegraphed attacks.
- When the fight starts, a barrier seals the arena. Players inside stay locked in until the boss dies or all of them are dead. Nobody outside can enter, including players who join mid-fight.
- If everyone inside dies, the boss resets to full health and the barrier drops.
- If a player disconnects inside, they are removed from the fight.

### Boss rewards

Every player inside the barrier when the boss dies (and only them) gets:

- **Regions 1–7:** a guaranteed trophy and that boss's distinct tier material, needed for the next tier of station upgrades and gear.
- **Volcanic (final):** a guaranteed Legendary item and a trophy. It gates nothing.
- **XP:** first-kill XP if this character has never beaten this boss, otherwise normal kill XP.

Any boss can be re-summoned for more loot with a new offering.

## Loot

### Per-player drops

- When an enemy dies, each eligible player gets their own independently rolled drops, visible and pickable only by them. Split-screen players on the same machine can't see each other's drops either.
- **Eligible:** within 50 m (starting value, tunable) of the enemy when it dies, or dealt damage to it. For bosses, only players inside the barrier.
- Every eligible player also gets the enemy's full kill XP.
- Unclaimed drops disappear after 10 minutes.
- Items you drop from your inventory become shared pickups anyone can see and take. This is how players share items. There is no trade screen.
- There is no gold or currency.

### Loot chests

- Placed by world generation in caves and ruins. They are not crafted storage containers.
- Each player can open each chest once, with their own roll. Only the opener gets that roll.
- Chests never refill. Who has opened each chest is saved with the world.

### Rarity and affixes

| Rarity | Affixes |
|--------|---------|
| Common | 0 |
| Magic | 1–2 |
| Rare | 3–4 |
| Epic | 4–5 |
| Legendary | 5 + one unique Legendary power |

- Rarity odds depend on the item-level band and are improved by Magic Find affixes.
- **Item level:** the enemy's level, or for a chest a random level within its region's band (rolled per player), or the crafted item level.
- Affix values scale with item level. No affix appears twice on one item.
- Affix examples: +STR/DEX/CON/INT/WIS/CHA, +% damage of a type, +resistance, +max health, +stamina regen, +Insulation/Cooling, +skill XP gain for a skill, +Magic Find, life on hit.

### Durability and requirements

- Equipment loses durability with use. At 0 it is unusable, not destroyed, and keeps its hotkey.
- Repair at the item's crafting station or with a Repair Kit. See [Crafting and Jobs](Crafting-and-Jobs.md).
- Gear may have stat requirements (for example STR 16). You can still equip it, at 50% damage or 50% Armor and resistance.

## Source spec

- [Combat and Loot](../../specs/combat-loot/combat-loot.md)
- [Character Creation](../../specs/character-creation/character-creation.md) (racial combat traits)
