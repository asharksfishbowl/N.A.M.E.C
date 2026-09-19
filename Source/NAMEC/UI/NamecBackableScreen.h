#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NamecBackableScreen.generated.h"

// A menu screen that Common UI's Back action closes. Closing deactivates it, which returns its
// stack to the screen underneath.
UCLASS(Abstract)
class NAMEC_API UNamecBackableScreen : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UNamecBackableScreen();

    virtual void GoBack();

protected:
    virtual bool NativeOnHandleBackAction() override;
};
