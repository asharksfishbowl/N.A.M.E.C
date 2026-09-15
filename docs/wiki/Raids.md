# Raids

This page covers what counts as a base, base value, when raids roll, who raids, raid size and difficulty, how raiders spawn and what they attack, how raids end, and the Raids world setting.

← [Home](Home.md)

Raids are random attacks on player bases. There is **no warning**: no message, sound or marker. Raids only happen while at least one player is near the base. Factions and reputation are covered on [Factions and Kingdoms](Factions-and-Kingdoms.md).

## The Raids setting

- "Raids" is a world setting, on by default. Turning it off stops all raids. See [World and Building](World-and-Building.md).
- No raids happen during the world's first 3 in-game days (starting value, tunable). Sleeping does count toward those 3 days.

## What counts as a base

- Every crafting station and every bed is an **anchor**.
- Each anchor collects every building piece within 30 m of it.
- Anchors that share any piece belong to the same base.
- A group counts as a base only with **at least 10 pieces**.

(30 m and 10 pieces are starting values, tunable)

The Dye Station and the Mirror are not anchors. Town buildings never count.

## Base value

```
Base value = number of pieces + 5 × total tier of every crafting station in the base
```

(coefficients are starting values, tunable)

## When a raid rolls

Every **5 real-time minutes** (starting value, tunable), for each base with at least one player within 100 m (starting value, tunable) at that moment and no raid already running, the server rolls:

```
Raid chance = base chance + coefficient × base value + hostility factor of every faction that can raid
```

(capped between 0 and 100%)

**Factions that can raid:**

- Bandits and Beastmen, always.
- Any kingdom that at least one player in the session is Hostile with. So one player's bad reputation can bring raids on the whole group.

**No roll happens:**

- with the Raids setting off
- while any boss arena is sealed with a player inside
- during the world's first 3 in-game days
- when sleeping skips time (the skip doesn't trigger a roll)

**Real time, not in-game time.** The raid roll clock and the raid duration run on real time, unlike the other world timers. The clock starts when the world loads, stops while the world is paused, isn't saved, and isn't moved forward by sleeping. A skipped roll doesn't reset it: the next roll still comes at the next 5-minute mark.

## Who raids, how many, how tough

- The raiders' source is picked at random among the factions that can raid, weighted by hostility, plus the base region's own monsters.
- Raiders come from that source's enemies for the base's region. A source with no enemies for that region is skipped, and if no source has any, the raid doesn't start.
- **Size** (number of raiders) and **difficulty** (a level bonus on each raider) are rolled randomly within bands that grow with base value and with the region's order.
- The level bonus raises raider health and damage, and the item level of their loot.
- Normal player-count scaling applies on top.
- Raiders can also gain levels during the raid, but never become Veterans.

## Veteran raid leaders

- When the source is Bandits or Beastmen and one of that faction's Veterans has its home within 300 m (starting value, tunable) of the base, the nearest one joins as **raid leader**, on top of the rolled raid size.
- It keeps its own level, health and gear, and counts as a raider for how the raid ends.
- After the raid it retreats with the others and later reappears at its home. If it dies, it's gone for good. See [Enemies and AI](Enemies-and-AI.md).

## Spawning

- The server samples spots every 5 m (starting value, tunable) on walkable terrain between the edge of the base and 100 m from the base center.
- A spot is valid when it's outside the base, at least 40 m from every player, out of every player's line of sight, and outside every town's protected radius. Raiders spawn at the valid spots closest to the base.
- If there is no valid spot, the raid doesn't start.

## What raiders do

- When raiders spawn, the server takes a **snapshot** of the base's building pieces and its center. New pieces join the snapshot only if the base merges with another group during the raid.
- Raiders go for the pieces in the snapshot first, including crafting stations, storage containers, beds and doors.
- Raiders can't open doors. A closed door blocks their path, so they have to destroy it to get through.
- A raider you hit turns on you, and returns to the base once you're Downed, dead or out of its sight range. Enemies and Guards that hit a raider get the same treatment.
- When nobody is attacking it, a raider can pick up items on the ground, including a destroyed chest's spilled contents.
- Raider attacks damage building pieces. Normal enemies never do.
- Destroyed containers drop their contents as shared pickups. A destroyed bed clears its respawn point. A destroyed station cancels its crafting queues.
- Killing raiders gives normal loot, gold, kill XP and Hunt quest credit.

## How a raid ends

| Ending | What happens |
|--------|--------------|
| Every raider is dead | The raid ends |
| 10 real-time minutes pass (starting value, tunable) | Survivors retreat and despawn, dropping any items they picked up where they vanish |
| Every piece in the snapshot is destroyed | Survivors retreat and despawn |
| No player within 100 m of the snapshot's base center | Survivors despawn at once |

- Destroying the base's beds and crafting stations doesn't end the raid early. Raiders keep going for the rest of the snapshot.
- A base has at most one raid at a time.
- Nobody can sleep through a raid: "Cannot sleep — a raid is in progress".
- A Veteran raid leader that survives keeps its items and goes home.
- Raids are not saved. After loading a world no raid is running, but anything destroyed stays destroyed.

## Source specs

- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (Requirements 50–60)
- [Voxel World](../../specs/voxel-world/voxel-world.md) (Requirements 11, 26 and 45)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (Requirement 43)
- [Enemy AI](../../specs/enemy-ai/enemy-ai.md) (Requirements 10, 23, 33 and 52)
