#include "Multiplayer/NamecSessionRules.h"
#include "Core/Tests/NamecTestFlags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSessionMaxPlayersTest, "Namec.Foundation.Session.MaxPlayersIsTheTableValueUnderTheHardCap", NamecFoundationTestFlags)

bool FNamecSessionMaxPlayersTest::RunTest(const FString& Parameters)
{
    FNamecSessionRow Row;
    Row.MaxPlayers = 6;
    TestEqual(TEXT("A table value of 6 still yields 4"), NamecSessionRules::GetMaxPlayers(Row), 4);
    Row.MaxPlayers = 2;
    TestEqual(TEXT("A table value of 2 yields 2"), NamecSessionRules::GetMaxPlayers(Row), 2);
    Row.MaxPlayers = -3;
    TestEqual(TEXT("A negative table value yields 0"), NamecSessionRules::GetMaxPlayers(Row), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSessionLocalSlotsTest, "Namec.Foundation.Session.LocalSlotsAreTheRemainingCapacity", NamecFoundationTestFlags)

bool FNamecSessionLocalSlotsTest::RunTest(const FString& Parameters)
{
    const FNamecSessionRow Row;
    TestEqual(TEXT("An empty session leaves four local slots"), NamecSessionRules::GetMaxLocalPlayers(Row, 0), 4);
    TestEqual(TEXT("Three players already in the session leaves one"), NamecSessionRules::GetMaxLocalPlayers(Row, 3), 1);
    TestEqual(TEXT("Four leaves none"), NamecSessionRules::GetMaxLocalPlayers(Row, 4), 0);
    TestEqual(TEXT("More than the cap never goes below 0"), NamecSessionRules::GetMaxLocalPlayers(Row, 7), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSessionTableTest, "Namec.Foundation.Session.ShippedTableHoldsTheStartingValues", NamecFoundationTestFlags)

bool FNamecSessionTableTest::RunTest(const FString& Parameters)
{
    const TOptional<FNamecSessionRow> Row = NamecSessionRules::LoadRow();
    if (TestTrue(TEXT("DT_MP_Session row"), Row.IsSet()))
    {
        TestEqual(TEXT("MaxPlayers"), Row->MaxPlayers, 4);
        TestEqual(TEXT("ConnectionTimeoutSeconds"), Row->ConnectionTimeoutSeconds, 20.f);
    }
    return true;
}

#endif
