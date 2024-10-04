#ifndef TTLDEBUGPANELBASE_H_DEFINED
#define TTLDEBUGPANELBASE_H_DEFINED

#include <ProcessorHeaders.h>

// Magic constants for data geometry.
#define TTLDEBUG_PANEL_BANK_BITS 8
#define TTLDEBUG_PANEL_MAX_BANKS 4
#define TTLDEBUG_PANEL_TOTAL_BITS (TTLDEBUG_PANEL_BANK_BITS * TTLDEBUG_PANEL_MAX_BANKS)

// Magic constants for parameter indices.
#define TTLDEBUG_PANEL_PARAM_BASE_ENABLED 0
#define TTLDEBUG_PANEL_PARAM_BASE_OUTPUT TTLDEBUG_PANEL_MAX_BANKS
#define TTLDEBUG_PANEL_PARAM_BASE_BOGUS (TTLDEBUG_PANEL_PARAM_BASE_OUTPUT + TTLDEBUG_PANEL_TOTAL_BITS)

namespace TTLDebugTools
{
class TTLPanelBase : public GenericProcessor
{
public:
    /** Constructor.
			Set "wantSource" true to make this a toggle panel (filter), or false for a front panel (sink) */
    TTLPanelBase (const std::string& name, bool wantSource);

    /** Destructor */
    ~TTLPanelBase();

    /** Create custom editor */
    AudioProcessorEditor* createEditor() override;

    /** Create event channels */
    void updateSettings() override;

    /** Processing loop. */
    void process (AudioBuffer<float>& buffer) override;

    // If we're configured as a front panel, we need to handle events.
    void handleTTLEvent (TTLEventPtr event) override;

    // This is guaranteed to be called under safe conditions.
    // Variables used by "process" should only be modified here.
    void parameterValueChanged (Parameter* parameter) override;

    // Accessors for querying and modifying state.
    // Modifying is done via parameterValueChanged, since that's guaranteed safe.
    // NOTE - Calling query accessors while running isn't safe!
    bool isEventSourcePanel();

    // Updates the display with latest state.
    void pushStateToDisplay();

protected:
    bool isTTLSource;
    std::map<uint16, uint32> currentTTLWord;
    std::map<uint16, uint32> lastTTLWord;
    std::map<uint16, EventChannel*> localEventChannels;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TTLPanelBase);
};

/** 
		Creates an array of buttons to toggle TTL events on and off
	*/
class TTLTogglePanel : public TTLPanelBase
{
public:
    /** Constructor */
    TTLTogglePanel();

    /** Destructor */
    ~TTLTogglePanel();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TTLTogglePanel);
};

/** 
	
		Creates an array of indicators to show the state of individual TTL lines
		
	*/
class TTLFrontPanel : public TTLPanelBase
{
public:
    /** Constructor */
    TTLFrontPanel();

    /** Destructor*/
    ~TTLFrontPanel();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TTLFrontPanel);
};
} // namespace TTLDebugTools

#endif
