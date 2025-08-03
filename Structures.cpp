

#include <vector>
#include <fstream>


// Floating-point 2d position
struct Vector2 { float x = 0; float y = 0; };


// UI element
struct UIElement
{
	Vector2 sprite;
	int textCol = 0; int bgCol = 0; // Colors for text and background.
};


// Audio channel
struct Channel
{
	bool muted = false;
	float volume = 1;
	float soundVolume = 0.0f;

	float effect = 0;
	float effectX = 0;
	float effectY = 0; // The second effect parameter for effects that use two.
	float pitch = 0;
	float realPitch = 0;


	std::vector <int> loopNoteIndex = {};
	std::vector <int> loopVolumeIndex = {};
	std::vector <int> loopEffectIndex = {};
};


// 24-bit color
struct RGBColor
{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
};


// An audio sample
struct Sample
{
	std::string sampleName = "NewSample";
};


// 1 channel of a frame.
struct FrameChannel
{
	std::vector <int> notes;
	std::vector <int> volumes;
	std::vector <int> effects;
};


// Song frame object
struct Frame
{
	uint8_t rows = 32; // Length of the frame in beats.
	FrameChannel channels[8];
};


// 1 row of an unrolled frame.
struct FrameRow
{
	int note[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	int instrument[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	int volume[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	int effect[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	int effectValue[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
};



// An unrolled frame object
struct UnrolledFrame
{
	// Frame format
	std::vector <FrameRow> rows;
};


// Song object
struct Song
{
	std::string songName = "NewSong";

	float bpm = 120;
	float ticksPerBeat = 6;

	std::vector <int> frameSequence;

	std::vector <Frame> frames;



	int currentFrame = 0; // Current frame in frameSequence.
	int currentNote = 0;
	float timeInNote = 0;
	float timeInTick = 0;
	float timeInSong = 0;


	int toNextChannelNote[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int toNextChannelVolume[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int toNextChannelEffect[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	int noteChannelIndex[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int volumeChannelIndex[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int effectChannelIndex[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
};



// Effects:

// -1 - No effect:

// 0 - Cosmic Arp: (Done)
//		Play alternating notes every tick, between the base note and the two offsets.
//		-x = semitone offset 1
//		-y = semitone offset 2

// 1 - Vibrato: (Done)
//		Change the note pitch over time.
//		-x = speed (1 cycle in x loops)
//		-y = depth (y/4 semitones)

// 2 - Tremolo: (Done)
//		Change the note volume over time.
//		-x = speed (1 cycle in x loops)
//		-y = depth (y/4 semitones)

// 3 - Set Panning: (Done)
//		Set the stereo panning position from 00(Left) to FF(Right).
//		-xy = position

// 4 - Volume Slide: (Done)
//		Slide the volume up by x speed or down by y speed.
//		-x = volume up (x/4 semitones per beat)
//		-y = volume down (x/4 semitones per beat)

// 5 - Pitch Slide: (done)
//		Slide the pitch up by x speed or down by y speed.
//		-x = pitch up (x semitones per beat)
//		-y = pitch down (x semitones per beat)

// 6 - Pitch Slide (fine): (done)
//		Slide the pitch up by x speed or down by y speed.
//		-x = speed up (x/4 semitones per beat)
//		-y = speed down (x/4 semitones per beat)

// 7 - Jump to frame:
//		Jump the the specified frame.
//		-xx = frame

// 8 - Note delay:
//		Start the note late, after x ticks.
//		-xx = ticks

// 9 - Sample offset:
//		Offset the sample starting on the beat by x ticks.
//		-xx = ticks

// A - Set BPM:
//		Set the song BPM to x.
//		-xx = BPM

// B - Set TPB:
//		Set the song TPB to x.
//		-xx = TPB

// C - Low Pass:
//		A Low pass filter.
//		-xx size.
// 
// D - High Pass:
//		A High pass filter.
//		-xx size.
// 
// E - Loop section:
//		Loop the next x beats y times.
//		Other notes, instruments, volumes and effects play over the repeated beats and take priority.
//		The loop ends when another loop effect is encountered. (A loop of length 0 stops loops entirely).
//		-xx = Number of beats.

// F - (To be decided)