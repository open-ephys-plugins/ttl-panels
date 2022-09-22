#ifndef TTLDEBUGPANELBASEEDITOR_H_DEFINED
#define TTLDEBUGPANELBASEEDITOR_H_DEFINED

#include <EditorHeaders.h>

// Magic constants for GUI geometry.
#define TTLDEBUG_PANEL_UI_MAX_ROWS 4
#define TTLDEBUG_PANEL_UI_MAX_COLS 1

// Magic constants for GUI colours.
#define TTLDEBUG_PANEL_DISABLED_COLOR juce::Colours::transparentWhite
#define TTLDEBUG_PANEL_BITZERO_COLOR juce::Colours::grey
#define TTLDEBUG_PANEL_BITONE_COLOR juce::Colours::lime

// Magic constants for display refresh.
#define TTLDEBUG_PANEL_DISPLAY_REFRESH_MS 20
//define TTLDEBUG_PANEL_DISPLAY_REFRESH_MS 1000

// FIXME - Appearance switch.
#define TTLDEBUG_USE_COLORBUTTON 1

namespace TTLDebugTools
{
	class TTLPanelBase;


	// One bank of TTLs with associated controls.
	// Maximum 64 bits per bank.
	class TTLPanelEditorRow : public Component, Button::Listener, Label::Listener
	{
	public:
		// Constructor and destructor.
		TTLPanelEditorRow(TTLPanelBase* newParent, int newBankIdx);
		~TTLPanelEditorRow();

		// GUI callbacks.
		// These push state changes to the plugin directly.
		void buttonClicked(Button* theButton);
		void labelTextChanged(Label* theLabel);

		// Accessors.

		// NOTE - the plugin has to push data to us, rather than us pulling it.
		void setDataState(bool wantEnabled, uint64 newDataValue);

		// GUI accessors.
		void updateGUIFromData(uint64 datavalue);
		void updateGUIEnabledState(bool wantBankEnabled, bool wantEnButtonEnabled);
		void refreshDisplay(bool isRunning);

                // NOTE - This sets parent bits one at a time, which may be expensive.
		void updateParentFromLabel(Label *theLabel);

	private:
		TTLPanelBase* parent;
		int bankIdx;
		bool isTTLSource;
		bool isBankEnabled;
		uint64 bankDataValue;

// FIXME - Experimenting with button types.
#if TTLDEBUG_USE_COLORBUTTON
		OwnedArray<ColorButton> bitButtons;
#else
		OwnedArray<UtilityButton> bitButtons;
#endif
		ScopedPointer<UtilityButton> enableButton;
		ScopedPointer<Label> hexLabel, decLabel;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLPanelEditorRow);
	};


	// GUI tray holding a small number of TTL banks.
	// NOTE - GenericEditor already inherits from Timer.
	class TTLPanelBaseEditor : public GenericEditor
	{
	public:
		// Constructor and destructor.
		TTLPanelBaseEditor(TTLPanelBase* newParent);
		~TTLPanelBaseEditor();

		// Plugin hooks.
		// NOTE - updateSettings() would go here if we needed it.

		// Timer hooks.
		void timerCallback();

		// Accessors.
		// NOTE - The plugin has to push data to us, rather than us pulling it.
		void pushStateToEditor(Array<bool>& parentBankEnabled, Array<bool>& parentBitValues);
		void redrawAllBanks();

	private:
		TTLPanelBase* parent;
		OwnedArray<TTLPanelEditorRow> banks;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLPanelBaseEditor);
	};


	// Canvas holding a large number of TTL banks.
	// FIXME - Canvas NYI.
	// For the canvas, render controls and text a bit bigger, too.
}

#endif
