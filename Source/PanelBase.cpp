#include "PanelBase.h"

using namespace TTLDebugTools;


//
// Base class for front panel and toggle panel.


// Constructor.
TTLPanelBase::TTLPanelBase(const string &name, bool wantSource) : GenericProcessor(name)
{
    isSource = wantSource;
}


// Destructor.
TTLPanelBase::~TTLPanelBase()
{
}


// Editor accessor.
AudioProcessorEditor* TTLPanelBase::createEditor()
{
    return new TTLPanelBaseEditor(this);
}


// Rebuild external configuration information.
void TTLPanelBase::updateSettings()
{
    // Enumerate channels here.
    // If we're a source, enumerate outputs based on which banks are enabled.
    // If we're a sink, enable banks based on how many inputs we have.

// FIXME - NYI.
}


// Processing loop.
void TTLPanelBase::process(AudioSampleBuffer& buffer)
{
    // If we're a source, report queued changes to TTL output state.
    // If we're a sink, input events will be received via handleEvent().

// FIXME - NYI.
}


// Input TTL events enter via this hook.
void TTLPanelBase::handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition)
{
   // If we're a sink, look for TTL events here.

// FIXME - NYI.
}


// Parameter accessor. This is guaranteed to be called under safe conditions.
// Variables used by "process" should only be modified here.
void TTLPanelBase::setParameter(int parameterIndex, float newValue)
{
// FIXME - NYI.
}


// XML configuration saving.
void TTLPanelBase::saveCustomParametersToXML(XmlElement* parentElement)
{
    if isSource
      parentElement->setAttribute("Type", "TTLTogglePanel");
    else
      parentElement->setAttribute("Type", "TTLFrontPanel");
// FIXME - NYI.
}


// XML configuration loading.
void TTLPanelBase::loadCustomParametersFromXml(XmlElement* parentElement)
{
// FIXME - NYI.
}


// Accessors for querying and modifying state.
// Modifying is done via setParameter, since that's guaranteed safe.


bool TTLPanelBase::isEventSource()
{
    return isSource;
}


bool TTLPanelBase::getBitValue(int bitNum)
{
  bool result = false;

  if ((bitNum >= 0) && (bitNum < TTLDEBUG_TOTAL_BITS))
    result = bitValue[bitNum];

  return result;
}


void TTLPanelBase::setBitValue(int bitNum, bool newState)
{
  if ((bitNum >= 0) && (bitNum < TTLDEBUG_TOTAL_BITS))
    setParameter( TTLDEBUG_PARAM_BASE_OUTPUT + bitNum, (newState ? 1 : -1) );
}


bool TTLPanelBase::isBankEnabled(int bankNum)
{
  bool result = false;

  if ((bankNum >= 0) && (bankNum < TTLDEBUG_MAX_BANKS))
    result = bankEnabled[bankNum];

  return result;
}


void TTLPanelBase::setBankEnabled(int bankNum, bool wantEnabled)
{
  if ((bankNum >= 0) && (bankNum < TTLDEBUG_MAX_BANKS))
    setParameter( TTLDEBUG_PARAM_BASE_ENABLED + bankNum, (wantEnabled ? 1 : -1) );
}


//
// Derived classes.


// Toggle panel (source).

TTLTogglePanel::TTLTogglePanel() : TTLPanelBase("TTL Toggle Panel", true)
{
}

TTLPanelBase::~TTLPanelBase()
{
}


// Front panel (sink).

TTLFrontPanel::TTLFrontPanel() : TTLPanelBase("TTL Front Panel", false)
{
}

TTLPanelBase::~TTLPanelBase()
{
}


// This is the end of the file.
