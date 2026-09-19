#include "UI/NamecBackableScreen.h"

UNamecBackableScreen::UNamecBackableScreen()
{
    bIsBackHandler = true;
}

void UNamecBackableScreen::GoBack()
{
    DeactivateWidget();
}

bool UNamecBackableScreen::NativeOnHandleBackAction()
{
    GoBack();
    return true;
}
