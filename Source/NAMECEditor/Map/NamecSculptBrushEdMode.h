#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"
#include "NamecSculptBrushEdMode.generated.h"

// Phase 4 — stub only. Editor sculpt brush mode for map authoring.
UCLASS()
class NAMECEDITOR_API UNamecSculptBrushEdMode : public UEdMode
{
    GENERATED_BODY()

public:
    static const FEditorModeID EM_NamecSculptBrush;
};
