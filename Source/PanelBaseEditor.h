#ifndef TTLDEBUGPANELBASEEDITOR_H_DEFINED
#define TTLDEBUGPANELBASEEDITOR_H_DEFINED

#include <EditorHeaders.h>

// Magic constants for GUI geometry.
#define TTLDEBUG_PANEL_UI_MAX_BUTTONS 32
#define TTLDEBUG_PANEL_UI_BUTTONS_PER_ROW 8

// Magic constants for GUI colours.
#define TTLDEBUG_PANEL_DISABLED_COLOR juce::Colours::transparentWhite
#define TTLDEBUG_PANEL_BITZERO_COLOR juce::Colours::grey
#define TTLDEBUG_PANEL_BITONE_COLOR juce::Colours::lime

// Magic constants for display refresh.
#define TTLDEBUG_PANEL_DISPLAY_REFRESH_MS 100

namespace TTLDebugTools
{
	class TTLPanelBase;

	// One TTL toggle button
	class TTLPanelButton : public Button
	{
	public:
		// Constructor
		TTLPanelButton(int line, Colour colour);

		// Destructor
		~TTLPanelButton();

		// Returns the ttl line
		int getLine() {
			return line;
		}

		// Draw the button
		void paintButton(Graphics& g, bool isHighlighted, bool isDown);
		
	private:
		int line;
		Colour colour;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLPanelButton);
	};


	// GUI tray holding a small number of TTL banks.
	// NOTE - GenericEditor already inherits from Timer.
	class TTLPanelBaseEditor : public GenericEditor,
							   public Button::Listener,
							   public Timer
	{
	public:
		/** Constructor */
		TTLPanelBaseEditor(TTLPanelBase* newParent);

		/** Destructor */
		~TTLPanelBaseEditor();

		/** Timer hooks */
		void timerCallback();

		/** Called at start of acquisition*/
		void startAcquisition() override;

		/** Called at start of acquisition*/
		void stopAcquisition() override;

		/** Button callback*/
		void buttonClicked(Button* button);

		/** The plugin has to push data to us, rather than us pulling it. */
		void pushStateToEditor(std::map<uint16, uint32> currentTTLWord);

		/** Redraws TTL indicators*/
		void redrawAllButtons();

	private:
		TTLPanelBase* parent;
		OwnedArray<TTLPanelButton> buttons;
		std::unique_ptr<Label> ttlWordLabel;
		std::unique_ptr<Label> editableLabel;
		std::unique_ptr<UtilityButton> setButton;
		std::unique_ptr<UtilityButton> clearButton;
		std::map<uint16, uint32> currentTTLWord;
		std::map<uint16, uint32> lastTTLWord;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLPanelBaseEditor);
	};


	// NOTE - Visualizer canvas holding a large number of TTL banks could go here.
}

#endif
