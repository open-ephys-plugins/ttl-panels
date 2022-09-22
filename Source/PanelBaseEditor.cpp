#include "PanelBaseEditor.h"
#include "PanelBase.h"
#include <sstream>

using namespace TTLDebugTools;


// Private magic constants for GUI geometry.

#define TITLEBAR_YOFFSET 30

#define BUTTONROW_YSIZE 16
#define BUTTONROW_YHALO 2
#define BUTTONROW_YPITCH (BUTTONROW_YSIZE + BUTTONROW_YHALO + BUTTONROW_YHALO)

#define BITBUTTON_XSIZE 20
#if TTLDEBUG_USE_COLORBUTTON
#define BITBUTTON_XHALO 1
#else
#define BITBUTTON_XHALO 0
#endif
#define BITBUTTON_XPITCH (BITBUTTON_XSIZE + BITBUTTON_XHALO + BITBUTTON_XHALO)
#define BITBUTTON_SLAB_XSIZE (BITBUTTON_XPITCH * TTLDEBUG_PANEL_BANK_BITS)

#define ENBUTTON_XSIZE 40
#define ENBUTTON_XGAP 8

#define NUMLABEL_XSIZE 60
#define NUMLABEL_XGAP 8

#define BUTTONROW_XSIZE (ENBUTTON_XSIZE + ENBUTTON_XGAP + BITBUTTON_SLAB_XSIZE + 2*NUMLABEL_XGAP + 2*NUMLABEL_XSIZE)
#define BUTTONROW_XHALO 10
#define BUTTONROW_XPITCH (BUTTONROW_XSIZE + BUTTONROW_XHALO + BUTTONROW_XHALO)


// Diagnostic tattle macros.
#define TTLEDITTATTLE
#ifdef TTLEDITTATTLE
#define T_DEBUG(x) do { x } while(false);
#else
#define T_DEBUG(x) {}
#endif
// Flushing should already happen with std::endl, but force it anyways.
#define T_PRINT(x) T_DEBUG(std::cout << "[PanelBaseEditor]  " << x << std::endl << std::flush;)


//
// One bank of TTLs with associated controls.


// Constructor.
TTLPanelEditorRow::TTLPanelEditorRow(TTLPanelBase* newParent, int newBankIdx)
{
T_PRINT( "Making editor row " << newBankIdx << "." );
    parent = newParent;
    bankIdx = newBankIdx;

    isTTLSource = parent->isEventSourcePanel();
    isBankEnabled = 0;
    bankDataValue = 0;

    int bitnum = bankIdx * TTLDEBUG_PANEL_BANK_BITS;
    int bitxpos = ENBUTTON_XSIZE + ENBUTTON_XGAP + BITBUTTON_XHALO;

    bitButtons.clear();
    for (int bidx = 0; bidx < TTLDEBUG_PANEL_BANK_BITS; bidx++)
    {
#if TTLDEBUG_USE_COLORBUTTON
        ColorButton* btn = new ColorButton( std::to_string(bitnum + bidx), Font("Small Text", 13, Font::plain) );
        btn->setColors(juce::Colours::black, TTLDEBUG_PANEL_DISABLED_COLOR);
#else
        // NOTE - Bit buttons are not toggle buttons.
        UtilityButton* btn = new UtilityButton( std::to_string(bitnum + bidx), Font("Small Text", 13, Font::plain) );
        btn->setRadius(3.0f);
#endif
        // Most significant on the left, least significant on the right.
        btn->setBounds(bitxpos + ((TTLDEBUG_PANEL_BANK_BITS - 1) - bidx) * BITBUTTON_XPITCH, BUTTONROW_YHALO, BITBUTTON_XSIZE, BUTTONROW_YSIZE);
        btn->addListener(this);
        addAndMakeVisible(btn);
        btn->setEnabled(false);
        bitButtons.add(btn);
    }

    // Enable button and label contents are for debugging. They'll be overwritten on refresh.

    enableButton = new UtilityButton( "En", Font("Small Text", 13, Font::plain) );
    enableButton->setRadius(3.0f);
    enableButton->setBounds(0, BUTTONROW_YHALO, ENBUTTON_XSIZE, BUTTONROW_YSIZE);
    enableButton->setClickingTogglesState(isTTLSource);
    enableButton->addListener(this);
    addAndMakeVisible(enableButton);
    enableButton->setEnabled(isTTLSource);

    int labelxpos = ENBUTTON_XSIZE + ENBUTTON_XGAP + BITBUTTON_SLAB_XSIZE + NUMLABEL_XGAP;

    hexLabel = new Label( "HexLabel", "Hex" );
    hexLabel->setBounds(labelxpos, BUTTONROW_YHALO, NUMLABEL_XSIZE, BUTTONROW_YSIZE);
    hexLabel->addListener(this);
    addAndMakeVisible(hexLabel);
    hexLabel->setEnabled(isTTLSource);

    labelxpos += NUMLABEL_XSIZE + NUMLABEL_XGAP;

    decLabel = new Label( "DecLabel", "Dec" );
    decLabel->setBounds(labelxpos, BUTTONROW_YHALO, NUMLABEL_XSIZE, BUTTONROW_YSIZE);
    decLabel->addListener(this);
    addAndMakeVisible(decLabel);
    decLabel->setEnabled(isTTLSource);
T_PRINT( "Finished making editor row " << newBankIdx << "." );
}


// Destructor.
TTLPanelEditorRow::~TTLPanelEditorRow()
{
    // Nothing to do.
}


// GUI callbacks.

void TTLPanelEditorRow::buttonClicked(Button* theButton)
{
    // If this is called at all, the button is enabled and we're an output.

    if (theButton == enableButton)
    {
        // Enable button toggled.
        parent->setBankEnabled(bankIdx, theButton->getToggleState());
    }
    else
    {
        // Bit button.
        int bidx = (theButton->getButtonText()).getIntValue();
        parent->setBitValue( bidx, !(parent->getBitValue(bidx)) );
    }
}


void TTLPanelEditorRow::labelTextChanged(Label* theLabel)
{
    // If this is called at all, the label is enabled and we're an output.

    // FIXME - This might be called multiple times during editing? Not sure.
    // By default we only get a changed value when editing is finished, with
    // calls during editing returning the previous value, so it should be ok
    // either way.

//    updateParentFromLabel(theLabel);
// FIXME - We want to distinguish textWasEdited() label changes from
// changes pushed to the label.
}


// Data push accessor.
void TTLPanelEditorRow::setDataState(bool wantEnabled, uint64 newDataValue)
{
    isBankEnabled = wantEnabled;
    bankDataValue = newDataValue;

// FIXME - Diagnostics. Very spammy.
//T_PRINT("Bank " << bankIdx << " set to " << (wantEnabled ? "On" : "Off") << " data: " << newDataValue);
}


// GUI data redraw accessor.
void TTLPanelEditorRow::updateGUIFromData(uint64 datavalue)
{
    // Update the bank enable button appearance.
    enableButton->setToggleState(isBankEnabled, dontSendNotification);

    // Update the bit buttons and hex/dec readouts.
    if (isBankEnabled)
    {
        // We're enabled. Show bit state and hex/dec values.

        // Button state.
        for (int bidx = 0; bidx < TTLDEBUG_PANEL_BANK_BITS; bidx++)
        {
            uint64 bitval = datavalue & ( ((uint64) 1) << bidx );

#if TTLDEBUG_USE_COLORBUTTON
            // Open Ephys ColorButton accessor.
            if (bitval)
                bitButtons[bidx]->setColors(juce::Colours::black, TTLDEBUG_PANEL_BITONE_COLOR);
            else
                bitButtons[bidx]->setColors(juce::Colours::black, TTLDEBUG_PANEL_BITZERO_COLOR);
#else
            bitButtons[bidx]->setToggleState(bitval, dontSendNotification);
            // We can't change UtilityButton colours; they're forced.
#endif
        }

        // Hex label.
        // FIXME - The cleaner way requires C++20. Do it the old dirty way.
        std::stringstream hexscratch;
        hexscratch << std::hex << datavalue;
        hexLabel->setText(hexscratch.str(), dontSendNotification);

        // Decimal label.
        decLabel->setText(std::to_string(datavalue), dontSendNotification);
    }
    else
    {
        // Not enabled. Blank the display.
        for (int bidx = 0; bidx < TTLDEBUG_PANEL_BANK_BITS; bidx++)
        {
#if TTLDEBUG_USE_COLORBUTTON
            // Open Ephys ColorButton accessor.
            bitButtons[bidx]->setColors(juce::Colours::black, TTLDEBUG_PANEL_DISABLED_COLOR);
#else
            bitButtons[bidx]->setToggleState(false, dontSendNotification);
            // We can't change UtilityButton colours; they're forced.
#endif
        }

        hexLabel->setText("- off -", dontSendNotification);
        decLabel->setText("- off -", dontSendNotification);
    }
}


// GUI enable/disable accessor.
void TTLPanelEditorRow::updateGUIEnabledState(bool wantBankEnabled, bool wantEnButtonEnabled)
{
    // If we're enabled, enable the bit buttons and labels if and only if we're a source.

    bool bankEnabledState = (isTTLSource && wantBankEnabled);

    for (int bidx = 0; bidx < TTLDEBUG_PANEL_BANK_BITS; bidx++)
        bitButtons[bidx]->setEnabled(bankEnabledState);

    hexLabel->setEnabled(bankEnabledState);
    decLabel->setEnabled(bankEnabledState);


    // If we're a source, enable the "enable" button if requested.

    enableButton->setEnabled(isTTLSource && wantEnButtonEnabled);
}


// GUI redraw entry point.
void TTLPanelEditorRow::refreshDisplay(bool isRunning)
{
// FIXME - Diagnostics. Spammy.
//T_PRINT("Refresh bank " << bankIdx << (isBankEnabled ? " (On)" : " (Off)") << (isTTLSource ? " (Source)" : " (Display)") << " data: " << bankDataValue);
    // Update the display state.
    // When we're running, lock out the bank enable/disable buttons.
    updateGUIFromData(bankDataValue);
    updateGUIEnabledState(isBankEnabled, isTTLSource && (!isRunning));
}


// NOTE - This sets parent bits one at a time, which may be expensive.
void TTLPanelEditorRow::updateParentFromLabel(Label *theLabel)
{
    uint64 datavalue = 0;

    // Convert to std::string so that we can use stoull.
    // JUCE only natively supports signed conversion, not unsigned.
    std::string labeltext = (theLabel->getText()).toStdString();

    // Figure out which label we're converting from to get the radix.
    if (theLabel == decLabel)
        // Decimal label.
        datavalue = (uint64) (std::stoull( labeltext ));
    else
        // Hexadecimal label.
        datavalue = (uint64) (std::stoull( labeltext, nullptr, 16 ));

    // Propagate the new bit values.
    int bitnum = bankIdx * TTLDEBUG_PANEL_BANK_BITS;
    uint64 bitmask = 1;
    for (int bidx = 0; bidx < TTLDEBUG_PANEL_BANK_BITS; bidx++)
    {
      parent->setBitValue( bitnum + bidx, datavalue & bitmask );
      bitmask <<= 1;
    }

    // NOTE - Don't update GUI state or data. Let the parent plugin and timer callback do that.
}



//
// GUI tray holding a small number of TTL banks.

// Constructor.
TTLPanelBaseEditor::TTLPanelBaseEditor(TTLPanelBase* newParent) : GenericEditor(newParent, true)
{
    parent = newParent;
T_PRINT("Editor constructor called.");
//T_PRINT("Using parent: " << parent);

    banks.clear();

    for (int bidx = 0; bidx < TTLDEBUG_PANEL_MAX_BANKS; bidx++)
    {
        TTLPanelEditorRow* thisrow = new TTLPanelEditorRow(parent, bidx);

        // Least significant bank is on the lower right.
        int ypos = (TTLDEBUG_PANEL_MAX_BANKS - 1) - bidx;
        int xpos = ypos % TTLDEBUG_PANEL_UI_MAX_COLS;
        ypos = (ypos - xpos) / TTLDEBUG_PANEL_UI_MAX_COLS;

        xpos = BUTTONROW_XHALO + xpos * BUTTONROW_XPITCH;
        ypos = TITLEBAR_YOFFSET + BUTTONROW_YHALO + ypos * BUTTONROW_YPITCH;
        thisrow->setBounds(xpos, ypos, BUTTONROW_XSIZE, BUTTONROW_YSIZE);

        addAndMakeVisible(thisrow);

        banks.add(thisrow);
    }

    setDesiredWidth(TTLDEBUG_PANEL_UI_MAX_COLS * BUTTONROW_XPITCH);

    // NOTE - The redraw timer should be running even if we're not acquiring data.
    startTimer(TTLDEBUG_PANEL_DISPLAY_REFRESH_MS);
T_PRINT("Editor constructor finished.");
}


// Destructor.
TTLPanelBaseEditor::~TTLPanelBaseEditor()
{
    // "OwnedArray" and "ScopedPointer" take care of de-allocation for us.

    // Clean up the timer. Parent constructor probably does this but be safe.
    stopTimer();
}


// Timer callback.
void TTLPanelBaseEditor::timerCallback()
{
    // FIXME - Pull data if not running.
    // If we're running, let process() push it to avoid race conditions.
    // Check the "acquisitionIsActive" state variable from GenericEditor.
    if (!acquisitionIsActive)
        parent->pushStateToDisplay();

    redrawAllBanks();
}


// Accessor to push plugin state to the editor.
void TTLPanelBaseEditor::pushStateToEditor(Array<bool>& parentBankEnabled, Array<bool>& parentBitValues)
{
    for (int bankIdx = 0; bankIdx < TTLDEBUG_PANEL_MAX_BANKS; bankIdx++)
    {
        uint64 dataval = 0;

        int bitnum = bankIdx * TTLDEBUG_PANEL_BANK_BITS;
        for (int bitIdx = 0; bitIdx < TTLDEBUG_PANEL_BANK_BITS; bitIdx++)
        {
            if (parentBitValues[bitnum + bitIdx])
                dataval |= ((uint64) 1) << bitIdx;
        }

        banks[bankIdx]->setDataState(parentBankEnabled[bankIdx], dataval);
    }
}


// Redraw function. Should be called from the timer, not the plugin.
void TTLPanelBaseEditor::redrawAllBanks()
{
    // Wrap the row update function.
    // Pass the "acquisitionIsActive" state variable from GenericEditor.
    for (int bidx = 0; bidx < TTLDEBUG_PANEL_MAX_BANKS; bidx++)
        banks[bidx]->refreshDisplay(acquisitionIsActive);

    // FIXME - Force a manual repaint. Otherwise colorbutton doesn't change while under mouse focus.
    repaint();
}



//
// Canvas holding a large number of TTL banks.

// FIXME - Canvas NYI.
// For the canvas, render controls and text a bit bigger, too.


// This is the end of the file.
