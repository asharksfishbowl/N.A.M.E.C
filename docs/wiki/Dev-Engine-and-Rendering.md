# Engine and Rendering

This page covers the engine version pin, which Unreal Engine 5.8 features are adopted, avoided or still being evaluated, the Nanite plan, how voxel terrain is rendered, the split-screen scalability tiers, console readiness, and the benchmark milestone.

← [Home](Home.md)

Every engine status here comes from the research file `research/unreal-5.6-5.8-features.md` (research date 2026-09-15).

## Engine version

- **Unreal Engine 5.8, pinned at 5.8.2.** Later 5.8 hotfixes are taken as they ship, and the version in `NAMEC.uproject` is updated with each one.
- The pin is fixed by the specs. Moving to 5.7 or UE6 would be a spec change, not a Researcher decision.
- The benchmark runs again on every new 5.8 hotfix, before that hotfix is merged.

## The readiness rule

- **No Experimental feature ships in a core system** (any system with a spec in the index).
- A Beta feature is used only where the adopted list gives it a fallback.
- An Evaluate feature isn't used in shipped code until its evaluation criterion passes and the spec moves it to the adopted list.

## Adopted

Rows marked *(Evaluate)* are provisional. The benchmark decides whether they keep their main path or switch to the fallback, and that decision is recorded in the engine-tech roadmap.

| Feature | 5.8 status | Used for | Fallback |
|---------|------------|----------|----------|
| Nanite for authored static meshes | Production-Ready | Building pieces, town structures, ruins, rocks, props | Same mesh with Nanite off, with LODs |
| Nanite Geometry Collections *(Evaluate)* | Supported; replication fixes still landing | Local fracture when a building piece is destroyed in a raid. Health and destroyed state stay server-side. | Authored damage-state mesh swaps plus VFX |
| Classic Nanite trees and foliage | Beta | Trees, forage, vegetation. Real geometry leaves, Preserve Area, clamped WPO wind, WPO Disable Distance. | Non-Nanite foliage with LODs and impostors |
| Felled trees as rigid Nanite meshes | Supported (rigid motion) | A felled tree swaps to a simulating actor with the same Nanite mesh | Non-Nanite LOD mesh on the falling actor |
| Lumen with Hardware Ray Tracing | Production-Ready | GI and reflections at 1–2 viewports, and terrain lighting | Lumen Lite, then Lumen Medium/Low with a skylight |
| Lumen Lite | Beta | GI at 3–4 viewports and the handheld console tier | Lumen Medium/Low with a skylight |
| Virtual Shadow Maps | Production-Ready | All shadows | Lower VSM quality |
| Substrate | Production-Ready, on by default | All materials. Dye zone colors are material parameters set at runtime on Mutable-built character meshes and held weapons and shields, not Mutable inputs. | Blendable GBuffer path |
| PCG Framework (runtime, seeded) | Production-Ready | Placing trees, forage, loose pickups, rocks, props and vegetation from the world seed | Seeded placement in C++ |
| Mutable | Production-Ready | Appearance presets, armor fit across the 12 race × sex bodies. Generated materials keep dye zones as runtime parameters. | Per-variant armor meshes with skeletal mesh merging |
| IK Retargeter | Production-Ready | Sharing animation across races and sexes | Authored animation sets per body |
| Motion Matching (core only) | Production-Ready | Player and NPC locomotion | State-machine locomotion |
| Motion Warping | Production-Ready | Execution and attack alignment | Unwarped root-motion montages |
| Gameplay Ability System | Production-Ready | Stats, abilities, damage, status effects | None (required) |
| StateTree and Smart Objects | Production-Ready | Enemy, boss, town NPC, escort and raider AI | Behavior Trees |
| Enhanced Input + Common UI (unified) | Production-Ready | Split-screen input and every menu and HUD | The two systems separately |
| MetaSounds + Audio Insights | Production-Ready | Combat, ambience and raid audio | Sound Cues |
| Data Validation | Not stated | Wearable variant, vendor stock, camp enemy and Nanite authoring validators | None needed |
| Legacy replication + Online Subsystem Null LAN | Existing path / not verified in the research | Listen server and LAN discovery | None |
| PlayerCameraManager + SpringArm | Not stated | First/third-person cameras, lock-on, execution camera | None needed |
| First Person Rendering *(Evaluate)* | Beta (conflicting doc metadata says Production-Ready) | First-person arms and weapon | Normal rendering without separate FOV or clipping prevention |

## Not used

| Feature | Why not |
|---------|---------|
| Nanite on skeletal meshes, characters, enemies, armor | Nanite skinning is Experimental, morph targets aren't supported, and Mutable + Nanite is unverified |
| Nanite Foliage and Dynamic Wind | Experimental. Wind has only a global direction and no collision, and felling is undocumented. |
| Mover | Experimental in every version. Characters use CharacterMovementComponent. |
| Gameplay Camera System | Experimental, with large API changes in 5.8 |
| Mesh Terrain | Experimental, and runtime dig/fill isn't stated |
| Voxel Plugin | Targets 5.6 and 5.7 only (would block the 5.8 pin), runtime edits aren't replicated out of the box, and its docs call it buggy at times |
| MetaHuman Creator and MetaHuman Crowd | Creator is built for human proportions, which doesn't fit five non-human races. Crowd is Experimental. |
| Motion Matching Chooser integration (and multi-character interaction search) | Experimental |
| Spatially Aware Retargeting | Experimental |
| Procedural Vegetation Editor | Experimental, and depends on Nanite Foliage |

## Evaluate

Each one is tested on the reference PC, and the result goes in the engine-tech roadmap. Until it passes, the fallback is what ships.

| Feature | Passes when | Fallback |
|---------|-------------|----------|
| MegaLights | A night town and raid scene holds the tier frame rates at 1–4 viewports, and the scene also looks acceptable without it (no Switch support) | Regular local lights with capped shadow casters |
| Iris | A 2 + 2 LAN session passes every multiplayer acceptance test with lower host CPU or bandwidth than legacy replication | Legacy replication |
| Chaos Destruction replication | A 2-machine LAN raid destroying 50+ pieces never crashes and shows the same destroyed pieces everywhere | Local cosmetic fracture with replicated health |
| Contextual Animation (executions) | A primary source confirms a non-Experimental status, and executions stay in sync on host and LAN client for every skeleton family | Montages + Motion Warping + server attacker/victim sync |
| Mass (ambient crowds) | No Experimental Mass module is needed, and a capital holds the frame rate at 4 viewports. Crowds would be cosmetic only. | No ambient crowd |
| World Partition (authored towns) | Authored town content streams correctly at seeded town positions with 4 viewports | Towns as generated structure placements |
| Game Features | Moving a region's or race's content into a Game Feature keeps generation and saves unchanged | Content stays in the main project |

## The Nanite plan

**Nanite:** building pieces, town structures, ruins, rocks, props, classic trees and foliage, felled trees, and destructible pieces (Geometry Collections).

**Not Nanite:** voxel terrain (no engine path builds Nanite data at runtime), and characters, enemies and armor (skeletal meshes).

### Authoring rules

1. **No masked card foliage.** Leaves and grass are real geometry. Masked-out pixels cost nearly as much as drawn ones.
2. **Vertex:triangle ratio.** 2:1 or more (faceted normals) is a problem, so meshes stay below it. Avoid closely stacked kitbash surfaces.
3. **Clamped WPO.** Every WPO material sets Max WPO Displacement, a WPO Disable Distance and Shadow Cache Invalidation Behavior. No Pixel Depth Offset on Nanite.
4. **No translucency.** Glass, water and ice are separate non-Nanite meshes. Projected decals are fine.
5. **No Lighting Channels** on Nanite meshes.
6. **No Nanite Tessellation or displacement** (Experimental).
7. **Instance cap.** Nanite has a hard cap of 16 million streamed-in instances (all instances). The streaming budget is 12 million (starting value, tunable), with all 4 players' streaming areas loaded.

An editor validator fails the content build when a Nanite mesh breaks rules 3–6.

## Terrain rendering

- A **custom C++ voxel mesher** builds the terrain. The mesh backend (Geometry Script / Dynamic Mesh or a procedural mesh component) is the Researcher's implementation choice.
- Chunks are **non-Nanite** meshes lit by the tier's GI method (**Lumen Hardware Ray Tracing** in the High tier, Lumen Lite in the Split tier, or whatever a benchmark fallback step switched the tier to), whichever backend is used.
- Chunk bounds fit tightly, because re-meshing a chunk invalidates the shadow pages overlapping its bounds.
- Shadows use Virtual Shadow Maps with the regular CPU shadow culling.
- Nothing relies on distance fields, which Dynamic Mesh lost in 5.6. Far terrain shadows, when needed, use far VSM rendering, and the benchmark measures its cost.
- The research doesn't say whether Lumen Lite lights terrain correctly without distance fields, so the benchmark checks it.

## Scalability tiers

Each machine picks a tier from its **own** number of local viewports. A LAN client with one player uses the High tier even if the host has three.

| Tier | Viewports | Target | GI and reflections | Shadows (VSM) | Foliage density | View distance |
|------|-----------|--------|--------------------|---------------|-----------------|---------------|
| High | 1–2 | 60 fps | Lumen HWRT, High | High | 100% | 100% |
| Split | 3–4 | 30 fps | Lumen Lite (Medium GI) | Medium | 50% | 70% |

(all settings are starting values, tuned from the benchmark results)

- The tier switches when a local player joins or leaves and the count crosses between 2 and 3.
- Foliage density only thins out cosmetic scatter. Trees, forage, loose pickups and collidable rocks are the same in both tiers.
- Tiers are rows in `DT_MP_SplitScreenScalability`. See [Data Tables](Dev-Data-Tables.md).

## Console readiness

Console builds are out of scope. These are the constraints kept for a later port:

- Every Nanite mesh has a usable non-Nanite fallback mesh (`r.Nanite.ProxyRenderMode`). Dense scenes can produce too many fallback draw calls, so cosmetic density is a tier setting.
- MegaLights isn't supported on Switch, so nothing depends on it.
- Lumen Lite targets Switch 2 and handhelds, so the handheld tier starts from the Split tier.
- Without hardware ray tracing, GI falls back to Lumen off with a skylight.

## Benchmark milestone

The **first required milestone**. No content production starts until a passing run's results are recorded. A failed milestone keeps content production blocked until the spec change is made and a run passes.

**Scene** (`L_Benchmark_SplitScreen`, reference PC, 1080p):

- Custom voxel terrain being edited: each character digs or fills every 2 seconds.
- A base of 200 Nanite building pieces, with one destructible piece destroyed during the run.
- Nanite rocks and trees at Temperate region density, with one tree felled during the run.
- 4 animated characters (Mutable, Motion Matching, IK Retargeter), one in first-person view.

(2 seconds, 200 pieces and the 120-second run are starting values, tunable)

**Measured at 1, 2, 3 and 4 viewports:** average and 1% low fps, `stat gpu`, `NaniteStats`, VSM page invalidation, and draw calls. It also checks that re-meshed terrain chunks are lit correctly in both tiers.

**Passes when** the run averages 60 fps or more at 1–2 viewports and 30 fps or more at 3–4. Results are recorded in the engine-tech roadmap, then the tiers are tuned to the best settings that still pass.

**If a target is missed,** these fallbacks are applied one at a time, in order, re-running after each:

1. Lower foliage density and view distance.
2. Lower VSM quality one level and shorten the WPO Disable Distance.
3. Step GI down (High tier: Lumen HWRT → Lumen Lite; Split tier: Lumen Lite → Lumen Medium → Lumen off with a skylight).
4. Replace Nanite trees and foliage with non-Nanite foliage.
5. Replace destructible fracture with damage-state mesh swaps.
6. Replace First Person Rendering with its fallback.

If every step is applied and a target is still missed, the performance targets go back to the user as a spec change.

## Source specs

- [Engine Tech](../../specs/engine-tech/engine-tech.md)
- [Game Foundation](../../specs/game-foundation/game-foundation.md) (Requirements 1 and 13)
- [Voxel World](../../specs/voxel-world/voxel-world.md) (Requirement 2)
