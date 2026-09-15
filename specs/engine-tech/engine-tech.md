# Spec: Engine Technology & Rendering

## Overview
This spec fixes which Unreal Engine 5.8 features N.A.M.E.C uses, which it does not use, and which are still being evaluated. It also sets how rendering scales across 1–4 local split-screen viewports and defines the benchmark milestone that must pass before content production starts. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

REFERENCE: `research/unreal-5.6-5.8-features.md` (research date 2026-09-15) is the evidence source for every engine status and capability claim in this spec. Its source keys (RN58, D-VSM, D-NaniteOverview, and others) map to Epic release notes and documentation in that file's section 5. A claim that file marks unverified is marked unverified here too.

## Goals
- Every engine feature a core system depends on has a known 5.8 status and a stated fallback.
- The performance targets in `specs/game-foundation/game-foundation.md` Requirement 13 are measured on the reference PC with 1–4 viewports before any content production.
- Nanite covers every authored static mesh it suits. Content it does not suit (voxel terrain, skeletal meshes) has a defined non-Nanite rendering path.
- A console port needs no core system rework to lose a PC-only rendering feature.

## Non-Goals
- Console builds (`specs/game-foundation/game-foundation.md` Non-Goals). This spec records console-readiness constraints only.
- Moving off Unreal Engine 5.8 (to 5.7 or to UE6).
- Choosing the terrain mesh backend. That is a Researcher implementation choice (`specs/voxel-world/voxel-world.md` Requirement 2).
- Art direction, and content budgets beyond the benchmark scene.
- Audio design. MetaSounds is adopted as the audio technology, but no system spec defines audio content yet.

## Requirements

### Engine Version
1. The engine is Unreal Engine 5.8, pinned at 5.8.2, and the project takes later 5.8 hotfix releases (`specs/game-foundation/game-foundation.md` Requirement 1). Every feature status in this spec is the 5.8 status from the research file.

### Feature Readiness Rule
2. No feature that is Experimental in 5.8 ships in a core system. A core system is any system defined by a spec in the System Spec Index of `specs/game-foundation/game-foundation.md`. A Beta feature is used only where Requirement 3 lists it with a fallback. A feature on the Evaluate list (Requirement 5) is not used in shipped code until its evaluation criterion passes, the result is recorded in `specs/engine-tech/engine-tech-roadmap.md` (created by the Researcher), and this spec moves the feature to Requirement 3.

### Adopted Features
3. N.A.M.E.C adopts these features. Rows marked **(Evaluate)** are adopted provisionally: the benchmark milestone (Requirement 11) decides whether each keeps its primary path or switches to its fallback.

   | Feature | Engine status in 5.8 | N.A.M.E.C system it serves | How it is used | Fallback |
   |---------|----------------------|----------------------------|----------------|----------|
   | **Nanite for authored static meshes** | Production-Ready (doc metadata) | Building pieces (`specs/voxel-world/voxel-world.md` Requirement 24), town structures (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 14), ruins and camps (`specs/voxel-world/voxel-world.md` Requirement 10), rocks, props | Every authored static mesh has Nanite enabled and follows the authoring rules in Requirement 8. Large shadow casters such as buildings being Nanite is the biggest Virtual Shadow Map win. | The same asset with Nanite off, as a standard static mesh with LODs |
   | **Nanite Geometry Collections** (Evaluate) | Supported, with fixes in each version. 5.6 added a non-Nanite fallback for Nanite Geometry Collections. Chaos Destruction replication crash fixes were still landing in 5.8. | Raid building damage (`specs/voxel-world/voxel-world.md` Requirement 26, `specs/factions-kingdoms/factions-kingdoms.md` Requirement 57) | When a building piece is destroyed, each client plays a local Nanite Geometry Collection fracture. Piece health and destroyed state stay server-authoritative and replicated. The fracture itself is not replicated (Chaos Destruction replication is on the Evaluate list, Requirement 5). | Authored damage-state mesh swaps selected from the replicated piece health, plus VFX |
   | **Classic Nanite static-mesh trees and foliage** | Beta ("Foliage using Nanite is considered Beta") | Trees (`specs/voxel-world/voxel-world.md` Requirement 17), forage nodes (`specs/voxel-world/voxel-world.md` Requirement 22), region vegetation (`specs/voxel-world/voxel-world.md` Requirement 8) | Trees and foliage are Nanite static meshes with real geometry leaves (no masked cards), Preserve Area on, wind from World Position Offset clamped with Max WPO Displacement, and a WPO Disable Distance set. They are placed through the PCG Framework. | Non-Nanite foliage with LODs and impostors |
   | **Felled trees as rigid Nanite static meshes** | Supported: Nanite supports dynamic translation, rotation, and non-uniform scaling | Tree felling (`specs/voxel-world/voxel-world.md` Requirements 18–19) | At 0 health the server swaps the standing tree for a replicated simulating actor that uses the same Nanite static mesh. Complex collision uses the fallback mesh or a chosen LOD. The moving caster invalidates Virtual Shadow Map pages under its bounds while it falls. | A non-Nanite LOD mesh on the falling actor |
   | **Lumen with Hardware Ray Tracing (HWRT)** | Lumen Production-Ready (doc metadata). HWRT split-screen support added in 5.7, with split-screen GI fixes in 5.8. HWRT needs SM6 and an RTX 2000-series or newer GPU on PC. | All world rendering, including voxel terrain lighting (`specs/voxel-world/voxel-world.md` Requirement 2) | Dynamic GI and reflections at the High tier (Requirement 6). Dig and fill rule out baked lighting. Terrain uses HWRT because Dynamic Mesh distance fields are deprecated, so software ray tracing cannot see dynamic-mesh terrain. | Lumen Lite (next row), then Lumen at Medium or Low scalability with a skylight (Low disables Lumen) |
   | **Lumen Lite** (Medium GI, Irradiance Field Gather) | Beta | Rendering at 3–4 local viewports (Requirement 6), and the handheld console tier (Requirement 10) | Medium-quality GI, about 2× cheaper than Lumen High, used as the GI method for 3–4 viewports | Lumen at Medium or Low scalability with a skylight |
   | **Virtual Shadow Maps (VSM)** | Production-Ready (doc metadata). 5.7 runs coarse page marking once across multiple views. | All shadows | Nanite content uses VSM with Nanite shadow caching. Non-Nanite terrain chunks use the regular CPU shadow culling (Requirement 9). A lower VSM quality level is used at 3–4 viewports (Requirement 6). | A lower VSM quality level. The research file names no non-VSM shadow method, so a different shadowing technique would need new research. |
   | **Substrate** | Production-Ready and on by default since 5.7 | Dye zones (`specs/crafting-jobs/crafting-jobs.md` Requirements 20–22, `specs/inventory/inventory.md` Requirement 23), all materials | Each dye zone color is a material parameter applied to that zone's material mask channel, set at runtime on the Mutable-built character meshes and on held weapon and shield meshes. Dye colors are not Mutable inputs, so a dye change never rebuilds a character. | The Blendable GBuffer path in place of the Adaptive GBuffer path |
   | **PCG Framework**, at runtime and seeded | Production-Ready since 5.7, for runtime and in-editor use. 5.8 adds GPU runtime scatter as fast as landscape GPU grass, and actor-less runtime generation. | Placement of trees, forage nodes, loose pickups, rocks, props, and vegetation (`specs/voxel-world/voxel-world.md` Requirements 6, 17, 22, and 30) | PCG graphs run at runtime per streamed chunk and are seeded from the world seed. Every placement that has gameplay state (trees, forage nodes, loose pickups) or collision produces identical results on every machine for the same seed, size, and voxel resolution (`specs/voxel-world/voxel-world.md` Requirement 6). GPU runtime scatter is used only for cosmetic placement that has no gameplay state and no collision. | Seeded placement written in C++ in `UNamecWorldGenerator` |
   | **Mutable** (Customizable Objects) | Production-Ready in 5.8 | Appearance presets and sliders (`specs/character-creation/character-creation.md` Requirements 15–17), armor fit across the 12 race × sex body variants (`specs/character-creation/character-creation.md` Requirement 18) | Mutable generates each character's skeletal meshes, materials, and textures from `FNamecAppearance` and the equipped items. Generated materials keep each dye zone as a runtime material parameter (Substrate row). | Authored per-variant armor meshes combined through skeletal mesh merging (not verified in the research file) |
   | **IK Retargeter** | Production-Ready (doc metadata). 5.8 adds Foot Definition and Retarget Override Sets (one asset handles several retarget relationships). | Sharing animation across the six races and two sexes (`specs/character-creation/character-creation.md` Requirement 14) | One authored animation set per skeleton family is retargeted to each race and sex body. The Spatially Aware retarget ops are not used (Requirement 4). | Authored animation sets per race and sex body |
   | **Motion Matching** (Pose Search), core only | Production-Ready since 5.4 | Player and NPC locomotion | Pose-database locomotion selection. The Chooser integration and multi-character interaction search are not used (Requirement 4). Its CPU cost with 4 players is measured by the benchmark (Requirement 11). | State-machine locomotion in the Animation Blueprint |
   | **Motion Warping** | Production-Ready (doc metadata). 5.8 adds mesh-scale support (`a.MotionWarping.UseMeshScale`). | Execution alignment (`specs/combat-loot/combat-loot.md` Requirements 47–48), attack alignment | Warps the root motion of the paired attacker and victim montages, and of attacks, toward their targets. Mesh-scale support covers different body sizes. The experimental switch-off condition and warp target direction APIs are not used. | Unwarped root-motion montages started from the positions validated by the `specs/combat-loot/combat-loot.md` Requirement 48 capsule sweep |
   | **Gameplay Ability System (GAS)** | Production-Ready (doc metadata) | Stats, abilities, damage, status effects (`specs/game-foundation/game-foundation.md` Requirement 3) | As specified in the system specs | None. `specs/game-foundation/game-foundation.md` Requirement 3 requires GAS. |
   | **StateTree and Smart Objects** | Both Production-Ready (doc metadata). StateTree scheduled ticks (5.6) lower CPU cost for idle NPCs. | Enemy and boss AI (`specs/combat-loot/combat-loot.md` Requirement 19), town NPCs and escort NPCs (`specs/factions-kingdoms/factions-kingdoms.md` Requirements 20–26 and 40), raider behavior (`specs/factions-kingdoms/factions-kingdoms.md` Requirements 56–58) | Each enemy and NPC row names a StateTree asset. Smart Objects mark NPC post interactions and raider attack positions on building pieces. Raid rolls stay in `UNamecRaidSubsystem`. | Behavior Trees (not covered by the research file) |
   | **Enhanced Input + Common UI unified input system** | Production-Ready in 5.8. Common UI supports registering inputs for multiple local players on one widget (5.6). | Split-screen input and UI (`specs/game-foundation/game-foundation.md` Requirements 9 and 14, `specs/multiplayer/multiplayer.md` Requirements 6–7) | All input mapping and every menu and HUD widget, per local player and gamepad-first | Enhanced Input and Common UI as separate systems |
   | **MetaSounds + Audio Insights** | MetaSounds Production-Ready (doc metadata). Audio Insights Production-Ready in 5.8. MetaSound Templates are Experimental. | Combat, ambience, and raid combat audio. Raids still have no warning cue (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 56). | Procedural audio graphs, with Audio Insights for debugging. MetaSound Templates are not used. | Sound Cues (not covered by the research file) |
   | **Data Validation** | Status not stated. 5.8 adds cook-time validation stats and a refactored commandlet. | Wearable variant validator (`specs/character-creation/character-creation.md` Requirement 20), vendor stock validator (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 35), camp enemy validator (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 62), Nanite authoring validator (Requirement 8) | Custom C++ validators in the `NAMECEditor` module fail the content build | None needed. Validators are editor-only tooling with no runtime dependency. |
   | **Legacy replication on a listen server, with Online Subsystem Null LAN sessions** | Legacy replication is the engine's existing replication path. Online Subsystem Null LAN sessions are not verified in the research file. | Session model and LAN discovery (`specs/multiplayer/multiplayer.md` Requirements 1 and 11) | Actor replication with GAS prediction on the host's listen server. LAN discovery through `OnlineSubsystemNull` with `bIsLANMatch = true`. Iris and the NetworkServiceDiscovery plugin are not used. | None. Iris (Requirement 5) replaces legacy replication only after its evaluation passes. NetworkServiceDiscovery is Experimental. |
   | **PlayerCameraManager + SpringArm** | Status not stated. The research file recommends it in place of the Experimental Gameplay Camera System. | First- and third-person cameras (`specs/game-foundation/game-foundation.md` Requirement 12), lock-on framing (`specs/combat-loot/combat-loot.md` Requirement 6), the execution camera (`specs/combat-loot/combat-loot.md` Requirement 52) | A custom PlayerCameraManager with a SpringArm for third person, and camera modifiers for lock-on and executions | None needed |
   | **First Person Rendering** (Evaluate) | Beta in the 5.6 notes. The 5.8 doc metadata says Production-Ready, with no release-note promotion (conflict). | First-person view (`specs/game-foundation/game-foundation.md` Requirement 12), first-person arms (`specs/character-creation/character-creation.md` Requirement 21) | Renders first-person arms and the held weapon with a separate FOV, no clipping into walls, and VSM self-shadows | First-person arm and weapon meshes rendered through the normal pipeline, without a separate FOV or clipping prevention |

### Not Used
4. These features are explicitly not used. Requirement 2 (no Experimental feature ships in a core system) applies to every feature not listed here as well.
   - **Nanite on skeletal meshes, characters, enemies, and armor.** Nanite skinning was last stated Experimental (5.7) with no later promotion, morph targets are not supported with Nanite, and Mutable output with Nanite enabled is unverified. Characters, enemies, and the 12 armor body variants are standard skeletal meshes with LODs.
   - **Nanite Foliage (Assemblies, Skinning, Voxels) and Dynamic Wind.** Experimental in 5.8. Dynamic Wind supports only a global wind direction and has no collision with players or objects, and felling skinned or assembly trees with physics is undocumented. Trees use classic Nanite static meshes (Requirement 3).
   - **Mover.** Experimental in every version through 5.8. Characters use CharacterMovementComponent.
   - **Gameplay Camera System.** Experimental, with large API changes in 5.8. Cameras use PlayerCameraManager + SpringArm (Requirement 3).
   - **Mesh Terrain.** Experimental in 5.8, and the release notes do not say it supports runtime dig and fill.
   - **Voxel Plugin.** Its release targets Unreal Engine 5.6 and 5.7 only, with no documented 5.8 support, so it would block the 5.8 pin. Runtime edits are not replicated out of the box, and its docs say it "can be buggy at times". Terrain uses a custom C++ voxel mesher (`specs/voxel-world/voxel-world.md` Requirement 2).
   - **MetaHuman Creator and MetaHuman Crowd.** MetaHuman Creator is built for human proportions and does not suit the five non-human races or the 12-variant shared armor rule. MetaHuman Crowd is Experimental.
   - **The Motion Matching Chooser integration**, and the multi-character interaction search (`MotionMatchMulti`). Both Experimental.
   - **Spatially Aware Retargeting.** Experimental (5.7).
   - **Procedural Vegetation Editor.** Experimental, and its output depends on Nanite Foliage.

### Evaluate
5. These features are evaluated before any system uses them. Each evaluation runs on 5.8 on the reference PC, and its result is recorded in `specs/engine-tech/engine-tech-roadmap.md` (created by the Researcher). Until a criterion passes, the fallback is the shipped path.

   | Feature | Engine status in 5.8 | Evaluation criterion | Fallback |
   |---------|----------------------|----------------------|----------|
   | **MegaLights** | Production-Ready. Designed for PS5/XSX and ray-tracing-capable PCs, with HWRT recommended. Not supported on Switch or mobile. | A night town and raid lighting scene holds the Requirement 6 tier frame rates at 1, 2, 3, and 4 viewports with MegaLights on, and the same scene has an acceptable non-MegaLights lighting setup, because MegaLights is not available on Switch (Requirement 10) | Regular local lights, with the number of shadow-casting lights capped |
   | **Iris** | Production-ready for licensees per the 5.8 notes, but the 5.8 doc page still says Experimental (conflict). Little gain expected at 4 players or fewer. | A 2 + 2 LAN session passes every acceptance criterion in `specs/multiplayer/multiplayer.md` with Iris on, with host CPU time or bandwidth lower than legacy replication in the same session | Legacy replication (Requirement 3) |
   | **Chaos Destruction replication** | Chaos Destruction Production-Ready (doc metadata). Geometry Collection replication crash fixes were still landing in 5.8. | A 2-machine LAN raid that destroys at least 50 building pieces (starting value) runs with replicated fracture and no crash, and every machine shows the same destroyed pieces | Local cosmetic fracture on each client with replicated health and destroyed state (Requirement 3, Nanite Geometry Collections row) |
   | **Contextual Animation** for executions | Status not found in any 5.6–5.8 primary source | A primary source confirms a non-Experimental status in 5.8, and a paired execution (`specs/combat-loot/combat-loot.md` Requirements 47–49) plays in sync on the host and on a LAN client for every skeleton family in `DT_Combat_Executions` | Montages + Motion Warping + a server-driven attacker and victim sync |
   | **Mass** for ambient crowds | Mass core Production-Ready (doc metadata). 5.8 is a major overhaul, and Mass navigation was Experimental (5.6). | An ambient crowd uses no Experimental Mass module and holds the Requirement 6 tier frame rates in a capital at 4 viewports. Ambient crowds are cosmetic only: no system spec requires them, and town NPCs and enemies stay Actors with GAS (`specs/combat-loot/combat-loot.md` Requirement 42). | No ambient crowd. Towns hold only the Actor-based town NPCs. |
   | **World Partition** for authored towns | Production-Ready (doc metadata). 5.8 adds World Partition Insights. It partitions authored actors, so its value in a runtime-seeded world is limited to authored content. | Town content authored with World Partition streams correctly at the seeded capital and town positions (`specs/voxel-world/voxel-world.md` Requirement 6) with 4 viewports as streaming sources. If adopted, World Partition Insights is used for streaming debugging. | Towns as generated structure placements spawned on chunk stream-in (`specs/factions-kingdoms/factions-kingdoms.md` Data Flow 2) |
   | **Game Features** | Production-Ready (doc metadata) | Moving one region's or one race's content into a Game Feature plugin keeps deterministic generation (`specs/voxel-world/voxel-world.md` Requirement 6) and save loading unchanged, and removes a module dependency | Content stays in the main project and its module folders (`specs/game-foundation/game-foundation.md` Requirement 4) |

### Rendering Scalability Tiers
6. Rendering uses one of two tiers, selected by the number of local viewports on each machine. Each tier is a row in `DT_MP_SplitScreenScalability` (`specs/multiplayer/multiplayer.md` Requirement 9), and the frame rate targets are those of `specs/game-foundation/game-foundation.md` Requirement 13. Every setting below is a starting value, replaced by the tuned values from the benchmark milestone (Requirement 12).

   | Tier | Local viewports | Target | Global illumination and reflections | VSM quality | Foliage density | View distance |
   |------|-----------------|--------|-------------------------------------|-------------|-----------------|---------------|
   | **High** | 1–2 | 60 fps | Lumen HWRT at High scalability | High (starting value) | 100% (starting value) | 100% (starting value) |
   | **Split** | 3–4 | 30 fps | Lumen Lite (Medium GI) | Medium (starting value) | 50% (starting value) | 70% (starting value) |

7. Tier rules:
   - Each machine picks its tier from its own local viewport count, not from the session's total player count. A LAN client with 1 local player uses the High tier even when the host has 3.
   - The tier changes whenever a local player joins or leaves and the count crosses between 2 and 3 (`specs/multiplayer/multiplayer.md` Requirements 4 and 8).
   - Foliage density scales only cosmetic PCG placement (Requirement 3, PCG Framework row). Trees, forage nodes, loose pickups, rocks with collision, and every other placement that has gameplay state or collision are identical in both tiers.

### Nanite Authoring Rules
8. Nanite content follows these rules. `NamecNaniteAuthoringValidator` fails the content build for any Nanite static mesh that breaks rule 3, 4, 5, or 6.
   1. **No masked card foliage.** Leaves, needles, and grass on Nanite meshes are real geometry. Masked materials are expensive on Nanite, masked-out pixels cost nearly as much as drawn pixels, and card foliage can be slower than non-Nanite.
   2. **Vertex:triangle ratio.** A vertex:triangle ratio of 2:1 or more (faceted normals from hard edges or split UVs) is a Nanite problem, so meshes are authored below 2:1 with smoothed normals where the surface allows. Closely stacked kitbash surfaces are avoided, because they cause overdraw.
   3. **Clamped WPO.** Every World Position Offset material on a Nanite mesh sets Max WPO Displacement, sets a WPO Disable Distance, and sets the mesh's Shadow Cache Invalidation Behavior, because WPO invalidates cached VSM pages every frame. Pixel Depth Offset invalidates VSM the same way and is not used on Nanite meshes.
   4. **No translucency on Nanite.** Nanite supports only Opaque and Masked materials. Translucent parts (glass, water surfaces, ice) are separate non-Nanite meshes, and translucent Mesh Decals are not used on Nanite meshes. Projected decals are allowed.
   5. **No Lighting Channels on Nanite.** Lighting Channels are not supported for Nanite meshes, so no Nanite mesh uses them.
   6. **No Nanite Tessellation or displacement.** It is Experimental (Requirement 2).
   7. **Instance cap.** Nanite has a hard cap of 16 million streamed-in instances, counting all instances, not just Nanite ones. PCG placement density, the streaming radius per player (`DT_World_Streaming`), and 4 streaming sources (`specs/multiplayer/multiplayer.md` Requirement 10) together keep streamed-in instances below the instance budget in `DT_World_Streaming` (starting value 12,000,000).

### Terrain Rendering Path
9. Voxel terrain chunks are not Nanite: no engine path builds Nanite data at runtime (research file, Nanite plan N2; this is absence of evidence, not an Epic statement). Terrain follows `specs/voxel-world/voxel-world.md` Requirement 2:
   - Chunks render as non-Nanite meshes, lit by the tier's GI method (Requirement 6): Lumen HWRT in the High tier and Lumen Lite in the Split tier, or the GI method a Requirement 13 fallback step switched the tier to. The research file does not say whether Lumen Lite lights dynamic-mesh geometry without distance fields, so the benchmark verifies terrain lighting in the Split tier too (Requirement 11).
   - Each chunk's bounds fit its extracted surface tightly, because re-meshing a chunk invalidates the VSM pages that overlap its bounds, and non-Nanite geometry does not get the Nanite skip for those invalidations.
   - Chunks use the regular VSM CPU shadow culling for non-Nanite geometry.
   - Nothing relies on distance fields. Dynamic Mesh distance field support is deprecated (5.6), so neither Lumen software ray tracing nor Distance Field Shadows beyond the dynamic shadow distance can see terrain. When far terrain shadows are needed, far VSM rendering (`r.Shadow.Virtual.UseFarShadowCulling 0`) is used, and the benchmark measures its cost.
   - Chunk count and material count per chunk drive CPU draw call cost, so the benchmark records draw calls with the terrain loaded.

### Console Readiness
10. Console-readiness constraints (console builds are out of scope):
    - Every Nanite mesh has a usable non-Nanite fallback mesh, rendered on platforms without Nanite (`r.Nanite.ProxyRenderMode`). Epic warns that dense instance scenes can produce an overwhelming number of fallback draw calls and that automatic compatibility may not be feasible, so cosmetic PCG density is a tier setting (Requirement 6), not a fixed value.
    - MegaLights is not supported on Switch, so no system requires MegaLights, and it stays on the Evaluate list (Requirement 5).
    - Lumen Lite targets Switch 2, handhelds, and medium-spec PC at 60 fps, so the handheld tier starts from the Split tier settings. 5.8 also improved Nanite rasterization and culling on handheld platforms, which are not named.
    - On a platform or GPU without HWRT, the GI fallback is Lumen at Low scalability (Lumen off) with a skylight, because dynamic-mesh terrain has no distance fields for software ray tracing.

### Benchmark Milestone
11. The benchmark milestone is the first required milestone in the roadmap. No content production milestone (shipped art, animation, region, enemy, or town content beyond the benchmark scene's placeholder assets) starts until the results of a passing run are recorded. A failed milestone (Requirement 13) keeps content production blocked until the resulting spec change is made and a run passes. The milestone is:
    - **Scene:** `L_Benchmark_SplitScreen`, run on the reference PC (`specs/game-foundation/game-foundation.md` Requirement 13) at 1080p, containing:
      - custom voxel terrain chunks (`specs/voxel-world/voxel-world.md` Requirements 1–4) being edited: each character applies one dig or fill edit every 2 seconds (starting value), so chunks re-mesh during the run;
      - a base of 200 Nanite building pieces (starting value), including one Nanite Geometry Collection piece destroyed during the run;
      - Nanite rocks and classic Nanite trees at the Temperate region's PCG placement density, including one tree felled during the run;
      - 4 animated characters built with Mutable, animated with Motion Matching and the IK Retargeter, one of them in first-person view with First Person Rendering.
    - **Run:** `ANamecBenchmarkDirector` drives scripted camera paths, movement, and edits for 120 seconds (starting value), at 1, 2, 3, and 4 local viewports, each with its Requirement 6 tier.
    - **Measurements** at each viewport count: average frame rate and 1% low frame rate, `stat gpu`, `NaniteStats`, VSM page invalidation, and draw calls.
    - **Checks** at each viewport count: terrain chunks are lit correctly after every re-mesh (no unlit or black chunks), under the GI method each tier uses in that run (Requirements 6 and 13).
    - **Pass:** the average frame rate over the run is at least 60 fps at 1 and 2 viewports and at least 30 fps at 3 and 4 viewports.
    - **Record:** the results table for 1, 2, 3, and 4 viewports, the tier settings used, and the pass or fail result are recorded in `specs/engine-tech/engine-tech-roadmap.md` (created by the Researcher). When the project moves to a new 5.8 hotfix, the benchmark runs again and the new results are recorded before the hotfix is merged.
12. After a passing run, `DT_MP_SplitScreenScalability` is tuned from the recorded results: each tier gets the highest settings that still pass at every viewport count it covers. The provisional (Evaluate) rows of Requirement 3 keep their primary path when the run passes with them on. The keep-primary or fallback decision for each Requirement 3 (Evaluate) row is recorded in `specs/engine-tech/engine-tech-roadmap.md` (created by the Researcher).
13. When a target is missed, fallbacks are applied one step at a time, in this order, and the benchmark runs again after each step until every target passes. Each step applies only to the tier that failed.
    1. Lower foliage density and view distance.
    2. Lower VSM quality by one level, and shorten the WPO Disable Distance on trees and foliage.
    3. Switch the tier's GI one step down: High tier from Lumen HWRT High to Lumen Lite; Split tier from Lumen Lite to Lumen at Medium scalability, then to Low scalability (Lumen off) with a skylight.
    4. Replace classic Nanite trees and foliage with non-Nanite foliage with LODs and impostors.
    5. Replace Nanite Geometry Collection fracture with authored damage-state mesh swaps.
    6. Replace First Person Rendering with its fallback.
    
    When every step is applied and a target is still missed, the milestone fails, and the targets in `specs/game-foundation/game-foundation.md` Requirement 13 or the tier split go back to the user as a spec change.

### Tuning Data
14. Every value marked "tuning value" or "starting value" in this spec that names no other table lives in `DT_Engine_Benchmark`.

## Data Flow
1. A local player joins or leaves (`specs/multiplayer/multiplayer.md` Requirements 4 and 8) → `UNamecSplitScreenScalabilitySubsystem` counts the machine's local viewports → selects the High or Split row of `DT_MP_SplitScreenScalability` → applies the GI method, VSM quality, and view distance to the engine scalability settings → regenerates cosmetic PCG placement in loaded chunks at the tier's foliage density. Gameplay placements are not touched.
2. A terrain edit replicates (`specs/voxel-world/voxel-world.md` Data Flow 4) → the client re-meshes the chunk with tightly fitted bounds → the new non-Nanite chunk mesh updates in the ray tracing scene for Lumen HWRT → VSM invalidates only the pages overlapping the chunk bounds.
3. A tree reaches 0 health (`specs/voxel-world/voxel-world.md` Data Flow 5) → the server replaces the tree instance with a replicated simulating actor using the same Nanite static mesh → every client renders the falling mesh (Nanite unless Requirement 13 step 4 replaced Nanite trees).
4. A building piece reaches 0 health → the server replicates the destroyed state → each client plays the local Nanite Geometry Collection fracture, or the damage-state fallback (Requirement 3).
5. The Researcher runs `L_Benchmark_SplitScreen` → `ANamecBenchmarkDirector` reads run parameters from `DT_Engine_Benchmark` → runs the scripted run at 1, 2, 3, and 4 viewports → writes the measurements per viewport count to a results file under `Saved/Benchmark/` → the Researcher records the results table in `specs/engine-tech/engine-tech-roadmap.md` (created by the Researcher) → tunes `DT_MP_SplitScreenScalability` (Requirement 12) or applies the Requirement 13 fallbacks.

## Edge Cases
1. When a machine goes from 2 to 3 local viewports, it switches to the Split tier at once, and switches back to the High tier when a local player leaves and the count drops to 2.
2. When the tier changes foliage density, every tree, forage node, loose pickup, and collidable rock keeps its position, harvest state, and collected state.
3. When two machines in a LAN session use different tiers, gameplay is identical on both. Only cosmetic placement density and rendering quality differ.
4. When a player's GPU does not support hardware ray tracing, GI falls back to Lumen at Low scalability (Lumen off) with a skylight (Requirement 10).
5. When a Nanite mesh is assigned a translucent material, the engine renders a default material and logs a warning, and `NamecNaniteAuthoringValidator` fails the content build (Requirement 8).
6. When many chunks re-mesh in the same frame (4 players digging), each re-mesh invalidates only the VSM pages inside that chunk's bounds, and the benchmark's per-player edit rate measures this case.
7. When an Evaluate feature fails its criterion, its fallback stays the shipped path, the result is recorded in the roadmap, and the feature stays on the Evaluate list.
8. When a later 5.8 hotfix changes the status of a feature in this spec, this spec is updated from new research before the hotfix is merged.

## Acceptance Criteria
- [ ] `NAMEC.uproject` names Unreal Engine 5.8, the project builds on 5.8.2, and no plugin for a Requirement 4 feature is enabled.
- [ ] No core system uses a feature that is Experimental in 5.8.
- [ ] `specs/engine-tech/engine-tech-roadmap.md` (created by the Researcher) holds a passing benchmark run's results for 1, 2, 3, and 4 viewports, with average and 1% low frame rate, `stat gpu`, `NaniteStats`, VSM page invalidation, and draw calls, before any content production milestone starts.
- [ ] On the reference PC, the benchmark averages at least 60 fps at 1 and 2 viewports and at least 30 fps at 3 and 4 viewports while all 4 characters edit terrain.
- [ ] Terrain chunks re-meshed during the benchmark are lit correctly by the GI method each tier uses in that run (Lumen HWRT in the High tier unless a Requirement 13 fallback step replaced it), with no unlit chunks.
- [ ] Adding a third local player switches that machine to the Split tier, and removing it switches back to the High tier.
- [ ] Switching tiers does not move or change any tree, forage node, or loose pickup.
- [ ] A felled tree falls as the standing tree's own static mesh on a simulating actor (Nanite unless Requirement 13 step 4 was applied), and every LAN client sees it fall.
- [ ] With Nanite fallback rendering forced (`r.Nanite.ProxyRenderMode`), every building piece, rock, and tree renders a fallback mesh.
- [ ] A content build with a Nanite static mesh that uses a translucent material, a Lighting Channel, or an unclamped WPO material fails validation.
- [ ] Each Requirement 5 Evaluate feature has a recorded criterion result in the roadmap before any system uses it, and each Requirement 3 (Evaluate) row has its recorded keep-primary or fallback decision from the benchmark (Requirement 12).
- [ ] A dye color applied at a Dye Station shows through the Substrate material parameter on every body variant.

## Key Files
- `research/unreal-5.6-5.8-features.md` — existing; evidence source for every engine claim in this spec (REFERENCE).
- `specs/engine-tech/engine-tech-roadmap.md` — new, created by the Researcher (not present until the Researcher writes the roadmap); benchmark results table, tier settings used, fallback steps applied, Requirement 3 (Evaluate) row decisions, and Evaluate criterion results (Requirements 5 and 11–13).
- `NAMEC.uproject` — changed; engine version 5.8 and enabled plugins for every Requirement 3 feature that ships as a plugin (`specs/game-foundation/game-foundation.md` Requirement 1).
- `Config/DefaultEngine.ini` — changed; renderer settings for Lumen with hardware ray tracing, Virtual Shadow Maps, Substrate, and Nanite.
- `Source/NAMEC/Multiplayer/NamecSplitScreenScalabilitySubsystem.h` — new; selects and applies the High or Split tier from the machine's local viewport count, and regenerates cosmetic PCG placement at the tier's foliage density (Requirements 6–7).
- `Source/NAMEC/Core/Benchmark/NamecBenchmarkDirector.h` — new; `ANamecBenchmarkDirector`, the scripted benchmark run at 1–4 viewports, measurement capture, and results file output (Requirement 11).
- `Source/NAMECEditor/NamecNaniteAuthoringValidator.h` — new; editor-only data validator for Nanite authoring rules 3–6 (Requirement 8).
- `Content/Maps/Benchmark/L_Benchmark_SplitScreen.umap` — new; benchmark scene (Requirement 11).
- `Content/World/PCG/` — new; runtime seeded PCG graphs for trees, forage nodes, loose pickups, rocks, props, and cosmetic vegetation (Requirement 3).
- `Content/Data/DT_MP_SplitScreenScalability.uasset` — changed; High and Split tier rows (GI method, VSM quality, foliage density, view distance) (Requirement 6).
- `Content/Data/DT_World_Streaming.uasset` — changed; adds the streamed-in instance budget (Requirement 8).
- `Content/Data/DT_Engine_Benchmark.uasset` — new; benchmark run length, per-character edit interval, building piece count, pieces destroyed in the Chaos Destruction replication evaluation (Requirement 5), and other benchmark values (Requirement 14).
