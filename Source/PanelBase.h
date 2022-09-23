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
		// Constructor and destructor.
		// Set "wantSource" true to make this a toggle panel (source), or false for a front panel (sink).
		TTLPanelBase(const std::string &name, bool wantSource);
		~TTLPanelBase();

		// We do have a custom editor.
		bool hasEditor() const { return true; }
		AudioProcessorEditor* createEditor() override;

		// Rebuild external configuration information.
		// NOTE - We're making event channels here, not in createEventChannels().
		void updateSettings() override;

		// We still need createEventChannels() as a hook.
		void createEventChannels() override;

		// Initialization.
		bool enable() override;

		// Accessor for go/no-go.
		bool isReady() override;

		// Processing loop.
		void process(AudioSampleBuffer& buffer) override;

		// If we're configured as a front panel, we need to handle events.
		void handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition) override;

		// This is guaranteed to be called under safe conditions.
		// Variables used by "process" should only be modified here.
		void setParameter(int parameterIndex, float newValue) override;

		// XML configuration hooks.
		void saveCustomParametersToXml(XmlElement* parentElement);
		void loadCustomParametersFromXml(XmlElement* parentElement);


		// Accessors for querying and modifying state.
		// Modifying is done via setParameter, since that's guaranteed safe.
		// NOTE - Calling query accessors while running isn't safe!
		bool isEventSourcePanel();
		bool getBitValue(int bitNum);
		void setBitValue(int bitNum, bool newState);
		bool isBankEnabled(int bankNum);
		void setBankEnabled(int bankNum, bool wantEnabled);

		// This propagates state to the display.
		// It's called by process() and can also be called manually.
		void pushStateToDisplay();

	protected:
		bool isTTLSource;
		Array<bool> bankEnabled;
		Array<bool> needUpdate;
		Array<bool> bitValue;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLPanelBase);
	};


	class TTLTogglePanel : public TTLPanelBase
	{
	public:
		TTLTogglePanel();
		~TTLTogglePanel();
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLTogglePanel);
	};


	class TTLFrontPanel : public TTLPanelBase
	{
	public:
		TTLFrontPanel();
		~TTLFrontPanel();
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLFrontPanel);
	};
}

#endif
