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

    bankEventChannels.clear();
    bankEventChannels.insertMultiple(0, NULL, TTLDEBUG_PANEL_MAX_BANKS);

    // NOTE Per Josh, we need to set the processor type here in addition to
    // reporting it from getLibInfo().
    if (wantSource)
        setProcessorType(PluginProcessorType::PROCESSOR_TYPE_SOURCE);
    else
        setProcessorType(PluginProcessorType::PROCESSOR_TYPE_SINK);
}


// Destructor.
TTLPanelBase::~TTLPanelBase()
{
    // Nothing to do.
}


// Editor accessor.
AudioProcessorEditor* TTLPanelBase::createEditor()
{
T_PRINT("Creating editor.");
    // NOTE - We need to set the "editor" variable in GenericProcessor.
    editor = new TTLPanelBaseEditor(this);
    return editor;
}


// Rebuild external configuration information.
// This is where we detect input geometry, for the front panel.
void TTLPanelBase::updateSettings()
{
T_PRINT("updateSettings() called.");

    if (!isTTLSource)
    {
        // If we're a sink, enable banks based on how many inputs we have.

        // The way this seems to work in GenericProcessor() is that input event channels are copied to the output event channel list, which we can walk through using accessors (or directly).
        // Since we only create new channels if we're a source, this should match what our parent node gave us.

        T_PRINT("Found " << eventChannelArray.size() << " event channels.");

        inputBankOffsets.clear();
        int bitOffset = 0;
        int bankIndex = 0;

        for (int bidx = 0; bidx < TTLDEBUG_PANEL_MAX_BANKS; bidx++)
            bankEnabled.set(bidx, false);

        for (int ridx = 0; ridx < eventChannelArray.size(); ridx++)
        {
            inputBankOffsets.add(bitOffset);

            // If this contains TTL channels, increment the offset.
            EventChannel *thisEvChan = eventChannelArray[ridx];
            if  ( EventChannel::TTL == thisEvChan->getChannelType() )
            {
                int thisBitCount = thisEvChan->getNumChannels();
                T_PRINT("Channel " << ridx << " has " << thisBitCount << " TTL signals.");

                // NOTE - Allocate an integer number of banks for these TTL lines.
                while (thisBitCount > 0)
                {
                    bankEnabled.set(bankIndex, true);
                    bankIndex++;
                    bitOffset += TTLDEBUG_PANEL_BANK_BITS;
                    thisBitCount -= TTLDEBUG_PANEL_BANK_BITS; // May end up negative; this is fine.
                }
            }
        }
    }
}


// Generate a list of TTL output channels, for the toggle panel.
void TTLPanelBase::createEventChannels()
{
T_PRINT("createEventChannels() called.");

    if (isTTLSource)
    {
        // Create TTL outputs.
        // One event channel per bank, with one virtual channel per bit.
        // Note that "length" for TTL channels is 1. This is not the payload size.

        bankEventChannels.clear();
        bankEventChannels.insertMultiple(0, NULL, TTLDEBUG_PANEL_MAX_BANKS);

        for (int bankIdx = 0; bankIdx < TTLDEBUG_PANEL_MAX_BANKS; bankIdx++)
            if (bankEnabled[bankIdx])
            {
                EventChannel *thisEv;
                // FIXME - Conflicting documentation for TTL channel length!
                // One source says TTL length is 1, other says it's the number of bytes needed to represent the number of virtual channels.
//                int evLength = 1;
                int evLength = (TTLDEBUG_PANEL_BANK_BITS + 7) / 8;
//                int evLength = sizeof(uint64);
                thisEv = new EventChannel(EventChannel::TTL, TTLDEBUG_PANEL_BANK_BITS, evLength, -1, this);

                std::string thisName = "Toggle bank ";
                thisName += std::to_string(bankIdx);
                thisEv->setName(thisName);

                thisName += " (bits ";
                thisName += std::to_string(bankIdx * TTLDEBUG_PANEL_BANK_BITS);
                thisName += " - ";
                thisName += std::to_string(bankIdx * TTLDEBUG_PANEL_BANK_BITS + TTLDEBUG_PANEL_BANK_BITS - 1);
                thisName += ").";
                thisEv->setDescription(thisName);

                thisName = "ttlpanel.toggle.bank";
                thisName += std::to_string(bankIdx);
                thisEv->setIdentifier(thisName);

                eventChannelArray.add(thisEv);
                bankEventChannels.set(bankIdx, thisEv);
            }
    }
}


// Initialization.
bool TTLPanelBase::enable()
{
T_PRINT("enable() called.");
    // Nothing to do.
    return true;
}



// Accessor for go/no-go.
bool TTLPanelBase::isReady()
{
T_PRINT("isReady() called.");
    // We're always ready, with or without channels.
    return true;
}


// Processing loop.
void TTLPanelBase::process(AudioSampleBuffer& buffer)
{
    // If we're a source, report queued changes to TTL output state.
    // If we're a sink, input events will be received via handleEvent(); we still need to call checkForEvents().

// Don't tattle this except for debugging; it gets spammy.
//T_PRINT("process() called.");

    if (isTTLSource)
    {
        // We're a source.
        // Generate state change events.

        for (int bankIdx = 0; bankIdx < TTLDEBUG_PANEL_MAX_BANKS; bankIdx++)
            if (bankEnabled[bankIdx])
            {
                const EventChannel *thisBankEvChannel = bankEventChannels[bankIdx];
                juce::int64 thisTimestamp = CoreServices::getGlobalTimestamp();
                // NOTE - Keeping sample numberas 0 is fine.
                // This is relative to the start of the buffer fragment in this polling interval.
                int thisSampleNum = 0;

                // NOTE - We can be asked to generate events before updateSettings() finishes setting up channels.
                if (NULL != thisBankEvChannel)
                {
                    int bitBase = bankIdx * TTLDEBUG_PANEL_BANK_BITS;
                    for (int bitIdx = 0; bitIdx < TTLDEBUG_PANEL_BANK_BITS; bitIdx++)
                        if (needUpdate[bitBase + bitIdx])
                        {
                            uint64 dataValue = 1;
                            dataValue <<= bitIdx;
                            if (!bitValue[bitBase + bitIdx])
                                dataValue = 0;

                            TTLEventPtr thisEv = TTLEvent::createTTLEvent(thisBankEvChannel, thisTimestamp, &dataValue, sizeof(dataValue), bitIdx);
                            addEvent(thisBankEvChannel, thisEv, thisSampleNum);
// Don't tattle this except for debugging; it gets spammy.
//T_PRINT("Generating TTL event on bank " << bankIdx << " bit " << bitIdx << " with value " << dataValue << ".");

                            needUpdate.set(bitBase + bitIdx, false);
                        }
                }
                else
                {
                    // This shouldn't happen, but report it if it does.
                    T_PRINT("NULL pointer for bank " << bankIdx << " events.");
                }
            }
    }
    else
    {
        // We're a sink.
        checkForEvents();
    }

    // Update UI state.
    // The editor shouldn't ask for this while running, to avoid race conditions.
    pushStateToDisplay();
}


// Input TTL events enter via this hook.
void TTLPanelBase::handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition)
{
   // If we're a sink, look for TTL events here.

// Don't tattle this except for debugging; it gets spammy.
//T_PRINT("handleEvent() called.");


    if (~isTTLSource)
        if (Event::getEventType(event) == EventChannel::TTL)
        {
            TTLEventPtr thisEvent = TTLEvent::deserializeFromMessage(event, eventInfo);

            bool ttlState = thisEvent->getState();
            int ttlBit = thisEvent->getChannel();
            int chanIdx = getEventChannelIndex(thisEvent);

// Don't tattle this except for debugging; it gets spammy.
// T_PRINT("TTL event on channel " << chanIdx << " sets bit " << ttlBit << " to " << ttlState << ".");

            int bitNum = inputBankOffsets[chanIdx] + ttlBit;
            if ((bitNum >= 0) && (bitNum < TTLDEBUG_PANEL_TOTAL_BITS))
                // No need to set needUpdate; that's for outputs.
                bitValue.set(bitNum, ttlState);
        }
}


// Parameter accessor. This is guaranteed to be called under safe conditions.
// Variables used by "process" should only be modified here.
void TTLPanelBase::setParameter(int parameterIndex, float newValue)
{
T_PRINT( "setParameter() called setting " << parameterIndex << " to: " << newValue );

    if (parameterIndex < TTLDEBUG_PANEL_PARAM_BASE_ENABLED)
    {
        // Bogus (negative) value.
        T_PRINT( "Bogus parameter index! (too low)" );
    }
    else if (parameterIndex < TTLDEBUG_PANEL_PARAM_BASE_OUTPUT)
    {
        // Setting or clearing a "bank enabled" flag.
        parameterIndex -= TTLDEBUG_PANEL_PARAM_BASE_ENABLED;
        if ((parameterIndex >= 0) && (parameterIndex <= TTLDEBUG_PANEL_MAX_BANKS))
        {
            bankEnabled.set( parameterIndex, (newValue > 0) );
            // Propagate the change in the number of outputs.
            CoreServices::updateSignalChain(editor);
        }
        else
        { T_PRINT( "Bank index to enable/disable (" << parameterIndex << ") is out of range." ); }
    }
    else if (parameterIndex < TTLDEBUG_PANEL_PARAM_BASE_BOGUS)
    {
        // Setting or clearing an output bit.
        parameterIndex -= TTLDEBUG_PANEL_PARAM_BASE_OUTPUT;
        if ((parameterIndex >= 0) && (parameterIndex <= TTLDEBUG_PANEL_TOTAL_BITS))
        {
            bitValue.set( parameterIndex, (newValue > 0) );
            needUpdate.set( parameterIndex, true );
        }
        else
        { T_PRINT( "Bit index to enable/disable (" << parameterIndex << ") is out of range." ); }
    }
    else
    {
        // Bogus (too-high) value.
        T_PRINT( "Bogus parameter index! (too high)" );
    }
}


// XML configuration saving.
void TTLPanelBase::saveCustomParametersToXml(XmlElement* parentElement)
{
T_PRINT("saveCustomParametersToXml() called.");
    if (isTTLSource)
      parentElement->setAttribute("Type", "TTLTogglePanel");
    else
      parentElement->setAttribute("Type", "TTLFrontPanel");

    // Front panel configuration gets rebuilt by updateSettings, so only save toggle panel configuration.
    if (isTTLSource)
    {
        // isTTLSource gets set by the constructor and is implied by "Type".
        // So, just save bankEnabled and bitValue.

        for (int bidx = 0; bidx < TTLDEBUG_PANEL_MAX_BANKS; bidx++)
        {
            XmlElement *bankElement = parentElement->createNewChildElement("Bank");
            bankElement->setAttribute("Number", bidx);
            bankElement->setAttribute("Enabled", (bankEnabled[bidx] ? 1 : 0));
        }

        for (int bidx = 0; bidx < TTLDEBUG_PANEL_TOTAL_BITS; bidx++)
        {
            XmlElement *bankElement = parentElement->createNewChildElement("Bit");
            bankElement->setAttribute("Number", bidx);
            bankElement->setAttribute("State", (bitValue[bidx] ? 1 : 0));
        }
    }
}


// XML configuration loading.
// NOTE - This reads from the "parametersAsXml" variable, rather than passing an argument.
void TTLPanelBase::loadCustomParametersFromXml()
{
T_PRINT("loadCustomParametersFromXml() called.");

    // Front panel configuration gets rebuilt by updateSettings, so only load toggle panel configuration.
    if (isTTLSource)
    {
        // The only toggle panel variables to set are bankEnabled and bitValue.

        forEachXmlChildElementWithTagName(*parametersAsXml, bankElement, "Bank")
        {
            int bidx = bankElement->getIntAttribute("Number");
            bool isEnabled = bankElement->getBoolAttribute("Enabled");
            bankEnabled.set(bidx, isEnabled);
        }

        forEachXmlChildElementWithTagName(*parametersAsXml, bankElement, "Bit")
        {
            int bidx = bankElement->getIntAttribute("Number");
            bool isHigh = bankElement->getBoolAttribute("State");
            bitValue.set(bidx, isHigh);
        }
    }
}


// Accessors for querying and modifying state.
// Modifying is done via setParameter, since that's guaranteed safe.
// NOTE - We're not calling query accessors any more!


bool TTLPanelBase::isEventSourcePanel()
{
// Don't tattle this except for debugging; it gets spammy.
//T_PRINT( "isEventSourcePanel() returning " << (isTTLSource ? "true." : "false.") );
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


// This propagates state to the display.
// It's called by process() and can also be called manually.

void TTLPanelBase::pushStateToDisplay()
{
    // NOTE - The editor is already stored as the GenericProcessor class variable "editor".
    ((TTLPanelBaseEditor *) editor.get())->pushStateToEditor(bankEnabled, bitValue);
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
