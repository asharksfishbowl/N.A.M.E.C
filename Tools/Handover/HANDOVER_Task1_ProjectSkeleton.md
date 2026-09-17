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
