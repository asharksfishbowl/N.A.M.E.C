#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "NamecMenuButton.generated.h"

class UTextBlock;

// The concrete Common UI button every Phase 2 screen uses. Its label is built in C++ until the
// phase that ships UI art replaces it with a Widget Blueprint (research 0919-2 §3.2).
UCLASS()
class NAMEC_API UNamecMenuButton : public UCommonButtonBase
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    void SetLabel(const FText& InLabel);
    FText GetLabel() const;

private:
    UPROPERTY()
    TObjectPtr<UTextBlock> Label;
};

namespace NamecMenuNavigation
{
    // Explicit Up and Down rules through Entries in order, wrapping. Pass only entries that can
    // take focus, so a disabled one is never a stop.
    NAMEC_API void LinkVertically(TConstArrayView<UWidget*> Entries);

    // Where a screen puts focus when it activates. Null when every button is disabled.
    NAMEC_API UNamecMenuButton* FindFirstEnabled(TConstArrayView<TObjectPtr<UNamecMenuButton>> Buttons);
}
