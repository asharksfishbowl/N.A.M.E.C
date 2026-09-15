# Combat and Loot

This page covers Souls-style combat, heavy attacks, block and parry sources, dual-wielding, torch bashes, bows and arrows, two-handing, tool hits on enemies, executions, damage types and status effects, fall damage, downed/revive/death, enemies and factions, town NPCs and raiders, perception and taunts, scaling, bosses and summoning, per-player loot and gold, rarity and affixes, chests, and durability.

← [Home](Home.md)

Combat is deliberate: stamina-gated attacks, dodge rolls, blocking, parrying and lock-on. Hits are physical, decided by weapon hitboxes during attack animations (or, for bows, by server-side projectiles), not dice rolls. There is no PvP and no mounted combat.

## Stamina costs

Light attacks, off-hand light attacks, heavy attacks, dodge rolls, blocking (per hit absorbed), sprinting (per second) and each bow shot cost stamina. Class abilities cost mana and/or stamina set per ability. See [Survival](Survival.md) for stamina regen.

## Heavy attacks

- RT (middle mouse) makes a heavy attack with your Right Hand weapon, or unarmed with an empty Right Hand.
- A heavy attack deals ×1.6 damage and ×2.0 poise damage compared with a light attack with the same weapon (starting values, tunable), and trains the same skill.
- With a bow, axe, pickaxe, Fishing Rod or torch in your Right Hand, RT does nothing. With a shovel or Hammer, RT does that tool's own action instead (see [Co-op and Controls](Co-op-and-Controls.md)).
- Class abilities never get the heavy attack bonus.

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

- **Block** to cut incoming damage by the blocking item's block percentage. Which item blocks depends on your hands (see Dual-wielding below). A shield or weapon of a category none of your classes allows blocks at half its percentage. Each blocked hit drains stamina. If stamina hits 0 while blocking, your guard breaks and you are staggered.
- Blocking also halves the poise damage you take (×0.5, starting value, tunable).
- A bow can't block, torches and tools never block or parry, and a stowed Left Hand item (see Two-handing) can't block, parry or attack.
- **Parry** uses the same item that blocks (or your Left Hand weapon when dual-wielding), and only works if that item is parry-capable (shields, some weapons). A parry inside the parry window staggers the attacker and opens a riposte window. A riposte is a critical hit. Parry isn't available with a bow.

## Dual-wielding

Any one-handed weapon except a Staff can go in your Left Hand. What LT (right mouse) does depends on your hands; the first matching row applies:

| Hands | LT | Parry (LB + LT) |
|-------|----|-----------------|
| Bow in the Right Hand | Aims | Not available |
| Two-handed weapon, or two-handing a one-handed weapon | Blocks with that weapon (no 50% empty-hand reduction) | That weapon, if parry-capable |
| One-handed weapon in the Left Hand | Off-hand light attack (you can't block) | The Left Hand weapon, if parry-capable |
| Shield in the Left Hand | Blocks with the shield | The shield, if parry-capable |
| Torch in the Left Hand | Nothing (no block) | The Right Hand weapon, if parry-capable |
| Empty Left Hand, weapon in the Right Hand | Blocks with the Right Hand weapon at 50% of its block percentage (starting value, tunable) | The Right Hand weapon, if parry-capable |
| Empty Left Hand, Right Hand empty or holding a tool or torch | Nothing | Not available |

- An **off-hand light attack** uses the Left Hand weapon's own damage values, poise damage and category skill, and has its own stamina cost (starting value, tunable).
- Heavy attacks, executions and class abilities always use your Right Hand.

## Torch bash

- A light attack with a torch in your Right Hand is a **torch bash**: an unarmed hit that also builds up Burn on the target (starting value, tunable). It trains One-Handed like other unarmed hits.
- A light attack with a tool in your Right Hand is still a tool swing (see Tools against enemies).

## Bows and arrows

- A bow is a **two-handed item**: it takes both hand slots, so you can't hold a shield or torch with it, and the two-handing toggle does nothing.
- LT (right mouse) aims instead of blocking, and RB (left mouse) fires an aimed shot while aiming. See [Co-op and Controls](Co-op-and-Controls.md).
- **Quick shot:** RB without aiming fires a quick shot at 50% of an aimed shot's damage (starting value, tunable). It flies where your camera points, or at your lock-on target, with no other aim assist.
- RT does nothing with a bow.
- **Each shot costs stamina**, aimed or quick (starting value, tunable).
- **Each shot uses 1 arrow** from your Ammo slot. With no arrows equipped the bow can't fire, and "No arrows" shows in your viewport.
- **Damage:** the normal damage formula, with the bow's base damage plus the arrow's own ArrowDamage. Arrows are Carpenter recipes, and the crafter's Carpenter level raises ArrowDamage. Looted or bought arrows have Job level 1 potency (see [Crafting and Jobs](Crafting-and-Jobs.md)).
- **Hits are decided by the server.** The server spawns the arrow and resolves what it hits. Your game may show a predicted arrow straight away, but that arrow never deals damage.
- **Class abilities with a bow:** a damaging non-spell class ability used with a bow equipped counts as a bow shot. It uses the bow's and the arrow's damage (times the ability's percentage), uses 1 arrow, and fires a server-side arrow. With no arrows it can't be used, and "No arrows" shows. It costs the ability's own mana and stamina, not the bow shot stamina.
- Hits that damage a hostile enemy train Archery. See [Skills](Skills.md).

## Two-handing

Press Y (keyboard R) with a one-handed weapon in your Right Hand to hold it in both hands. Press it again to go back. (While an execution prompt is showing, Y executes instead.)

- Your **Left Hand item is stowed**. It stays equipped and still counts toward equip load, but gives none of its effects (for example a stowed shield gives no Armor and can't block).
- Weapon damage ×1.25, and the STR part of the weapon's stat scaling ×1.5 (starting values, tunable).
- Hits still train the weapon's own skill, One-Handed.
- Two-handed weapons and bows are always held in two hands, so the toggle does nothing for them. Shields can't be two-handed.
- Two-handing ends by itself when your Right Hand weapon changes or you equip something into your Left Hand.
- Two-handing isn't saved. Every character loads into a world holding its weapon one-handed.

## Tools against enemies

- Tools (axe, pickaxe, shovel, Hammer, Fishing Rod) have no weapon category. Every tool except the Fishing Rod has its own base damage. A Fishing Rod casts instead of swinging, so it never makes a tool hit.
- With an axe or pickaxe in your Right Hand, RT does nothing (no heavy attack with any tool).
- A tool swing that hits a Hostile (including Bandits, Beastmen and raiders), Boss or Wildlife enemy deals 50% (starting value, tunable) of the damage the formula gives with the tool's base damage and the unarmed profile's scaling and damage type.
- Tool hits on enemies give **no skill XP at all**: no weapon skill, and no Woodcutting or Mining.
- A tool hit on a town NPC counts as an attack: it costs the same reputation and makes Guards react the same way.

## Lock-on

- Targets the hostile closest to screen center within 20 m (starting value, tunable).
- While locked on, the camera frames the target and movement strafes.
- Flick the right stick (or mouse) to switch to the next target in that direction.

## Executions

An execution is a finishing move on a weakened enemy.

**When the prompt appears** (only in your own viewport):

- The enemy is a Hostile enemy (including Bandits, Beastmen and raiders). Never a Boss, Wildlife or town NPC.
- It is within 2.5 m and in front of you (starting value, tunable).
- It is below 20% health (starting value, tunable), **or** it is in the riposte window after **your** parry.
- You aren't Downed, already executing, in the air, climbing or swimming, and you have no menu or interaction screen open (any menu or screen counts, for example the inventory, pause menu, build menu, a crafting station, a vendor, or the Guard Captain's fine screen).
- If several enemies qualify, your lock-on target is used when it qualifies, otherwise the closest one.

**Executing:**

- Press **Y** (gamepad) or **R** (keyboard) while the prompt shows. This replaces the two-handing toggle while the prompt is up. LB + Y still casts ability 4.
- Executions cost no stamina.
- The server checks range, that the enemy isn't already being executed or airborne, and that there's room for the animation (so slopes, walls and building pieces are handled). If anything fails, nothing happens and the prompt hides.
- A paired attacker/victim animation plays, picked by your Right Hand weapon category and the enemy's skeleton type. An empty hand, a bow, a tool or a torch uses the Unarmed set. If no animation exists for the pair, a generic one for that skeleton type plays.
- During the animation, **you and the victim are invulnerable** and can't be interrupted. Other enemies keep fighting.
- The victim dies at the animation's kill moment. It counts as your kill for loot, kill XP and quest credit.
- In first-person, the camera switches to a third-person execution camera and back afterward.

**Rewards** (starting values, tunable):

- Restore 20% of your max stamina.
- Weapon skill XP equal to 3 normal hits. Whenever the Unarmed set plays (including with a bow), the XP goes to One-Handed.

**Co-op:** if two players try to execute the same enemy, the first request the server gets wins. If the attacker disconnects mid-animation, the enemy is released at its current health.

**Settings:** "Execution prompts" (on by default, per local player, saved per local player slot) hides prompts and turns executions off. Y and R then always toggle two-handing.

## Poise and stagger

- Players and enemies have Poise. Hits deal poise damage. At 0 Poise the target staggers.
- Poise regenerates after 3 seconds (starting value, tunable) without taking hits.
- A player's max Poise = a base value + the Poise of every equipped armor piece.
- Poise damage comes from:
  - the weapon (or the unarmed profile) for weapon hits, ×2.0 for a heavy attack (starting value, tunable)
  - each class ability's own poise damage
  - racial abilities: Felari Pounce 60, Ursan Mauling Roar 100 (a Boss takes 25% of Mauling Roar's)
  - each enemy attack's poise damage
- While you block, poise damage you take is halved (×0.5, starting value, tunable).
- Stagger comes only from Poise reaching 0, a guard break, or a parry.

## Damage

Outgoing damage:

```
Damage = (WeaponBase + StatScaling + AffixFlat)
       × (1 + SkillBonus + AffixPercent)
       × (1 − TargetResistance)
       × RequirementPenalty × (1 − WeakenedPenalty) × CritMultiplier
       × TwoHandingMultiplier × HeavyAttackMultiplier
```

- **WeaponBase:** the weapon's base damage. For a bow shot, the bow's base damage plus the arrow's ArrowDamage. For spells, the spell's base damage.
- **StatScaling:** the weapon's scaling stat(s) and grade. For spells, INT (Destruction, Arcane) or WIS (Restoration, Nature). While two-handing, the STR part is ×1.5.
- **SkillBonus:** the skill of the weapon's category (One-Handed, Two-Handed, Archery; see [Stats and Classes](Stats-and-Classes.md)) or the spell's school. Tool hits get none.
- **RequirementPenalty:** ×0.5 for each that applies: below the weapon's stat requirements, weapon category allowed by no held class, below a class ability's required stat. They multiply.
- **WeakenedPenalty:** 0.2 while Weakened, otherwise 0. Weakened only lowers damage dealt, never healing.
- **CritMultiplier:** applies only to a player's riposte critical hits. Enemies never land critical hits.
- **TwoHandingMultiplier:** ×1.25 while two-handing a one-handed weapon, otherwise 1.
- **HeavyAttackMultiplier:** ×1.6 for a heavy attack hit (starting value, tunable), otherwise 1.
- **Unarmed:** with an empty Right Hand, or a tool or torch in it, the unarmed profile is used (Blunt damage, STR scaling, One-Handed skill). A torch bash also builds up Burn.
- **Off-hand light attacks** use the Left Hand weapon's base damage, scaling, category skill and requirement penalties.
- **Non-spell class abilities** deal a percentage of this damage (for example 150%) using the Right Hand weapon, or the unarmed profile.
- **Felari Pounce** always uses the unarmed profile at 100%, with every penalty above applied. See [Races and Character Creation](Races-and-Character-Creation.md).

Incoming enemy damage:

```
IncomingDamage = EnemyDamage × PlayerScaling × (1 − TargetResistance) × (1 − BlockPercent)
```

- Enemy and Guard attacks never deal critical damage.
- Class ability healing has no Weakened penalty.

### Damage types and resistance

| Group | Types |
|-------|-------|
| Physical | Slash, Pierce, Blunt |
| Elemental and other | Fire, Frost, Lightning, Poison, Holy, Shadow |

- Every weapon has one damage type. Weapon hits, off-hand attacks and non-spell class abilities deal the weapon's type, and bow shots deal the bow's type. Unarmed hits and tool swings deal Blunt. Each spell has its own damage type.

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
- On respawn: full health, full Mana, 50% Hunger and Thirst, body temperature 37 °C, Fatigue 0, and **Weakened** for 5 minutes (−20% damage dealt, −20% max stamina; healing isn't reduced).
- Equipped gear loses 10% durability (jewelry has no durability). **No items are dropped and no gold is lost.**
- Leaving cleanly while Downed (or any save written while Downed or dead) counts as a death: you respawn with the death effects on your next world entry. If your connection drops unexpectedly instead, your character keeps its last autosave.

## Enemies

| Category | Behaviour |
|----------|-----------|
| Hostile | Attacks players. Drops loot, gold and boss offerings. Includes Bandits, Beastmen and kingdom raiders. |
| Wildlife | Never attacks, flees when damaged. Gives no weapon or magic XP. Can be skinned. |
| Boss | One per region, summoned at an arena altar. |

- Each enemy has a category, a faction (none, Bandits, Beastmen or a kingdom), a skeleton type, a level, health, poise, resistances, damage, attacks, perception radius, loot table, optional hunting yield, region and XP rewards.
- Region monsters (Hostile and Wildlife rows with faction none) spawn from their region's list, filtered by time of day. Bosses never spawn this way: they appear only when summoned at their arena altar. Spawn density and the maximum live enemies per streamed chunk are set per region.
- Bandits and Beastmen spawn only at their camps and in raids. Kingdom raiders spawn only in raids. See [Factions and Kingdoms](Factions-and-Kingdoms.md) and [Raids](Raids.md).
- Enemies never spawn within 25 m of a placed building piece, within 40 m of a player (starting values, tunable), or inside a town's protected radius. Raiders ignore the building-piece rule but keep the 40 m rule. Camp spawn points ignore both rules, so camps always refill.

### Town NPCs

- Guards, Guard Captains, Vendors, Quest Givers and Citizens use the same combat system as enemies, but they aren't enemies: they give no XP, loot or gold and can't be executed.
- Your attacks always hurt them, whatever the friendly-fire setting, and cost reputation.
- Lock-on can target a Guard that is attacking you.
- Guard attacks use the Guard's own attack values, and town NPCs (including escort NPCs) don't get player-count scaling.
- Hostile enemies attack town NPCs and escort NPCs just like players.

### Raiders

- Raiders go for the pieces in the raid's snapshot of the base first (including stations, containers and beds), moving to the next nearest piece when one is destroyed. See [Raids](Raids.md).
- A raider you damage turns on you, and goes back to the base once you're Downed, dead or out of its perception radius. Taunts work as usual.
- Only raiders attack building pieces. Normal enemies never do.

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
- Town NPCs, including escort NPCs, never scale with player count.

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
- **Gold:** each Hostile enemy (and each chest type) has a gold range. Each eligible player rolls their own gold, which comes in their own drop. Wildlife and bosses drop no gold. Gold dropped with Drop Gold becomes a shared pickup. See [Factions and Kingdoms](Factions-and-Kingdoms.md).
- If you execute an enemy, you are always eligible for its kill.

### Loot chests

- Placed by world generation in caves, ruins and Bandit and Beastmen camps. They are not crafted storage containers.
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
- **Jewelry** (rings and amulets) dropped by enemies or found in chests never rolls Common, so it always has at least 1 affix. Jewelry has no base stats, only affixes. See [Inventory](Inventory.md).
- **Item level:** the enemy's level, or for a chest a random level within its region's band (rolled per player), or the crafted item level.
- Affix values scale with item level. No affix appears twice on one item.
- Affix examples: +STR/DEX/CON/INT/WIS/CHA, +% damage of a type, +resistance, +max health, +stamina regen, +Insulation/Cooling, +skill XP gain for a skill, +Magic Find, life on hit.

### Durability and requirements

- Equipment loses durability with use. At 0 it is unusable, not destroyed, and keeps its hotkey. Jewelry has no durability.
- Repair at the item's crafting station or with a Repair Kit. See [Crafting and Jobs](Crafting-and-Jobs.md).
- Gear may have stat requirements (for example STR 16). You can still equip it, at 50% damage or 50% Armor and resistance.

## Source spec

- [Combat and Loot](../../specs/combat-loot/combat-loot.md)
- [Character Creation](../../specs/character-creation/character-creation.md) (racial combat traits)
- [Character Progression](../../specs/character-progression/character-progression.md) (weapon categories and their skills)
- [Inventory](../../specs/inventory/inventory.md) (jewelry)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (factions, town NPCs, gold, raids)
