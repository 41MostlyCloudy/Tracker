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
	"SUPER SOUND",
	"",
	"",
	"",
	"",
	"#",
	"ABOUT",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"#",
	"HEXADECIMALS",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"#",
	"EFFECTS",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};