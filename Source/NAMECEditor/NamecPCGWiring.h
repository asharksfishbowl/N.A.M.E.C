#pragma once

#include "CoreMinimal.h"

class UPCGGraph;
class UPCGNode;
class UPCGPin;

namespace NamecPCGWiring
{
    bool PinsShareAnEdge(const UPCGPin* UpstreamPin, const UPCGPin* DownstreamPin);

    // Connects From's output pin to To's input pin and reports whether the edge now exists.
    // UPCGGraph::AddEdge returns its target node either way, and AddLabeledEdge returns whether
    // OTHER edges were broken, so neither tells a caller that the connection was made.
    bool ConnectPins(UPCGGraph& Graph, UPCGNode* From, const FName& FromOutputLabel, UPCGNode* To, const FName& ToInputLabel);
}
