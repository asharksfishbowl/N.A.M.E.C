#include "Settings/NamecScalabilitySubsystem.h"
#include "HAL/IConsoleManager.h"

void UNamecScalabilitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LogActiveTierCmd = TUniquePtr<IConsoleObject>(IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("NamecScalability.LogActiveTier"),
        TEXT("Log the currently active scalability tier"),
        FConsoleCommandDelegate::CreateUObject(this, &UNamecScalabilitySubsystem::LogActiveTier)));
}

void UNamecScalabilitySubsystem::Deinitialize()
{
    LogActiveTierCmd.Reset();
    Super::Deinitialize();
}

void UNamecScalabilitySubsystem::OnLocalPlayerCountChanged() {}
void UNamecScalabilitySubsystem::ApplyTierForViewportCount(int32 ViewportCount) {}
void UNamecScalabilitySubsystem::LogActiveTier() const {}
