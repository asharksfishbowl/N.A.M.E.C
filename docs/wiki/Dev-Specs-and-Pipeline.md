# Specs and Pipeline

This page indexes the system specs, explains how the wiki relates to them, and summarizes the agent pipeline.

← [Home](Home.md)

## Spec index

| Spec | Covers | Wiki pages |
|------|--------|------------|
| [game-foundation](../../specs/game-foundation/game-foundation.md) | Project setup, glossary, save layout, data-driven tuning, platform rules, default input bindings | [Game Overview](Game-Overview.md), [Co-op and Controls](Co-op-and-Controls.md), [Architecture](Dev-Architecture.md), [Save System](Dev-Save-System.md) |
| [character-creation](../../specs/character-creation/character-creation.md) | Character creation, six races with traits, sex, appearance customization, Mirror | [Races and Character Creation](Races-and-Character-Creation.md), [Save System](Dev-Save-System.md), [Networking](Dev-Networking.md) |
| [character-progression](../../specs/character-progression/character-progression.md) | D&D stats, character level, classes, use-based skills | [Stats and Classes](Stats-and-Classes.md), [Skills](Skills.md), [Factions and Kingdoms](Factions-and-Kingdoms.md) |
| [crafting-jobs](../../specs/crafting-jobs/crafting-jobs.md) | Jobs, crafting stations, recipes, crafted item quality, dyes | [Crafting and Jobs](Crafting-and-Jobs.md) |
| [survival](../../specs/survival/survival.md) | Hunger, thirst, temperature, stamina, fatigue | [Survival](Survival.md) |
| [voxel-world](../../specs/voxel-world/voxel-world.md) | World layout, climates, terrain dig/fill and dig depth, building, gathering | [World and Building](World-and-Building.md), [Raids](Raids.md) |
| [authored-map](../../specs/authored-map/authored-map.md) | The one authored map: map asset, bake tool and stroke log, map validator, authored towns, camps and arenas, save map identity and patch rule, expansion placeholders, repository and handover shape | [World and Building](World-and-Building.md), [Save System](Dev-Save-System.md), [Open Decisions](Open-Decisions.md) |
| [combat-loot](../../specs/combat-loot/combat-loot.md) | Souls-like combat, executions, downed/revive, enemies, bosses, randomized loot | [Combat and Loot](Combat-and-Loot.md), [Raids](Raids.md), [Enemies and AI](Enemies-and-AI.md) |
| [inventory](../../specs/inventory/inventory.md) | Weight inventory, equipment slots, favorites, containers | [Inventory](Inventory.md) |
| [multiplayer](../../specs/multiplayer/multiplayer.md) | Split-screen, LAN, authority, portable characters | [Co-op and Controls](Co-op-and-Controls.md), [Networking](Dev-Networking.md) |
| [factions-kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) | Factions, reputation, kingdoms and towns, NPCs, gold and vendors, quests, raids on player bases | [Factions and Kingdoms](Factions-and-Kingdoms.md), [Raids](Raids.md), [Inventory](Inventory.md), [Save System](Dev-Save-System.md), [Networking](Dev-Networking.md) |
| [engine-tech](../../specs/engine-tech/engine-tech.md) | Adopted, avoided and evaluated UE 5.8 features, Nanite plan, terrain rendering path, scalability tiers, benchmark milestone, console readiness | [Engine and Rendering](Dev-Engine-and-Rendering.md), [Architecture](Dev-Architecture.md), [Data Tables](Dev-Data-Tables.md) |
| [enemy-ai](../../specs/enemy-ai/enemy-ai.md) | Enemy and town NPC AI states and AI LOD, faction hostility, enemy loot pickup and equipment, enemy XP and levels, Veterans | [Enemies and AI](Enemies-and-AI.md), [Combat and Loot](Combat-and-Loot.md), [Raids](Raids.md), [Factions and Kingdoms](Factions-and-Kingdoms.md), [Architecture](Dev-Architecture.md), [Data Tables](Dev-Data-Tables.md), [Save System](Dev-Save-System.md), [Networking](Dev-Networking.md), [Engine and Rendering](Dev-Engine-and-Rendering.md) |

## Specs are authoritative

The specs are the source of truth. This wiki is a readable summary of them and adds nothing of its own. When a spec changes, update the matching wiki pages (see the table above) in the same change. If the wiki and a spec disagree, the spec wins. Unresolved items are tracked in [Open Decisions](Open-Decisions.md).

## Pipeline

Work flows **Director → Researcher → Builder**. The Director talks to the user, scopes problems and writes issues to the issue queue. The Researcher investigates each issue, makes the implementation decisions the specs assign to it (such as the terrain mesh backend, the enemy Mutable rebuild strategy, and running the engine benchmark milestone), and writes a roadmap and findings to the build queue. The Builder implements code changes from those findings.
