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
<!-- one line per filter this task adds, e.g. -->
Tools\Build\automation.bat <checkout> Namec.Foundation.<Area> <sha>
```

The wrapper runs `UnrealEditor-Cmd.exe -ExecCmds="Automation RunTests <filter>;Quit"` with `-nullrhi`.
Unreal 5.8 has no `-run=AutomationTest` commandlet. Measured over SSH on 2026-09-19:

- A passing filter exits 0 and writes `Saved\Automation\<sha>\index.json`.
- A filter matching no test exits 255 and writes no report. The engine logs
  `No automation tests matched '<filter>'`.
- Read the exit code from the SSH exit status, or with `cmd /v:on /c "... & echo !ERRORLEVEL!"`.
  `& echo %ERRORLEVEL%` on the same line is expanded before the run and always prints 0.

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

## OPEN PIE

Steps to open the map and verify this task's behavior end-to-end:

1. Open the project in the editor.
2. Open map: [specify path, e.g. Content/Maps/VMA_Benchmark.umap]
3. Set Play settings: Number of Players = [N], Net Mode = [Standalone | Listen Server | Client]
4. Run these console commands in PIE: [list commands]
5. Expected result: [observable outcome that constitutes a pass]

## PASTE BACK

What to copy from Windows back to the container after each proving run (for the handover
record and for the Researcher/Director to read):
- UBT log: last 20 lines of Saved/Logs/UBT-<sha>.log and the exit code
- Automation report: pass/fail line from Saved/Automation/<sha>/index.json (or equivalent)
- Commandlet log: last 20 lines of Saved/Logs/Commandlet-<sha>.log and exit code
- PIE console: paste or screenshot of the console output for each proving command run

## SSH IDENTITY

Pipeline SSH account: ashar (key ~/.ssh/id_ed25519_refpc, alias refpc).
⛔ Never launch two reference-PC builds concurrently from the container — the
cross-account WaitMutex does not queue; a second build fails immediately.

---
*Fill this template before submitting the branch for review.*
*Post-clone instruction: `git config core.hooksPath Tools/Hooks`*
