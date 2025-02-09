#include "omx_mode_grids.h"
#include "config.h"
#include "omx_util.h"
#include "omx_disp.h"
#include "omx_leds.h"
#include "sequencer.h"

using namespace grids;

enum GridModePage {
    GRIDS_DENSITY,
    GRIDS_XY,
    GRIDS_NOTES,
    GRIDS_CONFIG
};

OmxModeGrids::OmxModeGrids()
{
    // for (int i = 0; i < 4; i++)
    // {
    //     gridsXY[i][0] = grids_.getX(i);
    //     gridsXY[i][1] = grids_.getY(i);
    // }
    midiKeyboard.setMidiMode();

    // 4 pages
    params.addPage(4);
    params.addPage(4);
    params.addPage(4);
    params.addPage(4);
}

void OmxModeGrids::InitSetup()
{
    initSetup = true;
}

void OmxModeGrids::onModeActivated()
{
    if (!initSetup)
    {
        InitSetup();
    }

    sequencer.playing = false;
    grids_.stop();
    grids_.loadSnapShot(grids_.playingPattern);
    gridsAUX = false;

    params.setSelPageAndParam(0,0);
    encoderSelect = true;
}

void OmxModeGrids::onClockTick() {
    grids_.gridsTick();
}

void OmxModeGrids::onPotChanged(int potIndex, int prevValue, int newValue, int analogDelta)
{
    if(midiModeception){
        midiKeyboard.onPotChanged(potIndex, prevValue, newValue, analogDelta);
        return;
    }
    // Serial.println((String)"AnalogDelta: " + analogDelta);

    if (potIndex < 4)
    {
        if (analogDelta >= 10)
        {
            grids_.setDensity(potIndex, newValue * 2);

            if (params.getSelPage() == GRIDS_DENSITY)
            {
                setParam(potIndex);
                // setParam(GRIDS_DENSITY, potIndex + 1);
            }
        }

        omxDisp.setDirty();
    }
    else if (potIndex == 4)
    {
        int newres = (float(newValue) / 128.f) * 3;
        grids_.setResolution(newres);
        if (newres != prevResolution_)
        {
            String msg = String(rateNames[newres]);
            omxDisp.displayMessageTimed(msg, 5);
        }
        prevResolution_ = newres;
    }
}

void OmxModeGrids::loopUpdate()
{
    if (midiModeception)
    {
        midiKeyboard.loopUpdate();
        return;
    }

    auto keyState = midiSettings.keyState;

    f1_ = keyState[1] && !keyState[2];
    f2_ = !keyState[1] && keyState[2];
    f3_ = keyState[1] && keyState[2];
    fNone_ = !keyState[1] && !keyState[2];
}

void OmxModeGrids::setPageAndParam(uint8_t pageIndex, uint8_t paramPosition)
{
    encoderSelect = false;
    params.setSelPage(pageIndex);
    // int p = pageIndex * NUM_DISP_PARAMS + paramPosition;
    setParam(paramPosition);
    omxDisp.setDirty();
}

void OmxModeGrids::setParam(uint8_t paramIndex)
{
    params.setSelParam(paramIndex);

    // if (paramIndex >= 0)
    // {
    //     param = paramIndex % kNumParams;
    // }
    // else
    // {
    //     param = (paramIndex + kNumParams) % kNumParams;
    // }
    // page = param / NUM_DISP_PARAMS;


    // Select instrument on this page
    if(instLockView_ && params.getSelPage() == GRIDS_DENSITY)
    {
        lockedInst_ = paramIndex;

        // int pIndex = param % NUM_DISP_PARAMS;
        // if(pIndex > 0){
        //     lockedInst_ = pIndex - 1;
        // }
    }
    omxDisp.setDirty();
}

// Handles selecting params using encoder
void OmxModeGrids::onEncoderChangedSelectParam(Encoder::Update enc)
{
    if(enc.dir() == 0) return;

    if (enc.dir() < 0) // if turn CCW
    {
        params.decrementParam();
    }
    else if (enc.dir() > 0) // if turn CW
    {
        params.incrementParam();
    }

    omxDisp.setDirty();
}

void OmxModeGrids::onEncoderChanged(Encoder::Update enc)
{
    if (midiModeception)
    {
        midiKeyboard.onEncoderChanged(enc);
        return;
    }

    if (encoderSelect)
    {
        onEncoderChangedSelectParam(enc);
        return;
    }

    if (f1_)
    {
        // // Change selected param while holding F1
        // if (enc.dir() < 0) // if turn CCW
        // { 
        //     setParam(param - 1);
        //     omxDisp.setDirty();
        // }
        // else if (enc.dir() > 0) // if turn CW
        // { 
        //     setParam(param + 1);
        //     omxDisp.setDirty();
        // }

        return; // break;
    }

    auto amt = enc.accel(5); // where 5 is the acceleration factor if you want it, 0 if you don't)

    // int paramStep = param % 5;

    int8_t selPage = params.getSelPage(); // Add one for readability
    int8_t selParam = params.getSelParam() + 1;

    // if (paramStep != 0) // Page select mode if 0
    // {
    // }

    switch (selPage)
    {
    case GRIDS_DENSITY:
    {
        int newDensity = constrain(grids_.getDensity(selParam - 1) + amt, 0, 255);
        grids_.setDensity(selParam - 1, newDensity);
    }
    break;
    case GRIDS_XY:
    {
        if (selParam == 1) // Accent
        {
            int newAccent = constrain(grids_.accent + amt, 0, 255);
            grids_.accent = newAccent;
        }
        else if (selParam == 2) // GridX
        {
            if (instLockView_)
            {
                int newX = constrain(grids_.getX(lockedInst_) + amt, 0, 255);
                grids_.setX(lockedInst_, newX);
            }
            else
            {
                bool gridSel = false;

                for (int g = 0; g < kNumGrids; g++)
                {
                    if (gridsSelected[g])
                    {
                        int newX = constrain(grids_.getX(g) + amt, 0, 255);
                        grids_.setX(g, newX);
                        gridSel = true;
                    }
                }
                if (!gridSel) // No grids selected, modify 0
                {
                    int newX = constrain(grids_.getX(0) + amt, 0, 255);
                    grids_.setX(0, newX);

                    // for (int g = 0; g < kNumGrids; g++)
                    // {
                    //     int newX = constrain(grids_.getX(g) + amt, 0, 255);
                    //     // gridsXY[g][0] = newX;
                    //     grids_.setX(g, newX);
                    // }
                }
            }
        }
        else if (selParam == 3) // GridY
        {
            if (instLockView_)
            {
                int newY = constrain(grids_.getY(lockedInst_) + amt, 0, 255);
                grids_.setY(lockedInst_, newY);
            }
            else
            {
                bool gridSel = false;
                for (int g = 0; g < kNumGrids; g++)
                {
                    if (gridsSelected[g])
                    {
                        int newY = constrain(grids_.getY(g) + amt, 0, 255);
                        grids_.setY(g, newY);
                        gridSel = true;
                    }
                }
                if (!gridSel) // No grids selected, modify 0
                {
                    int newY = constrain(grids_.getY(0) + amt, 0, 255);
                    grids_.setY(0, newY);

                    // for (int g = 0; g < kNumGrids; g++)
                    // {
                    //     int newY = constrain(grids_.getY(g) + amt, 0, 255);
                    //     grids_.setY(g, newY);
                    // }
                }
            }
        }
        else if (selParam == 4) // Chaos
        {
            int newChaos = constrain(grids_.chaos + amt, 0, 255);
            grids_.chaos = newChaos;
        }
    }
    break;
    case GRIDS_NOTES:
    {
        if (selParam == 1)
        {
            grids_.grids_notes[0] = constrain(grids_.grids_notes[0] + amt, 0, 127);
        }
        else if (selParam == 2)
        {
            grids_.grids_notes[1] = constrain(grids_.grids_notes[1] + amt, 0, 127);
        }
        else if (selParam == 3)
        {
            grids_.grids_notes[2] = constrain(grids_.grids_notes[2] + amt, 0, 127);
        }
        else if (selParam == 4)
        {
            grids_.grids_notes[3] = constrain(grids_.grids_notes[3] + amt, 0, 127);
        }
    }
    break;
    case GRIDS_CONFIG:
    {
        if (instLockView_)
        {
            if (selParam == 1) // Note
            {
                grids_.grids_notes[lockedInst_] = constrain(grids_.grids_notes[lockedInst_] + amt, 0, 127);
            }
            else if (selParam == 2) // Midi Channel
            {
                auto chan = grids_.getMidiChan(lockedInst_);
                chan = constrain(chan + amt, 1, 16);
                grids_.setMidiChan(lockedInst_, chan);
            }
            else if (selParam == 3)
            {
            }
        }

        if (selParam == 4) // Tempo
        {
            clockConfig.newtempo = constrain(clockConfig.clockbpm + amt, 40, 300);
            if (clockConfig.newtempo != clockConfig.clockbpm)
            {
                // SET TEMPO HERE
                clockConfig.clockbpm = clockConfig.newtempo;
                omxUtil.resetClocks();
            }
        }
    }
    break;
    default:
        break;
    }
    omxDisp.setDirty();
}

void OmxModeGrids::onEncoderButtonDown()
{
    if (midiModeception)
    {
        midiKeyboard.onEncoderButtonDown();
        return;
    }

    encoderSelect = !encoderSelect;
    omxDisp.isDirty();

    // param = (param + 1 ) % kNumParams;
    // setParam(param);
}

void OmxModeGrids::onEncoderButtonDownLong()
{
    if (midiModeception)
    {
        midiKeyboard.onEncoderButtonDownLong();
        return;
    }
    
}

bool OmxModeGrids::shouldBlockEncEdit()
{
    if (midiModeception)
    {
        return midiKeyboard.shouldBlockEncEdit();
    }

    return false;
}

void OmxModeGrids::saveActivePattern(uint8_t pattIndex)
{
    grids_.saveSnapShot(pattIndex);

    // // F2 + PATTERN TO SAVE
    // for (int k = 0; k < 4; k++)
    // {
    //     grids_.gridSaves[pattIndex][k].density = grids_.getDensity(k);
    //     grids_.gridSaves[pattIndex][k].x = grids_.getX(k);
    //     grids_.gridSaves[pattIndex][k].y = grids_.getY(k);
    //     // Serial.print("saved:");
    //     // Serial.print(grids_wrapper.gridSaves[patt][k].density);
    //     // Serial.print(":");
    //     // Serial.print(grids_wrapper.gridSaves[patt][k].x);
    //     // Serial.print(":");
    //     // Serial.println(grids_wrapper.gridSaves[patt][k].y);
    // }

    String msg = "Saved " + String(pattIndex + 1);
    omxDisp.displayMessageTimed(msg, 5);
}

void OmxModeGrids::loadActivePattern(uint8_t pattIndex)
{
    grids_.loadSnapShot(pattIndex);

    // // SELECT
    // grids_.playingPattern = pattIndex;
    // for (int k = 0; k < 4; k++)
    // {
    //     grids_.setDensity(k, grids_.gridSaves[pattIndex][k].density);
    //     grids_.setX(k, grids_.gridSaves[pattIndex][k].x);
    //     grids_.setY(k, grids_.gridSaves[pattIndex][k].y);
    //     // Serial.print("state:");
    //     // Serial.print(grids_wrapper.gridSaves[patt][k].density);
    //     // Serial.print(":");
    //     // Serial.print(grids_wrapper.gridSaves[patt][k].x);
    //     // Serial.print(":");
    //     // Serial.println(grids_wrapper.gridSaves[patt][k].y);
    // }

    String msg = "Load " + String(pattIndex + 1);
    omxDisp.displayMessageTimed(msg, 5);
}

void OmxModeGrids::startPlayback()
{
    gridsAUX = true;
    grids_.start();
    sequencer.playing = true;
}
void OmxModeGrids::stopPlayback()
{
    gridsAUX = false;
    grids_.stop();
    sequencer.playing = false;
}

void OmxModeGrids::onKeyUpdate(OMXKeypadEvent e)
{
    int thisKey = e.key();
	auto keyState = midiSettings.keyState;
	
    if (midiModeception)
    {
        midiKeyboard.onKeyUpdate(e);

        if (midiSettings.keyState[0] && e.down() && thisKey == 26)
        {
            midiModeception = false;
            midiSettings.midiAUX = false;
            omxDisp.setDirty();
            omxLeds.setDirty();
        }

        return;
    }

    if (instLockView_)
    {
        onKeyUpdateChanLock(e);
        return;
    }
   
    if (!e.held())
    {
        if (e.down() && thisKey == 0) // Aux key down
        {
            // Sequencer shouldn't be a dependancy here but current is used to advance clocks. 
            if (sequencer.playing && gridsAUX)
            {
                stopPlayback();
            }
            else
            {
                startPlayback();
            }
        }
        else if (e.down() && e.clicks() == 0 && (thisKey > 2 && thisKey < 11))
        {
            int patt = thisKey - 3;
            
            if (f2_)
            { 
                saveActivePattern(patt);
            }
            else if(fNone_)
            {
                loadActivePattern(patt);
            }
        }
    }

    if (fNone_)
    {
        // Select Grid X param
        if (e.down() && (thisKey > 10 && thisKey < 15))
        {
            gridsSelected[thisKey - 11] = true;
            setPageAndParam(GRIDS_XY, 1);
            // setParam(GRIDS_XY, 2);
            omxDisp.setDirty();
        }
        else if (!e.down() && (thisKey > 10 && thisKey < 15))
        {
            gridsSelected[thisKey - 11] = false;
            omxDisp.setDirty();
        }

        // Select Grid Y param
        if (e.down() && (thisKey > 14 && thisKey < 19))
        {
            gridsSelected[thisKey - 15] = true;
            setPageAndParam(GRIDS_XY, 2);
            // setParam(GRIDS_XY, 3);
            omxDisp.setDirty();
        }
        else if (!e.down() && (thisKey > 14 && thisKey < 19))
        {
            gridsSelected[thisKey - 15] = false;
            omxDisp.setDirty();
        }

        // Select Grid X param
        if (e.down() && thisKey == 23) // Accent
        {
            setPageAndParam(GRIDS_XY, 0);
            // setParam(GRIDS_XY, 1);
        }
        else if (e.down() && thisKey == 24) // Xaos
        {
            setPageAndParam(GRIDS_XY, 3);
            // setParam(GRIDS_XY, 4);
        }
        else if (e.down() && thisKey == 26) // BPM
        {
            setPageAndParam(GRIDS_CONFIG, 3);
            // setParam(GRIDS_CONFIG, 4);
        }
    }
    if(f1_)
    {
        // Quick Select Note
        if (e.down() && (thisKey > 10 && thisKey < 15))
        {
            quickSelectInst(thisKey - 11);
        }

        if (e.down() && thisKey == 26)
        {
            midiKeyboard.onModeActivated();
            midiModeception = true;
            omxDisp.setDirty();
            omxLeds.setDirty();
        }

        // else if (!e.down() && (thisKey > 10 && thisKey < 15))
        // {
        // }

        // Select Grid Y param
        // if (e.down() && (thisKey > 14 && thisKey < 19))
        // {
        //     setParam(GRIDS_NOTES * NUM_DISP_PARAMS + (thisKey - 14));
        //     omxDisp.setDirty();
        // }
        // else if (!e.down() && (thisKey > 14 && thisKey < 19))
        // {
            
        // }
    }
    // RANDOM X or Y with bottom key + F2
    for (int j = 11; j < 19; j++)
    {
    	if(keyState[j])
    	{
			if (e.down() && (thisKey == 2))
			{
				if (j < 15) 
				{
					int whichX = j - 11;
					int newX = random(0, 255);
					grids_.setX(whichX, newX);
                    setPageAndParam(GRIDS_XY, 1);
					// setParam(GRIDS_XY, 2);
				}
				else
				{
					int whichY = j - 15;
					int newY = random(0, 255);
					grids_.setY(whichY, newY);
                    setPageAndParam(GRIDS_XY, 2);
					// setParam(GRIDS_XY, 3);
				}
				omxDisp.setDirty();
			}
    	}
    }
    
}

void OmxModeGrids::onKeyUpdateChanLock(OMXKeypadEvent e)
{
    int thisKey = e.key();

    auto keyState = midiSettings.keyState;

    if (!e.held())
    {
        if (e.down() && thisKey == 0) // Aux key down
        {
            // Serial.println("Exit aux");
            instLockView_ = false; // Exit out of channel lock
            omxDisp.setDirty();
            return;
        }
        // else if (e.down() && e.clicks() == 0 && (thisKey > 2 && thisKey < 11))
        // {
        //     int patt = thisKey - 3;
            
        //     if (f2_)
        //     { 
        //         saveActivePattern(patt);
        //     }
        //     else if(fNone_)
        //     {
        //         loadActivePattern(patt);
        //     }
        // }
    }

    // Function quick keys
    if (!f2_ && e.down() && thisKey == 2 && !keyState[1])
    {
        setPageAndParam(GRIDS_CONFIG, 0);
        // setParam(GRIDS_CONFIG, 1);
    }
    // if (!f3_ && e.down() && ((thisKey == 1 && keyState[2]) || (thisKey == 2 && keyState[1])))
    // {
    //     setParam(GRIDS_CONFIG, 2);
    // }

    // if (f2_)
    // {
    //     setParam(GRIDS_CONFIG, 1);
    // }
    // else if (f3_)
    // {
    //     setParam(GRIDS_CONFIG, 2);
    // }

    if (!f1_)
    {
        justLocked_ = false; // False once F1 released
    }

    if (fNone_)
    {
        // Midi channel
        if (e.down() && thisKey == 3)
        {
            setPageAndParam(GRIDS_CONFIG, 1);
            // setParam(GRIDS_CONFIG, 2);
        }

        // Select Grid X param
        if (e.down() && thisKey == 5) // Accent
        {
            setPageAndParam(GRIDS_XY, 0);
            // setParam(GRIDS_XY, 1);
        }
        if (e.down() && thisKey == 6) // Chan X
        {
            setPageAndParam(GRIDS_XY, 1);
            // setParam(GRIDS_XY, 2);
        }
        if (e.down() && thisKey == 7) // Chan Y
        {
            setPageAndParam(GRIDS_XY, 2);
            // setParam(GRIDS_XY, 3);
        }
        if (e.down() && thisKey == 8) // Xaos
        {
            setPageAndParam(GRIDS_XY, 3);
            // setParam(GRIDS_XY, 4);
        }

        // Quick Select inst
        // These LEDs are not lit since pattern is rendered
        if (e.down() && (thisKey > 10 && thisKey < 15))
        {
            quickSelectInst(thisKey - 11);
        }
        else if (e.down() && thisKey == 26) // BPM
        {
            setPageAndParam(GRIDS_CONFIG, 3);
            // setParam(GRIDS_CONFIG, 4);
        }
    }
    if(f1_ && !justLocked_)
    {
        // Quick Select Inst
        if (e.down() && (thisKey > 10 && thisKey < 15))
        {
            quickSelectInst(thisKey - 11);
        }

        if (e.down() && thisKey == 26)
        {
            midiKeyboard.onModeActivated();
            midiModeception = true;
            omxDisp.setDirty();
            omxLeds.setDirty();
        }
    }
}

void OmxModeGrids::quickSelectInst(uint8_t instIndex)
{
    if(instLockView_ && lockedInst_ == instIndex) return;

    instLockView_ = true;
    // justLocked_ = true; // Uncomment to immediately switch to channel view
    lockedInst_ = instIndex;

    if (params.getSelPage() == GRIDS_DENSITY || params.getSelPage() == GRIDS_NOTES)
    {
        setParam(lockedInst_);
        // setParam(page, lockedInst_ + 1);
    }

    String msg = "Inst " + String(lockedInst_ + 1);
    omxDisp.displayMessageTimed(msg, 5);
    omxDisp.setDirty();
}

void OmxModeGrids::onKeyHeldUpdate(OMXKeypadEvent e)
{
    if (midiModeception)
    {
        midiKeyboard.onKeyHeldUpdate(e);
        return;
    }
}

void OmxModeGrids::updateLEDs()
{
    if (midiModeception)
    {
        return;
    }

    omxLeds.updateBlinkStates();

    bool blinkState = omxLeds.getBlinkState();

    if (instLockView_)
    {
        int64_t instLockColor =  paramSelColors[lockedInst_];

        // Always blink to show you're in mode, don't need differation between playing or not since the playhead makes this obvious
        // auto color1 = blinkState ? instLockColor : LEDOFF;
        // strip.setPixelColor(0, color1);

        if (sequencer.playing)
        {
            // Blink left/right keys for octave select indicators.
            auto color1 = blinkState ? instLockColor : LEDOFF;
            strip.setPixelColor(0, color1);
        }
        else
        {
            strip.setPixelColor(0, instLockColor);
        }
    }
    else
    {
        if (sequencer.playing)
        {
            // Blink left/right keys for octave select indicators.
            auto color1 = blinkState ? LIME : LEDOFF;
            strip.setPixelColor(0, color1);
        }
        else
        {
            strip.setPixelColor(0, LEDOFF);
        }
    }

    // Function Keys
    if (f3_)
    {
        auto f3Color = blinkState ? LEDOFF : FUNKTHREE;
        strip.setPixelColor(1, f3Color);
        strip.setPixelColor(2, f3Color);
    }
    else
    {
        auto f1Color = (f1_ && blinkState) ? LEDOFF : FUNKONE;
        strip.setPixelColor(1, f1Color);

        auto f2Color = (f2_ && blinkState) ? LEDOFF : FUNKTWO;
        strip.setPixelColor(2, f2Color);
    }


    if (instLockView_)
    {
        updateLEDsChannelView();
    }
    else
    {
        updateLEDsPatterns();

        // Set 16 key leds to off to prevent them from sticking on after screensaver. 
        for (int k = 0; k < 16; k++)
        {
            strip.setPixelColor(k + 11, LEDOFF);
        }

        if (fNone_ || f2_)
            updateLEDsFNone();
        else if (f1_)
            updateLEDsF1();
    }

    omxLeds.setDirty();
}

void OmxModeGrids::updateLEDsFNone()
{
    bool blinkState = omxLeds.getBlinkState();

    // uint32_t colors[8] = {};
    // colors[0] = blinkState ? MAGENTA : LEDOFF;
    // colors[1] = blinkState ? ORANGE : LEDOFF;
    // colors[2] = blinkState ? RED : LEDOFF;
    // colors[3] = blinkState ? RBLUE : LEDOFF;
    // colors[4] = blinkState ? MAGENTA : LEDOFF;
    // colors[5] = blinkState ? ORANGE : LEDOFF;
    // colors[6] = blinkState ? RED : LEDOFF;
    // colors[7] = blinkState ? RBLUE : LEDOFF;

    auto keyState = midiSettings.keyState;

    for (int k = 0; k < 4; k++)
    {
        // Change color of 4 GridX keys when pushed
        // auto kColor = keyState[k + 11] ? (blinkState ? paramSelColors[k] : LEDOFF) : PINK;
        auto kColor = keyState[k + 11] ? (blinkState ? paramSelColors[k] : LEDOFF) : BLUE;

        strip.setPixelColor(k + 11, kColor);
    }

    for (int k = 4; k < 8; k++)
    {
        // Change color of 4 GridY keys when pushed
        // auto kColor = keyState[k + 11] ? (blinkState ? paramSelColors[k % 4] : LEDOFF) : GREEN;
        auto kColor = keyState[k + 11] ? (blinkState ? paramSelColors[k % 4] : LEDOFF) : LTCYAN;
        strip.setPixelColor(k + 11, kColor);
    }

    for (int k = 0; k < 4; k++)
    {
        bool triggered = grids_.getChannelTriggered(k);
        // Change color of 4 GridY keys when pushed
        auto kColor = triggered ? paramSelColors[k] : LEDOFF;
        strip.setPixelColor(k + 19, kColor);
    }

    strip.setPixelColor(23, (keyState[23] ? LBLUE : BLUE)); // Accent
    strip.setPixelColor(24, (keyState[24] ? WHITE : ORANGE)); // Xaos
    strip.setPixelColor(26, (keyState[26] ? WHITE : MAGENTA)); // BPM

}

void OmxModeGrids::updateLEDsF1()
{
    bool blinkState = omxLeds.getBlinkState();
    auto keyState = midiSettings.keyState;

    // updateLEDsChannelView();

    for (int k = 0; k < 4; k++)
    {
        // Change color of 4 GridX keys when pushed
        auto kColor = keyState[k + 11] ? (blinkState ? paramSelColors[k] : LEDOFF) : ORANGE;
        strip.setPixelColor(k + 11, kColor);
    }

    for (int k = 4; k < 16; k++)
    {
        strip.setPixelColor(k + 11, LEDOFF);
    }

    strip.setPixelColor(26, ORANGE);
}

void OmxModeGrids::updateLEDsChannelView()
{
    // bool blinkState = omxLeds.getBlinkState();
    auto keyState = midiSettings.keyState;

    int seqPos = 0;

    if (sequencer.playing)
    {
        seqPos = grids_.getSeqPos();
    }

    if (f1_ && !justLocked_)
    {
        updateLEDsF1();
        for (int j = 3; j < LED_COUNT - 16; j++)
        {
            strip.setPixelColor(j, LEDOFF);
        }
    }
    else
    {
        // Shortcut LEDS for top row
        for (int j = 3; j < LED_COUNT - 16; j++)
        {
            if (j == 3) // Midi Chan
            {
                strip.setPixelColor(j, (keyState[5] ? LBLUE : DKBLUE));
            }
            else if (j == 5) // Accent
            {
                strip.setPixelColor(j, (keyState[5] ? LBLUE : BLUE));
            }
            else if (j == 6) // ChanX
            {
                strip.setPixelColor(j, (keyState[6] ? LBLUE : RED));
            }
            else if (j == 7) // Chan Y
            {
                strip.setPixelColor(j, (keyState[7] ? WHITE : GREEN));
            }
            else if (j == 8) // Chaos
            {
                strip.setPixelColor(j,  (keyState[8] ? WHITE : ORANGE));
            }
            // else if (j == 10) // Tempo
            // {
            //     strip.setPixelColor(j,  (keyState[8] ? WHITE : MAGENTA));
            // }
            else
            {
                strip.setPixelColor(j, LEDOFF);
            }
        }

        auto channelLeds = grids_.getChannelLEDS(lockedInst_);

        auto channelHue = instLockHues_[lockedInst_];

        auto seqStart = seqPos >= 16 ? 16 : 0;

        for (int k = 0; k < 16; k++)
        {
            // Change color of 4 GridX keys when pushed
            auto level = channelLeds.levels[seqStart + k] * 2;
            auto kColor = strip.ColorHSV(channelHue, 255, level);
            strip.setPixelColor(k + 11, kColor);
        }

        if(sequencer.playing)
        {
            auto seq16 = seqPos % 16;
            strip.setPixelColor(seq16 + 11, HALFWHITE);
        }
    }
}

void OmxModeGrids::updateLEDsPatterns()
{
    int patternNum = grids_.playingPattern;

    // LEDS for top row
    for (int j = 3; j < LED_COUNT - 16; j++)
    {
        auto pColor = (j == patternNum + 3) ? seqColors[patternNum] : LEDOFF;
        strip.setPixelColor(j, pColor);
    }
}

void OmxModeGrids::setupPageLegends()
{
    // if (midiSettings.keyState[11] || midiSettings.keyState[15])
    //     {
    //         thisGrid = 0;
    //     }
    //     else if (keyState[12] || keyState[16])
    //     {
    //         thisGrid = 1;
    //     }
    //     else if (keyState[13] || keyState[17])
    //     {
    //         thisGrid = 2;
    //     }
    //     else if (keyState[14] || keyState[18])
    //     {
    //         thisGrid = 3;
    //     }

    omxDisp.clearLegends();

    // omxDisp.dispPage = page + 1;

    int8_t page = params.getSelPage();

    switch (page)
    {
    case GRIDS_DENSITY:
    {
        omxDisp.legends[0] = "DS 1";
        omxDisp.legends[1] = "DS 2";
        omxDisp.legends[2] = "DS 3";
        omxDisp.legends[3] = "DS 4";
        omxDisp.legendVals[0] = grids_.getDensity(0);
        omxDisp.legendVals[1] = grids_.getDensity(1);
        omxDisp.legendVals[2] = grids_.getDensity(2);
        omxDisp.legendVals[3] = grids_.getDensity(3);
    }
    break;
    case GRIDS_XY:
    {
        int targetChannel = 0;
        bool setLegendsToChannel = false;

        if (instLockView_)
        {
            targetChannel = lockedInst_;
            setLegendsToChannel = true;
        }
        else
        {
            int numGrids = sizeof(gridsSelected);
            int selGridsCount = 0;

            // Calculate which channels are selected
            for (int i = 0; i < numGrids; i++)
            {
                if (gridsSelected[i])
                {
                    selGridsCount++;
                    targetChannel = i;
                }
            }

            if (selGridsCount == 0)
            {
                targetChannel = 0;
                setLegendsToChannel = true;
            }
            else if (selGridsCount == 1)
            {
                setLegendsToChannel = true;
            }
            else if (selGridsCount == 4)
            {
                omxDisp.legends[1] = "X All";
                omxDisp.legends[2] = "Y All";
            }
            else
            {
                omxDisp.legends[1] = "X *";
                omxDisp.legends[2] = "Y *";
            }
        }

        if (setLegendsToChannel)
        {
            // Not sure why string.c_str doesn't work
            String l1 = "X " + String(targetChannel + 1);
            String l2 = "Y " + String(targetChannel + 1);

            omxDisp.legends[1] = l1.c_str();
            omxDisp.legends[2] = l2.c_str();

            // char bufx[4];
            // char bufy[4];
            // snprintf(bufx, sizeof(bufx), "X %d", thisGrid + 1);
            // snprintf(bufy, sizeof(bufy), "Y %d", thisGrid + 1);

            // omxDisp.legends[1] = bufx;
            // omxDisp.legends[2] = bufy;

            // Above string code not working at all? This is ugly
            // if (targetChannel == 0)
            // {
            //     omxDisp.legends[1] = "X 1";
            //     omxDisp.legends[2] = "Y 1";
            // }
            // else if (targetChannel == 1)
            // {
            //     omxDisp.legends[1] = "X 2";
            //     omxDisp.legends[2] = "Y 2";
            // }
            // else if (targetChannel == 2)
            // {
            //     omxDisp.legends[1] = "X 3";
            //     omxDisp.legends[2] = "Y 3";
            // }
            // else if (targetChannel == 3)
            // {
            //     omxDisp.legends[1] = "X 4";
            //     omxDisp.legends[2] = "Y 4";
            // }
        }

        omxDisp.legends[0] = "ACNT"; // "BPM";
        omxDisp.legends[3] = "XAOS";
        omxDisp.legendVals[0] = grids_.accent; // (int)clockbpm;
        if (targetChannel != -1)
        {
            omxDisp.legendVals[1] = grids_.getX(targetChannel);
            omxDisp.legendVals[2] = grids_.getY(targetChannel);
        }
        omxDisp.legendVals[3] = grids_.chaos;
    }
    break;
    case GRIDS_NOTES:
    {
        omxDisp.legends[0] = "NT 1";
        omxDisp.legends[1] = "NT 2";
        omxDisp.legends[2] = "NT 3";
        omxDisp.legends[3] = "NT 4";
        omxDisp.legendVals[0] = grids_.grids_notes[0];
        omxDisp.legendVals[1] = grids_.grids_notes[1];
        omxDisp.legendVals[2] = grids_.grids_notes[2];
        omxDisp.legendVals[3] = grids_.grids_notes[3];
    }
    break;
    case GRIDS_CONFIG:
    {
        if (instLockView_)
        {
            String noteLegend = "NT " + String(lockedInst_ + 1);

            omxDisp.legends[0] = noteLegend.c_str();
            omxDisp.legends[1] = "M-CHAN";
            omxDisp.legends[2] = "";
            omxDisp.legends[3] = "BPM";
            omxDisp.legendVals[0] = grids_.grids_notes[lockedInst_];
            omxDisp.legendVals[1] = grids_.getMidiChan(lockedInst_);
            omxDisp.legendVals[2] = -127;
            omxDisp.legendText[2] = "";
            omxDisp.legendVals[3] = (int)clockConfig.clockbpm;
        }
        else
        {
            omxDisp.legends[0] = "";
            omxDisp.legends[1] = "";
            omxDisp.legends[2] = "";
            omxDisp.legends[3] = "BPM";
            omxDisp.legendVals[0] = -127;
            omxDisp.legendVals[1] = -127;
            omxDisp.legendVals[2] = -127;
            omxDisp.legendVals[3] = (int)clockConfig.clockbpm;
            omxDisp.legendText[0] = "";
            omxDisp.legendText[1] = "";
            omxDisp.legendText[2] = "";
        }
    }
    break;
    default:
        break;
    }
}

void OmxModeGrids::onDisplayUpdate()
{
    if (midiModeception)
    {
        midiKeyboard.onDisplayUpdate();

        if (midiSettings.midiAUX)
        {
            strip.setPixelColor(26, RED); // Highlight aux exit key
        }

        return;
    }

    updateLEDs();

    if (omxDisp.isDirty())
    { // DISPLAY
        // Serial.println("Disp dirty");

        if (!encoderConfig.enc_edit)
        {
            setupPageLegends();
            omxDisp.dispGenericMode2(4, params.getSelPage(), params.getSelParam(), encoderSelect);
            
            // int pselected = param % NUM_DISP_PARAMS;
            // omxDisp.dispGenericMode(pselected);
        }
    }
}

void OmxModeGrids::SetScale(MusicScales *scale)
{
    midiKeyboard.SetScale(scale);
}

int OmxModeGrids::serializedPatternSize(bool eeprom)
{
    return sizeof(grids::SnapShotSettings);
}

grids::SnapShotSettings* OmxModeGrids::getPattern(uint8_t patternIndex)
{
    return grids_.getSnapShot(patternIndex);
}

void OmxModeGrids::setPattern(uint8_t patternIndex, grids::SnapShotSettings snapShot)
{
    grids_.setSnapShot(patternIndex, snapShot);
}
