#pragma once



#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <chrono> // Deals with time
#include <stdlib.h> // For rand()

// For searching file directories. New as of c++ 17.
#include <filesystem>
#include <cstdlib>



#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <thread>
#include <mutex>



// The thread lock to make sure that the audio and main threads do not interfere.
std::mutex mtx;



// Floating-point 2d position
struct Vector2 { float x = 0; float y = 0; };
struct Vector2i { int x = 0; int y = 0; };


// A square of the screen GUI
struct UIElement
{
	Vector2i sprite;
	int textCol = 0; int bgCol = 0; // Colors for text and background.
};


struct RGBColor
{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
};



struct SampleWave
{
	int operatorType = 0; // 0=Wave, 1=sample, 2=voice

	// Wave types: Sine, Square, Triangle, Saw, Noise, Bell thing
	int waveType = 0; // -1: Constantly open (so that the modulator can be used as a standalone wave.)

	int loopType = 1; // 0 = None, 1 = forwards, 2 = bounce
	int loopStart = 0;
	int loopEnd = 0;

	std::vector <float> pcmFrames;
	float fineTuneToC = 1.0f; // Since samples have an integer length, this value is used to scale their pitch to be in-tune.

	// The duty cycle.
	float dutyCycle = 1.0f;
	float smoothness = 0.0f; // Smoothness (not for sine wave).
	int numOfSineWaves = 15;
	float offset = 0.0f; // Offsets the wave volume from the center.
	
	// Envelope
	float attack = 0.0f;
	float sustain = 0.0f;
	float decay = 0.0f;
	float release = 0.0f;

	int lfo = 0; // 0=1, 1=0.1, 2=0.01, 3=0.001

	int stereo = 0; // 0=Mix, 1=Left, 2=Right


	// Frequencies (1 = fundemental frequency)
	// (1/4) (1/3) (1/2) (1) (2) (3) (4) (5) (6) (7) (8)
	float frequencies[11] = { 0,0,0,0,0,0,0,0,0,0,0 };


	// Boolean flags
	bool generateFromSines = false; // The square and saw waves can be generated from sine waves for a slightly different sound.
	bool reverseFrames = false;
	bool sustainForever = true;
	bool pitchToNote = true;
	bool continueNote = true;
};



// An audio sample
struct Sample
{
	float volume = 1.0f;

	bool enabled = false;

	SampleWave waveforms[4];
	int operatorWavesToUse[4] = { 0, 1, 2, 3 }; // Which sample is mapped to each operator.

	int modulationTypes[4] = { 0,0,0,0 };
	float lPResonances[4] = { 0.0f,0.0f,0.0f,0.0f }; // Low-pass filter resonance values.

	float arpPitches[15] = { 7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f };
	float arpSpeed = 0.5f; // Arpeggiation speed in subdivisions of a beat.
	float arpLength = 0.0f;

	std::string sampleName = "NewSample";
	std::vector <int> jumpPoints = {};

	float glide = 0.0f;

	int algorithmType = 0;
};


struct voiceSample
{
	std::vector <float> pcmFrames;
	bool loop = false;
};


struct VoiceSynth
{
	voiceSample phonemes[44];


	float findFrequencyInSample(std::vector <float> sample, float frequency)
	{
		// Finds the volume of a given frequency in a sample using the Goertzel Algorithm.
		float volume = 0.0f;


		int sampleSize = sample.size();
		float freq = (frequency * float(sampleSize)) / 48000.0f;
		float omega = 2.0f * 3.141593 * (freq / float(sampleSize));
		float coeff = 2.0f * cos(omega);

		float sPrev2 = 0.0f, sPrev1 = 0.0f, sCurr = 0.0f;

		for (int n = 0; n < sampleSize; ++n)
		{
			sCurr = sample[n] + coeff * sPrev1 - sPrev2;
			sPrev2 = sPrev1;
			sPrev1 = sCurr;
		}

		float real = sPrev1 - sPrev2 * cos(omega);
		float imag = sPrev2 * sin(omega);
		float magnitude = sqrt(real * real + imag * imag);

		volume = magnitude / (sampleSize / 2.0f);

		return volume;
	}
};



struct GUITheme
{
	RGBColor uiColors[9];
};


struct ScrollBar
{
	Vector2i topLeft;
	float position = 0.0f;
	float length = 10.0f;
	bool horizontal = false;
	bool drag = false;
};


struct GUI
{
	// Scrollbars
	std::vector <ScrollBar> scrollBars =
	{
	{ { 6, 3 }, 0.0f, 8, false, false }, // frameMenuScroll
	{ { 91, 3 }, 0.0f, 8, false, false }, // sampleMenuScroll
	{ { 91, 17 }, 0.0f, 38, false, false }, // NoteScroll
	{ { 6, 56 }, 0.0f, 84, true, false }, // NoteScrollHorizontal
	{ { -1, -1 }, 0.0f, 17, false, false }, // fileScroll
	//{ { -1, -1 }, 0.0f, 49, true, false }, // sampleEditorScroll
	};


	bool clickingOnFloatingWind = false;

	bool drawScreen = false;
	// Tells the program to draw certain parts of the screen.
	bool drawUIThisFrame = true;
	bool drawFrameThisFrame = true;

	// 92 x 57 tile ui.
	UIElement activeUI[92][57]; // Active user interface elements on the screen.

	Vector2 hoveredTile;
	Vector2 floatHoveredTile;
	Vector2 selectedTile;
	Vector2 selectedWindowTile;

	int frameListScroll = 0;
	int fileListScroll = 0;
	int sampleListScroll = 0;
	Vector2i frameScroll;


	int songLength = 0; // Song length in seconds.
	int songPos = 0; // Song position in seconds.


	int uiColorTheme = 26;
	std::vector <GUITheme> themes = {};
	bool background = false;
	bool lightMode = false;
	int uiDisplayMenuOption = 0; // 0 = Piano, 1 = Effects

	unsigned int uiTexture;


	std::string phonemes[44] =
	{
		"b",
		"d",
		"f",
		"g",
		"h",
		"j",
		"k",
		"l",
		"m",
		"n",
		"p",
		"r",
		"s",
		"t",
		"v",
		"w",
		"z",
		"s (treasure)",
		"ch",
		"sh",
		"th (three)",
		"th (the)",
		"ng (ring)",
		"y (you)",
		"a (cat)",
		"a (say)",
		"e (end)",
		"e (be)",
		"i (it)",
		"i (sky)",
		"aw (saw)",
		"o (open)",
		"o (look)",
		"u (blood)",
		"oo (who)",
		"oi (join)",
		"ow (how)",
		"e (ladder)",
		"air",
		"a (also)",
		"er (fern)",
		"aw (saw)",
		"ear",
		"ur (sure)"
	};


	float uiColors[54] =
	{
		0.0f, 0.0f, 0.0f, // Blue/Gray\n"
		30.0f / 255.0f, 30.0f / 255.0f, 60.0f / 255.0f,
		50.0f / 255.0f, 50.0f / 255.0f, 100.0f / 255.0f,
		90.0f / 255.0f, 90.0f / 255.0f, 140.0f / 255.0f,
		150.0f / 255.0f, 150.0f / 255.0f, 200.0f / 255.0f,
		1.0f, 1.0f, 1.0f,

		63.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, // Green/Yellow
		127.0f / 255.0f, 190.0f / 255.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		100.0f / 255.0f, 0.0f, 60.0f / 255.0f, // Red
		160.0f / 255.0f, 0.0f, 40.0f / 255.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 120.0f / 255.0f, // Blue
		0.0f, 90.0f / 255.0f, 180.0f / 255.0f,
		0.0f, 180.0f / 255.0f, 1.0f,

		63.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, // Accent colors
		127.0f / 255.0f, 190.0f / 255.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};


	
};


struct Screen
{
	Vector2 screenSize;
	float windowRatio = 1.0f;

	Vector2 mouseCoords;
	bool mouseDown = false;
	bool rightMouseDown = false;
	float mouseHoldTime = 0.0f; // The amount of time that the mouse has been held down.
	bool keyDown = false;
	bool shiftDown = false;
	bool ctrDown = false;
};


struct Editor
{
	bool playingSong = false;
	bool recordingSong = false;
	bool focusOnNotes = false; // Whether pressing keys will create notes.
	bool channelFocus = false; // When enabled, only one channel will be uncompressed at a time.


	// When a channel is being dragged to a new location.
	int channelBeingMoved = -1;
	int channelMoveTo = -1;

	// Editing settings
	int selectedOctave = 3;
	// Selectable Buttons:
	// 0 = Song name, 1 = BPM, 2 = TPB, 3 = OCT, 4 = ROW, 5 = BEAT, 6 = Artist name, 7 = notes window, 8 = sample name, 9-27 = None, 28 = Resample Size
	int selectedButton = -1;
	int selectedSample = 0;
	int selectedKey = -1;
	int selectedFile = 0;
	int selectedChannel = 0;
	int selectedVoiceSample = 0;

	Vector2 noteSelectionStart;
	Vector2 noteSelectionEnd;
	int transposeValue = 0; // The number of semitones to transpose by if the transpose window is used.

	std::string fileToLoad = "";



	int findNotePlayed(int input)
	{
		int noteNum = -1;

		if (input == 122) noteNum += 1; // z (C 1)
		else if (input == 115) noteNum += 2; // s (C# 1)	
		else if (input == 120) noteNum += 3; // x (D 1)
		else if (input == 100) noteNum += 4; // d (D# 1)
		else if (input == 99) noteNum += 5; // c (E 1)
		else if (input == 118) noteNum += 6; // v (F 1)
		else if (input == 103) noteNum += 7; // g (F# 1)
		else if (input == 98) noteNum += 8; // b (G 1)
		else if (input == 104) noteNum += 9; // h (G# 1)
		else if (input == 110) noteNum += 10; // n (A 2)
		else if (input == 106) noteNum += 11; // j (A# 2)
		else if (input == 109) noteNum += 12; // m (B 2)
		else if (input == 44) noteNum += 13; // , (C 2)
		else if (input == 108) noteNum += 14; // l (C# 2)
		else if (input == 46) noteNum += 15; // . (D 2)
		else if (input == 59) noteNum += 16; // ; (D# 2)
		else if (input == 47) noteNum += 17; // / (E 2)

		selectedKey = noteNum;


		if (input == 113) noteNum += 13; // q (C 2)
		else if (input == 50) noteNum += 14; // 2 (C# 2)
		else if (input == 119) noteNum += 15; // w (D 2)
		else if (input == 51) noteNum += 16; // 3 (D# 2)
		else if (input == 101) noteNum += 17; // e (E 2)
		else if (input == 114) noteNum += 18; // r (F 2)
		else if (input == 53) noteNum += 19; // 5 (F# 2)
		else if (input == 116) noteNum += 20; // t (G 2)
		else if (input == 54) noteNum += 21; // 6 (G# 2)
		else if (input == 121) noteNum += 22; // y (A 3)
		else if (input == 55) noteNum += 23; // 7 (A# 3)
		else if (input == 117) noteNum += 24; // u (B 3)
		else if (input == 105) noteNum += 25; // i (C 3)
		else if (input == 57) noteNum += 26; // 9 (C# 3)
		else if (input == 111) noteNum += 27; // o (D 3)
		else if (input == 48) noteNum += 28; // 0 (D# 3)
		else if (input == 112) noteNum += 29; // p (E 3)
		else if (input == 91) noteNum += 30; // [ (F 3)
		else if (input == 61) noteNum += 31; // = (F# 3)
		else if (input == 93) noteNum += 32; // ] (A 3)

		if (selectedKey == -1)
		{
			if (noteNum == -1)
				selectedKey = -1;
			else
				selectedKey = noteNum + 5;
		}
		if (noteNum != -1)
		{
			noteNum += selectedOctave * 12;

			if (noteNum > 255)
				noteNum = 255;

		}

		return(noteNum);
	}


	int findEDONotePlayed(int input, int edo)
	{
		int noteNum = -1;

		if (input == 122) noteNum = 1;
		else if (input == 120) noteNum = 2;
		else if (input == 99) noteNum = 3;
		else if (input == 118) noteNum = 4;
		else if (input == 98) noteNum = 5;
		else if (input == 110) noteNum = 6;
		else if (input == 109) noteNum = 7;
		else if (input == 44) noteNum = 8;
		else if (input == 46) noteNum = 9;
		else if (input == 47) noteNum = 10;

		else if (input == 97) noteNum = 11;
		else if (input == 115) noteNum = 12;
		else if (input == 100) noteNum = 13;
		else if (input == 102) noteNum = 14;
		else if (input == 103) noteNum = 15;
		else if (input == 104) noteNum = 16;
		else if (input == 106) noteNum = 17;
		else if (input == 107) noteNum = 18;
		else if (input == 108) noteNum = 19;
		else if (input == 59) noteNum = 20;
		else if (input == 39) noteNum = 21;

		else if (input == 113) noteNum = 22;
		else if (input == 119) noteNum = 23;
		else if (input == 101) noteNum = 24;
		else if (input == 114) noteNum = 25;
		else if (input == 116) noteNum = 26;
		else if (input == 121) noteNum = 27;
		else if (input == 117) noteNum = 28;
		else if (input == 105) noteNum = 29;
		else if (input == 111) noteNum = 30;
		else if (input == 112) noteNum = 31;
		else if (input == 91) noteNum = 32;
		else if (input == 93) noteNum = 33;

		else if (input == 49) noteNum = 34;
		else if (input == 50) noteNum = 35;
		else if (input == 51) noteNum = 36;
		else if (input == 52) noteNum = 37;
		else if (input == 53) noteNum = 38;
		else if (input == 54) noteNum = 49;
		else if (input == 55) noteNum = 40;
		else if (input == 56) noteNum = 41;
		else if (input == 57) noteNum = 42;
		else if (input == 48) noteNum = 43;
		else if (input == 45) noteNum = 44;
		else if (input == 61) noteNum = 45;

		selectedKey = noteNum - 1;

		if (noteNum != -1)
		{
			noteNum -= 1;

			noteNum += selectedOctave * edo;

			if (noteNum > 255)
				noteNum = 255;

		}

		return(noteNum);
	}


	int findVoiceSamplePlayed(int input)
	{
		if (input == 122) return 0;
		else if (input == 120) return 1;
		else if (input == 99) return 2;
		else if (input == 118) return 3;
		else if (input == 98) return 4;
		else if (input == 110) return 5;
		else if (input == 109) return 6;
		else if (input == 44) return 7;
		else if (input == 46) return 8;
		else if (input == 47) return 9;

		else if (input == 97) return 10;
		else if (input == 115) return 11;
		else if (input == 100) return 12;
		else if (input == 102) return 13;
		else if (input == 103) return 14;
		else if (input == 104) return 15;
		else if (input == 106) return 16;
		else if (input == 107) return 17;
		else if (input == 108) return 18;
		else if (input == 59) return 19;
		else if (input == 39)  return 20;

		else if (input == 113) return 21;
		else if (input == 119) return 22;
		else if (input == 101) return 23;
		else if (input == 114) return 24;
		else if (input == 116) return 25;
		else if (input == 121) return 26;
		else if (input == 117) return 27;
		else if (input == 105) return 28;
		else if (input == 111) return 29;
		else if (input == 112) return 30;
		else if (input == 91) return 31;
		else if (input == 93) return 32;

		else if (input == 49) return 33;
		else if (input == 50) return 34;
		else if (input == 51) return 35;
		else if (input == 52) return 36;
		else if (input == 53) return 37;
		else if (input == 54) return 38;
		else if (input == 55) return 39;
		else if (input == 56) return 40;
		else if (input == 57) return 41;
		else if (input == 48) return 42;
		else if (input == 45) return 43;
		else if (input == 61) return 44;
		else return 44;
	}
};


struct ChannelWaveform
{
	float frameReadPos = 0.0f;

	float pitch = 0.0f;
	float volume = 1.0f;
	float volumeSlide = 0.0f;
	int note = 0;

	// Envelope
	float envelopePos = 0.0f;
	float envelopeVol = 1.0f;
	float envelopeReleasePointVol = 0.0f;

	float glideDest = 0.0f;
	float glideVolume = 0.0f;

	bool reverse = false;


	// Low-pass filter.
	float y1, y2, y3, y4;
	float oldx, oldy1, oldy2, oldy3;

	// Filter parameters
	float cutoff = 1000.0f;      // Cutoff frequency in Hz

	// Coefficients
	float f, fb, q;


	// Process a single sample (4-pole Moog-style ladder filter)
	float process(float input)
	{
		// Apply feedback to create resonance
		input -= y4 * fb;
		input *= 0.35013f * (f * f) * (f * f);

		// Four cascaded one-pole filters
		y1 = input + 0.3f * oldx + (1.0f - f) * y1;
		oldx = input;

		y2 = y1 + 0.3f * oldy1 + (1.0f - f) * y2;
		oldy1 = y1;

		y3 = y2 + 0.3f * oldy2 + (1.0f - f) * y3;
		oldy2 = y2;

		y4 = y3 + 0.3f * oldy3 + (1.0f - f) * y4;
		oldy3 = y3;

		y4 = std::max(-1.0f, std::min(y4, 1.0f));

		return y4;
	}
};


// Audio channel
struct Channel
{
	int frameOffset = 0; // If set more than 0, decrement.
	int offsetNote = 0;
	int offsetInstrument = 0;
	int offsetPos = 0;

	bool muted = false;
	bool compressed = false;
	bool toUninitialize = false;

	bool noteStopped = false; // When true, skip sustain and go to release.

	bool hasVoiceColumns = false; // If enables, the channel has columns for voice sounds.

	
	ChannelWaveform waveforms[4];


	float panValue = 0.0f;
	float panSlide = 0.0f;

	float volume = 1.0f;
	float volumeSlide = 0.0f;

	float pitchSlide = 0.0f;

	float retrigger = 0.0f;
	float retriggerSlide = 0.0f;

	float fuzzLevel = 1.0f; // Larger values add more fuzz.
	float fuzzSlide = 0.0f;

	float sampleRate = 1.0f;
	float sampleRateSlide = 0.0f;


	// Channel arp pitches.
	float arpP[15] = { 7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f };
	float arpTimer = 0.0f;
	int arpIndex = -1;
	float arpOriginalNote = 0.0f;

	float jumpSlide = 0.0f;
	float jumpPoint = 0;

	int instrument = 0;


	float averageVolL = 0.0f; // The volume of the channel averaging frames.
	float averageVolR = 0.0f; // The volume of the channel averaging frames.


	int effectCountPerRow = 1; // The number of effects displayed on each line.


	float lowPass = 0.0f;
	float lowPassSlide = 0.0f;
	float highPass = 0.0f;
	float highPassSlide = 0.0f;

	float RC = (1.0f / (2.0f * 3.14159265f * 1.0f));
	float alphaHigh = RC / (RC + (1.0f / 48000.0f));


	float prevLowPassSampleL = 0.0f;      // Previous output sample (for filtering)
	float prevLowPassSampleR = 0.0f;      // Previous output sample (for filtering)
	float prevHighPassSampleL = 0.0f;      // Previous output sample (for filtering)
	float prevHighPassSampleR = 0.0f;      // Previous output sample (for filtering)
	float prevHighPassSampleLI = 0.0f;      // Previous output sample (for filtering)
	float prevHighPassSampleRI = 0.0f;      // Previous output sample (for filtering)

	/*
	float reverbCombVolL = 0.0f;
	float reverbCombVolR = 0.0f;
	float pastVolsL[128] = {0.0f};
	float pastVolsR[128] = {0.0f};
	*/
};



// 1 channel of a frame.
struct FrameChannel
{
	std::vector <int> notes;
	std::vector <int> volumes;
	std::vector <int> effects;

	// Optional voice samples.
	std::vector <int> voiceSamples;
};



// Song frame object
struct Frame
{
	uint8_t rows = 32; // Length of the frame in beats.
	int beatsPerMeasure = 4;
	std::vector <FrameChannel> channels;
};




struct UnrolledEffects
{
	std::vector <int> cEffect = { -1 };
	std::vector <int> cEffectValue = { -1 };
};


struct UnrolledVoiceSamples
{
	int sample[5] = { 44, 44, 44, 44, 44 };
};


// 1 row of an unrolled frame.
struct FrameRow
{
	std::vector <int> note = { };
	std::vector <int> instrument = { };
	std::vector <int> volume = { };
	std::vector <UnrolledEffects> effects = { };
	
	// Optional voice samples.
	std::vector <UnrolledVoiceSamples> voiceSamples = { };
};



struct UnrolledFrame
{
	// Frame format
	std::vector <FrameRow> rows;
	int beatsPerMeasure = 4;
};



struct Song
{
	std::string songName = "NewSong1";
	std::string artistName = "Me";
	std::string notes = "";

	int edo = 12; // Equal divisions of an octave. (Number of tones)
	float startingBPM = 120;
	float bpm = 120;

	std::vector <int> frameSequence;

	std::vector <Frame> frames;



	int currentFrame = 0; // Current frame in frameSequence.
	int currentNote = 0;
	float timeInNote = 0;
	float timeInSong = 0;

	int numberOfChannels = 1;

	std::vector <float> toNextChannelNote = { };
	std::vector <float> toNextChannelVolume = { };
	std::vector <float> toNextChannelEffect = { };

	std::vector <float> noteChannelIndex = { };
	std::vector <float> volumeChannelIndex = { };
	std::vector <float> effectChannelIndex = { };


	bool unsavedChanges = false;
	bool overWriteOldSong = false;
	bool unusedFrames = false;
};



struct SampleDisplay
{
	bool visible = false;

	bool zoomed = false;

	bool displayArp = false;

	Vector2i position;
	RGBColor pixelData[528 * 192]; // Screen data (528 x 160) pixels.

	//float zoomFactor = 1.0f; // Increase to zoom in on the sample.

	bool playingSample = false;

	bool drawing = false;
	Vector2 drawWavePos; // { pos in frames, volume }

	int sampleStartPos = 0;
	int sampleSelectionEnd = 0;

	int zoomStart = 0;
	int zoomEnd = 0;

	bool dragLoopStart = false;
	bool dragLoopEnd = false;


	float resampleNote = 48.0f;
	float resampleFineTune = 0;
	float resampleMultiplier = 1.0f;
	float volumeMultiplier = 1.0f;
	std::string resampleString = "000000000000000000";



	bool enableSnap = true;
	int snapSubdivisions = 16;

	int measurementSystem = 0; // 0 = frames, 1 = ms, 2 = seconds



	int selectedOperator = 0;
	int operatorMenuSelectedOperator = 0; // The operator selected in the operator menu.
	int swapMenuOperators[4] = { 1, 2, 3, 4 };


	std::vector <float> copyFrames = {};


	Sample SwapOperators(Sample sample)
	{
		drawing = false; // Stop sample drawing.
		zoomed = false; // Reset zoom.
		SampleWave swapWaves[4];

		for (int wave = 0; wave < 4; wave++)
			swapWaves[wave] = sample.waveforms[swapMenuOperators[wave]];

		for (int wave = 0; wave < 4; wave++)
			sample.waveforms[wave] = swapWaves[wave];

		return sample;
	}
};



struct FloatingWindow
{
	std::string name = "Floating Window";
	Vector2i position;
	Vector2i size;
	bool dragWindow = false;
	Vector2i dragPoint;
};



struct WindowController
{
	std::vector <FloatingWindow> windows;


	

	bool HasWindow(std::string windName)
	{
		bool exists = false;

		for (int i = 0; i < windows.size(); i++)
		{
			if (windows[i].name == windName)
				exists = true;
		}

		return exists;
	};


	void InitializeWindow(std::string windowName, Vector2i windPos, Vector2i windSize)
	{
		if (HasWindow(windowName)) // Don't create copies of existing windows.
			return;

		FloatingWindow newWindow;

		for (int i = 0; i < windows.size(); i++) // Move the window so that it doesn't fall on the exact same spot as old ones.
		{
			if (windows[i].position.x == windPos.x)
				windPos.x++;
			if (windows[i].position.y == windPos.y)
				windPos.y++;
		}

		if (windPos.x < 0) windPos.x = 0;
		if (windPos.x + windSize.x > 91) windPos.x = 91 - windSize.x;
		if (windPos.y < 0) windPos.y = 0;
		if (windPos.y + windSize.y > 56)windPos.y = 56 - windSize.y;

		if (windSize.x < windowName.length() + 6) windSize.x = (float)windowName.length() + 6;
		if (windSize.y < 3)  windSize.y = 3;
		if (windSize.y > 56)  windSize.y = 56;


		newWindow.name = windowName;
		newWindow.position = windPos;
		newWindow.size = windSize;


		windows.emplace(windows.begin(), newWindow);

		return;
	}
};



struct FileNavigator
{
	std::filesystem::path currentFilePath;

	std::string filePathName = "";

	std::vector <std::string> fileNames = {};



	// Currently, samples are saved with absolute file paths, making transferring songs between computers difficult.


	void NavigateToFile()
	{
		std::string pathName = "C:/" + currentFilePath.std::filesystem::path::string();

		filePathName = " " + currentFilePath.std::filesystem::path::filename().generic_string();

		fileNames.clear();
		for (auto const& dir_entry : std::filesystem::directory_iterator(pathName))
		{
			std::string sampleName;
			if (!dir_entry.path().std::filesystem::path::has_extension())
				sampleName = "0" + dir_entry.path().std::filesystem::path::filename().generic_string();
			else if (dir_entry.path().std::filesystem::path::extension() == ".song")
				sampleName = "1" + dir_entry.path().std::filesystem::path::filename().generic_string();
			else if (dir_entry.path().std::filesystem::path::extension() == ".wav")
				sampleName = "2" + dir_entry.path().std::filesystem::path::filename().generic_string();
			else
				sampleName = "3" + dir_entry.path().std::filesystem::path::filename().generic_string();

			fileNames.emplace_back(sampleName);
		}

		



		

		return;
	}

	void ExitFile()
	{
		currentFilePath = currentFilePath.std::filesystem::path::parent_path();
		
		NavigateToFile();

		return;
	}

	void EnterFile(std::string fileName)
	{
		currentFilePath = currentFilePath.std::filesystem::path::append(fileName);

		NavigateToFile();

		return;
	}

	void NavigateToSamplesFile()
	{
		currentFilePath = std::filesystem::relative("Samples", "C:/");

		NavigateToFile();

		return;
	}

};



struct PresetMenu
{
	// 32 * 6 = 192
	int instrumentType = 0;

	int selectedSample = 0;

	std::string presetNames[192] = {
		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",
		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",

		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",
		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",

		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",
		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",

		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",
		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",

		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",
		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",

		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",
		"(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)","(...)",
	};
};