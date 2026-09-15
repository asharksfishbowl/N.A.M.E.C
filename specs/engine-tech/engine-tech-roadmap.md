# Roadmap: N.A.M.E.C Implementation (umbrella)

Created by the Researcher on 2026-09-15. This is the umbrella roadmap for all 12 system specs. `specs/engine-tech/engine-tech.md` sends several records here (benchmark results, Evaluate results, and Researcher implementation decisions), so this file holds them as well.

- Feasibility review, spec issues and evidence: `research/specs-feasibility-review.md`
- System Spec Index: `specs/game-foundation/game-foundation.md`
- Engine evidence: `research/unreal-5.6-5.8-features.md`

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

**Status:** Blocked on the build environment and placeholder asset decisions (Inbox questions 1789450473110dizhxm and 1789450473117j76wcv). Build tasks are filed.

**Goal:** `L_Benchmark_SplitScreen` runs on the reference PC at 1–4 viewports and passes, or the Requirement 13 fallback steps are applied until it passes or the milestone fails back to the user.

| Spec | Requirements delivered |
|------|------------------------|
| engine-tech | 1, 2, 6, 9, 11, 12, 13, 14; 3 (the adopted rows the scene exercises: Nanite static meshes, Nanite Geometry Collections, classic Nanite trees, felled trees, Lumen HWRT, Lumen Lite, VSM, Substrate, PCG, Mutable, IK Retargeter, Motion Matching, StateTree + Smart Objects, PlayerCameraManager + SpringArm, First Person Rendering); 7 (core: tier selection and tier row application; user settings ceiling in Phase 2); Data Flows 1 (core), 2, 3, 4, 5; Edge Cases 1, 2, 3, 6 |
| game-foundation | 1, 2, 13 (measured) |
| voxel-world | 1, 2, 3, 4; 15 (core: server-authoritative replicated dig and fill, no save); 17–18 (core: tree health and felling swap); 26 (core: Nanite piece with replicated health and destroyed state); Data Flows 2, 4 |
| multiplayer | 5, 9, 10 |
| enemy-ai | 2, 11; 1 and 3 (core: StateTree host, Idle/Patrol and Loot only); 21, 23, 26, 27, 31 (core: Smart Object pickup, empty-slot equip, Mutable gear) |
| character-creation | 14 (core: shared animation set retargeted to 2 placeholder bodies); 18 (authoring decision recorded, validated in Phase 11) |

**Tasks (in `build-queue.groovy`):**

| # | Task | Depends on |
|---|------|-----------|
| 1 | Project skeleton and engine configuration | — |
| 2 | Split-screen scalability tier subsystem | 1 |
| 3 | Voxel chunk data, benchmark generator and chunk streaming | 1 |
| 4 | Voxel mesher and chunk render component ⏸ lighting checkpoint | 3 |
| 5 | Terrain LOD with transition cells | 4, 2 |
| 6 | Terrain collision, dig/fill edit RPC and re-mesh | 4 |
| 7 | Nanite building pieces with fracture and damage-state fallback | 1 |
| 8 | Runtime seeded PCG trees and rocks, and tree felling | 4, 2 |
| 9 | Benchmark characters: Mutable body, Motion Matching, cameras, First Person Rendering | 1 |
| 10 | Benchmark AI enemies: StateTree, AI LOD, dynamic navmesh | 6 |
| 11 | Humanoid enemy pickup and Mutable equip with rebuild budget | 9, 10 |
| 12 | Benchmark director, measurements, lighting check and results ⏸ benchmark run checkpoint | 2, 5, 6, 7, 8, 11 |

**Exit:** a passing run is recorded below, `DT_MP_SplitScreenScalability` is tuned (Requirement 12), and the Evaluate row decisions for Nanite Geometry Collections and First Person Rendering are recorded.

---

## Phase 2 — Project foundation

**Goal:** saves, settings, input, UI shell and content validators that every later phase writes into.

| Spec | Requirements delivered |
|------|------------------------|
| game-foundation | 4, 5, 6 (the three save types, each with only the fields that exist so far; later phases add theirs), 7, 8, 9, 10, 11, 12, 14 (the base mapping contexts and the context framework; each context's actions are finished in the phase that owns them); Edge Cases 1–4 |
| engine-tech | 4 (plugin exclusion check), 7 (user settings ceiling from `UNamecSettingsSave`), 8 (`NamecNaniteAuthoringValidator`), 10; Edge Cases 4, 5, 9 |
| multiplayer | 2, 3, 6, 7, 23 |

Open input: default machine-wide graphics settings (Director open item; see the feasibility review for the recommended default).

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

---

## Phase 4 — Progression XP and world

| Spec | Requirements delivered |
|------|------------------------|
| character-progression | 7–11, 14–26, 28, 30; Data Flows 1–7; Edge Cases 1–4, 6, 7 |
| voxel-world | 5–16, 17–23, 30–36; Data Flows 1, 3, 5, 7 (world fields so far); Edge Cases 2–4, 6–12 |
| survival | 9 (ambient temperature inputs from climate) (core) |

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

Open input: refund and repair maths for recipes with more than one output (Director open item).

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
| engine-tech | Evaluate: **Chaos Destruction replication**, **Mass**, **World Partition**, **MegaLights** (night town and raid scene) |

Open input: raider behaviour when only terrain blocks its path (Director open item).

---

## Phase 11 — Races, appearance and Mirror

| Spec | Requirements delivered |
|------|------------------------|
| character-creation | 11, 13, 15, 18–25, 27; Edge Cases 5–9, 14, 15 |
| engine-tech | Evaluate: **Game Features** (one race's content as a Game Feature plugin) |

---

## Researcher Implementation Decisions

### Terrain mesh backend (`specs/voxel-world/voxel-world.md` Requirement 2)

**Decision:** a custom `UNamecVoxelChunkComponent` (a `UPrimitiveComponent` subclass) with its own `FPrimitiveSceneProxy`. Surfaces are extracted with Transvoxel (marching cubes with transition cells for LOD seams).

Why this and not the two backends Requirement 2 names:
- **Geometry Script / Dynamic Mesh** is Beta (doc metadata, research file), and `specs/engine-tech/engine-tech.md` Requirement 3 does not list it with a fallback, so Requirement 2 of that spec rules it out. It has also lost distance field support.
- **`UProceduralMeshComponent`**: the research file does not record its status. It also rebuilds whole sections, and it gives no control over ray tracing geometry updates or render state reuse.
- **A custom scene proxy** uses only core renderer paths, so no Beta or Experimental feature is involved. It gives direct control over three things the terrain path depends on: tight per-chunk bounds (VSM invalidation), ray tracing geometry per chunk version (Lumen HWRT), and static draw commands, so an unchanged chunk costs no rebuild.
- **Transvoxel** is chosen over surface nets and dual contouring because it is the documented way to seal seams between LOD levels. Terrain LOD is needed for view distance at 25 cm voxels. Sharp features are not a goal ("natural fantasy landscape").

Director check: Requirement 2 lists "Geometry Script / Dynamic Mesh, or a procedural mesh component". This decision reads a custom procedural primitive component as falling under "a procedural mesh component". See the feasibility review, issue S3.

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

| Viewports | Tier | GI method | VSM | Foliage | View distance | Avg fps | 1% low fps | stat gpu (ms) | NaniteStats | VSM page invalidation | Draw calls | Lighting check | Pass |
|-----------|------|-----------|-----|---------|---------------|---------|------------|---------------|-------------|-----------------------|------------|----------------|------|
| 1 | High | — | — | — | — | — | — | — | — | — | — | — | Pending |
| 2 | High | — | — | — | — | — | — | — | — | — | — | — | Pending |
| 3 | Split | — | — | — | — | — | — | — | — | — | — | — | Pending |
| 4 | Split | — | — | — | — | — | — | — | — | — | — | — | Pending |

Engine version: — · Run date: — · Result file: —

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
| World Partition | Phase 10 | Pending |
| Game Features | Phase 11 | Pending |
