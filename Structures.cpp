

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
	uint8_t rows = 64; // Length of the frame in beats.
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
};