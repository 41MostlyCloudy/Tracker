#pragma once

#include "Structures.cpp"


#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <chrono> // Deals with time
#include <stdlib.h> // For rand()



float delta = 0.0f; // Time between frames.

bool playingSong = false;
bool editing = true;
bool recordingSong = false;
bool inHelpPage = false;

bool drawScreen = false;

//int uiColorTheme = 0;


Vector2 mouseCoords;
bool mouseDown = false;
bool keyDown = false;
float mouseHoldTime = 0.0f; // The amount of time that the mouse has been held down.
Vector2 hoveredTile;
Vector2 screenSize;
float windowRatio;

// 92 x 57 tile ui.
UIElement activeUI[92][57]; // Active user interface elements on the screen.

Channel channels[8];

Song loadedSong;
UnrolledFrame loadedFrame; // The frame currently in the editor.
std::vector <std::string> fileNameList; // The names of the samples in the "Samples" file.
std::vector <Sample> loadedSamples; // The names of the samples used in the song.
UnrolledFrame frameSelection; // A frame containing the portion of the frame that is selected, with that number of rows. All values not used are set to -2.


// Editing settings
int selectedOctave = 3;
// Selectable Buttons:
// 0 = Song name
int selectedButton = -1;
Vector2 selectedTile;
int selectedSample = 0;
int selectedKey = -1;
int selectedEffect = -1;
int selectedFile = 0;
bool filesSampleNotSong = true; // Whether you are loading samples or songs.


int frameListScroll = 0;
int fileListScroll = 0;
int sampleListScroll = 0;
float frameScroll = 0.0f;

Vector2 noteSelectionStart;
Vector2 noteSelectionEnd;


uint8_t uiColors[12][3] =
{
	{0, 0, 0}, // Blue/Gray
	{30, 30, 60},
	{50, 50, 100},
	{90, 90, 140},
	{150, 150,200},
	{255, 255, 255},

	{63, 100, 100}, // Green/Yellow
	{127, 190, 0},
	{255, 255, 0},

	{100, 0, 60}, // Red
	{160, 0, 40},
	{255, 0, 0},
};



std::vector <std::string> helpPageText =
{
	"WELCOME TO SUPER SOUND",
	"",
	"",
	"",
	"",
	"#",
	"ABOUT",
	"",
	"",
	"",
	"SUPER SOUND is a music making program inspired by MilkyTracker.",
	"",
	"SUPER SOUND allows for the creation of music from sequenced",
	"samples using a music tracker-styled interface.",
	"",
	"",
	"",
	"#",
	"HEXADECIMALS",
	"",
	"",
	"",
	"Hexidecimal numbers are used throughout SUPER SOUND.",
	"",
	"While regular numbers have 10 different numerals,",
	"hexadecimals have 16, represented by the numbers",
	"0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, and F.",
	"",
	"For example, the number 10 would be represented as A in hexidecimal,",
	"and the number 255 would be represented as FF.",
	"",
	"",
	"",
	"#",
	"FILES",
	"",
	"",
	"",
	"Each song is stored in a .song file in the Songs file.",
	"",
	"Each sample is stored in a .wav file in the Samples file.",
	"",
	"Samples and songs in these are displayed in the box",
	"under the FILES button.",
	"",
	"Samples loaded into the song are displayed in the box",
	"under (samples).",
	"",
	"A new sample or file can be loaded into a song be selecting",
	"it and then pressing the arrow to its left.",
	"",
	"",
	"",
	"#",
	"IMPORTANT BUTTONS",
	"",
	"",
	"",
	"RECORD",
	"",
	"    When the song is played and record is selected,",
	"    the song will be recorded to a .wav file in the Export folder.",
	"",
	"FILES",
	"",
	"    This button can be pressed to swap between loading samples from the Samples file,",
	"    and songs from the Songs file.",
	"",
	"SAVE",
	"",
	"    This button saves the currently loaded song, overriding any song of the same name.",
	"",
	"",
	"",
	"#",
	"SONG SETTINGS",
	"",
	"",
	"",
	"BPM",
	"",
	"    This number specifies the speed of the song in beats per minute.",
	"",
	"TPB",
	"",
	"    This number specifies the resolution of ticks between each beat, used in some effects.",
	"",
	"OCT",
	"",
	"    This number specifies the octave of the notes that are played on the keyboard and.",
	"    added to the song.",
	"",
	"ROW",
	"",
	"    This number specifies number of rows in the frame that you are currently editing.",
	"",
	"",
	"",
	"#",
	"SONGS",
	"",
	"",
	"",
	"A loaded song is made up of frames, selectable on the left side of the interface.",
	"",
	"Each number in the frame list represents a frame,",
	"with repeating numbers being repeated frames.",
	"",
	"Each frame is then made up of 8 different channels,",
	"represented by the numbered bubbles across the screen.",
	"",
	"",
	"",
	"#",
	"CHANNELS",
	"",
	"",
	"",
	"Each channel in a song frame can play one sound at a time.",
	"",
	"The rows going down the screen are a timeline of the notes being played.",
	"",
	"Parts of a channel",
	"",
	"    The leftmost value in a channel is the note pitch.",
	"    When selected, a key can be pressed to enter a note of the corresponding pitch.",
	"",
	"    Next is the note sample.",
	"    This number specifies the sample to be played on the note as an index",
	"    in the list of loaded samples.",
	"",
	"    After that is the volume value.",
	"    This sets the volume for the note and all future notes until changed again.",
	"",
	"    Then is the effect number.",
	"    This sets the effect for the note and all future notes until changed again.",
	"    Different effects will make different changes to the note or song.",
	"",
	"    To the furthest right is the effect value.",
	"    This specifies a parameter or two for the specified effect.",
	"    What is does depends on the effect.",
	"",
	"",
	"",
	"#",
	"EFFECTS",
	"",
	"",
	"",
	"Effects can be added to the song to change different things.",
	"",
	"Effect Types (from top to bottom in the interface)",
	"",
	"    Set Pan",
	"",
	"        This effect sets the spacial position of the channel audio,",
	"        from left (00), to right (FF).",
	"",
	"    Volume Slide",
	"",
	"        This effect slides th volume up or down until another effect stops it.",
	"        The left effect value gives a speed to increase the volume.",
	"        The right effect value gives a speed to decrease the volume.",
	"",
	"    Jump to Frame",
	"",
	"        This effect sets the position of the song to the frame",
	"        of the index in the song given by the effect value.",
	"",
	"    Offset Note",
	"",
	"        This effect offsets the note on the same row by the",
	"        number of ticks given by the effect value.",
	"",
	"    Set BPM",
	"",
	"        This effect sets the song BPM to the effect value.",
	"",
	"    Set TPB",
	"",
	"        This effect sets the song TPB to the effect value.",
	"",
	"",
	"",
	"",
};