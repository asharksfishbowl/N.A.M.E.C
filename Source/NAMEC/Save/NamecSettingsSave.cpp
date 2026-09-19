#include "Save/NamecSettingsSave.h"

FNamecLocalPlayerSettings& FNamecSettings::GetSlot(int32 SlotNumber)
{
    check(SlotNumber >= 1 && SlotNumber <= NamecLocalPlayerSlotCount);
    return Slots[SlotNumber - 1];
}

const FNamecLocalPlayerSettings& FNamecSettings::GetSlot(int32 SlotNumber) const
{
    check(SlotNumber >= 1 && SlotNumber <= NamecLocalPlayerSlotCount);
    return Slots[SlotNumber - 1];
}

void FNamecSettings::ClampToValidRanges()
{
    Machine.MasterVolume = FMath::Clamp(Machine.MasterVolume, 0.f, 1.f);
    Machine.MusicVolume = FMath::Clamp(Machine.MusicVolume, 0.f, 1.f);
    Machine.EffectsVolume = FMath::Clamp(Machine.EffectsVolume, 0.f, 1.f);
    Machine.GIQuality = FMath::Clamp(Machine.GIQuality, 0, 3);
    Machine.ShadowQuality = FMath::Clamp(Machine.ShadowQuality, 0, 3);
    Machine.FoliageDensityPercent = FMath::Clamp(Machine.FoliageDensityPercent, 0.f, 100.f);
    Machine.ViewDistancePercent = FMath::Clamp(Machine.ViewDistancePercent, 0.f, 100.f);
}
