# N.A.M.E.C Wiki

This page is the entry point to the N.A.M.E.C wiki: what the game is and where to find each topic.

← [Home](Home.md)

## What N.A.M.E.C is

N.A.M.E.C is a first/third-person 3D fantasy survival RPG for up to 4 players in local split-screen and LAN co-op, built in Unreal Engine 5.8. Players explore a seeded, smooth-voxel world of distinct climate regions, dig and build freely, level skills by using them, craft through independent Jobs, trade and take quests in the towns of six race kingdoms, defend their bases against raids, and fight Souls-like enemies and bosses that gate progression.

The wiki is a readable summary. The specs in `specs/` are authoritative (see [Specs and Pipeline](Dev-Specs-and-Pipeline.md)).

## Game Guide

- [Game Overview](Game-Overview.md): pitch, core loop, pillars, camera, player count
- [Races and Character Creation](Races-and-Character-Creation.md): creation steps, six races and their traits, male/female, appearance, names, the Mirror
- [Stats and Classes](Stats-and-Classes.md): six stats, character level, 11 classes, armor categories, ability bar
- [Skills](Skills.md): use-based skills, XP sources, perks, anti-farming rules
- [Crafting and Jobs](Crafting-and-Jobs.md): 9 Jobs, stations, hand-crafting, tiers, quality, runes, repair, dyes
- [Survival](Survival.md): hunger, thirst, temperature, stamina, fatigue, sleep, breath
- [World and Building](World-and-Building.md): world generation, 8 regions, terrain editing and dig depth, towns and camps, gathering, hazards, building
- [Combat and Loot](Combat-and-Loot.md): Souls-style combat, executions, downed and death, enemies, bosses, loot
- [Enemies and AI](Enemies-and-AI.md): enemy behaviour, who fights whom, enemies looting and wearing gear, enemy levels, Veterans
- [Inventory](Inventory.md): weight, equip load, slots, gold, dyes, favorites, hotkeys, containers
- [Factions and Kingdoms](Factions-and-Kingdoms.md): factions, reputation, kingdoms and towns, NPCs, camps, gold and vendors, quests
- [Raids](Raids.md): what counts as a base, raid chance, raiders, how raids end, the Raids setting
- [Co-op and Controls](Co-op-and-Controls.md): split-screen, LAN, portable characters, default controls

## Developer Docs

- [Architecture](Dev-Architecture.md): engine, GAS, module layout, platform abstraction, performance, key files
- [Engine and Rendering](Dev-Engine-and-Rendering.md): engine pin, adopted, avoided and evaluated UE 5.8 features, the Nanite plan, terrain rendering, scalability tiers, the benchmark milestone
- [Data Tables](Dev-Data-Tables.md): every `DT_*` tuning table by system
- [Save System](Dev-Save-System.md): character, world and settings saves, versioning, atomic writes, autosave
- [Networking](Dev-Networking.md): listen server, session flow, payload sync, loot relevance, disconnects
- [Specs and Pipeline](Dev-Specs-and-Pipeline.md): spec index and the agent pipeline

## Open Decisions

- [Open Decisions](Open-Decisions.md): items not yet resolved in the specs
