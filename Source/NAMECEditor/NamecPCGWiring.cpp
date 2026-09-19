#include "NamecPCGWiring.h"
#include "PCGEdge.h"
#include "PCGGraph.h"
#include "PCGNode.h"
#include "PCGPin.h"

bool NamecPCGWiring::PinsShareAnEdge(const UPCGPin* UpstreamPin, const UPCGPin* DownstreamPin)
{
    return UpstreamPin && DownstreamPin && UpstreamPin->Edges.ContainsByPredicate([UpstreamPin, DownstreamPin](const UPCGEdge* Edge)
    {
        return (Edge->InputPin == UpstreamPin && Edge->OutputPin == DownstreamPin) || (Edge->InputPin == DownstreamPin && Edge->OutputPin == UpstreamPin);
    });
}

bool NamecPCGWiring::ConnectPins(UPCGGraph& Graph, UPCGNode* From, const FName& FromOutputLabel, UPCGNode* To, const FName& ToInputLabel)
{
    Graph.AddLabeledEdge(From, FromOutputLabel, To, ToInputLabel);
    return From && To && PinsShareAnEdge(From->GetOutputPin(FromOutputLabel), To->GetInputPin(ToInputLabel));
}
