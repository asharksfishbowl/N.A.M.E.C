#include "Save/NamecSaveEnvelope.h"
#include "Misc/Crc.h"
#include "Serialization/MemoryReader.h"

namespace
{
    constexpr uint32 EnvelopeMagic = 0x4E4D5356; // "NMSV"
    constexpr int32 EnvelopeHeaderBytes = sizeof(uint32) + sizeof(int32) + sizeof(uint32);
}

int64 NamecSaveEnvelope::GetFramedSize(int32 PayloadSize)
{
    return static_cast<int64>(EnvelopeHeaderBytes) + PayloadSize;
}

void NamecSaveEnvelope::WriteFramed(FArchive& Writer, const TArray<uint8>& Payload)
{
    uint32 Magic = EnvelopeMagic;
    int32 PayloadSize = Payload.Num();
    uint32 PayloadCrc = FCrc::MemCrc32(Payload.GetData(), Payload.Num());

    Writer << Magic << PayloadSize << PayloadCrc;
    Writer.Serialize(const_cast<uint8*>(Payload.GetData()), Payload.Num());
}

bool NamecSaveEnvelope::StripFrame(TArray<uint8>& InOutBytes)
{
    if (InOutBytes.Num() < EnvelopeHeaderBytes)
    {
        return false;
    }

    uint32 Magic = 0;
    int32 PayloadSize = 0;
    uint32 PayloadCrc = 0;
    FMemoryReader Reader(InOutBytes);
    Reader << Magic << PayloadSize << PayloadCrc;

    if (Magic != EnvelopeMagic || PayloadSize != InOutBytes.Num() - EnvelopeHeaderBytes)
    {
        return false;
    }
    if (FCrc::MemCrc32(InOutBytes.GetData() + EnvelopeHeaderBytes, PayloadSize) != PayloadCrc)
    {
        return false;
    }

    InOutBytes.RemoveAt(0, EnvelopeHeaderBytes, EAllowShrinking::No);
    return true;
}
