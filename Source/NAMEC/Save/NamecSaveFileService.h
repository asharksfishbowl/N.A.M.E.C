#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
#include "NamecSaveFileService.generated.h"

class UNamecVersionedSave;

enum class ENamecSaveWriteResult : uint8
{
    Ok,
    DiskFull,
    WriteFailed,
};

enum class ENamecSaveLoadResult : uint8
{
    Ok,
    Missing,
    Corrupt,
    NewerVersion,
};

NAMEC_API const TCHAR* LexToString(ENamecSaveWriteResult Result);

struct FNamecSaveLoadOutcome
{
    ENamecSaveLoadResult Result = ENamecSaveLoadResult::Missing;

    // Set only when Result is Ok. The caller keeps it alive in a UPROPERTY.
    UNamecVersionedSave* Save = nullptr;
};

UCLASS()
class NAMEC_API UNamecSaveFileService : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // On any result but Ok the old file is untouched. SaveVersion is stamped first: an object
    // this build writes is at this build's version, whether or not the write then succeeds.
    ENamecSaveWriteResult Write(UNamecVersionedSave& Save, const FString& FileName) const;

    // Never rewrites, renames or deletes the file, whatever the result.
    FNamecSaveLoadOutcome Load(const FString& FileName, TSubclassOf<UNamecVersionedSave> SaveClass) const;

    // Bare names of the `<Prefix>*.sav` files in the save directory, sorted. Temp and .bak files never match.
    TArray<FString> FindSaveFiles(const FString& Prefix) const;

    static FString GetSaveFilePath(const FString& FileName);

    static FString GetTempFilePath(const FString& FileName);

    static FString GetBakFilePath(const FString& FileName);

    // Sets a file aside as `<file>.bak`, replacing an older one. Load never does this itself.
    bool MoveToBak(const FString& FileName) const;
};
