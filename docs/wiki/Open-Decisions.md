# Open Decisions

This page lists design questions that are still unresolved in the specs, and records what the
2026-09-16 round table closed so the history is not lost.

← [Home](Home.md)

## Still open

### Real map size

The Small/Medium/Large world size enum is gone. Map size is an authored constant on the map
asset (`specs/authored-map/authored-map.md` Requirement 1). The benchmark map is 1 km × 1 km.
The shipped map's size is the user's call when Phase 4 starts authoring it; a hand-authored
8 × 8 km map is a large art cost.

### Git LFS

Deferred to the first phase that ships real art. Until then `.gitattributes` carries no LFS
filter, third-party placeholder art is never committed, and a 20 MB pre-commit guard forces the
decision explicitly the day real art shows up (`specs/authored-map/authored-map.md`
Requirements 32–33).

### MetaHuman

Stays **not used** and is not evaluated. Human bodies are two of the 12 `Body` values in
`CO_NamecBody`, so a later source is just another skeletal mesh on the shared skeleton. Reopening
is the user's call at Phase 11. The Epic licence terms are still unread; issue 0916-3 lands the
findings as reference only.

### Untested assumptions (user-sitting results, not decisions)

- The black-terrain Lumen HWRT failure, if it occurs on the custom chunk proxy, is most likely a
  missing Lumen surface cache. Task 4's two-sitting checkpoint bisects this.
- Whether Lumen Lite lights a custom scene proxy at all (engine-tech Requirement 9).
- Stroke-log replay cost at 8 × 8 km. The 1 km benchmark bake gives the first number.

## Closed on 2026-09-16 (round table)

### One authored map instead of seeded worlds

User direction. One hand-authored, fully diggable smooth-voxel map replaces seeded generation.
`specs/authored-map/authored-map.md` owns the shape: heightfield tiles, masks and an append-only
text stroke log are versioned in plain git; a tiled, stateless bake commandlet produces a
gitignored map asset with per-chunk base hashes on the reference PC. Caves and ore are bake
passes; trees, rocks and forage stay runtime PCG seeded from the map's `AuthoringSeed`; towns,
camps, ruins and boss arenas are author-placed actors checked by an editor validator. New World
takes name and settings only. Saves carry map id, revision and hash; a patched map keeps every
player edit and marks changed chunks. Expansion content is a gated region on the same map,
unlocked as a map revision. World Partition stays Evaluate, moved to the Phase 4 bake checkpoint.

### Build environment

Nothing is compiled, run or measured in the pipeline container. The pipeline authors C++ and
text; the user compiles, tests and measures on the reference PC and pastes results back. Every
Phase 1 task ends "awaiting verification" with a handover note (assumed engine APIs, exact
commands, one proving command, per-asset source and path). Task 1 compiles first and carries
header-only stubs for every Key Files class. About fifteen user sittings for Phase 1. See the
roadmap's "Build environment split".

### Placeholder art

Epic Mannequins, starter content and Fab assets, kept on the reference PC under
`Content/Placeholder/` and never committed. Project-authored uassets commit to plain git, by the
user, on the task's sub-branch. Every C++ reference to a placeholder goes through a data-table
soft-object path. Task 12 writes a content manifest beside the benchmark results.

### Terrain mesh backend wording

The sentence lived in voxel-world Requirement 2, not engine-tech Requirement 2. Reworded: any
backend built on core renderer paths qualifies, including a custom `UPrimitiveComponent` with its
own scene proxy. Feasibility issue S3 closed.

### Lumen HWRT de-risking

Task 4's lighting checkpoint is two sittings: a static-mesh export of one chunk lit in a
side-by-side test level, then the custom proxy beside it. The handover note carries a
three-question decision tree (ray tracing scene, surface cache, hit lighting) with a
console-variable kill switch per stage. Fallback order is engine-tech Requirement 13's: Lumen
Lite, Lumen Medium, Low with a skylight. Software Lumen is not a fallback.

### Roadmap open items

- Default machine-wide graphics settings equal the High tier row of `DT_MP_SplitScreenScalability`.
- Multi-output recipe refund: `floor(input × deconstructed ÷ output)` per material; repair uses
  the same fraction.
- A raider blocked only by terrain digs through player-placed voxels at a per-row rate via a
  navmesh area class, never natural terrain, and otherwise holds at the nearest reachable point.

### Missing research documents

Issues 0916-1, 0916-2 and 0916-3 re-derive the feasibility review, the Unreal 5.6–5.8 evidence
and the MetaHuman findings. Issue 0916-4 re-files the Phase 1 build tasks in the authored-map
shape. Issue 0916-5 asks the Researcher to review the authored-map spec once 0916-2 lands.
