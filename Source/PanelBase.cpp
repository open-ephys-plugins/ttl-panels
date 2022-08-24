#include "PanelBaseEditor.h"
#include "PanelBase.h"

using namespace TTLDebugTools;

// Diagnostic tattle macros.
#define TTLTATTLE
#ifdef TTLTATTLE
#define T_DEBUG(x) do { x } while(false);
#else
#define T_DEBUG(x) {}
#endif
// Flushing should already happen with std::endl, but force it anyways.
#define T_PRINT(x) T_DEBUG(std::cout << "[PanelBase]  " << x << std::endl << std::flush;)


//
// Base class for front panel and toggle panel.


// Constructor.
TTLPanelBase::TTLPanelBase(const std::string &name, bool wantSource) : GenericProcessor(name)
{
    isTTLSource = wantSource;
T_PRINT("Constructor called.");

    bankEnabled.clear();
    bankEnabled.insertMultiple(0, false, TTLDEBUG_PANEL_MAX_BANKS);

    needUpdate.clear();
    needUpdate.insertMultiple(0, true, TTLDEBUG_PANEL_TOTAL_BITS);

    bitValue.clear();
    bitValue.insertMultiple(0, false, TTLDEBUG_PANEL_TOTAL_BITS);
}


// Destructor.
TTLPanelBase::~TTLPanelBase()
{
}


// Editor accessor.
AudioProcessorEditor* TTLPanelBase::createEditor()
{
T_PRINT("Creating editor.");
    // NOTE - We need to set the "editor" variable in GenericProcessor.
    editor = new TTLPanelBaseEditor(this);
    return editor;
}


// Role accessors.
#if 0

bool TTLPanelBase::isSource()
{
T_PRINT( "isSource() returning " << (isTTLSource ? "true." : "false.") );
	return isTTLSource;
}


bool TTLPanelBase::isSink()
{
T_PRINT( "isSink() returning " << (~isTTLSource ? "true." : "false.") );
	return !isTTLSource;
}
#endif


// Rebuild external configuration information.
// NOTE - We're making event channels here, not in createEventChannels().
void TTLPanelBase::updateSettings()
{
    // Enumerate channels here.
    // If we're a source, enumerate outputs based on which banks are enabled.
    // If we're a sink, enable banks based on how many inputs we have.

T_PRINT("updateSettings() called.");
// FIXME - NYI.
}


// We still need createEventChannels() as a hook.
void TTLPanelBase::createEventChannels()
{
T_PRINT("createEventChannels() called.");
// FIXME - NYI.
}


// Initialization.
bool TTLPanelBase::enable()
{
T_PRINT("enable() called.");
    // Nothing to do.
    return true;
}


// Processing loop.
void TTLPanelBase::process(AudioSampleBuffer& buffer)
{
    // If we're a source, report queued changes to TTL output state.
    // If we're a sink, input events will be received via handleEvent().

//T_PRINT("process() called.");
// FIXME - NYI.
}


// Input TTL events enter via this hook.
void TTLPanelBase::handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition)
{
   // If we're a sink, look for TTL events here.

//T_PRINT("handleEvent() called.");
// FIXME - NYI.
}


// Parameter accessor. This is guaranteed to be called under safe conditions.
// Variables used by "process" should only be modified here.
void TTLPanelBase::setParameter(int parameterIndex, float newValue)
{
// FIXME - NYI.
T_PRINT( "setParameter() called setting " << parameterIndex << " to: " << newValue );
}


// XML configuration saving.
void TTLPanelBase::saveCustomParametersToXml(XmlElement* parentElement)
{
T_PRINT("saveCustomParametersToXml() called.");
    if (isTTLSource)
      parentElement->setAttribute("Type", "TTLTogglePanel");
    else
      parentElement->setAttribute("Type", "TTLFrontPanel");
// FIXME - NYI.
}


// XML configuration loading.
void TTLPanelBase::loadCustomParametersFromXml(XmlElement* parentElement)
{
T_PRINT("loadCustomParametersFromXml() called.");
// FIXME - NYI.
}


// Accessors for querying and modifying state.
// Modifying is done via setParameter, since that's guaranteed safe.


bool TTLPanelBase::isEventSourcePanel()
{
T_PRINT( "isEventSourcePanel() returning " << (isTTLSource ? "true." : "false.") );
    return isTTLSource;
}


bool TTLPanelBase::getBitValue(int bitNum)
{
  bool result = false;

  if ((bitNum >= 0) && (bitNum < TTLDEBUG_PANEL_TOTAL_BITS))
    result = bitValue[bitNum];

  return result;
}


void TTLPanelBase::setBitValue(int bitNum, bool newState)
{
  if ((bitNum >= 0) && (bitNum < TTLDEBUG_PANEL_TOTAL_BITS))
    setParameter( TTLDEBUG_PANEL_PARAM_BASE_OUTPUT + bitNum, (newState ? 1 : -1) );
}


bool TTLPanelBase::isBankEnabled(int bankNum)
{
  bool result = false;

  if ((bankNum >= 0) && (bankNum < TTLDEBUG_PANEL_MAX_BANKS))
    result = bankEnabled[bankNum];

  return result;
}


void TTLPanelBase::setBankEnabled(int bankNum, bool wantEnabled)
{
  if ((bankNum >= 0) && (bankNum < TTLDEBUG_PANEL_MAX_BANKS))
    setParameter( TTLDEBUG_PANEL_PARAM_BASE_ENABLED + bankNum, (wantEnabled ? 1 : -1) );
}


//
// Derived classes.


// Toggle panel (source).

TTLTogglePanel::TTLTogglePanel() : TTLPanelBase("TTL Toggle Panel", true)
{
}

TTLTogglePanel::~TTLTogglePanel()
{
}


// Front panel (sink).

TTLFrontPanel::TTLFrontPanel() : TTLPanelBase("TTL Front Panel", false)
{
}

TTLFrontPanel::~TTLFrontPanel()
{
}


// This is the end of the file.
