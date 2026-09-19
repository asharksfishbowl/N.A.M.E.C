#include "Multiplayer/NamecSessionRules.h"
#include "Core/NamecDataTableRows.h"
#include "Save/NamecSettingsSave.h"

const TCHAR* const NamecSessionRules::TablePackageName = TEXT("/Game/Data/DT_MP_Session");
const FName NamecSessionRules::RowName(TEXT("Default"));

TOptional<FNamecSessionRow> NamecSessionRules::LoadRow()
{
    return NamecDataTableRows::LoadRow<FNamecSessionRow>(TablePackageName, RowName);
}

int32 NamecSessionRules::GetMaxPlayers(const FNamecSessionRow& Row)
{
    return FMath::Clamp(Row.MaxPlayers, 0, HardMaxPlayers);
}

int32 NamecSessionRules::GetMaxLocalPlayers(const FNamecSessionRow& Row, int32 PlayersInSession)
{
    return FMath::Clamp(GetMaxPlayers(Row) - PlayersInSession, 0, NamecLocalPlayerSlotCount);
}
