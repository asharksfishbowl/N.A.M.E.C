#include "NamecCreateInputAssetsCommandlet.h"
#include "NamecAssetAuthoring.h"
#include "Core/Input/NamecBaseInputBindings.h"
#include "Core/Input/NamecInputContextBuilder.h"
#include "InputAction.h"
#include "InputMappingContext.h"

int32 UNamecCreateInputAssetsCommandlet::Main(const FString& Params)
{
    // Actions first: a context refers to them.
    bool bOk = CreateActions();
    bOk = bOk && CreateContext(ENamecInputDevice::Gamepad);
    bOk = bOk && CreateContext(ENamecInputDevice::KeyboardMouse);
    if (!bOk)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecCreateInputAssets: one or more assets failed to save."));
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("NamecCreateInputAssets: all assets written successfully."));
    return 0;
}

bool UNamecCreateInputAssetsCommandlet::CreateActions()
{
    bool bOk = true;
    for (const FNamecBaseAction& BaseAction : NamecBaseInput::GetActions())
    {
        UInputAction* Action = NamecAssetAuthoring::NewAssetInPackage<UInputAction>(NamecBaseInput::GetActionPackageName(BaseAction.ActionName));
        Action->ValueType = BaseAction.ValueType;
        bOk &= NamecAssetAuthoring::SaveAsset(*Action);
    }
    return bOk;
}

bool UNamecCreateInputAssetsCommandlet::CreateContext(ENamecInputDevice Device)
{
    UInputMappingContext* Context = NamecAssetAuthoring::NewAssetInPackage<UInputMappingContext>(NamecBaseInput::GetContextPackageName(Device));
    for (const FNamecBaseBinding& Binding : NamecBaseInput::GetBindings(Device))
    {
        if (!NamecInputContextBuilder::AddBaseBinding(*Context, Binding))
        {
            UE_LOG(LogTemp, Error, TEXT("NamecCreateInputAssets: IA_%s did not load for binding %s."), *Binding.ActionName.ToString(), *Binding.BindingName.ToString());
            return false;
        }
    }
    return NamecAssetAuthoring::SaveAsset(*Context);
}
