#include "PanelBaseEditor.h"
#include "PanelBase.h"

using namespace TTLDebugTools;


// Private magic constants for GUI geometry.

#define BUTTONROW_YSIZE 16
#define BUTTONROW_YHALO 2
#define BUTTONROW_YPITCH (BUTTONROW_YSIZE + BUTTONROW_YHALO + BUTTONROW_YHALO)

#define BITBUTTON_XSIZE 20
#define BITBUTTON_XHALO 0
#define BITBUTTON_XPITCH (BITBUTTON_XSIZE + BITBUTTON_XHALO + BITBUTTON_XHALO)
#define BITBUTTON_SLAB_XSIZE (BITBUTTON_XPITCH * TTLDEBUG_PANEL_BANK_BITS)

#define ENBUTTON_XSIZE 40
#define ENBUTTON_XGAP 8

#define NUMLABEL_XSIZE 80
#define NUMLABEL_XGAP 8

#define BUTTONROW_XSIZE (ENBUTTON_XSIZE + ENBUTTON_XGAP + BITBUTTON_SLAB_XSIZE + 2*NUMLABEL_XGAP + 2*NUMLABEL_XSIZE)
#define BUTTONROW_XHALO 10
#define BUTTONROW_XPITCH (BUTTONROW_XSIZE + BUTTONROW_XHALO + BUTTONROW_XHALO)



//
// One bank of TTLs with associated controls.


// Constructor.
TTLPanelEditorRow::TTLPanelEditorRow(TTLPanelBase* newParent, int newBankIdx)
{
    parent = newParent;
    bankIdx = newBankIdx;

    int bitnum = bankIdx * TTLDEBUG_PANEL_BANK_BITS;
    int bitxpos = ENBUTTON_XSIZE + ENBUTTON_XGAP + BITBUTTON_XHALO;

    bitButtons.clear();
    for (int bidx = 0; bidx < TTLDEBUG_PANEL_BANK_BITS; bidx++)
    {
        UtilityButton* btn = new UtilityButton( std::to_string(bitnum + bidx), Font("Small Text", 13, Font::plain) );
        btn->setRadius(3.0f);
        btn->setBounds(bitxpos, BUTTONROW_YHALO, BITBUTTON_XSIZE, BUTTONROW_YSIZE);
        btn->addListener(this);
        addAndMakeVisible(btn);
        bitButtons.add(btn);
    }

    // Enable button and label contents are for debugging. They'll be overwritten on refresh.

    enableButton = new UtilityButton( "En", Font("Small Text", 13, Font::plain) );
    enableButton->setRadius(3.0f);
    enableButton->setBounds(0, BUTTONROW_YHALO, ENBUTTON_XSIZE, BUTTONROW_YSIZE);
    enableButton->addListener(this);
    addAndMakeVisible(enableButton);

    int labelxpos = ENBUTTON_XSIZE + ENBUTTON_XGAP + BITBUTTON_SLAB_XSIZE + NUMLABEL_XGAP;

    hexLabel = new Label( "HexLabel", "Hex" );
    hexLabel->setBounds(labelxpos, BUTTONROW_YHALO, NUMLABEL_XSIZE, BUTTONROW_YSIZE);
    hexLabel->addListener(this);
    addAndMakeVisible(hexLabel);

    labelxpos += NUMLABEL_XSIZE + NUMLABEL_XGAP;

    decLabel = new Label( "DecLabel", "Dec" );
    decLabel->setBounds(labelxpos, BUTTONROW_YHALO, NUMLABEL_XSIZE, BUTTONROW_YSIZE);
    decLabel->addListener(this);
    addAndMakeVisible(decLabel);
}


// Destructor.
TTLPanelEditorRow::~TTLPanelEditorRow()
{
}


// GUI callbacks.

void TTLPanelEditorRow::buttonClicked(Button* theButton)
{
// FIXME - NYI.
}


void TTLPanelEditorRow::labelTextChanged(Label* theLabel)
{
// FIXME - NYI.
}


// State refresh accessor.
// NOTE - This queries the parent one bit at a time, which may be expensive.
void TTLPanelEditorRow::refreshDisplay()
{
// FIXME - NYI.
}



//
// GUI tray holding a small number of TTL banks.

// Constructor.
TTLPanelBaseEditor::TTLPanelBaseEditor(TTLPanelBase* newParent) : GenericEditor(newParent, true)
{
    parent = newParent;

    banks.clear();

    for (int bidx = 0; bidx < TTLDEBUG_PANEL_MAX_BANKS; bidx++)
    {
        TTLPanelEditorRow* thisrow = new TTLPanelEditorRow(parent, bidx);

        // Least significant bank is on the lower right.
        int ypos = (TTLDEBUG_PANEL_MAX_BANKS - 1) - bidx;
        int xpos = ypos % TTLDEBUG_PANEL_UI_MAX_COLS;
        ypos = (ypos - xpos) / TTLDEBUG_PANEL_UI_MAX_COLS;

        xpos = BUTTONROW_XHALO + xpos * BUTTONROW_XPITCH;
        ypos = BUTTONROW_YHALO + ypos * BUTTONROW_YPITCH;
        thisrow->setBounds(xpos, ypos, BUTTONROW_XSIZE, BUTTONROW_YSIZE);

        banks.add(thisrow);
    }

    desiredWidth = TTLDEBUG_PANEL_UI_MAX_COLS * BUTTONROW_XPITCH;
}


// Destructor.
TTLPanelBaseEditor::~TTLPanelBaseEditor()
{
}


// State refresh accessor.
// NOTE - This queries the parent one bit at a time, which may be expensive.
void TTLPanelBaseEditor::updateAllBanks()
{
// FIXME - NYI.
}



//
// Canvas holding a large number of TTL banks.

// FIXME - NYI.
// For the canvas, render controls and text a bit bigger, too.


// This is the end of the file.
