#pragma once

#include "CoreMinimal.h"

// UGameplayStatics::LoadGameFromMemory reports no error on truncated or foreign bytes, so a size
// and CRC frame is what makes "Corrupt" detectable before the engine deserialises anything.
namespace NamecSaveEnvelope
{
    NAMEC_API int64 GetFramedSize(int32 PayloadSize);

    NAMEC_API void WriteFramed(FArchive& Writer, const TArray<uint8>& Payload);

    // On success the frame is stripped in place, leaving the payload.
    NAMEC_API bool StripFrame(TArray<uint8>& InOutBytes);
}
