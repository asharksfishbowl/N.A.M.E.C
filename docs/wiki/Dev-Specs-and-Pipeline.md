# Specs and Pipeline

This page indexes the system specs, explains how the wiki relates to them, and summarizes the agent pipeline.

← [Home](Home.md)

## Spec index

| Spec | Covers | Wiki pages |
|------|--------|------------|
| [game-foundation](../../specs/game-foundation/game-foundation.md) | Project setup, glossary, save layout, data-driven tuning, platform rules, default input bindings | [Game Overview](Game-Overview.md), [Co-op and Controls](Co-op-and-Controls.md), [Architecture](Dev-Architecture.md), [Save System](Dev-Save-System.md) |
| [character-creation](../../specs/character-creation/character-creation.md) | Character creation, six races with traits, sex, appearance customization, Mirror | [Races and Character Creation](Races-and-Character-Creation.md), [Save System](Dev-Save-System.md), [Networking](Dev-Networking.md) |
| [character-progression](../../specs/character-progression/character-progression.md) | D&D stats, character level, classes, use-based skills | [Stats and Classes](Stats-and-Classes.md), [Skills](Skills.md) |
| [crafting-jobs](../../specs/crafting-jobs/crafting-jobs.md) | Jobs, crafting stations, recipes, crafted item quality | [Crafting and Jobs](Crafting-and-Jobs.md) |
| [survival](../../specs/survival/survival.md) | Hunger, thirst, temperature, stamina, fatigue | [Survival](Survival.md) |
| [voxel-world](../../specs/voxel-world/voxel-world.md) | World generation, climates, terrain dig/fill, building, gathering | [World and Building](World-and-Building.md) |
| [combat-loot](../../specs/combat-loot/combat-loot.md) | Souls-like combat, downed/revive, enemies, bosses, randomized loot | [Combat and Loot](Combat-and-Loot.md) |
| [inventory](../../specs/inventory/inventory.md) | Weight inventory, equipment slots, favorites, containers | [Inventory](Inventory.md) |
| [multiplayer](../../specs/multiplayer/multiplayer.md) | Split-screen, LAN, authority, portable characters | [Co-op and Controls](Co-op-and-Controls.md), [Networking](Dev-Networking.md) |

## Specs are authoritative

The specs are the source of truth. This wiki is a readable summary of them and adds nothing of its own. When a spec changes, update the matching wiki pages (see the table above) in the same change. If the wiki and a spec disagree, the spec wins. Unresolved items are tracked in [Open Decisions](Open-Decisions.md).

## Pipeline

Work flows **Director → Researcher → Builder**. The Director talks to the user, scopes problems and writes issues to the issue queue. The Researcher investigates each issue, makes the decisions the specs assign to it (such as pinning the engine version and choosing Voxel Plugin or a custom voxel implementation), and writes a roadmap and findings to the build queue. The Builder implements code changes from those findings.
