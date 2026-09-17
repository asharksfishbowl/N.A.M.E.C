#include "Map/NamecMapBakeCommandlet.h"

UNamecMapBakeCommandlet::UNamecMapBakeCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

int32 UNamecMapBakeCommandlet::Main(const FString& Params)
{
    return 0;
}
