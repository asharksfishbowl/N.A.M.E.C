# Handover Note — Project skeleton, engine config, repository guards, handover template, and header stubs

## ASSUMED ENGINE CLASSES AND FUNCTIONS

| Class / Function | Doc URL |
|---|---|
| UPrimaryDataAsset | https://docs.unrealengine.com/5.8/en-US/API/Runtime/Engine/Engine/UPrimaryDataAsset/ |
| UGameInstanceSubsystem | https://docs.unrealengine.com/5.8/en-US/API/Runtime/Engine/Subsystems/UGameInstanceSubsystem/ |
| UWorldSubsystem | https://docs.unrealengine.com/5.8/en-US/API/Runtime/Engine/Subsystems/UWorldSubsystem/ |
| ACharacter | https://docs.unrealengine.com/5.8/en-US/API/Runtime/Engine/GameFramework/ACharacter/ |
| UCommandlet | https://docs.unrealengine.com/5.8/en-US/API/Runtime/Engine/Commandlets/UCommandlet/ |
| UEdMode (FEdMode successor) | https://docs.unrealengine.com/5.8/en-US/API/Editor/UnrealEd/UEdMode/ |
| UEditorValidatorBase | https://docs.unrealengine.com/5.8/en-US/API/Editor/DataValidation/UEditorValidatorBase/ |
| Substrate CVars | See research/unreal-5.6-5.8-features.md §1.5 |

## COMPILE COMMAND

```
UnrealBuildTool.exe NAMEC Win64 Development NAMEC.uproject
```

## AUTOMATION TESTS

None added by this task.

## COMMANDLETS

None added by this task.

## PROVING COMMAND

```
UnrealBuildTool.exe NAMEC Win64 Development
```

Zero compile errors confirms the include graph and module dependencies are sound.
Every sub-branch task cuts from the parent **only after this passes**.

## ASSETS NEEDED

None — this task is pure text files.

## POST-CLONE INSTRUCTION

```
git config core.hooksPath Tools/Hooks
```

Run this after cloning to activate the 20 MB file-size guard.

---

## SSH COMPILE COMMAND

```
SHA=$(git rev-parse --short HEAD)
ssh -i /var/lib/dex/ssh/namec_ref_pc ashar@host.docker.internal \
  powershell -NoProfile -ExecutionPolicy Bypass -File \
  "C:\Users\ashar\repos\N.A.M.E.C\Tools\Build\check-editor.ps1"

ssh -i /var/lib/dex/ssh/namec_ref_pc ashar@host.docker.internal \
  "C:\Users\ashar\repos\N.A.M.E.C\Tools\Build\ubt.bat" \
  "C:\Users\ashar\repos\N.A.M.E.C" NAMECEditor Win64 Development $SHA

ssh -i /var/lib/dex/ssh/namec_ref_pc ashar@host.docker.internal \
  "C:\Users\ashar\repos\N.A.M.E.C\Tools\Build\ubt.bat" \
  "C:\Users\ashar\repos\N.A.M.E.C" NAMEC Win64 Development $SHA
```

Log files on Windows: `Saved/Logs/UBT-<sha>.log`

**NOTE:** Windows Smart App Control (SAC) blocks UBT from loading NAMECModuleRules.dll (0x800711C7). Disable SAC first: Windows Security > App & browser control > Smart App Control > Off. Then re-run the build.

## MODULE NAMES TO CONFIRM

| Module in Build.cs | Engine .uplugin Modules[].Name (SSH read) | Match? |
|---|---|---|
| PCGFramework (was) → **PCG** | PCG (from PCG.uplugin at Engine/Plugins/PCG/) | ✓ Fixed |
| MutableRuntime | MutableRuntime (from Mutable.uplugin at Engine/Plugins/Mutable/) | ✓ Correct |
| MotionMatchingAnimGraphNodes (was) → **PoseSearch** | PoseSearch (from PoseSearch.uplugin at Engine/Plugins/Animation/PoseSearch/) | ✓ Fixed |
| StateTreeModule | StateTreeModule (from StateTree.uplugin at Engine/Plugins/Runtime/StateTree/) | ✓ Correct |
| SmartObjectsModule | SmartObjectsModule (from SmartObjects.uplugin at Engine/Plugins/Runtime/SmartObjects/) | ✓ Correct |
| IKRig | IKRig (from IKRig.uplugin at Engine/Plugins/Animation/IKRig/) | ✓ Correct |
| MetasoundEngine | MetasoundEngine (from Metasound.uplugin at Engine/Plugins/Runtime/Metasound/) | ✓ Correct |
| DataValidationModule (was) → **DataValidation** | DataValidation (from DataValidation.uplugin at Engine/Plugins/Editor/DataValidation/) | ✓ Fixed (NAMECEditor.Build.cs) |
| OnlineSubsystem (added) | OnlineSubsystem (from OnlineSubsystem.uplugin at Engine/Plugins/Online/OnlineSubsystem/) | ✓ Added |
| OnlineSubsystemNull (added) | OnlineSubsystemNull (from OnlineSubsystemNull.uplugin at Engine/Plugins/Online/OnlineSubsystemNull/) | ✓ Added |

.uproject plugin name corrections (folder name, not module name):
- PCGFramework → PCG | MotionMatchingAnimGraphNodes → PoseSearch
- SmartObjectsModule → SmartObjects | MetaSounds → Metasound

## IRIS PLUGIN FOLDER

Path: `C:\Program Files\Epic Games\UE_5.8\Engine\Plugins\Experimental\Iris\`
Module: Iris | IsBetaVersion: true | IsExperimentalVersion: false

## CONTEXTUAL ANIMATION STATUS

Path: `C:\Program Files\Epic Games\UE_5.8\Engine\Plugins\Experimental\Animation\ContextualAnimation\`
IsExperimentalVersion: **true** | IsBetaVersion: false

## UEditorValidatorBase SIGNATURE

Header: `Engine/Plugins/Editor/DataValidation/Source/DataValidation/Public/EditorValidatorBase.h`

UE 5.8 (current) signatures:
```cpp
// In protected:
virtual bool CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject,
    FDataValidationContext& InContext) const;
virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData,
    UObject* InAsset, FDataValidationContext& Context);
```

Old signatures (deprecated since 5.4 — removed from our header):
```cpp
virtual bool CanValidateAsset_Implementation(UObject* InAsset) const;
virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset,
    TArray<FText>& ValidationErrors);
```

FDataValidationContext is forward-declared in EditorValidatorBase.h — no extra include needed.

## PRE-COMMIT HOOK RESULT

Staged-delete test (2026-09-17): hook exits 0 for a deletion. PASS.
hooksPath set: `git config core.hooksPath Tools/Hooks`

## COMPILE RESULTS (2026-09-17)

NAMECEditor Win64 Development — exit 0 (SHA c889819)
NAMEC Win64 Development — exit 0 (SHA c889819)
Only warnings: C4996 deprecated APawn::GetMovementBase in engine header (engine-internal, not our code).

## OSS NULL SESSION TEST

Pending — compile now passes; OSS Null session test deferred to Task 2 (OPEN PIE step 3).

## OPEN PIE

1. Open the project in the editor (after compile passes).
2. Verify the project loads without errors (no missing module warnings).
3. Run: `NamecScalability.LogActiveTier` in the PIE console (Task 2 subsystem proves OSS Null is wired).
4. Expected: editor opens without crashing; no plugin load failures in the output log.

## PASTE BACK

After compile passes on Windows:
- UBT log: last 20 lines of `Saved/Logs/UBT-<sha>.log` and exit code → paste here
- Game target: last 20 lines of `Saved/Logs/UBT-<sha>-game.log` and exit code → paste here
