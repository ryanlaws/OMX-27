# OMX-27 Documentation

## Concepts

OMX-27 is a MIDI Keyboard and Sequencer. Both USBMIDI (in/out) and hardware MIDI out (via 1.8" TRS jack) are supported. Various "modes" can be accessed with the encoder and specific functions, parameters or sub-modes can be accessed with the encoder or key-presses/key-combinations.

Sequencer modes have 8 patterns (tracks). Sequencer modes currently send MIDI clock and transport control (start/stop) by default.

CV pitch output is limited to about 4.3 octaves.

USBMIDI should be plug-and-play with any USBMIDI compatible host. iPad works great with the camera connection kit. Hardware MIDI TRS output jack is switchable between Type-A and Type-B.


## Layout

<img src="images/omx27_layout4.png" alt="omx27_layout3" width="1045" height="425" />

Top left key = AUX Key.

Potentiometers are mapped by default to CCs 21, 22, 23, 24 and 7 (volume). These can be changed in the firmware. Hopefully this will be configurable from a device menu or a web-MIDI interface in future.

### Encoder

Long press encoder to enable mode change. Turn to change modes, short-press to enter mode.

Within the modes a short press on the encoder will select a parameter to edit.


### Key Switches

In MIDI modes the key switches work as a normal MIDI keyboard.

In sequencer modes the switches are broken into groups - Function Keys, Pattern Keys and Step Keys.


__"Black keys" (sharp/flat keyboard keys)__

The first 2 black keys are Function Keys (FUNC)
- F1
- F2

The next 8 are Pattern Keys and they select the active sequence pattern (P1-P8).

Hold a key (long press) to access parameters for that pattern. This is "Pattern Params".


__"White keys" (bottom row)__

Sequencer Step Keys - These are your sequencer step on/off keys.

Hold a key (long press) to access parameters for that step. This is "Note Select / Step Parameters". F1 + Step Key is also a quick shortcut.


---

## Modes

Long-press the encoder to enter Mode Select. Short-press to enter that mode.

While in Mode Select (before you short-press), you can save the state of the sequencer and settings to memory by hitting the AUX key.

### MI - MIDI

MIDI Keyboard. 

Quick Keys - Hold AUX  
- first 2 "white keys" are octave up/down  
- first 2 "black keys" cycle through the selected parameter on the display  

Parameters:

Page 1:
- `OCT`: current Octave  
- `CH`: active MIDI Channel 
- `CC`: displays CC being sent when turning a potentiometer
- `NOTE`: displays the last note number that was sent

Page 2:
- `RR`: RoundRobin MIDI Channel distribution
- `RROF`: RR offset value   
- `PGM`: MIDI program change  
- `BNK`: MIDI bank select.

Page 3:
- `PBNK`: Potentiometer bank select (CC assignments are still hard coded)  
- `THRU`: when "On" incoming USBMIDI will be passed to TRS MIDI Out.
- `MCRO`: MIDI Macro Mode Select (default is OFF)
- `M-CH`: MIDI Macro Mode Channel

Page 4:
- `ROOT` : select the root note for scale mode
- `SCALE` : select a scale or turn off scale mode
- `LOCK` : locks to the active scale. If this is enabled, you will only be able to play notes in the scale
- `GROUP` : groups all the notes of the scale across the lower row of 16 keys. 

#### Musical Scales

Scales can be turned on using the 4th parameter page. When a scale is enabled, the keys in the scale will light up, and the root notes will light up a brighter color. You can still play chromatically and out of key. Enabling the 'LOCK' param will make it so only the notes in the scale will send midi notes out. Turning on the 'GROUP' parameter will map the scale across the lower row of 16 keys. The root note will start one key in on what would normally be a C in chromatic mode. 

#### MIDI Macro Modes

__M8 Macro Mode__

From MI Mode, be sure `M8` is selected from the `MCRO` parameter and then double click the AUX button to enter Macro Mode.

`M-CH` should be set to the same value as the Control Map Channel in the M8 MIDI settings screen. Default is set to channel 10.  

Double click the AUX button again to exit Macro Mode.

The bottom row of keys correspond to mutes (orange) and solos (red). The top "black keys" are as follows:  

```
Orange - release all mutes  
Lime - go to mixer screen  
Cyan - snapshot load/paste *   
Magenta - snapshot save/enter selection mode *  
Red - release all solos  
Yellow - waveform display  
Blue - play  
```
<img src="images/omx27_m8macro.png" alt="omx27_m8_macro_mode" width="1045" height="425" />

When M8 is selected from the `MCRO` parameter - potentiometers will send on the `M-CH` MIDI channel in both regular keyboard mode and in the macro mode. However, notes played on keys will send on the currently selected `CH` MIDI channel.

\* __Notes:__ 
* M8 must be on the Mixer view for snapshots.  
* Snapshot Load uses the M8 key combo [SHIFT]+[OPTION]. On any view with a grid (song, chain, phrase, table, etc.) this key enters selection mode.  
* Snapshot Save uses the M8 key combo [SHIFT]+[EDIT]. On any view with a grid this key pastes the copied contents from selection mode.  


### S1 - Sequencer 1

Step sequencer - One pattern active at a time.

Keys/Commands:   
 - AUX is Start/Stop  
 - Start/Stop sends MIDI transport control, and MIDI clock when running  
 - Pattern Key: Selects playing pattern  
 - F1 + AUX: Reset sequences to first/last step  
 - F2 + AUX: Reverse pattern direction  
 - F1 + Pattern Key: Enter __Step Record__ (transport must be stopped)  
 - F2 + Pattern Key: Mute that pattern  
 - F1 + Step Key: Enter __Note Select / Step Parameters__  
 - Long press a Step Key: Enter __Note Select / Step Parameters__   
 - Long press a Pattern Key: Enter __Pattern Parameters__  
 - AUX-key exits sub-modes  
 - Hold F1 + F2: first 4 "white keys" select "page" of the current pattern (depending on pattern length)  

Parameters:  
(see below)  


### S2 - Sequencer 2

Step sequencer - All patterns active.

Keys/Commands:   
 - AUX is Start/Stop  
 - Start/Stop sends MIDI transport control, and MIDI clock when running  
 - Pattern Key: Selects active pattern  
 - Encoder changes "page" for sequence parameters (with no parameter highlighted)  
 - Short-press encoder to highlight active parameter to edit  
 - F1 + AUX: Reset sequences to first/last step  
 - F2 + AUX: Reverse pattern direction  
 - F1 + Pattern Key: Enter __Step Record__    
 - F2 + Pattern Key: Mute that pattern  
 - F1 + Step Key: Enter __Note Select / Step Parameters__  
 - Long press a Step Key: Enter __Note Select / Step Parameters__   
 - Long press a Pattern Key: Enter __Pattern Parameters__  
 - AUX-key exits sub-modes  
 - Hold F1 + F2: first 4 "white keys" select "page" of the current pattern (depending on pattern length)  

Parameters:  
- `PTN`: selected pattern  
- `TRSP`: transpose (by semitones)  
- `SWNG`: swing  
- `BPM`: tempo   

- `SOLO`: set the current pattern to MIDI Solo  
- `LEN`: pattern length  
- `RATE`: default note length (1/64th to whole note)  
- `CV`: enable to send CV from this pattern   


In the sequencer modes, the default setup is a GM Drum Map with each pattern on a consecutive midi channel. So that's notes 36, 38, 37, 39, 42, 46, 49, 51 on channels 1-8.


### OM - Organelle Mother

Pretty much the same as MI, but with the following tweaks for Organelle Mother on norns/fates/raspberry-pi.

- AUX key sends CC 25 (127 on press, 0 on release)
- Encoder turn sends CC 28 (127 on CW, 0 on CCW)


### GR - Grids Sequencer

An adaptation of the Mutable Instruments "Topographic drum sequencer" module.

See the original [Grids Manual](https://mutable-instruments.net/modules/grids/manual/) [or a video ?] for more.

Grids on OMX is a 4-channel/instrument MIDI trigger generator specialized in the creation and sculpting of rhythmic patterns. The "grid" refers to a map or library of preset drum patterns arranged in a 5x5 grid - which you can steer using X/Y controls.

Typical drum use would be Bass Drum, Snare, Closed HiHat, Open HiHat (The default note numbers are mapped to these in the GM drum map).

**Quick Keys**
Grids has many quick keys. Pressing these keys will quickly jump the display to select a specific parameter which can be adjusted with the encoder.

Keys/Commands:   
 - AUX is sequencer Start/Stop  
 - Pots 1-4 control "event density" (probability) of 4 instruments - values will show on display.
 - Pot 5 sets resolution (1/2, 1, 2)
 - Bottom row keys 1-8 are quick-keys for X/Y values - hold a key and turn encoder to change that instrument's X or Y value. You can hold multiple keys to change X/Y on multiple instruments at the same time.
 - LEDs on Keys 9-12 show trigger activity of the playing pattern  
 - Lighted Keys 13,14,16 are quick keys for ACNT/XAOS/BPM
 - Pattern keys (black keys) can load "snapshots" of density/x/y settings
 - F2 + Pattern saves a "snapshot" current state of that pattern. Patterns do not automatically save, this is a performance feature, allowing you to load a pattern, tweak it, then quickly load back to it's original state. 

  Instrument View Mode:
  - F1 + Keys 1-4 jump to Instrument View. This shows the current pattern on that instrument (over 2 pages since patterns are 32 steps) and playhead. Then render of the pattern will update to show each page while playing. 
  - Top row lighted keys(A#1, C#2, D#2, F#2) are quick-keys for ACNT/X/Y/XAOS
  - First 4 keys of bottom row will not be specially lit since they are rendering the pattern, but will allow you to quickly select a different instrument.
  - F2 is a quick key to jump to params page to set Note Number, MIDI Channel and BPM.
  - Key 3(F#1) in Instrument View is a quick key for Midi Channel for the instrument. 
  - AUX-key exits Instrument View 

  Midi Keyboard Mode:
   - F1 plus bottom key 16 enters into the Midi Keyboard for sending CCs or playing over the top of the sequencer. Everything works the same as mode MI.
   - Hold Aux and bottom key 16 to exit out of the Midi Keyboard mode. 

Parameters:  
Page 1 Event Densities:
- `DS 1`: event density - instrument 1  
- `DS 2`: event density - instrument 2  
- `DS 3`: event density - instrument 3  
- `DS 4`: event density - instrument 4     

Page 2:
- `NT 1`: note number - instrument 1  
- `NT 2`: note number - instrument 2  
- `NT 3`: note number - instrument 3  
- `NT 4`: note number - instrument 4  

Page 3:
- `ACNT`: accent amount (larger number is more variation) - applies to all instruments   
- `X `: X amount for selected instrument  
- `Y `: Y amount for selected instrument  
- `XAOS`: chaos amount - applies to all instruments

Page 4 - Main Mode:
- `BPM`: tempo

Page 4 - Instrument View Active:
- `NT -`: note number for active instrument
- `M-CHAN`: midi chanel for active instrument. 
- `BPM`: tempo


### Screensaver 
After a default timeout (3 minutes), the display will be blanked and in MI Mode a "screensaver" animation will show on the LEDs. The rightmost pot (#5) can be turned to adjust the color. Touching any keys or any of the other pots will exit the screensaver.  

In S1/S2 the screen will blank, but there is no LED animation.


## Sub-Modes

AUX-key exits sub-modes

### Note Select / Step Parameters

Long press a step key to enter this mode. Here you can change the note values (note number, velocity, note length and octave), set CC parameter-lock values with the knobs, and set step parameters (step events, step probability, trig conditions).

While in Note Select, the rightmost and leftmost keys will blink (orange or blue)- these 2 keys will shift the current octave up or down.

Press AUX to exit Note Select.  

Parameters:  

Page 1:  
- `NOTE`: midi note number  
- `OCT`: octave  
- `VEL`: note velocity  
- `LEN`: note length in steps (1-16)   

Page 2:  
- `TYPE`: step event type (see below)  
- `PROB`: percentage of the step triggering  
- `COND`: trig conditions (see below)  

Page 3: (set CC parameter-locks)  
- `L-1`: pot 1 p-lock value for this step  
- `L-2`: pot 2 p-lock value for this step  
- `L-3`: pot 3 p-lock value for this step  
- `L-4`: pot 4 p-lock value for this step  

Touching any potentiometer while in Note Select will set that p-lock value.

To reset/erase a p-lock - Highlight the parameter and turn the encoder to the left.

__Step Events (TYPE):__
"-" mute
"+" play
"1" reset to first step
">>" set parttern direction forward
"<<" set parttern direction reverse
"#?" jump to random step number
"?" set random event (of any of the previous events) for that one step

__Trig conditions - A/B Ratios (COND):__
Play that step on the A cycle of B total cycles (or bars) of the pattern. Default is 1:1 (every time).
First number - play step on that cycle thru the pattern
Second number - resets the counter after that pattern cycle.

So 1:4 would play on the first cycle, not play on the next three and then reset (after the 4th cycle). 3:8 would play only the 3rd cycle and reset after the 8th.


### Step Record

(SH-101-ish style note entry)

Holding F1 + a Pattern Key will enter Step Record Mode.

Enter notes from the keyboard and the sequence step will automatically advance to the next step. Change knob 1-4 positions to set a CC parameter lock for that step. Knob #5 (far right) will enter a velocity value for that step (there is no visual feedback when entering values from the knobs.

If you want to skip steps while entering notes, use the encoder button to select the STEP parameter and rotate to the step you want to change/update. While a step is selected, you can also record plocks/velocity for that step with the knobs without changing the note value. 

There are two pages of parameters in Step Record. First is the current octave (OCT), step number (STEP), note-value (NOTE), and pattern number (PTN). Second shows the step event parameters TYPE, PROB and COND as described above.

Press AUX to exit Step Record.

Keys/Commands:  
- Potentiometers 1-4 set a CC parameter lock  
- Potentiometers 5 sets a step velocity   
- AUX exit this sub-mode  


### Pattern Parameters

Long press Pattern Key to enter Pattern Params Mode.

Turning the encoder will show different pages of parameters.

A short-press on the encoder will select the active parameter for editing. Press the encoder repeatedly until nothing is selected to change pages.

Press AUX to exit Pattern Parameters.

Parameters:  

Page 1:  
- `PTN`: selected pattern  
- `LEN`: pattern length  
- `ROT`: rotation  
- `CH`: midi channel   

Page 2 (see Sequence Reset Automation below):  
- `START`: steart  
- `END`: end  
- `FREQ`: frequency  
- `PROB`: probability   

Page 3:  
- `RATE`: default note length (1/64th to whole note)    
- `SOLO`: MIDI solo   


Keys/Commands: 
- Step Keys set pattern length  
- F1 + pattern copies pattern  
- F2 + pattern pastes pattern (to other pattern slot)  
- F1 + F2 + pattern clears the pattern back to GM drum map default (and clears all plocks)  

(you can paste multiple times - paste buffer should stay the same until you copy again)

MIDI solo:
Set a pattern to MIDI solo and you can play the keyboard while that pattern is selected.

Note - once in MIDI solo, you will only be able to change the active pattern by using the encoder knob.

### Pattern Parameters: Sequence Reset Automation

This is located on the second page of pattern parameters

The goal of this "Sequence Reset Automation" feature was developed in the spirit of classic sequencers that can generate more complex sequences from simpler ones by setting any step in a given sequence to trigger a "reset" based on some constraint (i.e., number of cycles, probability, random).

Note - This behavior is a pattern-based solution. You can also execute step-based resets in Step Parameters.

Settings:

- START (Currently 0 - PatternLength-1): Use this to set the start step in current pattern to reset to for beginning a new cycle.

- END (Currently 0 - PatternLength-1): Use this to set the last step in current sequence to end/reset pattern cycles. This in essence is the step that will be used to trigger resets.

- FREQ of trigger reset (i.e., every X sequence cycle iterations)

- PROB of triggering reset (percentage)

NOTE: Setting STEP = 0 and PROB = 1 dictates random trigger steps which can lead to interesting results by jumping to random position/step.



# MIDI Switch for the mini TRS jack connection

A hardware switch on the device will let you swap between Type-A and Type-B for the hardware MIDI TRS jack.


Products That Use Type-A mini TRS Jack Connections
- ADDAC System products
- Arturia BeatStep (not to be confused with the BeatStep Pro)
- Dirtywave M8
- IK Multimedia products
- inMusic (Akai) products
- Korg products
- Line 6 products
- little Bits w5 MIDI module
- Make Noise 0-Coast

Products That Use Type-B mini TRS Jack Connections
- Arturia BeatStep Pro
- Faderfox products
- Novation products
- Polyend products
- 1010music Original Series 1 modules, Series 2 modules, Blackbox, MX4 and Euroshield

See https://1010music.com/stereo-minijacks-midi-connections-compatibility-guide for more information

