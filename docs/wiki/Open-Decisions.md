# Open Decisions

This page lists design questions that are still unresolved in the specs.

← [Home](Home.md)

## Build environment

Phase 1 is **Blocked**. The pipeline container has no Unreal Engine, no compiler, no GPU and no
git-lfs, so nothing can be built, run or measured there. The benchmark milestone needs the
reference PC and a person at it. See the Phase 1 status in
[`engine-tech-roadmap.md`](../../specs/engine-tech/engine-tech-roadmap.md).

## Placeholder art for Phase 1

No source is chosen for the placeholder meshes, animations and textures the benchmark scene needs
(two bodies, a shared animation set, building pieces, trees and rocks), and it is not settled
whether the project uses Git LFS for them. This is the second thing blocking Phase 1.

## Terrain mesh backend — decided, wording to confirm

The Researcher chose a custom `UNamecVoxelChunkComponent` (a `UPrimitiveComponent` subclass with
its own scene proxy), with surfaces extracted by **Transvoxel** for sealed LOD seams. The decision
and its reasoning are in the roadmap.

What is open is the spec wording: engine-tech Requirement 2 offers "Geometry Script / Dynamic Mesh,
or a procedural mesh component", and the decision reads a custom procedural primitive as falling
under that phrase. Either confirm that reading or reword the requirement.

## Lumen HWRT on procedural voxel terrain

Unverified, and the largest technical risk in the project. Bounce light may come back black on
procedurally generated chunks. Phase 1 task 4 carries a lighting checkpoint on the reference PC
before anything else depends on the result, and the benchmark fails a run with unlit terrain
whatever its frame rate. See
[Engine and Rendering](Dev-Engine-and-Rendering.md#benchmark-milestone).

## MetaHumans

The engine-tech spec lists MetaHuman Creator and MetaHuman Crowd as **not used** — Creator assumes
human proportions, which does not fit the five non-human races or the 12-variant shared armor rule,
and Crowd is Experimental. See [Engine and Rendering](Dev-Engine-and-Rendering.md).

A research pass on 2026-09-15 suggested two narrower uses worth evaluating: human town NPCs, and
the human player race as MetaHuman bodies and heads driven through Mutable. It recommended
avoiding every other use. Nothing is adopted, and three things block a decision:

- The Epic licence terms for commercial PC and console shipping have not been read.
- Art direction — photoreal humans standing beside animal-folk races — is the user's call.
- Any adoption is a spec change, moving a Beta feature with a fallback out of the not-used list.

## Missing research documents

The roadmap cites three research documents that no longer exist: the feasibility review, the
Unreal 5.6–5.8 feature evidence, and the MetaHuman findings. `research/` is per-user and
gitignored, so they were never committed and were lost when the project directory was
re-provisioned. The roadmap's own conclusions survive; the evidence behind them does not, and
anything tracked only in the feasibility review — including its numbered spec issues — has to be
re-derived.
