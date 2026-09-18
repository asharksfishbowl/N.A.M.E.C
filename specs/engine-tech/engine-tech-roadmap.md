# Roadmap: N.A.M.E.C Implementation (umbrella)

Created by the Researcher on 2026-09-15. This is the umbrella roadmap for all 12 system specs. `specs/engine-tech/engine-tech.md` sends several records here (benchmark results, Evaluate results, and Researcher implementation decisions), so this file holds them as well.

- Feasibility review, spec issues and evidence: `research/specs-feasibility-review.md` (being re-derived, issue 0916-1)
- System Spec Index: `specs/game-foundation/game-foundation.md`
- Engine evidence: `research/unreal-5.6-5.8-features.md` (being re-derived, issue 0916-2)
- Authored map, bake tool, save and load rules, repository and handover shape: `specs/authored-map/authored-map.md` (round table 2026-09-16)

Status legend: **Pending**, **In progress**, **Done**, **Blocked**.

---

## Ordering rules

1. **Phase 1 is the benchmark milestone** (`specs/engine-tech/engine-tech.md` Requirements 11–13). No content production phase starts until a passing run is recorded in the [Benchmark Results](#benchmark-results) section. Phase 1 contains only the code and placeholder assets the benchmark scene needs, plus the project skeleton it cannot run without.
2. Each later phase depends only on earlier phases. Shared types (save objects, GAS attribute sets, item definition and item instance, the progression XP hook) land before the systems that use them.
3. Each phase that adds save fields bumps that save type's `SaveVersion` and adds a migration that fills defaults (`specs/game-foundation/game-foundation.md` Requirement 7).
4. Requirement numbers below are the requirements a phase delivers **in full**. "(core)" means the phase delivers the part named, and a later phase finishes it.
5. Build tasks exist in `build-queue.groovy` for Phase 1 only. Later phases stay here until Phase 1 lands.

**Phase 1 branch shape:** parent + sub-branches. Parent `feature/benchmark-milestone/base`, cut from `main` (a parent named `feature/benchmark-milestone` would block every `feature/benchmark-milestone/<phase>` sub-branch in git). Every Phase 1 task cuts a sub-branch off the parent and merges back into it. The parent merges to `main` once, after the benchmark passes, and that merge is user-gated.

---

## Phase 1 — Benchmark milestone (tech slice)

**Status:** Pending. The build environment and placeholder asset decisions were made in the 2026-09-16 round table (see "Build environment split" and "Placeholder art and Git LFS" under Researcher Implementation Decisions). Build tasks are re-filed by issue 0916-4 (DRAFT until the user promotes it).

**Goal:** `L_Benchmark_SplitScreen` runs on the reference PC at 1–4 viewports and passes, or the Requirement 13 fallback steps are applied until it passes or the milestone fails back to the user.

**Verification is entirely off-pipeline.** The container compiles, runs and measures nothing. Every task is authored as C++ and text, ends in "awaiting verification" with a handover note (`specs/authored-map/authored-map.md` Requirement 34), and merges into the parent branch only after the user pastes the result of the note's proving command. Budget about fifteen user sittings for Phase 1. Tasks 7–11 ship as C++ plus a written editor authoring recipe, because their uassets can only be authored on the reference PC.

| Spec | Requirements delivered |
|------|------------------------|
| engine-tech | 1, 2, 6, 9, 11, 12, 13, 14; 3 (the adopted rows the scene exercises: Nanite static meshes, Nanite Geometry Collections, classic Nanite trees, felled trees, Lumen HWRT, Lumen Lite, VSM, Substrate, PCG, Mutable, IK Retargeter, Motion Matching, StateTree + Smart Objects, PlayerCameraManager + SpringArm, First Person Rendering); 7 (core: tier selection and tier row application; user settings ceiling in Phase 2); Data Flows 1 (core), 2, 3, 4, 5; Edge Cases 1, 2, 3, 6 |
| game-foundation | 1, 2, 13 (measured) |
| voxel-world | 1, 2, 3, 4; 15 (core: server-authoritative replicated dig and fill, no save); 17–18 (core: tree health and felling swap); 26 (core: Nanite piece with replicated health and destroyed state); Data Flows 2, 4 |
| multiplayer | 5, 9, 10 |
| enemy-ai | 2, 11; 1 and 3 (core: StateTree host, Idle/Patrol and Loot only); 21, 23, 26, 27, 31 (core: Smart Object pickup, empty-slot equip, Mutable gear) |
| character-creation | 14 (core: shared animation set retargeted to 2 placeholder bodies); 18 (authoring decision recorded, validated in Phase 11) |
| authored-map | 1, 2 (benchmark asset only), 3, 4, 5 (Noise source), 6, 7, 8, 8a, 9, 10, 11, 13, 21 (core: seed and exclusion bridge), 29, 32, 33, 34, 35; Data Flows 2 and 8; Edge Cases 1–4 |

**Tasks (in `build-queue.groovy`, re-filed by 0916-4):**

| # | Task | Depends on |
|---|------|-----------|
| 1 | Project skeleton, engine configuration, repository guards, handover template, header-only stubs for every Key Files class | — |
| 2 | Split-screen scalability tier subsystem | 1 |
| 3 | Voxel chunk data, map asset, write kernel, stroke-log bake commandlet (Noise source, cave and ore passes) and chunk streaming from the asset | 1 |
| 4 | Voxel mesher and chunk render component ⏸ lighting checkpoint in two sittings: (a) one chunk exported to a `UStaticMesh` and lit in a side-by-side test level, (b) the custom proxy beside it, with a three-question decision tree and a console-variable kill switch per stage | 3 |
| 5 | Terrain LOD with transition cells | 4, 2 |
| 6 | Terrain collision, dig/fill edit RPC and re-mesh | 4 |
| 7 | Nanite building pieces with fracture and damage-state fallback | 1 |
| 8 | Runtime PCG trees and rocks seeded from the map asset's `AuthoringSeed` per chunk, with authored exclusion volumes, and tree felling | 4, 2 |
| 9 | Benchmark characters: Mutable body, Motion Matching, cameras, First Person Rendering | 1 |
| 10 | Benchmark AI enemies: StateTree, AI LOD, dynamic navmesh | 6 |
| 11 | Humanoid enemy pickup and Mutable equip with rebuild budget | 9, 10 |
| 12 | Benchmark director, measurements, lighting check, content manifest and results ⏸ benchmark run checkpoint | 2, 5, 6, 7, 8, 11 |

**Exit:** a passing run is recorded below with its manifest hash, `DT_MP_SplitScreenScalability` is tuned (Requirement 12), and the Evaluate row decisions for Nanite Geometry Collections and First Person Rendering are recorded.

**Untested assumptions carried into Phase 1** (user-sitting results, not room decisions): that the black-terrain Lumen failure, if it occurs, is a missing surface cache on the custom proxy; whether Lumen Lite lights a custom proxy at all (`specs/engine-tech/engine-tech.md` Requirement 9); and the stroke-log replay cost at 8×8 km (the 1×1 km benchmark bake gives the first number).

---

## Phase 2 — Project foundation

**Goal:** saves, settings, input, UI shell and content validators that every later phase writes into.

| Spec | Requirements delivered |
|------|------------------------|
| game-foundation | 4, 5, 6 (the three save types, each with only the fields that exist so far; later phases add theirs), 7, 8, 9, 10, 11, 12, 14 (the base mapping contexts and the context framework; each context's actions are finished in the phase that owns them); Edge Cases 1–4 |
| engine-tech | 4 (plugin exclusion check), 7 (user settings ceiling from `UNamecSettingsSave`), 8 (`NamecNaniteAuthoringValidator`), 10; Edge Cases 4, 5, 9 |
| engine-tech | Evaluate: **CommonUI With Enhanced Input** (Requirement 5) — whether the settings and lobby UI built in this phase triggers the 5.8 Experimental dispatcher path (`research/unreal-5.6-5.8-features.md` §5.7); row added 2026-09-18 from the 09-18 research review |
| multiplayer | 2, 3, 6, 7, 23 |
| authored-map | 24 (`UNamecWorldSave` fields), 25 (load rule, `UNamecWorldLoader`) — row added 2026-09-18 to close Conflict C1 of `research/authored-map-review.md`; prose below unchanged |

**Closed 2026-09-16:** default machine-wide graphics settings equal the High tier row of `DT_MP_SplitScreenScalability`. Requirement 7 already treats user settings as a ceiling and the benchmark tunes that row, so the default is the measured reference configuration. No hardware auto-detect.

Phase 2 also lands the `UNamecWorldSave` fields of `specs/authored-map/authored-map.md` Requirement 24 and the load rule of Requirement 25, with the `UNamecWorldLoader`.

---

## Phase 3 — Characters, GAS, item data and sessions

**Goal:** a character is created, picked in the lobby, joins split-screen or LAN, plays, and saves on its own machine. **⏸ Test checkpoint at the end of the phase:** this is the first end-to-end portable-character loop, and every later save field builds on it.

| Spec | Requirements delivered |
|------|------------------------|
| game-foundation | 3; Data Flow 1–6 |
| multiplayer | 1, 4, 8, 11–22; Data Flows 1–6; Edge Cases 1–6 |
| character-progression | 1–6, 12, 13, 27, 29 (enums and class rows) |
| character-creation | 1–10, 12 (rows and stat bonuses; racial effects in Phase 11), 16, 17, 26, 28; Edge Cases 1–4, 10–13 |
| inventory | 1–6, 12, 20, 21, 23 (item definition, item instance, equip slots, weight) |
| engine-tech | Evaluate: **Iris** (Requirement 5) runs after this phase's multiplayer acceptance criteria pass |

Risk: Online Subsystem Null LAN sessions are unverified for 5.8 in the research file. Verify them at the start of the phase.

The checkpoint needs a second machine or a VM beside the reference PC for the LAN half. It also loads one save with real terrain edits once, so the per-chunk delta application on stream (`specs/authored-map/authored-map.md` Requirements 25 and 29) is timed on a save the benchmark never exercises. The LAN join refusal on map mismatch (`specs/authored-map/authored-map.md` Edge Cases 10–11) lands here with multiplayer Requirements 11–22.

---

## Phase 4 — Progression XP and world

| Spec | Requirements delivered |
|------|------------------------|
| character-progression | 7–11, 14–26, 28, 30; Data Flows 1–7; Edge Cases 1–4, 6, 7 |
| voxel-world | 5–16, 17–23, 30–36; Data Flows 1, 3, 5, 7 (world fields so far); Edge Cases 2–4, 6–12 |
| authored-map | 2 (`VMA_Namec`), 5 (Tiles source), 12 (sculpt brush), 14–20 (validator and authored structure classes, with placeholder town volumes on a greybox map), 22, 23, 26–28, 30–31; Data Flows 1, 3–7; Edge Cases 5–9, 12–13 |
| survival | 9 (ambient temperature inputs from climate) (core) |
| engine-tech | Evaluate: **World Partition** (Requirement 5) at this phase's bake checkpoint, against the authored `ANamecTown` positions with placeholder town volumes on the greybox map (moved from Phase 10 on 2026-09-16) |

**Bake design for Phase 4 (research 0918-4, 2026-09-18):** the bake commandlet is single-threaded and linear in chunk count (1 km² ≈ 25 min at 25 cm). Phase 4 files three build tasks in this order before the shipped map is authored: `ParallelFor` over the chunk loop with per-chunk results merged serially so `MapHash` order is unchanged; per-pass timing in the commandlet log; incremental bake of stroke-affected chunks (`--IncrementalFrom=<sha>`), valid only while `BakeSettings` are unchanged.

**⏸ Test checkpoint at the end of the phase:** the first real bake of `VMA_Namec` from the committed heightfield tiles on the reference PC, the validator run on `L_Namec.umap`, and the World Partition evaluation. This is the first phase with editor tooling the pipeline can write but never open (sculpt brush, validator), and the user's heightfield authoring is art time.

---

## Phase 5 — Building, containers and inventory UI

| Spec | Requirements delivered |
|------|------------------------|
| voxel-world | 24–29, 37–45; Data Flow 6; Edge Cases 1, 5 |
| inventory | 7–11, 13–19, 22 (footer display; gold counter lands in Phase 10), 25, 26; Data Flows 1–4; Edge Cases 1–5, 7, 8, 11–13 |

---

## Phase 6 — Combat, loot and enemy behaviour core

| Spec | Requirements delivered |
|------|------------------------|
| combat-loot | 1–41, 44–61 (42–43 in Phase 10); Data Flows 1–9; Edge Cases 1–15 |
| enemy-ai | 1–18 (states, hostility matrix, detection, fighting other groups), 56 |
| inventory | Edge Case 6 |
| engine-tech | Evaluate: **Contextual Animation** before executions are built |

---

## Phase 7 — Survival

| Spec | Requirements delivered |
|------|------------------------|
| survival | 1–25; Data Flows 1–6; Edge Cases 1–8 (Edge Case 8 raid half in Phase 10) |

---

## Phase 8 — Crafting, Jobs, dyes and enchanting

| Spec | Requirements delivered |
|------|------------------------|
| crafting-jobs | 1–25; Data Flows 1–9; Edge Cases 1–13 |
| engine-tech | Acceptance criterion for dye colours via Substrate on every body variant (with placeholder wearables) |

**Closed 2026-09-16:** for a recipe with more than one output, the refund per input material is `floor(input quantity × deconstructed quantity ÷ recipe output quantity)`. Repair cost uses the same fraction against the repair rule's existing input share. Integer and deterministic; a fraction that rounds to zero refunds nothing, which the existing "refunds nothing" acceptance criterion already permits.

---

## Phase 9 — Enemy AI: loot pickup, equipment, XP and Veterans

| Spec | Requirements delivered |
|------|------------------------|
| enemy-ai | 19–55 (finishing 21, 23, 26, 27, 31 from Phase 1); Data Flows 1–9; Edge Cases 1–26 |

---

## Phase 10 — Factions, kingdoms, economy, quests and raids

| Spec | Requirements delivered |
|------|------------------------|
| factions-kingdoms | 1–66; Data Flows 1–12; Edge Cases 1–17 |
| combat-loot | 42, 43 |
| inventory | 22, 24; Edge Cases 9, 10 |
| engine-tech | Evaluate: **Chaos Destruction replication**, **Mass**, **MegaLights** (night town and raid scene). World Partition moved to Phase 4 on 2026-09-16. |

**Closed 2026-09-16:** when only terrain blocks a raider's path, the raider mirrors the closed-door rule: it digs through player-placed voxels on its path using the server edit kernel at a per-row dig rate in `DT_Factions_Factions`, and never digs natural terrain. Implementation is a navmesh area class that marks player-placed terrain passable at a cost, the same shape as the closed-door area. If no path exists even so, the raider holds at the nearest reachable point to the base centre until the raid ends. The dynamic navmesh rebuild cost per edit at raid scale is measured in this phase's Mass evaluate scene, not separately.

This is the cost peak on both sides: towns, economy, quests and raids in C++, town content and the raid scene as user authoring, and three Evaluate rows in one checkpoint. Budget it as two phases of sittings.

---

## Phase 11 — Races, appearance and Mirror

| Spec | Requirements delivered |
|------|------------------------|
| character-creation | 11, 13, 15, 18–25, 27; Edge Cases 5–9, 14, 15 |
| engine-tech | Evaluate: **Game Features** (one race's content as a Game Feature plugin) |

MetaHuman stays not-used and is not evaluated (round table 2026-09-16). Human bodies are two of the 12 `Body` values in `CO_NamecBody`, so a later source is just another skeletal mesh on the shared skeleton; reopening is the user's call at this phase. Constraint recorded for character-creation Requirement 18: appearance for every race, humans included, is expressed as Mutable parameters, never a second face system.

**Reference PC checkpoints across the roadmap:** Phases 1, 3, 4, 6, 8, 10 and 11. Phases 2, 5, 7 and 9 verify by compile and automation tests alone.

---

## Researcher Implementation Decisions

### Authored map (`specs/authored-map/authored-map.md`, round table 2026-09-16)

User direction: one hand-authored map replaces seeded world generation. The spec owns the shape; the decisions that shaped it:
- Terrain stays fully diggable smooth voxel. The map is authored as heightfield tiles plus region and material masks plus an append-only text stroke log, baked on the reference PC by a tiled, stateless commandlet into a gitignored `UNamecVoxelMapAsset` with per-chunk base hashes. Caves and ore are stateless bake passes driven by `AuthoringSeed`; strokes override them.
- `UNamecWorldGenerator` is gone. Its runtime job is split: base voxel data comes from the asset at chunk stream; capitals, towns, camps, ruins, arenas and chests are author-placed actors validated by `UNamecMapValidator`; trees, rocks and forage stay runtime PCG seeded from `AuthoringSeed` plus chunk coordinates with authored exclusion volumes.
- New World takes name and settings only. The Small/Medium/Large size enum is removed; map size is an authored constant and the real size is the user's later call. A hand-authored 8×8 km map is a large art cost (Builder flag).
- Saves hold `MapId`, `MapRevision`, `MapHash` and per-edited-chunk base hashes. Same id loads with deltas applied per chunk on stream; a mismatched chunk gets a persisted "terrain here was updated" marker. Different id refuses. No load rule discards a delta.
- Expansion placeholders are gated `ANamecExpansionRegion` volumes on the one map, unlocked as a map revision.
- World Partition stays Evaluate (engine-tech Requirement 2 forbids adopting before the criterion is recorded); its criterion is reworded to authored town positions and scheduled at the Phase 4 bake checkpoint.
- The benchmark scene is a 1×1 km `VMA_Benchmark` baked from a Noise heightfield source and a hand-written stroke log, so Phase 1 task 3 stays pipeline-authorable.

### Build environment split (round table 2026-09-16)

Nothing in any phase is compiled, run or measured in the pipeline container. The pipeline authors C++, ini, JSON, CSV-fed data tables and text inputs; the user compiles, runs automation tests and commandlets, opens PIE and runs the benchmark on the reference PC. Task 1 compiles before any sub-branch cuts and declares header-only stubs for every Key Files class so include paths and module dependencies are validated in one round trip. Each handover note lists the engine classes and functions the task assumed against Unreal 5.8 documentation, so a compile error maps to a stated assumption. Results flow back as pasted output; `Saved/Benchmark/<timestamp>/results.json` and `content-manifest.json` are transcribed into the Benchmark Results table by the Researcher.

### Placeholder art and Git LFS (round table 2026-09-16)

Third-party placeholder art (Epic Mannequins, engine starter content, Fab) is not committed; it lives under `Content/Placeholder/`, ignored as a folder. Project-authored uassets are kilobytes to low megabytes and commit to plain git, authored and committed by the user on the task's sub-branch; pipeline branches never touch a uasset. `.gitattributes` carries no LFS filter until real art arrives, because the container has no git-lfs and a `filter=lfs` line would silently commit raw bytes. Two guards: a 20 MB pre-commit rejection and `*.uasset`/`*.umap` marked binary. Git LFS is decided in the first phase that ships real art.

### Lumen HWRT de-risking before task 4 (round table 2026-09-16)

Nothing can be lit in the pipeline, so task 4's checkpoint is two sittings. Sitting one exports one meshed chunk to a `UStaticMesh` through a commandlet (reusing the bake commandlet's plumbing) and lights it in a side-by-side test level next to a static mesh cube; if it lights, geometry and Lumen settings are sound and every later failure is proxy-side. Sitting two adds the custom proxy beside it. The handover note carries a decision tree over three visualisations (is the chunk in the ray tracing scene; does the Lumen surface cache cover it; does hit lighting light it when the cache does not), each stage behind a console-variable kill switch. The coded fallback order is Requirement 13's: Lumen Lite, then Lumen at Medium scalability, then Low with a skylight. Software Lumen is not a fallback because it cannot see terrain without distance fields. Stated assumption for the note: black terrain is most likely a surface cache the custom proxy must supply itself; hit lighting is the diagnostic, not the fix, because of its cost at four viewports.

### Terrain mesh backend (`specs/voxel-world/voxel-world.md` Requirement 2)

**Decision:** a custom `UNamecVoxelChunkComponent` (a `UPrimitiveComponent` subclass) with its own `FPrimitiveSceneProxy`. Surfaces are extracted with Transvoxel (marching cubes with transition cells for LOD seams).

Why this and not the two backends Requirement 2 names:
- **Geometry Script / Dynamic Mesh** is Beta (doc metadata, research file), and `specs/engine-tech/engine-tech.md` Requirement 3 does not list it with a fallback, so Requirement 2 of that spec rules it out. It has also lost distance field support.
- **`UProceduralMeshComponent`**: the research file does not record its status. It also rebuilds whole sections, and it gives no control over ray tracing geometry updates or render state reuse.
- **A custom scene proxy** uses only core renderer paths, so no Beta or Experimental feature is involved. It gives direct control over three things the terrain path depends on: tight per-chunk bounds (VSM invalidation), ray tracing geometry per chunk version (Lumen HWRT), and static draw commands, so an unchanged chunk costs no rebuild.
- **Transvoxel** is chosen over surface nets and dual contouring because it is the documented way to seal seams between LOD levels. Terrain LOD is needed for view distance at 25 cm voxels. Sharp features are not a goal ("natural fantasy landscape").

Wording resolved 2026-09-16 (feasibility issue S3 closed): the sentence lives in voxel-world Requirement 2, not engine-tech Requirement 2, and it was reworded to "Any backend built on core renderer paths qualifies, including a custom `UPrimitiveComponent` with its own scene proxy", because "procedural mesh component" reads as `UProceduralMeshComponent` to anyone who knows Unreal and this decision rejected that class. The "no reliance on distance fields" clause stays as a rule.

Verification: the Task 4 checkpoint (chunks lit under Lumen HWRT and Lumen Lite in PIE on the reference PC), then the benchmark lighting check.

### Enemy Mutable rebuild strategy (`specs/enemy-ai/enemy-ai.md` Requirement 31)

**Decision (starting values in `DT_EnemyAI_Rules`):**
- Each humanoid enemy owns one Mutable instance. It updates only on spawn and after an equip change, which enemy-ai Requirement 29 limits to "after a successful pickup".
- Updates are asynchronous. At most **2** enemy Mutable updates run at once. Queued updates are ordered by distance to the nearest local viewport camera, and player character updates always go before enemy updates.
- An enemy farther than **50 m** (the AI LOD distance) from every player defers its update until it comes within range, and keeps its last built mesh meanwhile. Gameplay (attributes, damage) never waits for the visual.
- No custom cache keyed by equipment set yet. Mutable's own generated-resource cache and streaming are used, plus the 5.8 parallel crowd updates if the 5.8.2 plugin exposes them (verify in Task 11).
- The benchmark records update time and memory (CSV stats). A cache or a lower concurrency limit is added only if the benchmark shows spikes.

### Wearable variant authoring as Mutable inputs (`specs/character-creation/character-creation.md` Requirement 18)

**Proposal (confirmed in Phase 11; Phase 1 uses the same structure with 2 placeholder bodies):**
- One base Customizable Object, `CO_NamecBody`, with an integer `Body` parameter (12 values, race × sex) that selects the body mesh, plus the appearance options as parameters.
- Wearables come from a Mutable Table node backed by a DataTable. Each wearable is one row, with one skeletal mesh column per body variant and one first-person variant column per body variant for hand armor. The row is selected by the equipped item. Requirement 19 visibility comes from Mesh Removal and Clip nodes driven by per-slot visibility columns.
- Dye zones are material mask channels. After each Mutable update, their colours are set as Substrate material parameters on the generated material instances, never as Mutable parameters.
- The wearable variant validator (Requirement 20) checks the DataTable rows (12 non-empty variant columns, visibility columns, mask channels) instead of walking the Customizable Object graph.
- To verify in 5.8.2: that Mutable Table nodes accept skeletal mesh columns.

### Benchmark tooling (`specs/engine-tech/engine-tech.md` Requirement 11)

- **Frame rate:** `ANamecBenchmarkDirector` records per-frame times itself and computes the average and 1% low. Frame rate numbers do not depend on a profiler.
- **GPU time, draw calls, VSM invalidation:** CSV Profiler captures for the timed segment. Which CSV categories 5.8.2 exposes for VSM invalidation is unverified; Task 12 checks the engine source.
- **NaniteStats:** an on-screen console command. The director captures one screenshot per viewport count unless a CSV stat exists.
- **Lighting check:** runs in a separate verification pass after the timed 120 s segment, with the same scripted edits, so captures do not distort frame times. After each re-mesh, and a settle frame count in `DT_Engine_Benchmark`, the director reads back the viewport. It compares mean luminance inside the edited chunk's projected bounds with a reference patch of unedited terrain in the same view, and fails the check below a ratio in `DT_Engine_Benchmark` (starting value 0.25). The route includes an overhang or trench lit only by terrain bounce, so indirect light from terrain is checked as well as direct light. Screenshots are saved with the results for user confirmation.
- **Output:** `Saved/Benchmark/<timestamp>/results.json`, plus CSVs and screenshots.

---

## Benchmark Results

Recorded by the Researcher from `Saved/Benchmark/` result files after the user runs the benchmark on the reference PC.

| Viewports | Tier | GI method | VSM | Foliage | View distance | Avg fps | 1% low fps | stat gpu (ms) | NaniteStats | VSM page invalidation | Draw calls | Lighting check | Manifest hash | Pass |
|-----------|------|-----------|-----|---------|---------------|---------|------------|---------------|-------------|-----------------------|------------|----------------|---------------|------|
| 1 | High | — | — | — | — | — | — | — | — | — | — | — | — | Pending |
| 2 | High | — | — | — | — | — | — | — | — | — | — | — | — | Pending |
| 3 | Split | — | — | — | — | — | — | — | — | — | — | — | — | Pending |
| 4 | Split | — | — | — | — | — | — | — | — | — | — | — | — | Pending |

Engine version: — · Run date: — · Result file: — · Content manifest: — (`specs/authored-map/authored-map.md` Requirement 35) · Benchmark map `MapHash`: —

### Fallback steps applied (Requirement 13)

None yet. Record for each run: the tier, the step applied, and whether Lumen at Medium scalability lit dynamic-mesh terrain.

### Tuned `DT_MP_SplitScreenScalability` (Requirement 12)

Pending a passing run.

## Requirement 3 (Evaluate) Row Decisions

| Feature | Decision | Evidence |
|---------|----------|----------|
| Nanite Geometry Collections | Pending (benchmark) | — |
| First Person Rendering | Pending (benchmark) | — |

## Requirement 5 Evaluate Results

| Feature | Scheduled | Criterion result |
|---------|-----------|------------------|
| MegaLights | Phase 10 | Pending |
| Iris | End of Phase 3 | Pending |
| Chaos Destruction replication | Phase 10 | Pending |
| Contextual Animation | Phase 6 | Pending |
| Mass | Phase 10 | Pending |
| World Partition | Phase 4 bake checkpoint (moved from Phase 10 on 2026-09-16; criterion reworded to authored `ANamecTown` positions) | Pending |
| Game Features | Phase 11 | Pending |
