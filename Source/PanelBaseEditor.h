#ifndef TTLDEBUGPANELBASEEDITOR_H_DEFINED
#define TTLDEBUGPANELBASEEDITOR_H_DEFINED

#include <EditorHeaders.h>

// Magic constants for GUI geometry.
#define TTLDEBUG_PANEL_UI_MAX_ROWS 4
#define TTLDEBUG_PANEL_UI_MAX_COLS 2

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
		void labelTextChanged(Label* theLabel);

		// Accessors.
		// NOTE - This queries the parent one bit at a time, which may be expensive.
		void refreshDisplay();

	private:
		TTLPanelBase* parent;
		int bankIdx;

		OwnedArray<UtilityButton> bitButtons;
		ScopedPointer<UtilityButton> enableButton;
		ScopedPointer<Label> hexLabel, decLabel;
	};


	// GUI tray holding a small number of TTL banks.
	class TTLPanelBaseEditor : public genericEditor
	{
	public:
		// Constructor and destructor.
		TTLPanelEditor(TTLPanelBase* newParent);
		~TTLPanelEditor();

		// Accessors.
		// NOTE - This queries the parent one bit at a time, which may be expensive.
		void updateAllBanks();

	private:
		TTLPanelBase* parent;
		OwnedArray<TTLPanelEditorRow> banks;
	};


	// Canvas holding a large number of TTL banks.
	// FIXME - NYI.
	// For the canvas, render controls and text a bit bigger, too.
}

#endif
