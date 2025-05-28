/**
 * @file main.cpp
 * @author Piotr Zapart
 * @brief 	Teensy effect module using hexefx_audiolib_i16 library
 * @version 0.1
 * @date 2025-05-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <Arduino.h>

#ifndef DBG_SERIAL 
	#define DBG_SERIAL Serial
#endif

#include "midimap.h"

#include <Audio.h>
#include <hexefx_audiolib_i16.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=169.0833282470703,303.0833435058594
AudioEffectDelayStereo_i16 DelayPP;      //xy=401,286
AudioEffectPlateReverb_i16 PlateRvb; //xy=402,335
AudioMixer4              FX_Mix_R;         //xy=625.0833129882812,520.083251953125
AudioMixer4              FX_Mix_L;         //xy=630.0833129882812,155.08334350585938
AudioMixer4              DlRv_Mix_R;         //xy=631.0833129882812,392.08331298828125
AudioMixer4              DlRv_Mix_L;         //xy=638.0833129882812,280.08331298828125
AudioEffectPhaserStereo_i16 Phaser;  //xy=822.0000610351562,334
AudioOutputI2S           i2s2;           //xy=976.0833129882812,334.08331298828125

AudioConnection          patchCord1(i2s1, 0, DelayPP, 0);
AudioConnection          patchCord2(i2s1, 0, PlateRvb, 0);
AudioConnection          patchCord3(i2s1, 0, FX_Mix_L, 0);
AudioConnection          patchCord4(i2s1, 1, DelayPP, 1);
AudioConnection          patchCord5(i2s1, 1, PlateRvb, 1);
AudioConnection          patchCord6(i2s1, 1, FX_Mix_R, 0);
AudioConnection          patchCord7(DelayPP, 0, DlRv_Mix_L, 0);
AudioConnection          patchCord8(DelayPP, 1, DlRv_Mix_R, 0);
AudioConnection          patchCord9(PlateRvb, 0, DlRv_Mix_L, 1);
AudioConnection          patchCord10(PlateRvb, 1, DlRv_Mix_R, 1);
AudioConnection          patchCord11(FX_Mix_R, 0, Phaser, 1);
AudioConnection          patchCord12(FX_Mix_L, 0, Phaser, 0);
AudioConnection          patchCord13(DlRv_Mix_R, 0, FX_Mix_R, 1);
AudioConnection          patchCord14(DlRv_Mix_L, 0, FX_Mix_L, 1);
AudioConnection          patchCord15(Phaser, 0, i2s2, 0);
AudioConnection          patchCord16(Phaser, 1, i2s2, 1);

AudioControlSGTL5000     codec;     //xy=191.0833282470703,405.0833282470703
// GUItool: end automatically generated code



void cb_NoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void cb_NoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void cb_ControlChange(uint8_t channel, uint8_t control, uint8_t value);

int8_t pitch=0, pitchShm=0;

uint32_t timeNow, timeLast;
const char PROGMEM *termPosHome = "\x1b[;H";

void printMemInfo(void);

void setup()
{
	DBG_SERIAL.begin(115200);
	
	AudioMemory(20);

	if (!codec.enable()) DBG_SERIAL.println("Codec init error!");
	codec.inputSelect(AUDIO_INPUT_LINEIN);
	codec.volume(0.8f);
	codec.lineInLevel(10, 10);
	codec.adcHighPassFilterDisable();
	DBG_SERIAL.println("Codec initialized.");

	// set callbacks for USB MIDI
	usbMIDI.setHandleNoteOn(cb_NoteOn);
	usbMIDI.setHandleNoteOff(cb_NoteOff);
	usbMIDI.setHandleControlChange(cb_ControlChange);

	DelayPP.mix(1.0f); // using external dry7wet mixer
	DelayPP.bypass_setMode(BYPASS_MODE_OFF);
	PlateRvb.bypass_setMode(BYPASS_MODE_OFF);
	PlateRvb.mix(1.0);
	Phaser.bypass_set(true);


}
void loop()
{
	usbMIDI.read();
	timeNow = millis();
    if (timeNow - timeLast > 500)
    {
        printMemInfo();		
        timeLast = timeNow;
	}
}

/**
 * @brief USB MIDI NoteOn callback
 * 
 * @param channel 
 * @param note 
 * @param velocity 
 */
void cb_NoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
	switch(note)
	{
		case MIDI_NOTE_DELAY:
			DelayPP.bypass_set(false);
			break;
		case MIDI_NOTE_DELAY_FREEZE:
			DelayPP.freeze(true);
			break;
		case MIDI_NOTE_DELAY_TAP:
			DelayPP.tap_tempo();
			break;
		case MIDI_NOTE_REVERB:
			PlateRvb.bypass_set(false);
			break;
		case MIDI_NOTE_REVERB_FREEZE:
			PlateRvb.freeze(true);
			break;			
		case MIDI_NOTE_PHASER:
			Phaser.bypass_set(false);
			break;
		case MIDI_NOTE_MCU_RESET:
			SCB_AIRCR = 0x05FA0004; // MCU reset
			break;
		default:
			break;
	}
}

void cb_NoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
	switch(note)
	{

		case MIDI_NOTE_MCU_RESET:
			break;
		case MIDI_NOTE_DELAY:
			DelayPP.bypass_set(true);	
			break;
		case MIDI_NOTE_DELAY_FREEZE:
			DelayPP.freeze(false);
			break;
		case MIDI_NOTE_REVERB:
			PlateRvb.bypass_set(true);	
			break;
		case MIDI_NOTE_REVERB_FREEZE:
			PlateRvb.freeze(false);
			break;
		case MIDI_NOTE_PHASER:
			Phaser.bypass_set(true);
		default:
			break;
	}
}

void cb_ControlChange(uint8_t channel, uint8_t control, uint8_t value)
{
	float32_t tmp_f;
	float32_t valuef = (float32_t) value / 127.0f;
	switch(control)
	{
	
		// --- Delay---
		case MIDI_CC_DELAY_TIME:
			DelayPP.time(valuef);
			break;
		case MIDI_CC_DELAY_INERTIA:
			DelayPP.inertia(valuef);
			break;
		case MIDI_CC_DELAY_FEEDBACK:
			DelayPP.feedback(valuef);
			break;			
		case MIDI_CC_DELAY_MIX:
			DelayPP.mix(valuef);
			break;
		case MIDI_CC_DELAY_MOD_RATE:
			DelayPP.mod_rate(valuef);
			break;
		case MIDI_CC_DELAY_MOD_DEPTH:
			DelayPP.mod_depth(valuef);
			break;			
		case MIDI_CC_DELAY_BASS_CUT:
			DelayPP.bass_cut(valuef);
			break;
		case MIDI_CC_DELAY_TREBLE_CUT:
			DelayPP.treble_cut(valuef);
			break;
		case MIDI_CC_DELAY_BASS:
			DelayPP.bass(valuef);
			break;
		case MIDI_CC_DELAY_TREBLE:
			DelayPP.treble(valuef);
			break;

// --- Reverb ---
		case MIDI_CC_REVERBPL_TIME:
			PlateRvb.size(valuef);
			break;
		case MIDI_CC_REVERBPL_DIFF:
			PlateRvb.diffusion(valuef);
			break;
		case MIDI_CC_REVERBPL_MIX:
			PlateRvb.mix(valuef);
			break;
		case MIDI_CC_REVERBPL_BASS_CUT:
			PlateRvb.lodamp(valuef);
			break;
		case MIDI_CC_REVERBPL_TREBLE_CUT:
			PlateRvb.hidamp(valuef);
			break;
		case MIDI_CC_REVERBPL_BASS:
			tmp_f = 1.0f - valuef;
			PlateRvb.hipass(tmp_f);
			break;
		case MIDI_CC_REVERBPL_TREBLE:
			tmp_f = 1.0f - valuef;
			PlateRvb.lowpass(tmp_f);
			break;
		case MIDI_CC_REVERBPL_BLEEDIN:
			PlateRvb.freezeBleedIn(valuef);
			break;
		case MIDI_CC_REVERBPL_PITCH:
			PlateRvb.pitchNormalized(valuef);
			break;
		case  MIDI_CC_REVERBPL_PITCHMIX:
			PlateRvb.pitchMix(valuef);
			break;
		case MIDI_CC_REVERBPL_SHIMMER:
			PlateRvb.shimmer(valuef);
			break;
		case MIDI_CC_REVERBPL_SHIMM_PITCH:
			PlateRvb.shimmerPitchNormalized(valuef);
			break;

		case MIDI_CC_PHASER_DEPTH_TOP:
			Phaser.top(valuef);
			break;
		case MIDI_CC_PHASER_DEPTH_BTM:
			Phaser.btm(valuef);
			break;
		case MIDI_CC_PHASER_LFO_SPD:
			Phaser.lfo_rate(valuef * valuef * 10.0f);
			break;
		case MIDI_CC_PHASER_FEEDBACK:
			Phaser.feedback((valuef*2.0f) - 1.0f);
			break;
		case MIDI_CC_PHASER_STEREO_DEPTH:
			Phaser.stereo(valuef);
			break;
		case MIDI_CC_PHASER_STAGES:
			Phaser.stages(map(value, 0, 127, 1, 6)<<1);
			break;
		case MIDI_CC_PHASER_MIX:
			Phaser.mix(valuef);
			break;
		case MIDI_CC_DELAY_REVERB_MIX:
			DlRv_Mix_L.gain(0, 1.0f - valuef);	// dry signal 
			DlRv_Mix_L.gain(1, valuef);			// wet signal 
			DlRv_Mix_R.gain(0, 1.0f - valuef);	// dry signal 
			DlRv_Mix_R.gain(1, valuef);			// wet signal 
			break;
		case MIDI_CC_DRY_WET_MIX:
			FX_Mix_L.gain(0, 1.0f - valuef);	// dry signal 
			FX_Mix_L.gain(1, valuef);			// wet signal 
			FX_Mix_R.gain(0, 1.0f - valuef);	// dry signal 
			FX_Mix_R.gain(1, valuef);			// wet signal 
			break;

		default:    break;
	}	
}

void printMemInfo(void)
{
    static const char *on = "\x1b[32mon \x1b[0m";
    static const char *off = "\x1b[31moff\x1b[0m";
    float load_dl = DelayPP.processorUsageMax();
    DelayPP.processorUsageMaxReset();
    float load_rv = PlateRvb.processorUsageMax();
    PlateRvb.processorUsageMaxReset();
	float load_ph = Phaser.processorUsageMax();
	Phaser.processorUsageMaxReset();
	float load = AudioProcessorUsageMax();


    AudioProcessorUsageMaxReset();

    DBG_SERIAL.printf("%sCPU usage: delay=%2.2f%%  reverb=%2.2f%% phaser=%2.2f%% total = %2.2f%%   \r\n", termPosHome, 
			load_dl, load_rv, load_ph, load);

    DBG_SERIAL.printf("Delay %s \tFreeze %s\r\n", 
                        DelayPP.bypass_get()?off:on,
                        DelayPP.freeze_get()?on:off);
    DBG_SERIAL.printf("Reverb %s \tFreeze %s\r\n", 
                        PlateRvb.bypass_get()?off:on,
                        PlateRvb.freeze_get()?on:off);					
    DBG_SERIAL.printf("Reverb pitch \t%d semitones          \r\n", pitch);
	DBG_SERIAL.printf("Shimmer pitch \t%d semitones          \r\n", pitchShm);												
	DBG_SERIAL.printf("Phaser %s \tStages %d    \r\n", 
                        Phaser.bypass_get()?off:on,
                        Phaser.stages_get());	
}