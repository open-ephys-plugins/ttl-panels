#include "PanelBase.h"
#include "PanelBaseEditor.h"
#include <climits>

using namespace TTLDebugTools;

// Base class for front panel and toggle panel.

// Constructor.
TTLPanelBase::TTLPanelBase (const std::string& name, bool wantSource) : GenericProcessor (name)
{
    isTTLSource = wantSource;
}

// Destructor.
TTLPanelBase::~TTLPanelBase()
{
    // Nothing to do.
}

void TTLPanelBase::registerParameters()
{
    if (isTTLSource)
    {
        addIntParameter (Parameter::STREAM_SCOPE,
                         "ttl_word",
                         "Word",
                         "TTL word for a given stream",
                         INT_MIN + 1,
                         INT_MIN,
                         INT_MAX,
                         false);
    }
}

// Editor accessor.
AudioProcessorEditor* TTLPanelBase::createEditor()
{
    // NOTE - We need to set the "editor" variable in GenericProcessor.
    editor = std::make_unique<TTLPanelBaseEditor> (this);
    return editor.get();
}

// Rebuild external configuration information.
// This is where we detect input geometry, for the front panel.
void TTLPanelBase::updateSettings()
{
    localEventChannels.clear();
    currentTTLWord.clear();
    lastTTLWord.clear();

    for (auto stream : getDataStreams())
    {
        const uint16 streamId = stream->getStreamId();

        if (isTTLSource)
        {
            // TTL Channel
            EventChannel* ttlChan;
            EventChannel::Settings ttlChannelSettings {
                EventChannel::Type::TTL,
                "TTL Toggle Panel output",
                "Triggers whenever a TTL button is toggled.",
                "togglepanel.ttl",
                getDataStream (stream->getStreamId())
            };

            ttlChan = new EventChannel (ttlChannelSettings);
            ttlChan->addProcessor (this);
            eventChannels.add (ttlChan);

            localEventChannels[streamId] = eventChannels.getLast();
        }

        currentTTLWord[streamId] = 0;
        lastTTLWord[streamId] = 0;

        if (isTTLSource)
            parameterValueChanged (stream->getParameter ("ttl_word"));
    }

    pushStateToDisplay();
}

bool TTLPanelBase::stopAcquisition()
{
    // Reset the last TTL word.
    for (auto& pair : lastTTLWord)
    {
        pair.second = 0;
    }

    return true; // Indicate that acquisition was stopped successfully.
}

// Processing loop.
void TTLPanelBase::process (AudioSampleBuffer& buffer)
{
    // If we're a filter, report queued changes to TTL output state.
    // If we're a sink, input events will be received via handleEvent(); we still need to call checkForEvents().

    if (isTTLSource)
    {
        // We're a filter.
        // Generate state change events on the appropriate stream.

        for (auto stream : dataStreams)
        {
            const uint16 streamId = stream->getStreamId();

            if (currentTTLWord[streamId] != lastTTLWord[streamId])
            {
                Array<TTLEventPtr> events = TTLEvent::createTTLEvent (localEventChannels[streamId],
                                                                      getFirstSampleNumberForBlock (streamId),
                                                                      static_cast<uint64> (currentTTLWord[streamId]));

                for (auto event : events)
                {
                    addEvent (event, 0);
                }

                lastTTLWord[streamId] = currentTTLWord[streamId];
            }
        }
    }
    else
    {
        // We're a sink.
        checkForEvents();
    }
}

// Input TTL events enter via this hook.
void TTLPanelBase::handleTTLEvent (TTLEventPtr event)
{
    const uint16 streamId = event->getStreamId();
    bool ttlState = event->getState();
    int ttlBit = event->getLine();

    if (ttlState)
        currentTTLWord[streamId] |= (1 << ttlBit);
    else
        currentTTLWord[streamId] &= ~(1 << ttlBit);

    pushStateToDisplay();
}

// Pushes latest state to display
void TTLPanelBase::pushStateToDisplay()
{
    TTLPanelBaseEditor* editor = (TTLPanelBaseEditor*) getEditor();

    editor->pushStateToEditor (currentTTLWord);
}

void TTLPanelBase::setParameter (int index, float newValue)
{
    DataStream* stream = nullptr;

    if (dataStreams.size() > 0)
    {
        if (editor != nullptr)
            stream = getDataStream (editor->getCurrentStream());
        else
            stream = getDataStream (0); // Default to first stream if no editor.
    }
    else
    {
        return;
    }

    if (auto* ttlWordParam = stream->getParameter ("ttl_word"))
    {
        ttlWordParam->updateValue();
        parameterValueChanged (ttlWordParam);
        CoreServices::sendStatusMessage ("Set " + getName() + " " + ttlWordParam->getDisplayName() + ": " + String (currentTTLWord[stream->getStreamId()]));
    }
}

// Parameter accessor. This is guaranteed to be called under safe conditions.
// Variables used by "process" should only be modified here.
void TTLPanelBase::parameterValueChanged (Parameter* parameter)
{
    LOGD ("Parameter value changed for ", parameter->getName());
    const uint16 streamId = parameter->getStreamId();

    int64 valueI64 = int64 (parameter->getValue());
    uint32 valueU32 = uint32 (valueI64 + INT_MAX);

    currentTTLWord[streamId] = valueU32;

    pushStateToDisplay();
}

bool TTLPanelBase::isEventSourcePanel()
{
    // Don't tattle this except for debugging; it gets spammy.
    //T_PRINT( "isEventSourcePanel() returning " << (isTTLSource ? "true." : "false.") );
    return isTTLSource;
}

// Toggle panel (filter).

TTLTogglePanel::TTLTogglePanel() : TTLPanelBase ("TTL Toggle Panel", true)
{
}

TTLTogglePanel::~TTLTogglePanel()
{
}

// Front panel (sink).

TTLFrontPanel::TTLFrontPanel() : TTLPanelBase ("TTL Display Panel", false)
{
}

TTLFrontPanel::~TTLFrontPanel()
{
}

// This is the end of the file.
