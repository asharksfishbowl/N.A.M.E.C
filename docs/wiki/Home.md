# N.A.M.E.C Wiki

This page is the entry point to the N.A.M.E.C wiki: what the game is and where to find each topic.

← [Home](Home.md)

## What N.A.M.E.C is

N.A.M.E.C is a first/third-person 3D fantasy survival RPG for up to 4 players in local split-screen and LAN co-op, built in Unreal Engine 5. Players explore a seeded, smooth-voxel world of distinct climate regions, dig and build freely, level skills by using them, craft through independent Jobs, and fight Souls-like enemies and bosses that gate progression. There are no NPCs, towns, vendors or currency.

The wiki is a readable summary. The specs in `specs/` are authoritative (see [Specs and Pipeline](Dev-Specs-and-Pipeline.md)).

## Game Guide

- [Game Overview](Game-Overview.md): pitch, core loop, pillars, camera, player count
- [Stats and Classes](Stats-and-Classes.md): six stats, character level, 11 classes, armor categories, ability bar
- [Skills](Skills.md): use-based skills, XP sources, perks, anti-farming rules
- [Crafting and Jobs](Crafting-and-Jobs.md): 9 Jobs, stations, hand-crafting, tiers, quality, runes, repair
- [Survival](Survival.md): hunger, thirst, temperature, stamina, fatigue, sleep, breath
- [World and Building](World-and-Building.md): world generation, 8 regions, terrain editing, gathering, hazards, building
- [Combat and Loot](Combat-and-Loot.md): Souls-style combat, downed and death, enemies, bosses, loot
- [Inventory](Inventory.md): weight, equip load, slots, favorites, hotkeys, containers
- [Co-op and Controls](Co-op-and-Controls.md): split-screen, LAN, portable characters, default controls

## Developer Docs

- [Architecture](Dev-Architecture.md): engine, GAS, module layout, platform abstraction, performance, key files
- [Data Tables](Dev-Data-Tables.md): every `DT_*` tuning table by system
- [Save System](Dev-Save-System.md): character and world saves, versioning, atomic writes, autosave
- [Networking](Dev-Networking.md): listen server, session flow, payload sync, loot relevance, disconnects
- [Specs and Pipeline](Dev-Specs-and-Pipeline.md): spec index and the agent pipeline

## Open Decisions

- [Open Decisions](Open-Decisions.md): items not yet resolved in the specs
