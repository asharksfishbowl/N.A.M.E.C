# Enemies and AI

This page covers how enemies and town NPCs behave, who fights whom, enemies picking up and wearing loot, enemy levels, and Veterans.

← [Home](Home.md)

Enemy stats, spawning, bosses and your loot are on [Combat and Loot](Combat-and-Loot.md). Camps, towns and raids are on [Factions and Kingdoms](Factions-and-Kingdoms.md) and [Raids](Raids.md).

## How enemies think

All AI runs on the host. Other machines only see the result (movement, animations, level, name, gear). Every enemy and town NPC is always in one of these states:

| State | What it does |
|-------|--------------|
| Idle/Patrol | Wanders inside a 15 m home radius around its spawn point or camp. Town NPCs stand at their posts. |
| Investigate | Something was noticed but not confirmed (you're in range but out of sight, or in sight just outside its range, up to 1.5×). It walks over to look, and gives up after 10 seconds. |
| Combat | Fights whatever it has detected, highest threat first. |
| Loot | Walks to an item on the ground and picks it up (see below). |
| Flee | Runs away. Only some enemies flee at low health, set per enemy. Wildlife always flees. |
| Return | Walks home after going more than 40 m from home. It ignores everything on the way. A normal enemy heals to full when it gets home. A Veteran doesn't. |
| Raid | Raiders follow the raid rules. A raider with no path to its target attacks the first building piece in its way, even one outside the raid's snapshot. See [Raids](Raids.md). |

(15 m, 1.5×, 10 s and 40 m are starting values, tunable)

- An enemy that is only investigating hasn't detected you, so Stealth still trains. See [Skills](Skills.md).
- Enemies far from every player (over 50 m, starting value) check their surroundings less often, and nothing is simulated in unloaded parts of the world.

## Who fights whom

Enemies fight each other, using the same sight ranges and threat rules they use on you.

| Group | Hostile to |
|-------|-----------|
| Region monsters | Players, Bandits, Beastmen, Guards, town NPCs |
| Bandits | Players, Beastmen, region monsters, kingdom raiders, Guards, town NPCs |
| Beastmen | Players, Bandits, region monsters, kingdom raiders, Guards, town NPCs |
| Kingdom raiders | Players, Guards, Bandits, Beastmen |
| Guards | Region monsters, Bandits, Beastmen, kingdom raiders, and players per reputation |
| Bosses | Players only |
| Wildlife | Nobody. Wildlife flees from everyone, and only players attack it. |
| Other town NPCs | Nobody |

- An enemy's attacks never hurt members of its own group, including area attacks.
- Guards still only fight enemies inside their town (raiders excepted) and never chase past the town's protected radius.
- Kill rules don't change with the killer: if a monster kills a Bandit near you, you still get your loot and kill XP if you were within 50 m or hit it.
- Player-count scaling is the same whoever the enemy is fighting.

## Enemies pick up loot

**What they can take:** items and gold on the ground that anyone can take:

- items and gold players dropped
- contents spilled from a destroyed or deconstructed container
- items dropped by dead enemies and Veterans
- your own loot, once you've left it for **2 real-time minutes** (starting value, tunable)

**Your loot after 2 minutes:** unclaimed per-player drops turn into shared pickups that every player can see and take, and enemies can grab. They still disappear 10 minutes after the original drop. Boss materials and trophies count too.

**What they can't take:** loose sticks and stones, forage, wood from fallen trees, mined ore and stone, and materials from destroyed building pieces.

**How it works:**

- Only Hostile enemies loot (including Bandits, Beastmen, raiders and Veterans). Wildlife, bosses and town NPCs never do.
- An enemy loots only when it isn't fighting. It spots an item within 12 m that it can see and reach, walks over and plays a short pickup animation (1 s).
- Hitting it, being seen by it, staggering it, or starting an execution on it interrupts the pickup, and the item stays on the ground.
- If you grab the item first, you get it. The server takes whichever request reaches it first.
- Two enemies never go for the same item.
- Raiders loot too when nobody is attacking them, so a destroyed chest's contents can walk away with them.
- Enemies carry any amount, with no weight limit.

(12 m and 1 s are starting values, tunable)

## Enemies wear what they pick up

Only **humanoid** enemies equip items. Every other enemy just carries them.

- **Slots:** Right Hand, Left Hand, Head, Chest, Hands, Legs, Feet.
- They never equip rings, amulets, cloaks, bags, arrows, bows, tools or torches, or anything at 0 durability. Those are carried.
- A humanoid enemy equips a picked-up item only if its **Item Score** beats what's already in that slot (an empty slot always loses). Item Score adds up the item's damage, armor, resistances and affixes. A two-handed weapon has to beat the main-hand and off-hand items combined.
- Worn armor gives the enemy its Armor, resistances and Poise. A wielded weapon adds its damage to the enemy's weapon attacks, changes their damage type, and adds the weapon's own status buildup (Poison, Bleed, Burn or Frostbite) if it has one. A poison or oil coating on the weapon does nothing for the enemy and isn't used up.
- Other affixes do nothing for the enemy. Stat requirements and class penalties don't apply to enemies.
- Gear is visible on the enemy, dyes included. Humanoid enemies use the same bodies as the playable races, so every armor piece fits them.
- Enemies never wear gear down: durability doesn't change while they use it.

## Getting it back

- **When an enemy dies,** everything it picked up (worn and carried) and all its picked-up gold drops on the ground as shared pickups, on top of your normal loot. Affixes, dyes and durability are unchanged.
- **When a normal enemy vanishes without dying** (it can't get home, a raider retreats, or its area unloads), it drops its picked-up items where it vanishes.
- **When the world saves,** items held by normal enemies are saved as pickups on the ground where those enemies stood.
- A Veteran keeps its items until it dies.

## Enemy levels

Hostile enemies (including Bandits, Beastmen and raiders) gain XP. Bosses, Wildlife and town NPCs never do.

| XP source | XP |
|-----------|----|
| Downing a player | 50 |
| Killing a player | 100 |
| Killing a Guard or Citizen | 40 |
| Killing an enemy it's hostile to | Half of that enemy's kill XP |
| Staying alive | 5 per real-time minute while its area is loaded |

(starting values, tunable)

- Level = its normal level + gained levels. Gained levels cost 100 XP more each (100, 300, 600… total) and stop at +10.
- Each gained level gives ×1.10 health, ×1.05 damage and ×1.05 poise. On a level-up it glows, its name plate updates, and its current health rises by the added max health.
- Its loot uses its current level for item level. Your kill XP rises by 10% per gained level.
- The name plate shows the enemy's level while you're locked on to it or within 15 m.
- Newly spawned enemies always start at their normal level.

(multipliers, the cap and 15 m are starting values, tunable)

## Veterans

**Promotion:** an enemy that reaches **3 gained levels** becomes a **Veteran**:

- It gets a name (from its faction's or creature type's name list) and a title from where most of its XP came from, for example "the Kinslayer" for player kills or "the Survivor" for staying alive.
- Its name plate shows its name, title and level.
- Raiders never become Veterans.

**Persistent:**

- Veterans are saved with the world: level, XP, health, gear, carried items, gold, home, name, title and kill counts.
- When you leave its area it stops, and it's back at home the next time the area loads.
- It roams 3× further from home than normal enemies.
- Its health doesn't reset when it walks home, and it doesn't regenerate. Only level-ups raise it.
- If its home ends up inside a base or a town, the home moves to the nearest valid spot outside.
- A Veteran from a camp still belongs to its camp: the camp isn't cleared until the Veteran dies.

**Limits:** at most 5 Veterans per region and 20 per world. At a limit, enemies keep levelling but aren't promoted.

**Raids:** when Bandits or Beastmen raid a base and one of that faction's Veterans lives within 300 m, the Veteran leads the raid, and goes home afterwards.

**Killing one:**

- Veterans can be executed like any Hostile enemy.
- A killed Veteran is gone from the world for good.
- Every player eligible for the kill gets bonus kill XP and one extra guaranteed item of Rare rarity or better.

(3 levels, 3×, 5, 20, 300 m and the reward are starting values, tunable)

## Source spec

- [Enemy AI](../../specs/enemy-ai/enemy-ai.md)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (enemies, perception, threat, loot)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (camps, Guards, raids)
