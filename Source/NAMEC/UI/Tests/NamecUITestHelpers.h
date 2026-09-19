#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Blueprint/UserWidget.h"
#include "CommonInputSettings.h"
#include "ICommonInputModule.h"

namespace NamecUITestHelpers
{
    // In the game UCommonInputSubsystem::Initialize does this for each local player; a test has
    // none, and the first Back handler would otherwise trip ensure(bInputDataLoaded).
    inline void LoadCommonInputData()
    {
        ICommonInputModule::GetSettings().LoadData();
    }

    // UMG keeps only a weak pointer to a widget's Slate tree. In the game the viewport owns the
    // strong one; a test holds this instead, or a menu stack is destroyed the moment it is built.
    inline TSharedRef<SWidget> BuildSlate(UUserWidget& Widget)
    {
        LoadCommonInputData();
        return Widget.TakeWidget();
    }
}

#endif
