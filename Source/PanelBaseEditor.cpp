#include "PanelBaseEditor.h"
#include "PanelBase.h"
#include <sstream>

using namespace TTLDebugTools;


// Private magic constants for GUI geometry.

#define TITLEBAR_YOFFSET 30
#define MAX_BUTTONS 32

#define BUTTONROW_YSIZE 14
#define BUTTONROW_YHALO 4
#define BUTTONROW_YPITCH (BUTTONROW_YSIZE + BUTTONROW_YHALO + BUTTONROW_YHALO)

#define BITBUTTON_XSIZE 22
#define BITBUTTON_XHALO 1
#define BITBUTTON_XPITCH (BITBUTTON_XSIZE + BITBUTTON_XHALO + BITBUTTON_XHALO)
#define BITBUTTON_SLAB_XSIZE (BITBUTTON_XPITCH * TTLDEBUG_PANEL_BANK_BITS)

#define ENBUTTON_XSIZE 40
#define ENBUTTON_XGAP 8

#define NUMLABEL_XTITLE 30
#define NUMLABEL_XSIZE 40
#define NUMLABEL_XGAP 8
#define NUMLABEL_XPITCH (NUMLABEL_XGAP + NUMLABEL_XTITLE + NUMLABEL_XSIZE)

#define BUTTONROW_XSIZE_EN_NO (BITBUTTON_SLAB_XSIZE + 2*NUMLABEL_XPITCH)
#define BUTTONROW_XSIZE_EN_YES (ENBUTTON_XSIZE + ENBUTTON_XGAP + BUTTONROW_XSIZE_EN_NO)
#define BUTTONROW_XHALO 10
#define BUTTONROW_XPITCH_EN_NO (BUTTONROW_XSIZE_EN_NO + 2*BUTTONROW_XHALO)
#define BUTTONROW_XPITCH_EN_YES (BUTTONROW_XSIZE_EN_YES + 2*BUTTONROW_XHALO)


//
// One bank of TTLs with associated controls.


// Constructor.
TTLPanelButton::TTLPanelButton(int line_, Colour colour_) :
    Button(String(line)), line(line_), colour(colour_)
{
   
}


// Destructor.
TTLPanelButton::~TTLPanelButton()
{
    // Nothing to do.
}


// GUI callbacks.

void TTLPanelButton::paintButton(Graphics& g, bool isHighlighted, bool isDown)
{

    
    if (getToggleState())
        g.fillAll(colour);
    else
        g.fillAll(Colours::grey);
    
    g.setColour(Colours::black);
    g.drawRect(getBounds());
	g.setFont(10.0f);
    g.drawText(String(line + 1), Rectangle<float>(getWidth(), getHeight()), Justification::centred);

}


//
// GUI tray holding a small number of TTL banks.

// Constructor.
TTLPanelBaseEditor::TTLPanelBaseEditor(TTLPanelBase* newParent) : GenericEditor(newParent)
{
    parent = newParent;

    for (int bidx = 0; bidx < TTLDEBUG_PANEL_UI_MAX_BUTTONS; bidx++)
    {
        TTLPanelButton* button = new TTLPanelButton(bidx, Colours::orange);
        buttons.add(button);

        int row = bidx / TTLDEBUG_PANEL_UI_BUTTONS_PER_ROW;
        int column = bidx % TTLDEBUG_PANEL_UI_BUTTONS_PER_ROW;

		button->setBounds(BUTTONROW_XHALO + column * BITBUTTON_XPITCH,
                          TITLEBAR_YOFFSET + row * BUTTONROW_YPITCH,
                          BITBUTTON_XSIZE, BUTTONROW_YSIZE);

        addAndMakeVisible(button);

        if (parent->isEventSourcePanel())
		{
            button->setClickingTogglesState(true);
			button->addListener(this);
        }
        else {
            button->setToggleState(false, dontSendNotification);
        }
    }

    setDesiredWidth(250);

}

// Destructor.
TTLPanelBaseEditor::~TTLPanelBaseEditor()
{
    // "OwnedArray" and "ScopedPointer" take care of de-allocation for us.

}

void TTLPanelBaseEditor::startAcquisition()
{
    if (!parent->isEventSourcePanel())
    {
        startTimer(TTLDEBUG_PANEL_DISPLAY_REFRESH_MS);
    }

}

void TTLPanelBaseEditor::stopAcquisition()
{
    if (!parent->isEventSourcePanel())
    {
        stopTimer();
    }
}


void TTLPanelBaseEditor::buttonClicked(Button* button)
{
    TTLPanelButton* panelButton = (TTLPanelButton*)button;

    if (panelButton->getToggleState())
        currentTTLWord[getCurrentStream()] |= (1 << panelButton->getLine());
    else
		currentTTLWord[getCurrentStream()] &= ~(1 << panelButton->getLine());
        
	LOGD("Current ttlWord: ", currentTTLWord[getCurrentStream()]);
    
    IntParameter* ttlWordParam = (IntParameter*) parent->getDataStream(getCurrentStream())->getParameter("ttl_word");

    if (ttlWordParam != nullptr)
    {
        int64 valueI64 = int64(currentTTLWord[getCurrentStream()]);
        int converted = int(valueI64 - INT_MAX);
        ttlWordParam->setNextValue(var(converted));
    }
    else {
        LOGD("No parameter found.");
    }
    
}

// Timer callback.
void TTLPanelBaseEditor::timerCallback()
{
    redrawAllButtons();
}


// Accessor to push plugin state to the editor.
void TTLPanelBaseEditor::pushStateToEditor(std::map<uint16, uint32> currentTTLWord_)
{
    currentTTLWord = currentTTLWord_;
}


// Redraw function. Should be called from the timer, not the plugin.
void TTLPanelBaseEditor::redrawAllButtons()
{
    // Wrap the row update function.
    // Pass the "acquisitionIsActive" state variable from GenericEditor.

    uint32 state = currentTTLWord[getCurrentStream()];

    for (int bidx = 0; bidx < TTLDEBUG_PANEL_UI_MAX_BUTTONS; bidx++)
    {
        buttons[bidx]->setToggleState((state >> bidx) & 1, dontSendNotification);
        buttons[bidx]->repaint();
    }
        
}



//
// NOTE - Visualizer canvas holding a large number of TTL banks could go here.


// This is the end of the file.
