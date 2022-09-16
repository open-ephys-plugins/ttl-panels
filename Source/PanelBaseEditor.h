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
// FIXME - Debugging
#define TTLDEBUG_USE_COLORBUTTON 0

namespace TTLDebugTools
{
	class TTLPanelBase;


	// One bank of TTLs with associated controls.
	class TTLPanelEditorRow : public Component, Button::Listener, Label::Listener
	{
	public:
		// Constructor and destructor.
		TTLPanelEditorRow(TTLPanelBase* newParent, int newBankIdx);
		~TTLPanelEditorRow();

		// GUI callbacks.
		void buttonClicked(Button* theButton);
		void labelTextChanged(Label* theLabel);

		// Accessors.
		void updateGUIFromData(uint64 datavalue);
                // NOTE - These query and set parent bits one at a time, which may be expensive.
		void updateDataFromLabel(Label *theLabel);
		void refreshDisplay();

	private:
		TTLPanelBase* parent;
		int bankIdx;

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
	class TTLPanelBaseEditor : public GenericEditor
	{
	public:
		// Constructor and destructor.
		TTLPanelBaseEditor(TTLPanelBase* newParent);
		~TTLPanelBaseEditor();

		// Plugin hooks.
		void updateSettings() override;

		// Accessors.
		// NOTE - This queries the parent one bit at a time, which may be expensive.
		void updateAllBanks();

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
