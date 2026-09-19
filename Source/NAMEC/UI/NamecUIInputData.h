#pragma once

#include "CoreMinimal.h"
#include "CommonInputBaseTypes.h"
#include "CommonUITypes.h"
#include "NamecUIInputData.generated.h"

// A DT_UI_InputActions row. The base struct's two key fields are protected, and a commandlet
// authors the table in C++, so this subclass exists to reach them.
USTRUCT(BlueprintType)
struct NAMEC_API FNamecUIInputActionRow : public FCommonInputActionDataBase
{
    GENERATED_BODY()

    FNamecUIInputActionRow() = default;
    FNamecUIInputActionRow(const FText& InDisplayName, const FKey& KeyboardKey, const FKey& GamepadKey);
};

// Names the Click and Back rows of DT_UI_InputActions for Common UI
// (Config/DefaultGame.ini [/Script/CommonInput.CommonInputSettings] InputData).
UCLASS()
class NAMEC_API UNamecUIInputData : public UCommonUIInputData
{
    GENERATED_BODY()

public:
    static const TCHAR* const TablePackageName;
    static const FName ClickRowName;
    static const FName BackRowName;

    UNamecUIInputData();
};
