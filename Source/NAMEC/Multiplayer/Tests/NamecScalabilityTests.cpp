#include "Multiplayer/NamecScalabilitySubsystem.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Save/Tests/NamecSaveTestHelpers.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    FNamecMachineSettings SpecExampleUserSettings()
    {
        FNamecMachineSettings User;
        User.GIQuality = 2;
        User.ShadowQuality = 3;
        User.FoliageDensityPercent = 30.f;
        User.ViewDistancePercent = 100.f;
        return User;
    }

    void TestAppliedEquals(FAutomationTestBase& Test, const FString& What, const FNamecAppliedGraphics& Applied, int32 GI, int32 Shadow, float Foliage, float View)
    {
        Test.TestEqual(What + TEXT(": GI"), Applied.GIQuality, GI);
        Test.TestEqual(What + TEXT(": VSM"), Applied.ShadowQuality, Shadow);
        Test.TestEqual(What + TEXT(": foliage"), Applied.FoliageDensityPercent, Foliage);
        Test.TestEqual(What + TEXT(": view distance"), Applied.ViewDistancePercent, View);
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecScalabilityCeilingTest, "Namec.Foundation.Scalability.SplitTierIsTheLowerOfUserAndRow", NamecFoundationTestFlags)

bool FNamecScalabilityCeilingTest::RunTest(const FString& Parameters)
{
    const TOptional<FNamecScalabilityTierRow> High = UNamecScalabilitySubsystem::FindTierRow(UNamecScalabilitySubsystem::HighTierRowName);
    const TOptional<FNamecScalabilityTierRow> Split = UNamecScalabilitySubsystem::FindTierRow(TEXT("Split"));
    if (!TestTrue(TEXT("High row"), High.IsSet()) || !TestTrue(TEXT("Split row"), Split.IsSet()))
    {
        return true;
    }

    const FNamecMachineSettings User = SpecExampleUserSettings();
    TestAppliedEquals(*this, TEXT("Three viewports, the spec's example"), UNamecScalabilitySubsystem::ComputeAppliedSettings(User, *Split, true), 2, 2, 30.f, 70.f);
    TestAppliedEquals(*this, TEXT("Two viewports"), UNamecScalabilitySubsystem::ComputeAppliedSettings(User, *High, true), 2, 3, 30.f, 100.f);

    FNamecScalabilityTierRow TunedDownHigh = *High;
    TunedDownHigh.GIQuality = 1;
    TunedDownHigh.ViewDistancePercent = 40.f;
    TestAppliedEquals(*this, TEXT("A High row tuned below the user still applies the user's values"), UNamecScalabilitySubsystem::ComputeAppliedSettings(User, TunedDownHigh, true), 2, 3, 30.f, 100.f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecScalabilityNoHWRTTest, "Namec.Foundation.Scalability.WithoutHardwareRayTracingGIIsLowInBothTiers", NamecFoundationTestFlags)

bool FNamecScalabilityNoHWRTTest::RunTest(const FString& Parameters)
{
    const TOptional<FNamecScalabilityTierRow> High = UNamecScalabilitySubsystem::FindTierRow(UNamecScalabilitySubsystem::HighTierRowName);
    const TOptional<FNamecScalabilityTierRow> Split = UNamecScalabilitySubsystem::FindTierRow(TEXT("Split"));
    if (!TestTrue(TEXT("High row"), High.IsSet()) || !TestTrue(TEXT("Split row"), Split.IsSet()))
    {
        return true;
    }

    const FNamecMachineSettings User = SpecExampleUserSettings();
    TestEqual(TEXT("High tier GI"), UNamecScalabilitySubsystem::ComputeAppliedSettings(User, *High, false).GIQuality, 0);
    TestEqual(TEXT("Split tier GI"), UNamecScalabilitySubsystem::ComputeAppliedSettings(User, *Split, false).GIQuality, 0);
    TestEqual(TEXT("The other settings are untouched"), UNamecScalabilitySubsystem::ComputeAppliedSettings(User, *High, false).ShadowQuality, 3);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecScalabilityRestoreTest, "Namec.Foundation.Scalability.TwoToThreeToTwoViewportsRestoresEveryUserValue", NamecFoundationTestFlags)

bool FNamecScalabilityRestoreTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSettingsService* Settings = StartSettingsService();
    FNamecSettings Edited = Settings->GetSettings();
    Edited.Machine = SpecExampleUserSettings();
    Edited.Machine.GIQuality = 0; // what a headless run without ray tracing applies anyway, so the comparison is exact
    Settings->Apply(Edited);

    UNamecScalabilitySubsystem* Scalability = NewObject<UNamecScalabilitySubsystem>(NewGameInstance());
    Scalability->UseSettings(*Settings);

    Scalability->ApplyTierForViewportCount(2);
    TestEqual(TEXT("Two viewports select High"), Scalability->GetActiveTierName(), UNamecScalabilitySubsystem::HighTierRowName);
    TestAppliedEquals(*this, TEXT("Two viewports"), Scalability->GetAppliedGraphics(), 0, 3, 30.f, 100.f);

    Scalability->ApplyTierForViewportCount(3);
    TestEqual(TEXT("Three viewports select Split"), Scalability->GetActiveTierName(), FName(TEXT("Split")));
    TestAppliedEquals(*this, TEXT("Three viewports"), Scalability->GetAppliedGraphics(), 0, 2, 30.f, 70.f);

    Scalability->ApplyTierForViewportCount(2);
    TestAppliedEquals(*this, TEXT("Back to two viewports"), Scalability->GetAppliedGraphics(), 0, 3, 30.f, 100.f);
    TestEqual(TEXT("The saved VSM setting was never modified"), Settings->GetSettings().Machine.ShadowQuality, 3);
    TestEqual(TEXT("The saved view distance was never modified"), Settings->GetSettings().Machine.ViewDistancePercent, 100.f);

    Edited.Machine.ShadowQuality = 1;
    Scalability->ApplyTierForViewportCount(3);
    Settings->Apply(Edited);
    TestEqual(TEXT("A settings change recomputes at once"), Scalability->GetAppliedGraphics().ShadowQuality, 1);
    return true;
}

#endif
