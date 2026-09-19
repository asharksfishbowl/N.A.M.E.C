#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

// Phase 2 screens build their widget tree in C++ (research 0919-2 §3.2). A user widget creates its
// tree inside Super::Initialize, and UCommonButtonBase wraps the root there too, so the root has
// to exist before Super runs. Call this first in Initialize and build only when it returns a tree.
namespace NamecCppWidgetTree
{
    inline UWidgetTree* BeginBuild(UUserWidget& Widget)
    {
        if (!Widget.WidgetTree)
        {
            Widget.WidgetTree = NewObject<UWidgetTree>(&Widget, TEXT("WidgetTree"), RF_Transient);
        }
        return Widget.WidgetTree->RootWidget ? nullptr : Widget.WidgetTree.Get();
    }
}
