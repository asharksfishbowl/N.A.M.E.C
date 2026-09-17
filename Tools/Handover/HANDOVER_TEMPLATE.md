# Handover Note — [TASK TITLE]

## ASSUMED ENGINE CLASSES AND FUNCTIONS

| Class / Function | Doc URL |
|---|---|
| <!-- e.g. UPrimaryDataAsset | https://docs.unrealengine.com/5.8/en-US/API/Runtime/Engine/Engine/UPrimaryDataAsset/ --> |

List every UE class or function called by name in this task. Each entry must include the
documentation page URL from research/unreal-5.6-5.8-features.md.

## COMPILE COMMAND

```
UnrealBuildTool.exe NAMEC Win64 Development NAMEC.uproject
```

Replace with the exact command used. Include any flags required.

## AUTOMATION TESTS

```
<!-- exact -run=AutomationTest commands for any automation tests added by this task -->
```

Leave blank if this task adds no automation tests.

## COMMANDLETS

```
<!-- exact -run= commands for commandlet tasks introduced by this task -->
```

Leave blank if this task adds no commandlets.

## PROVING COMMAND

The single command or test that proves this task's core claim:

```
<!-- e.g. UnrealBuildTool.exe NAMEC Win64 Development — zero compile errors -->
```

Describe the expected output that constitutes a pass.

## ASSETS NEEDED

| Asset Name | Content/ Path | Source |
|---|---|---|
| <!-- e.g. VMA_Namec | Content/Map/Baked/VMA_Namec.uasset | Task 3 bake commandlet --> |

List every .uasset the task references but does not create. Leave blank if none.

---
*Fill this template before submitting the branch for review.*
*Post-clone instruction: `git config core.hooksPath Tools/Hooks`*
