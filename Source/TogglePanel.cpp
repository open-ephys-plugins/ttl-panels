#include "TogglePanel.h"

using namespace TTLDebugTools;

// Constructor.
TTLTogglePanel::TTLTogglePanel() : GenericProcessor("TTL Toggle Panel")
{
}


// Destructor.
TTLTogglePanel::~TTLTogglePanel()
{
}


// Editor accessor.
TTLTogglePanel::createEditor()
{
    return new TTLTogglePanelEditor(this);
}


// Rebuild configuration information.
TTLTogglePanel::updateSettings()
{
    // FIXME - Enumerate channels here.
}


// Processing loop.
void ProcessorPlugin::process(AudioSampleBuffer& buffer)
{
    // We don't handle input data or events.
    // Just report queued changes to TTL output state.

// FIXME - NYI.
}


// Parameter accessor. The editor uses this to toggle bit states.
void setParameter(int parameterIndex, float newValue)
{
// FIXME - NYI.
}


// Configuration saving.
void saveCustomParametersToXML(XmlElement* parentElement)
{
    parentElement->setAttribute("Type", "TTLTogglePanel");
// FIXME - NYI.
}


// Configuration loading.
void loadCustomParametersFromXml(XmlElement* parentElement)
{
}
