#include "Core/Platform/INamecPlatform.h"
#include "Core/Platform/NamecDesktopPlatform.h"

namespace
{
    TSharedRef<INamecPlatform>& GetInstalledPlatform()
    {
        static TSharedRef<INamecPlatform> Installed = MakeShared<FNamecDesktopPlatform>();
        return Installed;
    }
}

INamecPlatform& FNamecPlatform::Get()
{
    return GetInstalledPlatform().Get();
}

TSharedRef<INamecPlatform> FNamecPlatform::Set(TSharedRef<INamecPlatform> NewPlatform)
{
    TSharedRef<INamecPlatform> Previous = GetInstalledPlatform();
    GetInstalledPlatform() = NewPlatform;
    return Previous;
}
