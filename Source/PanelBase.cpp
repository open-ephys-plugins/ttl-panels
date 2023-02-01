#include "PanelBaseEditor.h"
#include "PanelBase.h"
#include <climits>

using namespace TTLDebugTools;


// Base class for front panel and toggle panel.


// Constructor.
TTLPanelBase::TTLPanelBase(const std::string &name, bool wantSource) : GenericProcessor(name)
{
    isTTLSource = wantSource;

    addIntParameter(Parameter::STREAM_SCOPE,
		"ttl_word",
		"TTL word for a given stream",
		0,
		INT_MIN,
        INT_MAX,
		false);

}


// Destructor.
TTLPanelBase::~TTLPanelBase()
{
    // Nothing to do.
}


// Editor accessor.
AudioProcessorEditor* TTLPanelBase::createEditor()
{
    // NOTE - We need to set the "editor" variable in GenericProcessor.
    editor = std::make_unique<TTLPanelBaseEditor>(this);
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
            EventChannel::Settings ttlChannelSettings{
                EventChannel::Type::TTL,
                "TTL Toggle Panel output",
                "Triggers whenever a TTL button is toggled.",
                "togglepanel.ttl",
                getDataStream(stream->getStreamId())
            };

            ttlChan = new EventChannel(ttlChannelSettings);
            ttlChan->addProcessor(processorInfo.get());
            eventChannels.add(ttlChan);

            localEventChannels[streamId] = eventChannels.getLast();
        }
        
        currentTTLWord[streamId] = 0;
        lastTTLWord[streamId] = 0;

    }

    pushStateToDisplay();
}


// Processing loop.
void TTLPanelBase::process(AudioSampleBuffer& buffer)
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
                for (int bit = 0; bit < 32; bit++)
                {
					if ((currentTTLWord[streamId] & (1 << bit)) != (lastTTLWord[streamId] & (1 << bit)))
					{
						// Bit changed, generate event
                       
                        TTLEventPtr event = TTLEvent::createTTLEvent(localEventChannels[streamId],
                            getFirstSampleNumberForBlock(streamId),
                            bit,
                            (currentTTLWord[streamId] >> bit) & 1);

                        addEvent(event, 0);
					}
                }

                lastTTLWord[streamId] = currentTTLWord[streamId];
            }
        }
    }
    else
    {
        // We're a sink.
        checkForEvents();

        pushStateToDisplay();
    }
}


// Input TTL events enter via this hook.
void TTLPanelBase::handleTTLEvent(TTLEventPtr event)
{
    
	const uint16 streamId = event->getStreamId();
    bool ttlState = event->getState();
    int ttlBit = event->getLine();

    if (ttlState)
        currentTTLWord[streamId] |= (1 << ttlBit);
    else
        currentTTLWord[streamId] &= ~(1 << ttlBit);

}

// Pushes latest state to display
void TTLPanelBase::pushStateToDisplay()
{
    TTLPanelBaseEditor* editor = (TTLPanelBaseEditor*) getEditor();

	editor->pushStateToEditor(currentTTLWord);

}


// Parameter accessor. This is guaranteed to be called under safe conditions.
// Variables used by "process" should only be modified here.
void TTLPanelBase::parameterValueChanged(Parameter* parameter)
{
    LOGD("Parameter value changed!");
    const uint16 streamId = parameter->getStreamId();

    int64 valueI64 = int64(parameter->getValue());
	uint32 valueU32 = uint32(valueI64 + INT_MAX);

	LOGD("New TTL word for stream ", streamId, ": ", valueU32);

	currentTTLWord[streamId] = valueU32;
    
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


// Toggle panel (filter).

TTLTogglePanel::TTLTogglePanel() : TTLPanelBase("TTL Toggle Panel", true)
{
}

TTLTogglePanel::~TTLTogglePanel()
{
}


// Front panel (sink).

TTLFrontPanel::TTLFrontPanel() : TTLPanelBase("TTL Display Panel", false)
{
}

TTLFrontPanel::~TTLFrontPanel()
{
}


// This is the end of the file.
