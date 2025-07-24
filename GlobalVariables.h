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

bool drawScreen = false;


Vector2 mouseCoords;
bool mouseDown = false;
float mouseHoldTime = 0.0f; // The amount of time that the mouse has been held down.
Vector2 hoveredTile;
Vector2 screenSize;
float windowRatio;

// 92 x 57 tile ui.
UIElement activeUI[92][57]; // Active user interface elements on the screen.

Channel channels[8];

Song loadedSong;
UnrolledFrame loadedFrame; // The frame currently in the editor.
std::vector <Sample> fileSamples; // The names of the samples in the "Samples" file.
std::vector <Sample> loadedSamples; // The names of the samples used in the song.


// Editing settings
int selectedOctave = 4;
// Selectable Buttons:
// 0 = Song name
int selectedButton = -1;
Vector2 selectedTile;
int selectedSample = 0;
int selectedFile = 0;


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