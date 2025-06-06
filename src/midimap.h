#ifndef _MIDIMAP_H_
#define _MIDIMAP_H_


// Control Change mapping
enum 
{
	// Delay controls
	MIDI_CC_DELAY_TIME				= 1,	
	MIDI_CC_DELAY_INERTIA			= 2,
	MIDI_CC_DELAY_FEEDBACK			= 3,
	MIDI_CC_DELAY_MIX				= 4,
	MIDI_CC_DELAY_MOD_RATE			= 5,
	MIDI_CC_DELAY_MOD_DEPTH			= 6,
	MIDI_CC_DELAY_TREBLE_CUT		= 7,
	MIDI_CC_DELAY_BASS_CUT			= 8,
	MIDI_CC_DELAY_TREBLE			= 9,
	MIDI_CC_DELAY_BASS				= 10,

	// Reverb - Plate
	MIDI_CC_REVERBPL_TIME			= 20,
	MIDI_CC_REVERBPL_DIFF			= 21,
	MIDI_CC_REVERBPL_MIX			= 22,
	MIDI_CC_REVERBPL_BASS_CUT		= 23,
	MIDI_CC_REVERBPL_TREBLE_CUT		= 24,
	MIDI_CC_REVERBPL_BASS			= 25,
	MIDI_CC_REVERBPL_TREBLE			= 26,
	MIDI_CC_REVERBPL_BLEEDIN		= 27,
	MIDI_CC_REVERBPL_PITCH			= 28,
	MIDI_CC_REVERBPL_PITCHMIX		= 29,
	MIDI_CC_REVERBPL_SHIMMER		= 30,
	MIDI_CC_REVERBPL_SHIMM_PITCH	= 31,

	// Phaser
	MIDI_CC_PHASER_DEPTH_TOP		= 40,
	MIDI_CC_PHASER_DEPTH_BTM		= 41,
	MIDI_CC_PHASER_LFO_SPD			= 42,
	MIDI_CC_PHASER_STEREO_DEPTH		= 43,
	MIDI_CC_PHASER_FEEDBACK			= 44,
	MIDI_CC_PHASER_MIX				= 45,
	MIDI_CC_PHASER_STAGES			= 46,

	MIDI_CC_DELAY_REVERB_MIX		= 50,
	MIDI_CC_DRY_WET_MIX				= 51

};

enum
{
	MIDI_NOTE_DELAY					= 1,
	MIDI_NOTE_DELAY_TAP				= 2,
	MIDI_NOTE_DELAY_FREEZE			= 3,

	MIDI_NOTE_REVERB 				= 4,
	MIDI_NOTE_REVERB_FREEZE			= 5,

	MIDI_NOTE_PHASER				= 6,

	// --- diverse ---
	MIDI_NOTE_MCU_RESET			= 127
};



#endif // _MIDIMAP_H_
