#ifndef TTLDEBUGPANELBASE_H_DEFINED
#define TTLDEBUGPANELBASE_H_DEFINED

#include <ProcessorHeaders.h>

// Magic constants for data geometry.
#define TTLDEBUG_BANK_BITS 8
#define TTLDEBUG_MAX_BANKS 4
#define TTLDEBUG_TOTAL_BITS (TTLDEBUG_BANK_BITS * TTLDEBUG_MAX_BANKS)

// Magic constants for parameter indices.
#define TTLDEBUG_PARAM_BASE_ENABLED 0
#define TTLDEBUG_PARAM_BASE_OUTPUT TTLDEBUG_MAX_BANKS
#define TTLDEBUG_PARAM_BASE_BOGUS (TTLDEBUG_PARAM_BASE_OUTPUT + TTLDEBUG_TOTAL_BITS)

namespace TTLDebugTools
{
	class TTLPanelBase : public GenericProcessor
	{
	public:
		// Constructor and destructor.
		// Set "wantSource" true to make this a toggle panel (source), or false for a front panel (sink).
		TTLPanelBase(const string &name, bool wantSource);
		~TTLPanelBase();

		// We do have a custom editor.
		bool hasEditor() const { return true; }
		AudioProcessorEditor* createEditor() override;

		// Rebuild external configuration information.
		void updateSettings() override;

		// Processing loop.
		void process(AudioSampleBuffer& buffer) override;

		// If we're configured as a front panel, we need to handle events.
		void handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition) override;

		// This is guaranteed to be called under safe conditions.
		// Variables used by "process" should only be modified here.
		void setParameter(int parameterIndex, float newValue) override;

		// XML configuration hooks.
		void saveCustomParametersToXml(XmlElement* parentElement) override;
		void loadCustomParametersFromXml(XmlElement* parentElement) override;


		// Accessors for querying and modifying state.
		// Modifying is done via setParameter, since that's guaranteed safe.
		bool isEventSource();
		bool getBitValue(int bitNum);
		void setBitValue(int bitNum, bool newState);
		bool isBankEnabled(int bankNum);
		void setBankEnabled(int bankNum, bool wantEnabled);

	protected:
		bool isSource;
		Array<bool> bankEnabled;
		Array<bool> needUpdate;
		Array<bool> bitValue;
	};


	class TTLTogglePanel : public TTLPanelBase
	{
	public:
		TTLTogglePanel();
		~TTLTogglePanel();
	}


	class TTLFrontPanel : public TTLPanelBase
	{
	public:
		TTLFrontPanel();
		~TTLFrontPanel();
	}
}

#endif
