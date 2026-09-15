# Game Overview

This page covers the pitch, core loop, design pillars, camera and player count of N.A.M.E.C.

← [Home](Home.md)

## Pitch

N.A.M.E.C is a first/third-person 3D fantasy survival RPG for up to 4 players in local split-screen and LAN co-op. Players explore a seeded, smooth-voxel world of distinct climate regions, dig and build freely, level skills by using them, craft through independent Jobs, trade and take quests in the towns of six race kingdoms, defend their bases against raids, and fight Souls-like enemies and bosses that gate progression.

## Creating a character

Each player creates a character by choosing a race (Human, Felari, Hundari, Sauren, Vanari or Ursan), a sex (cosmetic only), an appearance, a starting class and a name. See [Races and Character Creation](Races-and-Character-Creation.md).

## Core loop

1. **Gather**: collect loose sticks and stones, forage plants, chop trees, mine stone and ore. See [World and Building](World-and-Building.md).
2. **Craft**: turn materials into tools, stations, gear, meals and potions through 9 Jobs. See [Crafting and Jobs](Crafting-and-Jobs.md).
3. **Survive**: manage hunger, thirst, temperature, stamina and fatigue in each climate. See [Survival](Survival.md).
4. **Fight bosses**: summon each region's boss with an offering and defeat it for that region's boss material. See [Combat and Loot](Combat-and-Loot.md).
5. **Next region**: the boss material unlocks the next station tier and gear tier, which lets the group take on the next, harsher region.

Alongside the loop:

- **Towns and quests**: each race kingdom has a capital and 2 towns with vendors and quest boards. Quests earn gold, reputation and XP. See [Factions and Kingdoms](Factions-and-Kingdoms.md).
- **Defend your base**: Bandits, Beastmen, region monsters and kingdoms you've angered can raid a base with no warning while you're nearby. See [Raids](Raids.md).

The final (Volcanic) boss gates nothing. It drops a guaranteed Legendary item and a trophy and can be re-summoned as endgame content.

## Pillars

| Pillar | What it means in play |
|--------|-----------------------|
| Learn by doing | Every repeated action (swinging an axe, casting a fireball, sprinting) raises its skill and improves that action. Skill use also feeds character XP. |
| D&D-style character | Six playable races, each with stat bonuses, a passive, an active ability and a downside. Six stats (STR, DEX, CON, INT, WIS, CHA) with modifiers, 11 classes, up to four classes combined by level 40. |
| Jobs are separate from combat | Any character can level any of the 9 crafting Jobs. Job XP never feeds character XP. |
| Climate matters | Each of the 8 regions has its own survival pressure (heat, cold, poison, altitude, lava). |
| Diggable, buildable world | Smooth (never blocky) voxel terrain that can be dug, filled and reshaped, plus snap-together building. |
| Souls-like combat | Stamina-gated attacks, dodge rolls with invulnerability frames, blocking, parrying, poise, lock-on, executions and multi-phase bosses. |
| Reputation matters | Each character has standing with six kingdoms. It changes vendor prices, services, questline access, guard hostility and raids. |
| Co-op without competition | Loot is rolled per player, so co-op players never compete for drops. Downed players can be revived. |

## What the game does not have

- No story campaign. Quests come only from town Quest Boards and each kingdom's questline.
- No NPC daily schedules, trade screen between players, or raid warnings.
- No online play, dedicated servers, cross-platform play, mod support or PvP.
- No farming, animal husbandry, food spoilage, sailing, mounted combat, or fast travel.

## Camera

- First-person and third-person, toggled per local player. The default is third-person.
- Gamepad: hold View for 0.5 s (starting value, tunable). Keyboard: H.

## Player count

- Up to 4 players total, in any mix of local split-screen and LAN (for example 2 players on one PC plus 2 on another PC).
- Up to 4 local players per machine, limited by the remaining session capacity.
- The game is fully playable with a gamepad only. Keyboard and mouse is supported for local player 1 only.

See [Co-op and Controls](Co-op-and-Controls.md).

## Source spec

- [Game Foundation](../../specs/game-foundation/game-foundation.md)
- [Character Creation](../../specs/character-creation/character-creation.md) (races)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (towns, quests, raids)
