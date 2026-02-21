#pragma once



void DrawEverything();

void DrawTopUI();
void DrawKeyBoard();
void DrawEDOKeyboard();
void DrawVoiceKeyboard();
void DrawLogo();

void DrawKey(int button, int key, Vector2i pos);
void DrawEDOKey(int key, Vector2i pos);
void DrawVoiceKey(int key, Vector2i pos);

void DrawMute();
int DrawChannelMuteGlass(int xPos, int i);
int DrawChannelMuteSynth(int xPos, int i);
int DrawChannelMuteModulator(int xPos, int i, int dialNum);
void DrawChannelNumber(int channelNum, int offsetX);

void DrawFrameBorder();
void DrawVolumeBars();

void DrawChannel(int channelNum, int offsetX);
void DrawChannelLine(int channelNum, int offsetX, int y);
void DrawChannelLineValues(int channelNum, int offsetX, int y);
void DrawChannelCompressed(int channelNum, int offsetX);

void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol);
void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);
void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, bool continueDown);

void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol);
void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol);
void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawNote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);
void DrawEDONote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawFloatingWindow(FloatingWindow* wind);

void DrawWaveTypeButton(int type, int xVal, int yVal);

void DrawHorizontalSlider(int xVal, int yVal, float value);

void DrawDial(Vector2 pos, float val, int style);

void DrawBackground(float startPos);

void DrawEffect(Vector2 pos, int type, int value);

void DrawAlgorithm(Vector2i pos, int type, bool selected);



void DrawEverything()
{

	// Scroll vertically when playing the song.
	if (editor.playingSong)
	{
		gui.frameScroll.y = loadedSong.currentNote - 8;

		if (gui.frameScroll.y > float(loadedFrame.rows.size()) - 40)
			gui.frameScroll.y = float(loadedFrame.rows.size()) - 40;


		if (gui.frameScroll.y < 0)
			gui.frameScroll.y = 0;
	}

	gui.drawScreen = true;

	if (gui.drawFrameThisFrame)
	{
		DrawMute();
		DrawFrameBorder();
		
		int xStart = 5;
		for (int i = 0; i < loadedSong.numberOfChannels; i++)
		{
			if (channels[i].compressed)
				DrawChannelCompressed(i, xStart - gui.frameScroll.x);
			else
				DrawChannel(i, xStart - gui.frameScroll.x);

			if (channels[i].compressed)
				xStart += 3;
			else
			{
				xStart += 8 + channels[i].effectCountPerRow * 4;

				// Voice samples (if enabled in channel).
				if (channels[i].hasVoiceColumns)
					xStart += 5;
			}
		}

		DrawTopUI();
	}
	else if (gui.drawUIThisFrame)
	{
		DrawTopUI();
	}

	DrawVolumeBars();


	for (int wind = windowController.windows.size() - 1; wind > -1; wind--) // Draw windows.
		DrawFloatingWindow(&windowController.windows[wind]);

	return;
}



void  DrawTopUI()
{
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 92; x++)
		{
			if (gui.lightMode)
			{
				gui.activeUI[x][y].bgCol = 4;
				gui.activeUI[x][y].sprite = { 18, 0 };
			}
			else
			{
				gui.activeUI[x][y].bgCol = 0;
				gui.activeUI[x][y].sprite = { 15, 0 };
			}
		}
	}


	// Song editor
	DrawGUIText("Song", 1, 5, 0, 3, 0);
	DrawGUIText("Num", 7, 10, 4, 3, 0);
	DrawGUIText("Add", 7, 10, 7, 3, 0);

	// Frame list
	gui.activeUI[0][1].sprite = { 0, 3 };
	gui.activeUI[1][1].sprite = { 2, 3 };
	gui.activeUI[2][1].sprite = { 2, 3 };
	gui.activeUI[3][1].sprite = { 3, 3 };
	gui.activeUI[4][1].sprite = { 2, 3 };
	gui.activeUI[5][1].sprite = { 2, 3 };
	gui.activeUI[6][1].sprite = { 1, 3 };

	gui.activeUI[0][12].sprite = { 0, 4 };
	gui.activeUI[1][12].sprite = { 2, 3 };
	gui.activeUI[2][12].sprite = { 2, 3 };
	gui.activeUI[3][12].sprite = { 4, 4 };
	gui.activeUI[4][12].sprite = { 3, 3 };
	gui.activeUI[6][12].sprite = { 4, 4 };

	for (int y = 2; y < 12; y++)
	{
		gui.activeUI[0][y].sprite = { 2, 4 };
		gui.activeUI[3][y].sprite = { 2, 4 };
	}
	for (int y = 3; y < 10; y++)
		gui.activeUI[6][y].sprite = { 31, 3 };

	gui.activeUI[6][3].sprite = { 31, 2 };
	gui.activeUI[6][10].sprite = { 31, 4 };

	for (int y = 0; y < 10; y++)
	{
		if (y + gui.frameListScroll < loadedSong.frameSequence.size())
		{
			
			if (y + gui.frameListScroll == loadedSong.currentFrame)
			{
				DrawHex(loadedSong.frameSequence[y + gui.frameListScroll], 4, 2 + y, 5, 3, -1, -1);
				DrawHex(y + int(gui.frameListScroll), 1, 2 + y, 4, 3, -1, -1);
				gui.activeUI[3][y + 2].sprite = { 5, 4 };

				if (gui.lightMode)
				{
					gui.activeUI[4][y + 2].textCol = 5;
					gui.activeUI[5][y + 2].textCol = 5;
				}
			}
			else
			{
				DrawHex(loadedSong.frameSequence[y + gui.frameListScroll], 4, 2 + y, 5, 0, -1, -1);
				DrawHex(y + int(gui.frameListScroll), 1, 2 + y, 3, 0, -1, -1);
			}
		}
		else
		{
			if (gui.lightMode)
			{
				gui.activeUI[4][y + 2].sprite = { 18, 0 };
				gui.activeUI[4][y + 2].bgCol = 4;
				gui.activeUI[5][y + 2].sprite = { 18, 0 };
				gui.activeUI[5][y + 2].bgCol = 4;
			}
			else
			{
				gui.activeUI[4][y + 2].sprite = { 15, 0 };
				gui.activeUI[4][y + 2].bgCol = 0;
				gui.activeUI[5][y + 2].sprite = { 15, 0 };
				gui.activeUI[5][y + 2].bgCol = 0;
			}
			DrawHex(y + int(gui.frameListScroll), 1, 2 + y, 3, 0, -1, -1);
		}
	}

	

	// Edit button box
	for (int x = 7; x < 10; x++)
	{
		gui.activeUI[x][3].sprite = { 2, 3 };
		gui.activeUI[x][9].sprite = { 2, 3 };
	}
	for (int y = 4; y < 9; y++)
	{
		gui.activeUI[10][y].sprite = { 2, 4 };
	}
	gui.activeUI[10][3].sprite = { 1, 3 };
	gui.activeUI[10][9].sprite = { 1, 4 };


	// Song/Frame settings box
	DrawGUIText("Settings", 11, 21, 0, 3, 0);
	gui.activeUI[10][1].sprite = { 0, 3 };
	gui.activeUI[18][1].sprite = { 1, 3 };

	
	for (int x = 11; x < 18; x++)
	{
		gui.activeUI[x][1].sprite = { 2, 3 };
	}
	for (int y = 2; y < 12; y++)
	{
		gui.activeUI[10][y].sprite = { 2, 4 };
		gui.activeUI[18][y].sprite = { 2, 4 };
		gui.activeUI[35][y].sprite = { 2, 4 };
		gui.activeUI[42][y].sprite = { 2, 4 };
	}

	gui.activeUI[10][3].sprite = { 4, 3 };
	gui.activeUI[10][9].sprite = { 4, 3 };

	gui.activeUI[35][1].sprite = { 0, 3 };
	for (int x = 36; x < 42; x++)
		gui.activeUI[x][1].sprite = { 2, 3 };
	gui.activeUI[42][1].sprite = { 1, 3 };

	// Buttons
	DrawGUIText("BPM", 11, 14, 2, 3, 0);
	DrawNum(loadedSong.bpm, 15, 18, 2, 4, 0, -1, 1);
	DrawGUIText("EDO", 11, 14, 4, 3, 0);
	DrawNum(loadedSong.edo, 15, 17, 4, 4, 0, -1, 2);
	DrawGUIText("OCT", 11, 14, 6, 3, 0);
	DrawNum(editor.selectedOctave, 15, 16, 6, 4, 0, -1, 3);
	DrawGUIText("ROW", 11, 14, 8, 3, 0);
	DrawNum(loadedFrame.rows.size(), 15, 18, 8, 4, 0, -1, 4);
	DrawGUIText("BEAT", 11, 15, 10, 3, 0);
	DrawNum(loadedFrame.beatsPerMeasure, 15, 17, 10, 4, 0, -1, 5);



	// Song name box
	if (gui.uiDisplayMenuOption != 2) // Don't draw with Effect GUI.
	{
		DrawGUIText("Song Name", 46, 21 + 45, 0, 3, 0);




		gui.activeUI[44][1].sprite = { 0, 3 };
		gui.activeUI[44][2].sprite = { 2, 4 };
		gui.activeUI[44][3].sprite = { 2, 4 };
		gui.activeUI[44][4].sprite = { 0, 4 };

		gui.activeUI[22 + 44][1].sprite = { 1, 3 };
		gui.activeUI[22 + 44][2].sprite = { 2, 4 };
		gui.activeUI[22 + 44][3].sprite = { 2, 4 };
		gui.activeUI[22 + 44][4].sprite = { 1, 4 };
		for (int x = 0; x < 21; x++)
		{
			gui.activeUI[45 + x][1].sprite = { 2, 3 };
			gui.activeUI[45 + x][4].sprite = { 2, 3 };
		}
		DrawGUIText(loadedSong.songName, 45, 22 + 44, 2, 5, 0, -1, 0);
		DrawGUIText("BY:", 45, 22 + 44, 3, 5, 0, -1, -1);
		DrawGUIText(loadedSong.artistName, 48, 22 + 44, 3, 5, 0, -1, 6);
	}


	// GUI Display Menu
	DrawGUIText("Song", 36, 42, 2, 3, 0);
	DrawGUIText("Keys", 36, 42, 3, 3, 0);
	DrawGUIText("Effect", 36, 42, 4, 3, 0);
	DrawGUIText("Voice", 36, 42, 5, 3, 0);

	if (gui.lightMode)
	{
		for (int x = 36; x < 42; x++)
		{
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].textCol = 5;
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].bgCol = 1;
		}
	}
	else
	{
		for (int x = 36; x < 42; x++)
		{
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].textCol = 5;
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].bgCol = 3;
		}
	}

	gui.activeUI[35][gui.uiDisplayMenuOption + 2].sprite = { 5, 4 };
	

	

	DrawGUIText("Instruments", 70, 81, 0, 3, 0);
	
	


	// File select box
	for (int y = 3; y < 11; y++)
	{
		gui.activeUI[68][y].sprite = { 2, 4 };
		gui.activeUI[71][y].sprite = { 2, 4 };
		gui.activeUI[91][y].sprite = { 31, 3 };
	}
	for (int x = 68; x < 91; x++)
	{
		gui.activeUI[x][1].sprite = { 2, 3 };
	}
	gui.activeUI[68][2].sprite = { 2, 4 };
	gui.activeUI[68][11].sprite = { 2, 4 };
	gui.activeUI[71][2].sprite = { 2, 4 };
	gui.activeUI[71][11].sprite = { 2, 4 };

	gui.activeUI[91][3].sprite = { 31, 2 };
	gui.activeUI[91][10].sprite = { 31, 4 };

	for (int y = 2; y < 12; y++)
	{
		if (y - 2 + gui.sampleListScroll < 256 && y - 2 + gui.sampleListScroll >= 0) // Draw samples
		{
			if (!loadedSamples[y - 2 + gui.sampleListScroll].enabled)
			{
				if (y - 2 + gui.sampleListScroll == editor.selectedSample)
				{
					DrawGUIText("", 72, 91, y, 5, 3);
					DrawHex(y - 2 + int(gui.sampleListScroll), 69, y, 4, 3);
					gui.activeUI[71][y].sprite = { 5, 4 };
					gui.activeUI[90][y].sprite = { 26, 2 }; // Create sample button

					if (gui.lightMode)
					{
						for (int x = 69; x < 71; x++)
						{
							gui.activeUI[x][y].textCol = 5;
						}
					}
				}
				else
				{
					DrawGUIText("", 72, 91, y, 4, 0);
					DrawHex(y - 2 + int(gui.sampleListScroll), 69, y, 3, 0);
				}
			}
			else
			{
				if (y - 2 + gui.sampleListScroll == editor.selectedSample)
				{
					if (editor.selectedButton == 8)
					{
						if (gui.lightMode)
							DrawGUIText(loadedSamples[y - 2 + gui.sampleListScroll].sampleName, 72, 91, y, 5, 2, -1, 8);
						else
							DrawGUIText(loadedSamples[y - 2 + gui.sampleListScroll].sampleName, 72, 91, y, 5, 1, -1, 8);
					}
					else
						DrawGUIText(loadedSamples[y - 2 + gui.sampleListScroll].sampleName, 72, 91, y, 5, 3);
					gui.activeUI[71][y].sprite = { 5, 4 };
					DrawHex(y - 2 + int(gui.sampleListScroll), 69, y, 4, 3);
					gui.activeUI[90][y].sprite = { 31, 9 }; // Delete button

					if (gui.lightMode)
					{
						for (int x = 69; x < 91; x++)
						{
							gui.activeUI[x][y].textCol = 5;
						}
					}
				}
				else
				{
					DrawGUIText(loadedSamples[y - 2 + gui.sampleListScroll].sampleName, 72, 91, y, 4, 0);
					DrawHex(y - 2 + int(gui.sampleListScroll), 69, y, 3, 0);
				}
			}
		}
		else
		{
			DrawGUIText("", 72, 91, y, 4, 0);
			DrawHex(y - 2 + int(gui.sampleListScroll), 69, y, 2, 0);
		}
	}


	


	gui.activeUI[88][0].sprite = { 29, 10 }; // Minimize program button
	gui.activeUI[89][0].sprite = { 30, 10 };
	gui.activeUI[90][0].sprite = { 29, 9 }; // Exit program button
	gui.activeUI[91][0].sprite = { 30, 9 };
	
	
	

	gui.activeUI[68][1].sprite = { 0, 3 };
	gui.activeUI[71][1].sprite = { 3, 3 };
	gui.activeUI[91][1].sprite = { 1, 3 };


	// Logo
	DrawLogo();
	

	// UI Buttons.
	DrawGUIText("FILE", 20, 34, 8, 4, -1);
	DrawGUIText("SETTINGS", 20, 34, 9, 4, -1);
	DrawGUIText("HELP", 20, 34, 10, 4, -1);
	DrawGUIText("INSTRUMENT", 20, 34, 11, 4, -1);


	gui.activeUI[19][8].sprite = { 5, 27 };
	gui.activeUI[34][8].sprite = { 6, 27 };

	for (int y = 9; y < 11; y++)
	{
		gui.activeUI[19][y].sprite = { 5, 28 };
		gui.activeUI[34][y].sprite = { 6, 28 };
	}

	gui.activeUI[19][11].sprite = { 5, 29 };
	gui.activeUI[34][11].sprite = { 6, 29 };
	/*
	if (loadedSong.unsavedChanges)
		DrawText("SAVE", 20, 26, 8, 4, 2);
	else
		DrawText("SAVE", 20, 26, 8, 1, 2);
	gui.activeUI[19][8].sprite = { 24, 0 };
	gui.activeUI[26][8].sprite = { 25, 0 };
	

	DrawText("LOAD", 20, 26, 9, 4, 2);
	gui.activeUI[19][9].sprite = { 24, 0 };
	gui.activeUI[26][9].sprite = { 25, 0 };

	DrawText("NEW", 20, 26, 10, 4, 2);
	gui.activeUI[19][10].sprite = { 24, 0 };
	gui.activeUI[26][10].sprite = { 25, 0 };

	DrawText("EXPORT", 20, 26, 11, 4, 2);
	gui.activeUI[19][11].sprite = { 24, 0 };
	gui.activeUI[26][11].sprite = { 25, 0 };


	DrawText("HELP", 28, 35, 9, 4, 2);
	gui.activeUI[27][9].sprite = { 24, 0 };
	gui.activeUI[34][9].sprite = { 25, 0 };

	DrawText("CONFIG", 28, 35, 10, 4, 2);
	gui.activeUI[27][10].sprite = { 24, 0 };
	gui.activeUI[34][10].sprite = { 25, 0 };

	DrawText("EDITOR", 28, 35, 11, 4, 2);
	gui.activeUI[27][11].sprite = { 24, 0 };
	gui.activeUI[34][11].sprite = { 25, 0 };
	*/
	// Eye
	/*
	logoY -= 2;
	activeUI[logoX + 15][logoY].sprite = { 26, 1 }; activeUI[logoX + 16][logoY].sprite = { 27, 1 };
	activeUI[logoX + 15][logoY + 1].sprite = { 26, 2 }; activeUI[logoX + 16][logoY + 1].sprite = { 27, 2 };
	activeUI[logoX + 15][logoY + 2].sprite = { 28, 2 }; activeUI[logoX + 16][logoY + 2].sprite = { 29, 2 };
	*/



	// Draw the play and pause buttons.
	if (editor.playingSong)
	{
		gui.activeUI[0][13].sprite = { 14, 7 };
		gui.activeUI[0][14].sprite = { 14, 8 };
		gui.activeUI[1][13].sprite = { 15, 7 };
		gui.activeUI[1][14].sprite = { 15, 8 };

		gui.activeUI[2][13].sprite = { 16, 7 };
		gui.activeUI[2][14].sprite = { 16, 8 };
		gui.activeUI[3][13].sprite = { 17, 7 };
		gui.activeUI[3][14].sprite = { 17, 8 };
	}
	else
	{
		gui.activeUI[0][13].sprite = { 12, 7 };
		gui.activeUI[0][14].sprite = { 12, 8 };
		gui.activeUI[1][13].sprite = { 13, 7 };
		gui.activeUI[1][14].sprite = { 13, 8 };

		gui.activeUI[2][13].sprite = { 18, 7 };
		gui.activeUI[2][14].sprite = { 18, 8 };
		gui.activeUI[3][13].sprite = { 19, 7 };
		gui.activeUI[3][14].sprite = { 19, 8 };
	}



	// Buttons
	for (int y = 0; y < 5; y++) // Revert buttons to unpressed.
	{
		gui.activeUI[11][3 + 2 * y].sprite = { 8, 3 };
		gui.activeUI[12][3 + 2 * y].sprite = { 9, 3 };
		gui.activeUI[13][3 + 2 * y].sprite = { 10, 3 };
	}
	gui.activeUI[7][5].sprite = { 8, 3 };
	gui.activeUI[8][5].sprite = { 9, 3 };
	gui.activeUI[9][5].sprite = { 10, 3 };
	gui.activeUI[7][8].sprite = { 8, 3 };
	gui.activeUI[8][8].sprite = { 9, 3 };
	gui.activeUI[9][8].sprite = { 10, 3 };
	// Frame menu scroll arrows
	gui.activeUI[6][2].sprite = { 6, 3 };
	gui.activeUI[6][11].sprite = { 6, 4 };
	// Frame scroll arrows
	gui.activeUI[91][16].sprite = { 6, 3 };
	gui.activeUI[91][55].sprite = { 6, 4 };
	gui.activeUI[5][56].sprite = { 30, 4 };
	gui.activeUI[90][56].sprite = { 30, 3 };
	// Sample scroll arrows
	gui.activeUI[91][2].sprite = { 6, 3 };
	gui.activeUI[91][11].sprite = { 6, 4 };




	gui.activeUI[54][12].sprite = { 2, 3 };
	if (gui.uiDisplayMenuOption == 0) // Song GUI.
	{
		gui.songLength = calculateSongLength();
		int songSecs = gui.songLength % 60;
		int songMins = gui.songLength / 60;
		DrawGUIText("Song Length:", 44, 56, 6, 3, 0);
		DrawNum(songMins, 56, 58, 6, 4, 0);
		DrawGUIText(":", 58, 59, 6, 4, 0);
		DrawNum(songSecs, 59, 61, 6, 4, 0);

		gui.songPos = calculateSongPosition();
		songSecs = gui.songPos % 60;
		songMins = gui.songPos / 60;
		DrawGUIText("Current Position:", 44, 61, 8, 3, 0);
		DrawNum(songMins, 61, 63, 8, 4, 0);
		DrawGUIText(":", 63, 64, 8, 4, 0);
		DrawNum(songSecs, 64, 66, 8, 4, 0);
	}
	else if (gui.uiDisplayMenuOption == 1) // Key GUI.
	{
		// Draw keyboard
		if (loadedSong.edo != 12)
			DrawEDOKeyboard();
		else
			DrawKeyBoard();
	}
	else if (gui.uiDisplayMenuOption == 2) // Effect GUI.
	{
		for (int x = 0; x < 25; x++)
		{
			gui.activeUI[43 + x][0].sprite = { 2, 3 };
		}

		DrawGUIText("V:Volume", 43, 54, 2, 3, 0);
		DrawGUIText("P:Pitch", 43, 54, 3, 3, 0);
		DrawGUIText("L:Low-Pass", 43, 54, 4, 3, 0);
		DrawGUIText("H:High-Pass", 43, 54, 5, 3, 0);
		DrawGUIText("J:Jump", 43, 54, 6, 3, 0);
		DrawGUIText("R:Retrigger", 43, 54, 7, 3, 0);
		DrawGUIText("M:Mute", 43, 54, 8, 3, 0);
		DrawGUIText("Z:Fuzz", 43, 54, 9, 3, 0);
		for (int y = 1; y < 12; y++)
			gui.activeUI[54][y].sprite = { 2, 4 };
		DrawGUIText("S:Stereo", 55, 68, 2, 3, 0);
		DrawGUIText("D:Delay Note", 55, 68, 3, 3, 0);
		DrawGUIText("F:Go To Frame", 55, 68, 4, 3, 0);
		DrawGUIText("T:Tempo", 55, 68, 5, 3, 0);
		DrawGUIText("O:Operator V.", 55, 68, 6, 3, 0);
		DrawGUIText("A:Arpeggio", 55, 68, 7, 3, 0);
		DrawGUIText("C:Restart Ch.", 55, 68, 8, 3, 0);
		DrawGUIText("B:Sample R.", 55, 68, 9, 3, 0);

		gui.activeUI[42][0].sprite = { 0, 3 };
		gui.activeUI[42][1].sprite = { 4, 3 };
		gui.activeUI[54][0].sprite = { 3, 3 };
		gui.activeUI[54][12].sprite = { 4, 4 };
		gui.activeUI[68][1].sprite = { 3, 4 };
		gui.activeUI[68][0].sprite = { 1, 3 };
	}
	else if (gui.uiDisplayMenuOption == 3) // Voice Sample GUI.
	{
		DrawVoiceKeyboard();
	}
	

	return;
}


void DrawLogo()
{
	int logoX = 22;
	int logoY = 1;

	// D
	gui.activeUI[logoX][logoY].sprite = { 24, 7 }; gui.activeUI[logoX+1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY+1].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY+1].sprite = { 27, 7 };
	gui.activeUI[logoX][logoY+2].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY+2].sprite = { 23, 8 };
	logoX += 2;
	// U
	gui.activeUI[logoX][logoY].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 27, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 27, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 22, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// A
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 25, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 27, 7 };
	logoX += 2;
	// L
	gui.activeUI[logoX][logoY].sprite = { 26, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 26, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX = 20;
	logoY += 3;
	// T
	gui.activeUI[logoX][logoY].sprite = { 24, 8 };
	gui.activeUI[logoX][logoY + 1].sprite = { 26, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// R
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 7 };
	logoX += 2;
	// A
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 25, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 27, 7 };
	logoX += 2;
	// C
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 26, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 22, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// K
	gui.activeUI[logoX][logoY].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 7 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 27, 7 };
	logoX += 2;
	// E
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 22, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// R
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 7 };



	return;
}



void DrawKeyBoard()
{
	gui.activeUI[44][7].sprite = { 24, 9 };
	gui.activeUI[44][8].sprite = { 25, 9 };

	for (int x = 45; x < 60; x++)
	{
		gui.activeUI[x][8].sprite = { 26, 9 };
	}
	gui.activeUI[60][8].sprite = { 27, 9 };
	gui.activeUI[60][9].sprite = { 28, 9 };

	DrawKey(17, 0, { 44, 6 });
	DrawKey(18, 1, { 45, 5 });
	DrawKey(19, 2, { 46, 6 });
	DrawKey(20, 3, { 47, 5 });
	DrawKey(21, 4, { 48, 6 });
	DrawKey(22, 5, { 50, 6 });
	DrawKey(23, 6, { 51, 5 });
	DrawKey(24, 7, { 52, 6 });
	DrawKey(25, 8, { 53, 5 });
	DrawKey(26, 9, { 54, 6 });
	DrawKey(27, 10, { 55, 5 });
	DrawKey(28, 11, { 56, 6 });

	DrawKey(29, 0, { 58, 6 });
	DrawKey(30, 1, { 59, 5 });
	DrawKey(31, 2, { 60, 6 });
	DrawKey(32, 3, { 61, 5 });
	DrawKey(33, 4, { 62, 6 });
	DrawKey(34, 5, { 64, 6 });
	DrawKey(35, 6, { 65, 5 });
	DrawKey(36, 7, { 66, 6 });


	DrawKey(0, 0, { 46, 10 });
	DrawKey(1, 1, { 47, 9 });
	DrawKey(2, 2, { 48, 10 });
	DrawKey(3, 3, { 49, 9 });
	DrawKey(4, 4, { 50, 10 });
	DrawKey(5, 5, { 52, 10 });
	DrawKey(6, 6, { 53, 9 });
	DrawKey(7, 7, { 54, 10 });
	DrawKey(8, 8, { 55, 9 });
	DrawKey(9, 9, { 56, 10 });
	DrawKey(10, 10, { 57, 9 });
	DrawKey(11, 11, { 58, 10 });

	DrawKey(12, 0, { 60, 10 });
	DrawKey(13, 1, { 61, 9 });
	DrawKey(14, 2, { 62, 10 });
	DrawKey(15, 3, { 63, 9 });
	DrawKey(16, 4, { 64, 10 });

	return;
}



void DrawEDOKeyboard()
{
	DrawEDOKey(0, { 46, 10 });
	DrawEDOKey(1, { 48, 10 });
	DrawEDOKey(2, { 50, 10 });
	DrawEDOKey(3, { 52, 10 });
	DrawEDOKey(4, { 54, 10 });
	DrawEDOKey(5, { 56, 10 });
	DrawEDOKey(6, { 58, 10 });
	DrawEDOKey(7, { 60, 10 });
	DrawEDOKey(8, { 62, 10 });
	DrawEDOKey(9, { 64, 10 });

	DrawEDOKey(10, { 45, 9 });
	DrawEDOKey(11, { 47, 9 });
	DrawEDOKey(12, { 49, 9 });
	DrawEDOKey(13, { 51, 9 });
	DrawEDOKey(14, { 53, 9 });
	DrawEDOKey(15, { 55, 9 });
	DrawEDOKey(16, { 57, 9 });
	DrawEDOKey(17, { 59, 9 });
	DrawEDOKey(18, { 61, 9 });
	DrawEDOKey(19, { 63, 9 });
	DrawEDOKey(20, { 65, 9 });

	DrawEDOKey(21, { 44, 8 });
	DrawEDOKey(22, { 46, 8 });
	DrawEDOKey(23, { 48, 8 });
	DrawEDOKey(24, { 50, 8 });
	DrawEDOKey(25, { 52, 8 });
	DrawEDOKey(26, { 54, 8 });
	DrawEDOKey(27, { 56, 8 });
	DrawEDOKey(28, { 58, 8 });
	DrawEDOKey(29, { 60, 8 });
	DrawEDOKey(30, { 62, 8 });
	DrawEDOKey(31, { 64, 8 });
	DrawEDOKey(32, { 64, 8 });
	DrawEDOKey(33, { 66, 8 });

	DrawEDOKey(34, { 43, 7 });
	DrawEDOKey(35, { 45, 7 });
	DrawEDOKey(36, { 47, 7 });
	DrawEDOKey(37, { 49, 7 });
	DrawEDOKey(38, { 51, 7 });
	DrawEDOKey(39, { 53, 7 });
	DrawEDOKey(40, { 55, 7 });
	DrawEDOKey(41, { 57, 7 });
	DrawEDOKey(42, { 59, 7 });
	DrawEDOKey(43, { 61, 7 });
	DrawEDOKey(44, { 63, 7 });
	DrawEDOKey(45, { 65, 7 });
	


	return;
}


void DrawVoiceKeyboard()
{
	for (int x = 0; x < 10; x++)
		DrawVoiceKey(x, { (x * 2) + 46, 10 });

	for (int x = 0; x < 11; x++)
		DrawVoiceKey(x + 10, { (x * 2) + 45, 9 });

	for (int x = 0; x < 12; x++)
		DrawVoiceKey(x + 21, { (x * 2) + 44, 8 });

	for (int x = 0; x < 11; x++)
		DrawVoiceKey(x + 33, { (x * 2) + 43, 7 });

	if (editor.selectedVoiceSample > -1)
		DrawGUIText("  " + gui.phonemes[editor.selectedVoiceSample], 46, 65, 5, 14, 12);
	else
		DrawGUIText("  ---", 46, 65, 5, 14, 12);

	return;
}



void DrawMute()
{
	for (int y = 12; y < 17; y++)
	{
		for (int x = 0; x < 92; x++)
		{
			if (gui.lightMode)
			{
				gui.activeUI[x][y].bgCol = 4;
				gui.activeUI[x][y].sprite = { 18, 0 };
			}
			else
			{
				gui.activeUI[x][y].bgCol = 0;
				gui.activeUI[x][y].sprite = { 15, 0 };
			}
		}
	}


	int modCount = 4;
	int xPos = 5 - gui.frameScroll.x;
	for (int i = 0; i < loadedSong.numberOfChannels; i++)
	{
		xPos = DrawChannelMuteGlass(xPos, i);
	}


	// Draw add/remove frame button.
	if (xPos > 90) return;
	if (xPos > 4)
	{
		gui.activeUI[xPos][13].sprite = { 11, 3 }; gui.activeUI[xPos][14].sprite = { 11, 4 };
	}
	xPos++;
	if (xPos > 90) return;
	if (xPos > 4)
	{
		gui.activeUI[xPos][13].sprite = { 26, 3 }; gui.activeUI[xPos][14].sprite = { 26, 4 };
	}
	xPos++;
	if (xPos > 90) return;
	if (xPos > 4)
	{
		gui.activeUI[xPos][13].sprite = { 27, 3 }; gui.activeUI[xPos][14].sprite = { 27, 4 };
	}
	xPos++;
	if (xPos > 90) return;
	if (xPos > 4)
	{
		gui.activeUI[xPos][13].sprite = { 28, 3 }; gui.activeUI[xPos][14].sprite = { 28, 4 };
	}
	xPos++;
	if (xPos > 90) return;
	if (xPos > 4)
	{
		gui.activeUI[xPos][13].sprite = { 13, 3 }; gui.activeUI[xPos][14].sprite = { 13, 4 };
	}

	return;
}



int DrawChannelMuteGlass(int xPos, int i)
{
	if (xPos > 90) return xPos;

	// Border
	int muteBorderEnd = xPos + 7 + channels[i].effectCountPerRow * 4;

	if (channels[i].hasVoiceColumns)
		muteBorderEnd += 5;

	for (int x = 0; x < 7 + muteBorderEnd - xPos; x++)
	{
		if (xPos + x > 4 && xPos + x < 91)
		{
			gui.activeUI[xPos + x][12].sprite = { 2, 3 };
			gui.activeUI[xPos + x][15].sprite = { 2, 3 };
		}
	}
	
	if (muteBorderEnd > 4 && muteBorderEnd < 91)
	{
		gui.activeUI[muteBorderEnd][12].sprite = { 3, 3 };
		gui.activeUI[muteBorderEnd][15].sprite = { 5, 3 };
	}

	int muteOffset = 0;

	if (channels[i].muted)
		muteOffset = 3;

	int color = 1;



	if (channels[i].compressed)
	{
		if (xPos > 4)
		{
			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 11 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 11 + muteOffset, 4 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 13 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 13 + muteOffset, 4 };
			}
			xPos++;
			if (xPos > 90) return xPos;
			/*
			if (channels[i].muted)
				DrawNum(i + 1, xPos, xPos + 2, 13, 2, 0);
			else
			DrawNum(i + 1, xPos, xPos + 2, 13, 16, 0);
			*/
		}
		//xPos += 2;
		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 27, 10 }; gui.activeUI[xPos][14].sprite = { 27, 11 };
			gui.activeUI[xPos][15].sprite = { 3, 3 };
		}
		xPos++;

		if (xPos > 90) return xPos;
	}
	else
	{
		// Glass
		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 11 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 11 + muteOffset, 4 };
		}
		xPos++;
		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
		}
		xPos++;
		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
		}
		xPos++;
		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
		}
		xPos++;
		if (xPos + 1 > 90) return xPos;

		DrawChannelNumber(i + 1, xPos);
		xPos += 2;

		if (channels[i].hasVoiceColumns)
		{
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 6 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 6 + muteOffset, 13 };
			}
			xPos++;

			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 5 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 5 + muteOffset, 13 };
			}
			xPos++;

			///////////////// Voice sample icon
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 20, 7 }; gui.activeUI[xPos][14].sprite = { 20, 8 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 21, 7 }; gui.activeUI[xPos][14].sprite = { 21, 8 };
			}
			xPos++;
			/////////////////

			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
			}
			xPos++;
		}

		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 6 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 6 + muteOffset, 13 };
		}
		xPos++;

		// Draw the channel effects
		if (channels[i].effectCountPerRow > 1)
		{
			for (int ef = 0; ef < channels[i].effectCountPerRow - 1; ef++)
			{
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 5 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 5 + muteOffset, 13 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
				}
				xPos++;

				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 6 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 6 + muteOffset, 13 };
				}
				xPos++;
			}

			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				if (channels[i].effectCountPerRow < 8)
				{
					gui.activeUI[xPos][13].sprite = { 0 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 0 + muteOffset, 13 };
				}
				else
				{
					gui.activeUI[xPos][13].sprite = { 5 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 5 + muteOffset, 13 };
				}
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
			}
			xPos++;
			if (xPos > 90) return xPos;


			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 1 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 1 + muteOffset, 13 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 13 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 13 + muteOffset, 4 };
			}
			xPos++;
		}
		else
		{
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 0 + muteOffset, 12 }; gui.activeUI[xPos][14].sprite = { 0 + muteOffset, 13 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 12 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 12 + muteOffset, 4 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 13 + muteOffset, 3 }; gui.activeUI[xPos][14].sprite = { 13 + muteOffset, 4 };
			}

			xPos++;
		}



		if (xPos > 90) return xPos;
		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 28, 10 }; gui.activeUI[xPos][14].sprite = { 28, 11 };
			gui.activeUI[xPos][15].sprite = { 3, 3 };
		}
		xPos++;
	}

	return xPos;
}



int DrawChannelMuteSynth(int xPos, int i)
{
	// Border
	for (int x = 0; x < 7 + channels[i].effectCountPerRow * 5; x++)
	{
		if (xPos + x > 4 && xPos + x < 91)
		{
			gui.activeUI[xPos + x][12].sprite = { 2, 3 }; gui.activeUI[xPos + x][15].sprite = { 2, 3 };
		}
	}
	int muteBorderEnd = xPos + 7 + channels[i].effectCountPerRow * 5;
	if (muteBorderEnd > 4 && muteBorderEnd < 91)
	{
		gui.activeUI[muteBorderEnd][12].sprite = { 3, 3 }; gui.activeUI[muteBorderEnd][15].sprite = { 5, 3 };
	}

	float muteOffset = 0;

	if (channels[i].muted)
		muteOffset = 3;

	int color = 1;



	if (channels[i].compressed)
	{
		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 0, 14 }; gui.activeUI[xPos][14].sprite = { 0, 15 };
		}
		xPos++;
		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			if (channels[i].muted)
			{
				gui.activeUI[xPos][13].sprite = { 17, 14 }; gui.activeUI[xPos][14].sprite = { 17, 15 };
			}
			else
			{
				gui.activeUI[xPos][13].sprite = { 15, 14 }; gui.activeUI[xPos][14].sprite = { 15, 15 };
			}
			
		}
		xPos++;

		/*
		if (xPos > 4)
		{
			DrawDial({ float(xPos), 13 }, 0);
			gui.dials[dialNum].position = { float(xPos + 1), 14.0f };
		}
		xPos += 2;*/

		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 27, 10 }; gui.activeUI[xPos][14].sprite = { 27, 11 };

			if (channels[i].muted)
				gui.activeUI[xPos][15].sprite = { 16, 15 };
			else
				gui.activeUI[xPos][15].sprite = { 16, 14 };
		}
		xPos++;

		if (xPos > 90) return xPos;
	}
	else
	{
		// Glass
		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 0, 14 }; gui.activeUI[xPos][14].sprite = { 0, 15 };
		}
		xPos++;
		if (xPos > 90) return xPos;

		//DrawDial({ float(xPos), 13 }, 0);
		//gui.dials[dialNum].position = { float(xPos + 1), 14.0f };

		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 6, 14 }; gui.activeUI[xPos][14].sprite = { 6, 15 };
		}
		xPos++;

		for (int x = 0; x < 4; x++)
		{
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				if (channels[i].muted)
				{
					gui.activeUI[xPos][13].sprite = { 17, 14 }; gui.activeUI[xPos][14].sprite = { 17, 15 };
				}
				else
				{
					gui.activeUI[xPos][13].sprite = { 15, 14 }; gui.activeUI[xPos][14].sprite = { 15, 15 };
				}
			}
			xPos++;
		}

		


		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 1, 14 }; gui.activeUI[xPos][14].sprite = { 1, 15 };
		}
		xPos++;

		// Draw the channel effects
		if (channels[i].effectCountPerRow > 1)
		{
			for (int ef = 0; ef < channels[i].effectCountPerRow - 1; ef++)
			{
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 6, 14 }; gui.activeUI[xPos][14].sprite = { 6, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 1, 14 }; gui.activeUI[xPos][14].sprite = { 1, 15 };
				}
				xPos++;
			}

			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				if (channels[i].effectCountPerRow < 8)
				{
					gui.activeUI[xPos][13].sprite = { 2, 14 }; gui.activeUI[xPos][14].sprite = { 2, 15 };
				}
				else
				{
					gui.activeUI[xPos][13].sprite = { 6, 14 }; gui.activeUI[xPos][14].sprite = { 6, 15 };
				}
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;


			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 3, 14 }; gui.activeUI[xPos][14].sprite = { 3, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				if (i < channels.size() - 1)
				{
					gui.activeUI[xPos][13].sprite = { 5, 14 }; gui.activeUI[xPos][14].sprite = { 5, 15 };
				}
				else
				{
					gui.activeUI[xPos][13].sprite = { 9, 14 }; gui.activeUI[xPos][14].sprite = { 9, 15 };
				}
			}
			xPos++;
		}
		else
		{
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 2, 14 }; gui.activeUI[xPos][14].sprite = { 2, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 8, 14 }; gui.activeUI[xPos][14].sprite = { 8, 15 };
			}

			xPos++;
		}



		if (xPos > 90) return xPos;


		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 27, 10 }; gui.activeUI[xPos][14].sprite = { 27, 11 };

			if (channels[i].muted)
				gui.activeUI[xPos][15].sprite = { 16, 15 };
			else
				gui.activeUI[xPos][15].sprite = { 16, 14 };
		}
		xPos++;
	}

	return xPos;
}




int DrawChannelMuteModulator(int xPos, int i, int dialNum)
{
	// Border
	for (int x = 0; x < 7 + channels[i].effectCountPerRow * 5; x++)
	{
		if (xPos + x > 4 && xPos + x < 91)
		{
			gui.activeUI[xPos + x][12].sprite = { 2, 3 };
			gui.activeUI[xPos + x][15].sprite = { 2, 3 };
		}
	}
	int muteBorderEnd = xPos + 7 + channels[i].effectCountPerRow * 5;
	if (muteBorderEnd > 4 && muteBorderEnd < 91)
	{
		gui.activeUI[muteBorderEnd][12].sprite = { 3, 3 };
		gui.activeUI[muteBorderEnd][15].sprite = { 5, 3 };
	}

	float muteOffset = 0;

	if (channels[i].muted)
		muteOffset = 3;

	int color = 1;



	if (channels[i].compressed)
	{
		if (xPos > 4)
		{
			
		}
		xPos += 2;
		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 27, 10 }; gui.activeUI[xPos][14].sprite = { 27, 11 };

			if (channels[i].muted)
				gui.activeUI[xPos][15].sprite = { 16, 15 };
			else
				gui.activeUI[xPos][15].sprite = { 16, 14 };
		}
		xPos++;

		if (xPos > 90) return xPos;
	}
	else
	{
		// Glass
		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 0, 14 }; gui.activeUI[xPos][14].sprite = { 0, 15 };
		}
		xPos++;
		if (xPos > 90) return xPos;

		
		
		xPos += 2;
		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 12, 14 };
			gui.activeUI[xPos][14].sprite = { 12, 15 };
		}
		xPos++;

		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 13, 14 };
			gui.activeUI[xPos][14].sprite = { 13, 15 };
		}
		xPos++;

		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 14, 14 };
			gui.activeUI[xPos][14].sprite = { 14, 15 };
		}
		xPos++;

		if (xPos > 90) return xPos;

		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 1, 14 }; gui.activeUI[xPos][14].sprite = { 1, 15 };
		}
		xPos++;

		// Draw the channel effects
		if (channels[i].effectCountPerRow > 1)
		{
			for (int ef = 0; ef < channels[i].effectCountPerRow - 1; ef++)
			{
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 6, 14 }; gui.activeUI[xPos][14].sprite = { 6, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
				}
				xPos++;
				if (xPos > 90) return xPos;

				if (xPos > 4)
				{
					gui.activeUI[xPos][13].sprite = { 1, 14 }; gui.activeUI[xPos][14].sprite = { 1, 15 };
				}
				xPos++;
			}

			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				if (channels[i].effectCountPerRow < 8)
				{
					gui.activeUI[xPos][13].sprite = { 2, 14 }; gui.activeUI[xPos][14].sprite = { 2, 15 };
				}
				else
				{
					gui.activeUI[xPos][13].sprite = { 6, 14 }; gui.activeUI[xPos][14].sprite = { 6, 15 };
				}
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;


			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 3, 14 }; gui.activeUI[xPos][14].sprite = { 3, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				if (i < channels.size() - 1)
				{
					gui.activeUI[xPos][13].sprite = { 5, 14 }; gui.activeUI[xPos][14].sprite = { 5, 15 };
				}
				else
				{
					gui.activeUI[xPos][13].sprite = { 9, 14 }; gui.activeUI[xPos][14].sprite = { 9, 15 };
				}
			}
			xPos++;
		}
		else
		{
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 2, 14 }; gui.activeUI[xPos][14].sprite = { 2, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 4, 14 }; gui.activeUI[xPos][14].sprite = { 4, 15 };
			}
			xPos++;
			if (xPos > 90) return xPos;

			if (xPos > 4)
			{
				gui.activeUI[xPos][13].sprite = { 9, 14 }; gui.activeUI[xPos][14].sprite = { 9, 15 };
			}

			xPos++;
		}



		if (xPos > 90) return xPos;


		if (xPos > 4)
		{
			gui.activeUI[xPos][13].sprite = { 27, 10 }; gui.activeUI[xPos][14].sprite = { 27, 11 };

			if (channels[i].muted)
				gui.activeUI[xPos][15].sprite = { 16, 15 };
			else
				gui.activeUI[xPos][15].sprite = { 16, 14 };
		}
		xPos++;
	}

	return xPos;
}



void DrawChannelNumber(int channelNum, int offsetX)
{
	int channelNum2 = 0;
	int channelNumOg = channelNum - 1;
	if (channelNum > 9)
	{
		channelNum2 = int(channelNum * 0.1);
		channelNum = channelNum % 10;

		offsetX -= 2;
		if (offsetX > 4 && offsetX < 91)
		{
			if (channels[channelNumOg].muted)
			{
				gui.activeUI[offsetX][13].sprite = { 2 * channelNum2, 10 };
				gui.activeUI[offsetX][14].sprite = { 2 * channelNum2, 11 };
			}
			else
			{
				gui.activeUI[offsetX][13].sprite = { 2 * channelNum2, 5 };
				gui.activeUI[offsetX][14].sprite = { 2 * channelNum2, 6 };
			}
		}
		else if (offsetX > 90)
			return;

		offsetX++;
		if (offsetX > 4 && offsetX < 91)
		{
			if (channels[channelNumOg].muted)
			{
				gui.activeUI[offsetX][13].sprite = { 2 * channelNum2 + 1, 10 };
				gui.activeUI[offsetX][14].sprite = { 2 * channelNum2 + 1, 11 };
			}
			else
			{
				gui.activeUI[offsetX][13].sprite = { 2 * channelNum2 + 1, 5 };
				gui.activeUI[offsetX][14].sprite = { 2 * channelNum2 + 1, 6 };
			}
		}
		offsetX++;
	}

	if (offsetX > 4 && offsetX < 91)
	{
		if (channels[channelNumOg].muted)
		{
			gui.activeUI[offsetX][13].sprite = { 2 * channelNum, 10 };
			gui.activeUI[offsetX][14].sprite = { 2 * channelNum, 11 };
		}
		else
		{
			gui.activeUI[offsetX][13].sprite = { 2 * channelNum, 5 };
			gui.activeUI[offsetX][14].sprite = { 2 * channelNum, 6 };
		}
	}
	else if (offsetX > 90)
		return;

	offsetX++;
	if (offsetX > 4 && offsetX < 91)
	{
		if (channels[channelNumOg].muted)
		{
			gui.activeUI[offsetX][13].sprite = { 2 * channelNum + 1, 10 };
			gui.activeUI[offsetX][14].sprite = { 2 * channelNum + 1, 11 };
		}
		else
		{
			gui.activeUI[offsetX][13].sprite = { 2 * channelNum + 1, 5 };
			gui.activeUI[offsetX][14].sprite = { 2 * channelNum + 1, 6 };
		}
	}

	return;
}



void DrawFrameBorder()
{
	gui.activeUI[4][12].sprite = { 0, 3 };
	gui.activeUI[4][13].sprite = { 2, 4 };
	gui.activeUI[4][14].sprite = { 2, 4 };
	gui.activeUI[4][15].sprite = { 5, 3 };

	gui.activeUI[91][12].sprite = { 4, 3 };
	gui.activeUI[91][13].sprite = { 2, 4 };
	gui.activeUI[91][14].sprite = { 2, 4 };
	gui.activeUI[91][15].sprite = { 4, 3 };

	gui.activeUI[0][15].sprite = { 0, 3 };
	gui.activeUI[1][15].sprite = { 2, 3 };
	gui.activeUI[2][15].sprite = { 2, 3 };
	gui.activeUI[3][15].sprite = { 2, 3 };




	for (int x = 0; x < 4; x++)
		gui.activeUI[x][56].sprite = { 2, 3 };
	for (int x = 7; x < 89; x++)
		gui.activeUI[x][56].sprite = { 28, 2 };
	
	gui.activeUI[6][56].sprite = { 27, 2 };
	gui.activeUI[89][56].sprite = { 29, 2 };


	gui.activeUI[4][56].sprite = { 4, 4 };

	// Frame borders.
	int posX = 4 - gui.frameScroll.x;
	for (int i = 0; i < loadedSong.numberOfChannels; i++)
	{
		if (channels[i].compressed)
			posX += 3;
		else
		{
			posX += 8 + channels[i].effectCountPerRow * 4;

			// Voice samples (if enabled in channel).
			if (channels[i].hasVoiceColumns)
				posX += 5;
		}
	}
	if (posX < 91)
	{
		if (posX < -1)
			posX = -1;
		for (int x = posX + 6; x < 91; x++)
		{
			for (int y = 13; y < 15; y++)
			{
				if (gui.lightMode)
				{
					gui.activeUI[x][y].bgCol = 4;
					gui.activeUI[x][y].sprite = { 18, 0 };
				}
				else
				{
					gui.activeUI[x][y].bgCol = 0;
					gui.activeUI[x][y].sprite = { 15, 0 };
				}
			}
		}
		if (posX < 4)
			posX = 4;

		// Fill in after frames.
		if (gui.background)
			DrawBackground(posX + 1);
		else
		{
			for (int x = posX + 1; x < 91; x++)
			{
				gui.activeUI[x][12].sprite = { 2, 3 };
				for (int y = 16; y < 56; y++)
				{
					if (gui.lightMode)
					{
						gui.activeUI[x][y].bgCol = 4;
						gui.activeUI[x][y].sprite = { 18, 0 };
					}
					else
					{
						gui.activeUI[x][y].bgCol = 0;
						gui.activeUI[x][y].sprite = { 15, 0 };
					}
				}
				gui.activeUI[x][15].sprite = { 2, 3 };
			}
		}
	}




	
	for (int y = 18; y < 54; y++)
	{
		//gui.activeUI[91][y].sprite = { 2, 4 };
		gui.activeUI[91][y].sprite = { 31, 3 };
	}
	gui.activeUI[91][17].sprite = { 31, 2 };
	gui.activeUI[91][54].sprite = { 31, 4 };

	for (int y = 16; y < 56; y++)
	{
		gui.activeUI[0][y].sprite = { 2, 4 };
		gui.activeUI[4][y].sprite = { 2, 4 };

		// Beat numbering.
		if (loadedFrame.beatsPerMeasure > 0 && (y - 16 + int(gui.frameScroll.y)) % loadedFrame.beatsPerMeasure == 0)
			DrawNum(y - 16 + gui.frameScroll.y, 1, 4, y, 4, 0);
		else
			DrawNum(y - 16 + gui.frameScroll.y, 1, 4, y, 2, 0);

		if (y - 16 + gui.frameScroll.y == loadedSong.currentNote)
		{
			gui.activeUI[1][y].bgCol = 2;
			gui.activeUI[2][y].bgCol = 2;
			gui.activeUI[3][y].bgCol = 2;
			gui.activeUI[1][y].textCol++;
			gui.activeUI[2][y].textCol++;
			gui.activeUI[3][y].textCol++;
			gui.activeUI[0][y].sprite = { 5, 4 };
			gui.activeUI[4][y].sprite = { 5, 4 };
		}
	}

	gui.activeUI[0][16].sprite = { 2, 4 };
	gui.activeUI[0][55].sprite = { 2, 4 };




	gui.activeUI[0][56].sprite = { 0, 4 };
	gui.activeUI[91][56].sprite = { 1, 4 };

	// Where the top UI and frames meet.
	gui.activeUI[10][12].sprite = { 4, 4 };
	gui.activeUI[18][12].sprite = { 4, 4 };
	gui.activeUI[35][12].sprite = { 4, 4 };
	gui.activeUI[42][12].sprite = { 4, 4 };

	gui.activeUI[68][12].sprite = { 4, 4 };
	gui.activeUI[71][12].sprite = { 4, 4 };
	gui.activeUI[91][12].sprite = { 4, 3 };

	return;
}



void DrawBackground(float startPos)
{
	srand(0);


	for (int x = startPos; x < 92; x++)
	{
		gui.activeUI[x][12].sprite = { 2, 3 };
		gui.activeUI[x][15].sprite = { 2, 3 };

		

		for (int y = 16; y < 40; y++)
		{
			int star = rand() % 6;

			if (y + star * 4 < 20)
				star += 4;
			else if (y + star * 4 > 30)
			{
				if (y + star * 4 > 60)
					gui.activeUI[x][y].sprite = { 27, 21 };
				else
				{
					star += 4;
					if (y + star * 4 > 50)
						star += 4;
					gui.activeUI[x][y].sprite = { 16 + star, 21 };
				}
			}
				
			gui.activeUI[x][y].sprite = { 16 + star, 21 };
		}
		for (int y = 32; y < 56; y++)
		{
			gui.activeUI[x][y].sprite = { 16 + y - 36, 22 };
		}
	}

	srand(3);

	int yVal = 20;
	bool goUp = true;

	// Draw the mountains.
	for (int x = startPos; x < 92; x++)
	{
		bool newGoUp = rand() % 2;


		if (yVal > 20) newGoUp = false;
		else if (yVal < 1) newGoUp = true;


		if (goUp == newGoUp)
		{
			if (goUp) yVal++;
			else yVal--;
		}

		goUp = newGoUp;

		int mountTexX = 22 + rand() % 3;
		int mountTexY = 23;

		if (yVal < 20) mountTexY++;
		if (yVal < 14) mountTexY++;
		if (yVal < 12) mountTexY++;
		if (yVal < 9) mountTexY++;

		if (!goUp) mountTexX += 3;

		gui.activeUI[x][57 - yVal].sprite = { mountTexX, mountTexY };

		for (int y = 58 - yVal; y < 56; y++)
		{
			gui.activeUI[x][y].sprite.x += 2;
		}
	}

	srand(4);

	yVal = 10;
	goUp = false;

	for (int x = startPos; x < 92; x++)
	{
		if (int(x - startPos) % 2 == 0)
		{
			bool newGoUp = rand() % 2;


			if (yVal > 20)
				newGoUp = false;
			else if (yVal < 1)
				newGoUp = true;


			if (goUp == newGoUp)
			{
				if (goUp) yVal++;
				else yVal--;
			}

			goUp = newGoUp;
		}
		else
		{
			if (goUp) yVal++;
			else yVal--;
		}

		

		int mountTexX = 16 + rand() % 3;
		int mountTexY = 23;

		if (yVal < 22) mountTexY++;
		if (yVal < 16) mountTexY++;
		if (yVal < 14) mountTexY++;
		if (yVal < 11) mountTexY++;

		if (!goUp) mountTexX += 3;

		gui.activeUI[x][57 - yVal].sprite = { mountTexX, mountTexY };

		if (goUp) mountTexY = 28;
		else mountTexY = 29;

		for (int y = 58 - yVal; y < 56; y++)
		{
			mountTexX = 16 + rand() % 4;

			if (y + yVal > 60) mountTexX++;
			if (y + yVal > 62) mountTexX++;
			if (y + yVal > 64) mountTexX++;
			if (y + yVal > 66) mountTexX++;
			if (y + yVal > 68) mountTexX++;

			gui.activeUI[x][y].sprite = { mountTexX, mountTexY };
		}
	}

	gui.activeUI[int(startPos) + 8][24].sprite = { 25, 28 };
	gui.activeUI[int(startPos) + 9][24].sprite = { 26, 28 };
	gui.activeUI[int(startPos) + 8][25].sprite = { 25, 29 };
	gui.activeUI[int(startPos) + 9][25].sprite = { 26, 29 };

	return;
}



void DrawVolumeBars()
{
	// Frame borders.
	int posX = 4 - gui.frameScroll.x;


	for (int i = 0; i < loadedSong.numberOfChannels; i++)
	{
		if (channels[i].compressed)
			posX += 3;
		else
		{
			posX += 8 + channels[i].effectCountPerRow * 4;

			// Voice samples (if enabled in channel).
			if (channels[i].hasVoiceColumns)
				posX += 5;
		}

		if (posX > 4 && posX < 91)
		{
			if (editor.channelBeingMoved == i)
			{
				gui.activeUI[posX][12].sprite = { 26, 6 };
			}
			else
			{
				gui.activeUI[posX][12].sprite = { 25, 6 };
			}

			gui.activeUI[posX][16].sprite = { 2, 4 };
			gui.activeUI[posX][17].sprite = { 4, 21 };


			for (int y = 0; y < 36; y++)
			{
				if (editor.channelMoveTo == i) // Draw selected channel move-to point.
				{
					gui.activeUI[posX][y + 18].sprite = { 27, 6 };
				}
				else
				{
					if (36 - y < channels[i].averageVolR)
					{
						if (36 - y < channels[i].averageVolL)
						{
							gui.activeUI[posX][y + 18].sprite = { 1, 21 }; // Yellow
						}
						else
							gui.activeUI[posX][y + 18].sprite = { 0, 21 }; // Red
					}
					else if (36 - y < channels[i].averageVolL)
					{
						gui.activeUI[posX][y + 18].sprite = { 2, 21 }; // Blue
					}
					else
						gui.activeUI[posX][y + 18].sprite = { 3, 21 }; // Grey

					gui.activeUI[posX][y + 18].bgCol = 0;
				}
			}
			gui.activeUI[posX][54].sprite = { 5, 21 };
			gui.activeUI[posX][55].sprite = { 2, 4 };
		}
	}

	

	return;
}



void DrawChannel(int channelNum, int offsetX)
{
	if (loadedSong.currentNote >= loadedFrame.rows.size())
		return;


	for (int y = 0; y < 40; y++)
	{
		DrawChannelLine(channelNum, offsetX, y);
		DrawChannelLineValues(channelNum, offsetX, y);
	}

	return;
}


void DrawChannelLine(int channelNum, int offsetX, int y)
{
	int intScrollY = int(gui.frameScroll.y);


	int xOffset = offsetX;
	int bgColor = 0;
	int textColor = 1;


	if (y + intScrollY == loadedSong.currentNote) // Note position marker.
	{
		bgColor = 2;
		textColor = 3;
	}
	else if (loadedFrame.beatsPerMeasure > 0 && y + intScrollY < loadedFrame.rows.size() && (y + intScrollY) % loadedFrame.beatsPerMeasure == 0)
	{
		bgColor = 1;
		textColor = 2;
	}


	if (channels[channelNum].muted || intScrollY + y >= loadedFrame.rows.size())
	{
		bgColor--;
		if (bgColor < 0) bgColor = 0;
		textColor--;
		if (textColor < 0) textColor = 0;
	}

	if (gui.lightMode)
	{
		if (bgColor < 5) bgColor++;
		if (textColor < 5) textColor++;
		if (bgColor < 6 && bgColor >= 0)
			bgColor = 5 - bgColor;
		if (textColor < 6 && textColor >= 0)
			textColor = 5 - textColor;
	}
	

	for (int note = 0; note < 2; note++) // Note
	{
		if (xOffset > 4 && xOffset < 91)
		{
			gui.activeUI[xOffset][y + 16].sprite = { 16, 0 };
			gui.activeUI[xOffset][y + 16].bgCol = bgColor;
			gui.activeUI[xOffset][y + 16].textCol = textColor;
		}
		xOffset++;
	}
	// Note
	if (xOffset > 4 && xOffset < 91)
	{
		gui.activeUI[xOffset][y + 16].sprite = { 12, 0 };
		gui.activeUI[xOffset][y + 16].bgCol = bgColor;
		gui.activeUI[xOffset][y + 16].textCol = textColor;
	}
	xOffset++;
	// Instrument
	if (xOffset > 4 && xOffset < 91)
	{
		gui.activeUI[xOffset][y + 16].sprite = { 16, 0 };
		gui.activeUI[xOffset][y + 16].bgCol = bgColor;
		gui.activeUI[xOffset][y + 16].textCol = textColor;
	}
	xOffset++;
	// Instrument
	if (xOffset > 4 && xOffset < 91)
	{
		gui.activeUI[xOffset][y + 16].sprite = { 12, 0 };
		gui.activeUI[xOffset][y + 16].bgCol = bgColor;
		gui.activeUI[xOffset][y + 16].textCol = textColor;
	}
	xOffset++;

	// Volume
	if (xOffset > 4 && xOffset < 91)
	{
		gui.activeUI[xOffset][y + 16].sprite = { 16, 0 };
		gui.activeUI[xOffset][y + 16].bgCol = bgColor;
		gui.activeUI[xOffset][y + 16].textCol = textColor;
	}
	xOffset++;
	// Volume
	if (xOffset > 4 && xOffset < 91)
	{
		gui.activeUI[xOffset][y + 16].sprite = { 12, 0 };
		gui.activeUI[xOffset][y + 16].bgCol = bgColor;
		gui.activeUI[xOffset][y + 16].textCol = textColor;
	}
	xOffset++;


	// Voice samples (if enabled in channel).
	if (channels[channelNum].hasVoiceColumns)
	{
		for (int i = 0; i < 5; i++)
		{
			if (xOffset > 4 && xOffset < 91)
			{
				gui.activeUI[xOffset][y + 16].bgCol = bgColor;
				gui.activeUI[xOffset][y + 16].textCol = textColor;
				if (i == 4)
					gui.activeUI[xOffset][y + 16].sprite = { 12, 0 };
				else
					gui.activeUI[xOffset][y + 16].sprite = { 16, 0 };
				xOffset++;
			}
		}
	}


	for (int ef = 0; ef < channels[channelNum].effectCountPerRow; ef++)
	{
		for (int efNum = 0; efNum < 4; efNum++)
		{
			if (xOffset > 4 && xOffset < 91)
			{
				gui.activeUI[xOffset][y + 16].bgCol = bgColor;
				gui.activeUI[xOffset][y + 16].textCol = textColor;
				if (efNum == 1 || efNum == 3)
					gui.activeUI[xOffset][y + 16].sprite = { 12, 0 };
				else
					gui.activeUI[xOffset][y + 16].sprite = { 16, 0 };
				xOffset++;
			}
		}
	}

	return;
}



void DrawChannelLineValues(int channelNum, int offsetX, int y)
{
	int intScrollY = int(gui.frameScroll.y);


	int xOffset = offsetX;
	int bgColor = 0;
	int textColor = 5;


	if (channels[channelNum].muted || y >= loadedFrame.rows.size())
	{
		bgColor--;
		if (bgColor < 0)
			bgColor = 0;
		textColor -= 2;
	}

	if (y + intScrollY == loadedSong.currentNote) // Note position marker.
	{
		bgColor = 2;
	}
	else if (loadedFrame.beatsPerMeasure > 0 && y + intScrollY < loadedFrame.rows.size() && (y + intScrollY) % loadedFrame.beatsPerMeasure == 0)
		bgColor = 1;


	if (y + intScrollY < loadedFrame.rows.size())
	{
		if (loadedFrame.rows[y + intScrollY].note[channelNum] == 255) // Stop Note
		{
			for (int i = 0; i < 5; i++)
			{
				if (xOffset > 4 && xOffset < 91)
					gui.activeUI[xOffset][y + 16].sprite = { 21 + i, 3 };
				xOffset++;
			}
		}
		else
		{
			if (loadedFrame.rows[y + intScrollY].note[channelNum] > -1) // Note
			{
				if (xOffset > 4 && xOffset + 2 < 91)
				{
					DrawEDONote(uint8_t(loadedFrame.rows[y + intScrollY].note[channelNum]), xOffset, y + 16, textColor, bgColor, -1, -1);
				}
			}

			xOffset += 3;
			if (loadedFrame.rows[y + intScrollY].instrument[channelNum] > -1) // Instrument
			{
				if (xOffset > 4 && xOffset + 1 < 91)
					DrawHex(uint8_t(loadedFrame.rows[y + intScrollY].instrument[channelNum]), xOffset, y + 16, textColor + 3, bgColor, -1, -1);
			}
			xOffset += 2;
		}


		if (loadedFrame.rows[y + intScrollY].volume[channelNum] > -1) // Volume
		{
			if (xOffset > 4 && xOffset + 1 < 91)
				DrawHex(int(loadedFrame.rows[y + intScrollY].volume[channelNum]), xOffset, y + 16, textColor + 6, bgColor);
		}
		xOffset += 2;

		float tempVar = 0;
		// Voice samples (if enabled in channel).
		if (channels[channelNum].hasVoiceColumns)
		{
			for (int i = 0; i < 5; i++)
			{
				if (xOffset > 4 && xOffset < 91)
				{
					int voiceNum = loadedFrame.rows[y + intScrollY].voiceSamples[channelNum].sample[i];
					if (voiceNum < 44) // Note
					{
						if (voiceNum < 22)
							gui.activeUI[xOffset][y + 16].sprite = { 4 + voiceNum, 17 };
						else
							gui.activeUI[xOffset][y + 16].sprite = { 4 + voiceNum - 22, 18 };

						gui.activeUI[xOffset][y + 16].textCol = textColor + 9;
					}
					/*
					else
					{
						gui.activeUI[xOffset][y + 16].sprite = { 16, 0 };
						gui.activeUI[xOffset][y + 16].textCol = gui.activeUI[xOffset][y + 16].bgCol;
					}*/
					xOffset++;
				}
			}
		}


		for (int ef = 0; ef < channels[channelNum].effectCountPerRow; ef++)
		{
			int effectVal = loadedFrame.rows[y + intScrollY].effects[channelNum].cEffectValue[ef];

			if (loadedFrame.rows[y + intScrollY].effects[channelNum].cEffect[ef] > -1) // Effect
			{
				int effectNum = loadedFrame.rows[y + intScrollY].effects[channelNum].cEffect[ef];
				int offset = 0;

				if (effectNum > 99)
				{
					int val = effectNum % 100;
					int valExtended = effectNum - val;
					if (xOffset > 4 && xOffset < 91)
					{
						gui.activeUI[xOffset][y + 16].sprite = { val, 1 };
						gui.activeUI[xOffset][y + 16].textCol = textColor;
					}
					xOffset++;

					if (xOffset > 4 && xOffset < 91)
					{
						if (valExtended == 100)
							gui.activeUI[xOffset][y + 16].sprite = { 3, 1 };
						else
							gui.activeUI[xOffset][y + 16].sprite = { 8, 1 };
						gui.activeUI[xOffset][y + 16].textCol = textColor;
					}
					xOffset++;
				}
				else
				{
					if (xOffset > 4 && xOffset < 91)
					{
						gui.activeUI[xOffset][y + 16].sprite = { effectNum, 1 };
						gui.activeUI[xOffset][y + 16].textCol = textColor;
					}
					xOffset++;
					if (xOffset > 4 && xOffset < 91)
					{
						gui.activeUI[xOffset][y + 16].sprite = { 30, 0 };
						gui.activeUI[xOffset][y + 16].textCol = textColor;
					}
					xOffset++;
				}

				if (xOffset > 4 && xOffset + 1 < 91)
					DrawHex(effectVal, xOffset, y + 16, textColor + 6, bgColor);

				if (xOffset > 4 && xOffset < 91)
					gui.activeUI[xOffset][y + 16].textCol = textColor + 3;
				xOffset++;
				if (xOffset > 4 && xOffset < 91)
					gui.activeUI[xOffset][y + 16].textCol = textColor + 3;
				xOffset++;
			}
			else
				xOffset += 4;
		}
	}


	if (y + intScrollY >= editor.noteSelectionStart.y && y + intScrollY <= editor.noteSelectionEnd.y) // Draw selected notes.
	{
		int xStart = editor.noteSelectionStart.x + 5 - gui.frameScroll.x;
		int xEnd = editor.noteSelectionEnd.x + 5 - gui.frameScroll.x;

		if (xStart < 5)
			xStart = 5;
		else if (xStart > 91)
			xStart = 91;
		if (xEnd < 5)
			xEnd = 5;
		else if (xEnd > 91)
			xEnd = 91;

		for (int x = xStart; x < xEnd; x++)
		{
			if (gui.lightMode)
			{
				gui.activeUI[x][y + 16].bgCol = 1;
				gui.activeUI[x][y + 16].textCol = 4;
			}
			else
			{
				gui.activeUI[x][y + 16].bgCol = 4;
				gui.activeUI[x][y + 16].textCol = 0;
			}
		}
	}

	return;
}



void DrawChannelCompressed(int channelNum, int offsetX)
{
	if (loadedSong.currentNote >= loadedFrame.rows.size())
		return;

	int intScrollY = int(gui.frameScroll.y);


	for (int y = 0; y < 40; y++)
	{
		int bgColor = 0;
		int textColor = 5;

		if (channels[channelNum].muted || y >= loadedFrame.rows.size())
		{
			bgColor--;
			if (bgColor < 0)
				bgColor = 0;
			textColor -= 2;
		}

		if (y + intScrollY == loadedSong.currentNote) // Note position marker.
		{
			bgColor = 2;
		}
		else if (loadedFrame.beatsPerMeasure > 0 && y + intScrollY < loadedFrame.rows.size() && (y + intScrollY) % loadedFrame.beatsPerMeasure == 0)
			bgColor = 1;


		if (gui.lightMode)
		{
			if (bgColor < 5) bgColor++;
			if (textColor < 5) textColor++;
			if (bgColor < 6 && bgColor >= 0)
				bgColor = 5 - bgColor;
			if (textColor < 6 && textColor >= 0)
				textColor = 5 - textColor;
		}



		if (offsetX > 4 && offsetX < 91)
		{
			gui.activeUI[offsetX][y + 16].sprite = { 16, 0 };
			gui.activeUI[offsetX][y + 16].bgCol = bgColor;
			gui.activeUI[offsetX][y + 16].textCol = bgColor + 1;
			if (channels[channelNum].muted || y >= loadedFrame.rows.size())
				gui.activeUI[offsetX][y + 16].textCol = 0;
		}
		if (offsetX > 3 && offsetX < 90)
		{
			gui.activeUI[1 + offsetX][y + 16].sprite = { 16, 0 };
			gui.activeUI[1 + offsetX][y + 16].bgCol = bgColor;
			gui.activeUI[1 + offsetX][y + 16].textCol = bgColor + 1;
			if (channels[channelNum].muted || y >= loadedFrame.rows.size())
				gui.activeUI[offsetX + 1][y + 16].textCol = 0;
		}
		




		if (y + intScrollY < loadedFrame.rows.size())
		{
			if (loadedFrame.rows[y + intScrollY].note[channelNum] > -1) // Note
			{
				if (offsetX > 4)
				{
					if (loadedFrame.rows[y + intScrollY].volume[channelNum] > -1) // Volume
					{
						if (channels[channelNum].muted)
							gui.activeUI[offsetX][y + 16].sprite = { 23, 11 };
						else
							gui.activeUI[offsetX][y + 16].sprite = { 25, 11 };
					}
					else
					{
						if (channels[channelNum].muted)
							gui.activeUI[offsetX][y + 16].sprite = { 23, 10 };
						else
							gui.activeUI[offsetX][y + 16].sprite = { 25, 10 };
					}
					
					//DrawNote(uint8_t(loadedFrame.rows[y + intScrollY].note[channelNum]), offsetX, y + 16, textColor, bgColor, -1, -1);
				}
				else if (loadedFrame.rows[y + intScrollY].volume[channelNum] > -1) // Volume
				{
					if (channels[channelNum].muted)
						gui.activeUI[offsetX][y + 16].sprite = { 23, 12 };
					else
						gui.activeUI[offsetX][y + 16].sprite = { 25, 12 };
				}
			}
			else // No note
			{
				if (loadedFrame.rows[y + intScrollY].volume[channelNum] > -1) // Volume
				{
					if (channels[channelNum].muted)
						gui.activeUI[offsetX][y + 16].sprite = { 23, 12 };
					else
						gui.activeUI[offsetX][y + 16].sprite = { 25, 12 };
				}
			}


			if (offsetX > 3)
			{
				bool effectExists = false;

				for (int ef = 0; ef < channels[channelNum].effectCountPerRow; ef++)
				{
					if (loadedFrame.rows[y + intScrollY].effects[channelNum].cEffect[ef] > -1) // Effect
					{
						effectExists = true;
					}
				}

				if (effectExists)
				{
					if (loadedFrame.rows[y + intScrollY].volume[channelNum] > -1) // Volume
					{
						if (channels[channelNum].muted)
							gui.activeUI[offsetX + 1][y + 16].sprite = { 24, 11 };
						else
							gui.activeUI[offsetX + 1][y + 16].sprite = { 26, 11 };
					}
					else
					{
						if (channels[channelNum].muted)
							gui.activeUI[offsetX + 1][y + 16].sprite = { 24, 10 };
						else
							gui.activeUI[offsetX + 1][y + 16].sprite = { 26, 10 };
					}
				}
				else if (loadedFrame.rows[y + intScrollY].volume[channelNum] > -1) // Volume
				{
					if (channels[channelNum].muted)
						gui.activeUI[offsetX + 1][y + 16].sprite = { 24, 12 };
					else
						gui.activeUI[offsetX + 1][y + 16].sprite = { 26, 12 };
				}
			}


			// Draw stop note
			if (loadedFrame.rows[y + intScrollY].note[channelNum] == 255) // Stop Note
			{
				if (offsetX > 4 && offsetX < 91)
					gui.activeUI[offsetX][y + 16].sprite = { 22, 10 };
			}




			
			
		}


		if (y + intScrollY >= editor.noteSelectionStart.y && y + intScrollY <= editor.noteSelectionEnd.y) // Draw selected notes.
		{
			int xStart = editor.noteSelectionStart.x + 5 - gui.frameScroll.x;
			int xEnd = editor.noteSelectionEnd.x + 5 - gui.frameScroll.x;

			if (xStart < 5)
				xStart = 5;
			else if (xStart > 91)
				xStart = 91;
			if (xEnd < 5)
				xEnd = 5;
			else if (xEnd > 91)
				xEnd = 91;

			for (int x = xStart; x < xEnd; x++)
			{
				gui.activeUI[x][y + 16].bgCol = 4;
				gui.activeUI[x][y + 16].textCol = 0;
			}
		}
	}

	return;
}



void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	if (textEnd > 90)
		textEnd = 90;

	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		selectedChar = gui.selectedTile.x - textStart;
	}

	std::string textS = std::to_string(int(text));
	for (int i = 0; i < textEnd - textStart; i++)
	{
		int textI = i - (textEnd - textStart) + textS.length();
		Vector2i textSprite = { 17, 0 };

		if (textI > -1 && textI < textS.length())
		{
			if (textS.at(textI) > 47 && textS.at(textI) < 58) // 0-9
				textSprite = { textS.at(textI) - 48, 0 };
		}
		else
			textSprite = { 0, 0 };

		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
		gui.activeUI[textStart + i][textY].bgCol = bgCol;

		if (i == selectedChar)
			gui.activeUI[textStart + i][textY].bgCol = 3;
	}

	return;
}



void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	if (textEnd > 90)
		textEnd = 90;

	std::string textS = std::to_string(int(text));
	for (int i = 0; i < textEnd - textStart; i++)
	{
		int textI = i - (textEnd - textStart) + textS.length();
		Vector2i textSprite = { 17, 0 };

		if (textI > -1 && textI < textS.length())
		{
			if (textS.at(textI) > 47 && textS.at(textI) < 58) // 0-9
				textSprite = { textS.at(textI) - 48, 0 };
		}
		else
			textSprite = { 0, 0 };

		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
		gui.activeUI[textStart + i][textY].bgCol = bgCol;
	}

	return;
}



void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol)
{
	if (gui.lightMode)
	{
		if (bgCol >= 0)
		{
			if (bgCol < 5) bgCol++;
			if (bgCol < 6 && bgCol >= 0)
				bgCol = 5 - bgCol;
		}

		if (textCol < 5) textCol++;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	for (int i = 0; i < textEnd - textStart; i++)
	{
		if (i > textEnd)
			return;

		Vector2i textSprite = { 17, 0 };

		if (i < text.length())
		{
			if (text.at(i) > 39 && text.at(i) < 42) // (-)
				textSprite = { text.at(i) - 40 + 13, 0 };
			else if (text.at(i) > 47 && text.at(i) < 58) // 0-9
				textSprite = { text.at(i) - 48, 0 };
			else if (text.at(i) > 64 && text.at(i) < 91) // A-Z
				textSprite = { text.at(i) - 65, 1 };
			else if (text.at(i) > 96 && text.at(i) < 123) // a-z
				textSprite = { text.at(i) - 97, 2 };
			else if (text.at(i) == 44) // ,
				textSprite = { 23, 0 };
			else if (text.at(i) == 45) // -
				textSprite = { 11, 0 };
			else if (text.at(i) == 46) // .
				textSprite = { 22, 0 };
			else if (text.at(i) == 33) // !
				textSprite = { 27, 0 };
			else if (text.at(i) == 63) // ?
				textSprite = { 26, 0 };
			else if (text.at(i) == 58 || text.at(i) == 59) // :
				textSprite = { 30, 0 };
			else if (text.at(i) == 38) // &
				textSprite = { 28, 0 };
			else if (text.at(i) == 39 || text.at(i) == 34) // '
				textSprite = { 26, 1 };
		}

		// Draw text as button.
		if (bgCol == -1)
		{
			if (textSprite.y == 0)
			{
				textSprite.y = 31;
			}
			else
			{
				if (textSprite.y != 1)
					textSprite.x = 26;
				textSprite.y = 30;
			}
		}
		else
			gui.activeUI[textStart + i][textY].bgCol = bgCol;


		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
	}

	return;
}



void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}



	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		selectedChar = gui.selectedTile.x - textStart;
	}

	for (int i = 0; i < textEnd - textStart; i++)
	{
		if (i > textEnd)
			return;

		Vector2i textSprite = { 17, 0 };

		if (i < text.length())
		{
			if (text.at(i) > 39 && text.at(i) < 42) // (-)
				textSprite = { text.at(i) - 40 + 13, 0 };
			else if (text.at(i) > 47 && text.at(i) < 58) // 0-9
				textSprite = { text.at(i) - 48, 0 };
			else if (text.at(i) > 64 && text.at(i) < 91) // A-Z
				textSprite = { text.at(i) - 65, 1 };
			else if (text.at(i) > 96 && text.at(i) < 123) // a-z
				textSprite = { text.at(i) - 97, 2 };
			else if (text.at(i) == 44) // ,
				textSprite = { 23, 0 };
			else if (text.at(i) == 45) // -
				textSprite = { 11, 0 };
			else if (text.at(i) == 46) // .
				textSprite = { 22, 0 };
			else if (text.at(i) == 33) // !
				textSprite = { 27, 0 };
			else if (text.at(i) == 63) // ?
				textSprite = { 26, 0 };
			else if (text.at(i) == 58) // :
				textSprite = { 30, 0 };
			else if (text.at(i) == 38) // &
				textSprite = { 28, 0 };
		}

		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
		gui.activeUI[textStart + i][textY].bgCol = bgCol;

		if (i == selectedChar)
			gui.activeUI[textStart + i][textY].bgCol = 3;
	}

	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		if (loadedSong.songName.length() == 0)
			gui.activeUI[textStart][textY].bgCol = 3;
	}

	return;
}



void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, bool continueDown)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	int yPos = textY;
	int xPos = textStart;

	for (int i = 0; i < text.length(); i++)
	{
		if (xPos > textEnd)
		{
			yPos++;
			xPos = textStart;

			if (yPos > 55)
				return;
		}

		Vector2i textSprite = { 17, 0 };

		if (i < text.length())
		{
			if (text.at(i) > 39 && text.at(i) < 42) // (-)
				textSprite = { text.at(i) - 40 + 13, 0 };
			else if (text.at(i) > 47 && text.at(i) < 58) // 0-9
				textSprite = { text.at(i) - 48, 0 };
			else if (text.at(i) > 64 && text.at(i) < 91) // A-Z
				textSprite = { text.at(i) - 65, 1 };
			else if (text.at(i) > 96 && text.at(i) < 123) // a-z
				textSprite = { text.at(i) - 97, 2 };
			else if (text.at(i) == 44) // ,
				textSprite = { 23, 0 };
			else if (text.at(i) == 45) // -
				textSprite = { 11, 0 };
			else if (text.at(i) == 46) // .
				textSprite = { 22, 0 };
			else if (text.at(i) == 33) // !
				textSprite = { 27, 0 };
			else if (text.at(i) == 63) // ?
				textSprite = { 26, 0 };
			else if (text.at(i) == 58) // :
				textSprite = { 30, 0 };
			else if (text.at(i) == 38) // &
				textSprite = { 28, 0 };
		}

		gui.activeUI[xPos][yPos].sprite = textSprite;
		gui.activeUI[xPos][yPos].textCol = textCol;
		gui.activeUI[xPos][yPos].bgCol = bgCol;

		xPos++;
	}

	return;
}


void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	Vector2i textSprite = { 15, 0 };

	int value1 = num / 16;
	int value2 = num % 16;

	//std::cout << "  " << value1 << "  " << value2;

	if (value1 < 10)
		textSprite = { value1, 0 };
	else
		textSprite = { value1 - 10, 1 };

	gui.activeUI[textStart][textY].sprite = textSprite;
	gui.activeUI[textStart][textY].textCol = textCol;
	gui.activeUI[textStart][textY].bgCol = bgCol;

	if (value2 < 10)
		textSprite = { value2, 0 };
	else
		textSprite = { value2 - 10, 1 };

	gui.activeUI[textStart + 1][textY].sprite = textSprite;
	gui.activeUI[textStart + 1][textY].textCol = textCol;
	gui.activeUI[textStart + 1][textY].bgCol = bgCol;

	return;
}



void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	if (textStart > 90)
		return;

	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		selectedChar = gui.selectedTile.x - textStart;
	}

	Vector2i textSprite = { 15, 0 };

	int value1 = num / 16;
	int value2 = num % 16;

	//std::cout << "  " << value1 << "  " << value2;

	if (value1 < 10)
		textSprite = { value1, 0 };
	else
		textSprite = { value1 - 10, 1 };

	gui.activeUI[textStart][textY].sprite = textSprite;
	gui.activeUI[textStart][textY].textCol = textCol;
	gui.activeUI[textStart][textY].bgCol = bgCol;

	if (textStart > 89)
		return;

	if (value2 < 10)
		textSprite = { value2, 0 };
	else
		textSprite = { value2 - 10, 1 };

	gui.activeUI[textStart + 1][textY].sprite = textSprite;
	gui.activeUI[textStart + 1][textY].textCol = textCol;
	gui.activeUI[textStart + 1][textY].bgCol = bgCol;


	if (selectedChar == 0)
		gui.activeUI[textStart][textY].bgCol = 3;
	else if (selectedChar == 1)
		gui.activeUI[textStart + 1][textY].bgCol = 3;

	return;
}


void DrawNote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	if (textStart > 90)
		return;

	// A, A#, B, C, C#, D, D#, E, F, F#, G, G#
	int letter = num % 12;
	int octave = int(float(num) / 12.0f);

	switch (letter)
	{
		case 0: // C
		{
			gui.activeUI[textStart][textY].sprite = { 2, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}
		case 1: // C#
		{
			gui.activeUI[textStart][textY].sprite = { 2, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 2: // D
		{
			gui.activeUI[textStart][textY].sprite = { 3, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 3: // D#
		{
			gui.activeUI[textStart][textY].sprite = { 3, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 4: // E
		{
			gui.activeUI[textStart][textY].sprite = { 4, 1 };
			if (textStart < 89)
				gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 5: // F
		{
			gui.activeUI[textStart][textY].sprite = { 5, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 6: // F#
		{
			gui.activeUI[textStart][textY].sprite = { 5, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 7: // G
		{
			gui.activeUI[textStart][textY].sprite = { 6, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 8: // G#
		{
			gui.activeUI[textStart][textY].sprite = { 6, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 9: // A
		{
			gui.activeUI[textStart][textY].sprite = { 0, 1 };
			if (textStart < 90)
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 10: // A#
		{
			gui.activeUI[textStart][textY].sprite = { 0, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		default: // B
		{
			gui.activeUI[textStart][textY].sprite = { 1, 1 };
			if (textStart < 90)
				gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}
	}

	if (textStart > 90)
		return;
	gui.activeUI[textStart][textY].textCol = textCol;
	gui.activeUI[textStart][textY].bgCol = bgCol;
	if (textStart > 89)
		return;
	gui.activeUI[textStart + 1][textY].textCol = textCol;
	gui.activeUI[textStart + 1][textY].bgCol = bgCol;
	DrawNum(octave, textStart + 2, textStart + 3, textY, textCol, bgCol);

	return;
}


void DrawEDONote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{

	if (textStart > 90)
		return;

	// A, A#, B, C, C#, D, D#, E, F, F#, G, G#
	int scaleDeg = num % loadedSong.edo;
	int octave = num / loadedSong.edo;

	float floatLetter = (float(scaleDeg) / float(loadedSong.edo)) * 12.0f;
	int scaleLetter = floatLetter;
	float betweenNotes = floatLetter - scaleLetter;

	bool sharped = false;

	switch (scaleLetter)
	{
	case 0: // C
	{
		gui.activeUI[textStart][textY].sprite = { 2, 1 };
		break;
	}
	case 1: // C#
	{
		gui.activeUI[textStart][textY].sprite = { 2, 1 };
		sharped = true;
		break;
	}

	case 2: // D
	{
		gui.activeUI[textStart][textY].sprite = { 3, 1 };
		break;
	}

	case 3: // D#
	{
		gui.activeUI[textStart][textY].sprite = { 3, 1 };
		sharped = true;
		break;
	}

	case 4: // E
	{
		gui.activeUI[textStart][textY].sprite = { 4, 1 };
		break;
	}

	case 5: // F
	{
		gui.activeUI[textStart][textY].sprite = { 5, 1 };
		break;
	}

	case 6: // F#
	{
		gui.activeUI[textStart][textY].sprite = { 5, 1 };
		sharped = true;
		break;
	}

	case 7: // G
	{
		gui.activeUI[textStart][textY].sprite = { 6, 1 };
		break;
	}

	case 8: // G#
	{
		gui.activeUI[textStart][textY].sprite = { 6, 1 };
		sharped = true;
		break;
	}

	case 9: // A
	{
		gui.activeUI[textStart][textY].sprite = { 0, 1 };
		break;
	}

	case 10: // A#
	{
		gui.activeUI[textStart][textY].sprite = { 0, 1 };
		sharped = true;
		break;
	}

	default: // B
	{
		gui.activeUI[textStart][textY].sprite = { 1, 1 };
		break;
	}
	}

	int lightT = textCol;
	int lightB = bgCol;
	if (gui.lightMode)
	{
		if (lightB < 5) lightB++;
		if (lightT < 5) lightT++;
		if (lightB < 6 && lightB >= 0)
			lightB = 5 - lightB;
		if (lightT < 6 && lightT >= 0)
			lightT = 5 - lightT;
	}
	if (textStart > 90)
		return;
	gui.activeUI[textStart][textY].textCol = lightT;
	gui.activeUI[textStart][textY].bgCol = lightB;
	if (textStart > 89)
		return;
	gui.activeUI[textStart + 1][textY].textCol = lightT;
	gui.activeUI[textStart + 1][textY].bgCol = lightB;


	DrawNum(octave, textStart + 2, textStart + 3, textY, textCol, bgCol);


	////////////////////////////////////////////////////////////////////////////////////

	if (textStart < 90)
	{
		if (betweenNotes < 0.25f)
			gui.activeUI[textStart + 1][textY].sprite = { 0, 17 + sharped };
		else if (betweenNotes < 0.5f)
			gui.activeUI[textStart + 1][textY].sprite = { 1, 17 + sharped };
		else if (betweenNotes < 0.75f)
			gui.activeUI[textStart + 1][textY].sprite = { 2, 17 + sharped };
		else
			gui.activeUI[textStart + 1][textY].sprite = { 3, 17 + sharped };
	}

	return;
}


void DrawKey(int button, int key, Vector2i pos)
{
	if (button == editor.selectedKey) // Key selected
	{
		gui.activeUI[pos.x][pos.y].textCol = 5;
		gui.activeUI[pos.x][pos.y].bgCol = 16;
		gui.activeUI[pos.x + 1][pos.y].textCol = 5;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 16;
	}
	else
	{
		gui.activeUI[pos.x][pos.y].textCol = 3;
		gui.activeUI[pos.x][pos.y].bgCol = 0;
		gui.activeUI[pos.x + 1][pos.y].textCol = 3;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 0;
	}

	gui.activeUI[pos.x][pos.y].sprite = { key * 2, 9 };
	gui.activeUI[pos.x + 1][pos.y].sprite = { key * 2 + 1, 9 };

	return;
}


void DrawEDOKey(int key, Vector2i pos)
{
	if (key == editor.selectedKey) // Key selected
	{
		gui.activeUI[pos.x][pos.y].bgCol = 3;
		gui.activeUI[pos.x][pos.y].textCol = 5;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 3;
		gui.activeUI[pos.x + 1][pos.y].textCol = 5;
	}
	else
	{
		gui.activeUI[pos.x][pos.y].bgCol = 1;
		gui.activeUI[pos.x][pos.y].textCol = 3;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 1;
		gui.activeUI[pos.x + 1][pos.y].textCol = 3;
	}


	int scaleDeg = key % loadedSong.edo;

	float floatLetter = (float(scaleDeg) / float(loadedSong.edo)) * 12.0f;
	int scaleLetter = floatLetter;
	float betweenNotes = floatLetter - scaleLetter;


	

	bool sharped = false;

	if (scaleLetter < 2) // C/C#
	{
		if (scaleLetter == 1)
			sharped = true;
		else if (betweenNotes < 0.25f)
		{
			if (key == editor.selectedKey) // Key selected
				gui.activeUI[int(pos.x)][int(pos.y)].textCol = 8;
			else
				gui.activeUI[int(pos.x)][int(pos.y)].textCol = 7;
		}
		scaleLetter = 2;
	}
	else if (scaleLetter < 4) // D/D#
	{
		if (scaleLetter == 3)
			sharped = true;
		scaleLetter = 3;
	}
	else if (scaleLetter == 4) // E
	{
		scaleLetter = 4;
	}
	else if (scaleLetter < 7) // F/F#
	{
		if (scaleLetter == 6)
			sharped = true;
		scaleLetter = 5;
	}
	else if (scaleLetter < 9) // G/G#
	{
		if (scaleLetter == 8)
			sharped = true;
		scaleLetter = 6;
	}
	else if (scaleLetter < 11) // A/A#
	{
		if (scaleLetter == 10)
			sharped = true;
		scaleLetter = 0;
	}
	else // B
	{
		scaleLetter = 1;
	}

	//int keyLetter = (key )

	

	gui.activeUI[int(pos.x)][int(pos.y)].sprite = { scaleLetter, 16 };
	if (betweenNotes < 0.25f)
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 0, 17 + sharped };
	else if (betweenNotes < 0.5f)
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 1, 17 + sharped };
	else if (betweenNotes < 0.75f)
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 2, 17 + sharped };
	else
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 3, 17 + sharped };

	return;
}


void DrawVoiceKey(int key, Vector2i pos)
{
	if (key < 22)
		gui.activeUI[pos.x][pos.y].sprite = { 4 + key, 17 };
	else if (key < 44)
		gui.activeUI[pos.x][pos.y].sprite = { 4 + key - 22, 18 };

	if (key == editor.selectedVoiceSample)
	{
		gui.activeUI[pos.x][pos.y].textCol = 5;
		gui.activeUI[pos.x][pos.y].bgCol = 12;
	}
	else
		gui.activeUI[pos.x][pos.y].textCol = 13;

	return;
}


void DrawFloatingWindow(FloatingWindow* wind)
{
	//std::cout << wind->name;
	// Top
	gui.activeUI[int(wind->position.x)][int(wind->position.y)].sprite = { 20, 5 };
	for (int i = wind->position.x + 1; i < wind->position.x + wind->size.x; i++)
		gui.activeUI[i][int(wind->position.y)].sprite = { 21, 5 };

	DrawGUIText(wind->name, int(wind->position.x + 2), int(wind->position.x + 2 + wind->name.length()), int(wind->position.y), 5, 16);

	if (gui.lightMode)
	{
		for (int x = 2; x < wind->name.length() + 2; x++)
			gui.activeUI[int(wind->position.x + x)][int(wind->position.y)].textCol = 5;
	}


	gui.activeUI[int(wind->position.x + wind->size.x - 2)][int(wind->position.y)].sprite = { 29, 9 }; // Exit button
	gui.activeUI[int(wind->position.x + wind->size.x - 1)][int(wind->position.y)].sprite = { 30, 9 };
	gui.activeUI[int(wind->position.x + wind->size.x - 2)][int(wind->position.y)].bgCol = 16;
	gui.activeUI[int(wind->position.x + wind->size.x - 1)][int(wind->position.y)].bgCol = 16;


	gui.activeUI[int(wind->position.x + wind->size.x)][int(wind->position.y)].sprite = { 22, 5 };

	// Body
	for (int y = wind->position.y + 1; y < wind->position.y + wind->size.y - 1; y++)
	{
		gui.activeUI[int(wind->position.x)][y].sprite = {20, 6};
		for (int x = wind->position.x + 1; x < wind->position.x + wind->size.x; x++)
		{
			gui.activeUI[x][y].sprite = { 21, 6 };
		}
		gui.activeUI[int(wind->position.x + wind->size.x)][y].sprite = { 22, 6 };
	}

	// Bottom
	gui.activeUI[int(wind->position.x)][int(wind->position.y + wind->size.y - 1)].sprite = { 11, 4 };
	for (int i = wind->position.x + 1; i < wind->position.x + wind->size.x; i++)
		gui.activeUI[i][int(wind->position.y + wind->size.y - 1)].sprite = { 12, 4 };
	gui.activeUI[int(wind->position.x + wind->size.x)][int(wind->position.y + wind->size.y - 1)].sprite = { 13, 4 };


	// Draw contents
	if (wind->name == "Themes")
	{
		std::string themeText[32] = {
			"RED", "BROWN PINK", "CLAY", "MAGNOLIA",
			"PALE", "MAHOGANY", "CLOUDY", "RED SAND",
			"SAND", "GOLD", "BURNT", "WHITE",
			"NEUTRAL GREY","BLUE GREEN", "CREEK", "TOXIC",
			"RUST", "FOREST", "EMERALD", "BLUE GOLD",
			"LOTIS", "SEA FOAM", "NAUTICAL", "SILVER",
			"LUNAR", "BLUE GLASS", "STANDARD", "PLASTIC",
			"DEEP SEA", "BRIGHT PINK", "PURPLE", "COSMIC",
		};

		std::string lightModeText = "LIGHT MODE";
		if (gui.lightMode)
			lightModeText = "DARK MODE";
		


		for (int i = 0; i < 32; i++)
		{
			DrawGUIText(themeText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 1 + i, 4, -1);
			if (i == gui.uiColorTheme)
			{
				gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1 + i)].sprite = { 5, 4 };
				gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1 + i)].sprite = { 5, 4 };
			}
			else
			{
				gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1 + i)].sprite = { 5, 28 };
				gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1 + i)].sprite = { 6, 28 };
			}
		}

		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1)].sprite = { 5, 27 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1)].sprite = { 6, 27 };

		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 32)].sprite = { 5, 29 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 32)].sprite = { 6, 29 };

		DrawGUIText(lightModeText, wind->position.x + 2, wind->position.x + 15, wind->position.y + 34, 4, -1);
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 34)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 34)].sprite = { 6, 26 };
	}
	else if (wind->name == "Save and Exit")
	{
		DrawGUIText("Would you like to", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("save your song", wind->position.x + 2, wind->position.x + 19, wind->position.y + 3, 3, 0);
		DrawGUIText("before exiting?", wind->position.x + 2, wind->position.x + 19, wind->position.y + 4, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 6)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 6, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 6)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 8)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 8, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 8)].sprite = { 6, 26 };
	}
	else if (wind->name == "Save and Load")
	{
		DrawGUIText("Would you like to", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("save your current", wind->position.x + 2, wind->position.x + 19, wind->position.y + 3, 3, 0);
		DrawGUIText("song before", wind->position.x + 2, wind->position.x + 19, wind->position.y + 4, 3, 0);
		DrawGUIText("loading this?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Delete Frames")
	{
		DrawGUIText("This song contains", wind->position.x + 2, wind->position.x + 19, wind->position.y + 3, 4, 0);
		DrawGUIText("unused frames.", wind->position.x + 2, wind->position.x + 16, wind->position.y + 4, 3, 0);
		DrawGUIText("Would you like ", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);
		DrawGUIText("to keep them?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 6, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Overwrite Song")
	{
		DrawGUIText("This song already", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("exists. Would", wind->position.x + 2, wind->position.x + 16, wind->position.y + 3, 3, 0);
		DrawGUIText("you like to", wind->position.x + 2, wind->position.x + 17, wind->position.y + 4, 3, 0);
		DrawGUIText("Replace it?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Load File")
	{
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1)].sprite = { 25, 5 };
		DrawGUIText(fileNavigator.filePathName, wind->position.x + 2, wind->position.x + 40, wind->position.y + 1, 4, 2);

		for (int y = 2; y < 19; y++)
		{
			if (y - 2 + gui.fileListScroll < fileNavigator.fileNames.size())
			{
				if (y - 2 + gui.fileListScroll == editor.selectedFile)
					DrawGUIText(fileNavigator.fileNames[y - 2 + gui.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 5, 1);
				else
					DrawGUIText(fileNavigator.fileNames[y - 2 + gui.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

				if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '0')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 26, 5 };
				else if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '1')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 27, 5 };
				else if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '2')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 28, 5 };
				else
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 29, 5 };
			}
			else
				DrawGUIText("", wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

			gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + y)].sprite = { 31, 3 };
		}

		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 2)].sprite = { 31, 2 };
		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 18)].sprite = { 31, 4 };


		gui.scrollBars[4].topLeft.x = wind->position.x + 39;
		gui.scrollBars[4].topLeft.y = wind->position.y + 2;

		DrawGUIText("LOAD", wind->position.x + 30, wind->position.x + 34, wind->position.y + 19, 4, -1);
		
		gui.activeUI[int(wind->position.x + 29)][int(wind->position.y + 19)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 34)][int(wind->position.y + 19)].sprite = { 6, 26 };
	}
	else if (wind->name == "Save Song")
	{
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1)].sprite = { 25, 5 };
		DrawGUIText(fileNavigator.filePathName, wind->position.x + 2, wind->position.x + 40, wind->position.y + 1, 4, 2);

		for (int y = 2; y < 19; y++)
		{
			if (y - 2 + gui.fileListScroll < fileNavigator.fileNames.size())
			{
				if (y - 2 + gui.fileListScroll == editor.selectedFile)
					DrawGUIText(fileNavigator.fileNames[y - 2 + gui.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 5, 1);
				else
					DrawGUIText(fileNavigator.fileNames[y - 2 + gui.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

				if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '0')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 26, 5 };
				else if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '1')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 27, 5 };
				else if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '2')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 28, 5 };
				else
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 29, 5 };
			}
			else
				DrawGUIText("", wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

			gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + y)].sprite = { 31, 3 };
		}

		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 2)].sprite = { 31, 2 };
		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 18)].sprite = { 31, 4 };


		gui.scrollBars[4].topLeft.x = wind->position.x + 39;
		gui.scrollBars[4].topLeft.y = wind->position.y + 2;

		DrawGUIText("SAVE", wind->position.x + 30, wind->position.x + 34, wind->position.y + 19, 4, -1);

		gui.activeUI[int(wind->position.x + 29)][int(wind->position.y + 19)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 34)][int(wind->position.y + 19)].sprite = { 6, 26 };
	}
	else if (wind->name == "Settings")
	{
		std::string settingsText[4] = {
			"THEMES", "CHANNEL FOCUS", "BACKGROUND", "PHONEMES"
		};

		

		for (int i = 0; i < 4; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);

			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}

		if (editor.channelFocus)
			gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 4)].sprite = { 24, 6 };
		else
			gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 4)].sprite = { 23, 6 };

		if (gui.background)
			gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 6)].sprite = { 24, 6 };
		else
			gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 6)].sprite = { 23, 6 };
	}
	else if (wind->name == "Selection")
	{
		std::string settingsText[5] = {
			"COPY", "PASTE", "DELETE", "TRANSPOSE", "SET SAMPLE"
		};
		for (int i = 0; i < 5; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);

			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Transpose")
	{
		DrawGUIText("Octaves", wind->position.x + 2, wind->position.x + 12, wind->position.y + 2, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 3)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 3)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 3)].sprite = { 10, 3 };
		DrawGUIText("Semitones", wind->position.x + 2, wind->position.x + 12, wind->position.y + 4, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 5)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 5)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 5)].sprite = { 10, 3 };
		DrawGUIText("C-4 to ", wind->position.x + 2, wind->position.x + 12, wind->position.y + 7, 3, 0);
		DrawEDONote(editor.transposeValue + 48, wind->position.x + 9, wind->position.y + 7, 3, 0, -1, -1);

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 9)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Help")
	{

		int helpPageScroll = ((helpPageText.size() + 40) / 39) * gui.scrollBars[4].position;
		
		for (int y = 2; y < 39; y++)
		{
			if (y - 2 + helpPageScroll < helpPageText.size())
			{
				if (helpPageText[y - 2 + helpPageScroll][0] == '#')
				{
					for (int x = 1; x < 39; x++)
					{
						gui.activeUI[int(wind->position.x + x)][int(wind->position.y + y)].sprite = { 21, 0 };
					}
				}
				else
					DrawGUIText(helpPageText[y - 2 + helpPageScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);
			}
			else
				DrawGUIText("", wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);
			gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + y)].sprite = { 31, 3 };
		}

		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 2)].sprite = { 31, 2 };
		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 38)].sprite = { 31, 4 };


		gui.scrollBars[4].topLeft.x = wind->position.x + 39;
		gui.scrollBars[4].topLeft.y = wind->position.y + 2;
	}
	else if (wind->name == "Export as .WAV")
	{
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1)].sprite = { 25, 5 };
		DrawGUIText(fileNavigator.filePathName, wind->position.x + 2, wind->position.x + 40, wind->position.y + 1, 4, 2);

		for (int y = 2; y < 19; y++)
		{
			if (y - 2 + gui.fileListScroll < fileNavigator.fileNames.size())
			{
				if (y - 2 + gui.fileListScroll == editor.selectedFile)
					DrawGUIText(fileNavigator.fileNames[y - 2 + gui.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 5, 1);
				else
					DrawGUIText(fileNavigator.fileNames[y - 2 + gui.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

				if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '0')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 26, 5 };
				else if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '1')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 27, 5 };
				else if (fileNavigator.fileNames[y - 2 + gui.fileListScroll].at(0) == '2')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 28, 5 };
				else
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 29, 5 };
			}
			else
				DrawGUIText("", wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

			gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + y)].sprite = { 31, 3 };
		}

		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 2)].sprite = { 31, 2 };
		gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 18)].sprite = { 31, 4 };


		gui.scrollBars[4].topLeft.x = wind->position.x + 39;
		gui.scrollBars[4].topLeft.y = wind->position.y + 2;

		DrawGUIText("EXPORT", wind->position.x + 30, wind->position.x + 36, wind->position.y + 19, 4, -1);
		gui.activeUI[int(wind->position.x + 29)][int(wind->position.y + 19)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 36)][int(wind->position.y + 19)].sprite = { 6, 26 };

	}
	else if (wind->name == "Overwrite WAV")
	{
		DrawGUIText("This file already", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("exists. Would", wind->position.x + 2, wind->position.x + 16, wind->position.y + 3, 3, 0);
		DrawGUIText("you like to", wind->position.x + 2, wind->position.x + 17, wind->position.y + 4, 3, 0);
		DrawGUIText("Replace it?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Instrument Editor")
	{
		sampleDisplay.position.x = wind->position.x + 17.0f;
		sampleDisplay.position.y = wind->position.y + 31.0f;


		// Sample name
		DrawGUIText("INSTRUMENT NAME: ", wind->position.x + 1, wind->position.x + 17, wind->position.y + 1, 3, 1);
		if (!loadedSamples[editor.selectedSample].enabled)
			DrawGUIText("NO SAMPLE LOADED", wind->position.x + 17, wind->position.x + 34, wind->position.y + 1, 3, 1);
		else
			DrawGUIText(loadedSamples[editor.selectedSample].sampleName, wind->position.x + 14, wind->position.x + 34, wind->position.y + 1, 3, 1);

		// Volume/Arp speed
		if (sampleDisplay.displayArp)
		{
			DrawGUIText("Speed:", wind->position.x + 18, wind->position.x + 25, wind->position.y + 2, 3, 0);
			DrawHorizontalSlider(int(wind->position.x + 25), int(wind->position.y + 2), loadedSamples[editor.selectedSample].arpSpeed);
		}
		else
		{
			DrawGUIText("Volume:", wind->position.x + 18, wind->position.x + 25, wind->position.y + 2, 3, 0);
			DrawHorizontalSlider(int(wind->position.x + 25), int(wind->position.y + 2), loadedSamples[editor.selectedSample].volume);
		}

		// waveform or sample.
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 2)].sprite = { 5, 26 };
		if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
			DrawGUIText("WAVEFORM", wind->position.x + 3, wind->position.x + 12, wind->position.y + 2, 4, -1);
		else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
			DrawGUIText("SAMPLE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 2, 4, -1);
		else
			DrawGUIText("VOICE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 2, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 2)].sprite = { 6, 26 };


		if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
		{
			if (loadedSamples[editor.selectedSample].enabled)
			{
				DrawGUIText("SHAPE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 3, 3, 0);
				DrawWaveTypeButton(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType, int(wind->position.x) + 3, int(wind->position.y) + 4);

				// Reverse wave toggle.
				DrawGUIText("Reverse:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 11, 3, 0);
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].reverseFrames)
					gui.activeUI[int(wind->position.x + 9)][int(wind->position.y + 11)].sprite = { 24, 6 };
				else
					gui.activeUI[int(wind->position.x + 9)][int(wind->position.y + 11)].sprite = { 23, 6 };

				// Offset
				DrawGUIText("Offset:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 16, 3, 0);
				DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 16), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset);
			}
		}

		// Draw frequency/arp display.
		for (int x = 0; x < 15; x++)
		{
			if (sampleDisplay.displayArp)
			{
				if (x > loadedSamples[editor.selectedSample].arpLength)
				{
					for (int y = 1; y < 14; y++)
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 10, 19 };
				}
				else
				{
					for (int y = 1; y < 14; y++)
					{
						int offsetSprite = 0;

						float arpVal = loadedSamples[editor.selectedSample].arpPitches[x];

						bool onOct = (y == 13 || y == 10 || y == 7 || y == 4 || y == 1);
						bool overOct = (y - 1 == 13 || y - 1 == 10 || y - 1 == 7 || y - 1 == 4 || y - 1 == 1);

						if (y == int(arpVal))
						{
							if (arpVal - y == 0.0f)
							{
								offsetSprite++;
								if (overOct)
									offsetSprite += 5;
								if (y > 0)
									gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y + 1)].sprite = { 11 + offsetSprite, 19 };
							}
							else if (arpVal - y == 0.25f)
							{
								offsetSprite += 2;
								if (overOct)
									offsetSprite += 5;
								if (y > 0)
									gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y + 1)].sprite = { 11 + offsetSprite, 19 };
							}
							else if (arpVal - y == 0.5f)
							{
								offsetSprite += 3;
								if (overOct)
									offsetSprite += 5;
								if (y > 0)
									gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y + 1)].sprite = { 11 + offsetSprite, 19 };
							}
							else if (arpVal - y == 0.75f)
							{
								if (overOct)
									offsetSprite += 5;
								offsetSprite += 4;
							}

							if (overOct)
								offsetSprite -= 5;
						}

						if (onOct)
							offsetSprite += 5;

						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 11 + offsetSprite, 20 };
					}

					if (x == loadedSamples[editor.selectedSample].arpLength)
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16)].sprite = { 9, 19 };
					else
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16)].sprite = { 8, 19 };
				}
			}
			else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0 && x > 3)
			{
				int offsetSprite = 0;
				if (x == 0 || x == 4 || x == 6 || x == 8 || x == 10 || x == 14)
					offsetSprite = 1;
				else if (x == 7)
					offsetSprite = 2;


				for (int y = 0; y < 14; y++)
				{
					if (y < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4])
					{
						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4] - y == 0.25)
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 7, 20 + offsetSprite };
						else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4] - y == 0.5)
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 8, 20 + offsetSprite };
						else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4] - y == 0.75)
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 9, 20 + offsetSprite };
						else
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 10, 20 + offsetSprite };
					}
					else
					{
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 6, 20 + offsetSprite };
					}
				}
			}
		}
		


		// Draw the play and pause buttons.
		if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
		{
			if (sampleDisplay.playingSample)
			{
				gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 38].sprite = { 14, 7 };
				gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 39].sprite = { 14, 8 };
				gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 38].sprite = { 15, 7 };
				gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 39].sprite = { 15, 8 };

				gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 38].sprite = { 16, 7 };
				gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 39].sprite = { 16, 8 };
				gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 38].sprite = { 17, 7 };
				gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 39].sprite = { 17, 8 };
			}
			else
			{
				gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 38].sprite = { 12, 7 };
				gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 39].sprite = { 12, 8 };
				gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 38].sprite = { 13, 7 };
				gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 39].sprite = { 13, 8 };

				gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 38].sprite = { 18, 7 };
				gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 39].sprite = { 18, 8 };
				gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 38].sprite = { 19, 7 };
				gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 39].sprite = { 19, 8 };
			}
		}
		if (loadedSamples[editor.selectedSample].enabled && loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
		{
			if (sampleDisplay.drawing) // Draw the drawing button.
			{
				gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 38].sprite = { 19, 3 };
				gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 39].sprite = { 19, 4 };
				gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 38].sprite = { 20, 3 };
				gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 39].sprite = { 20, 4 };
			}
			else
			{
				gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 38].sprite = { 17, 3 };
				gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 39].sprite = { 17, 4 };
				gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 38].sprite = { 18, 3 };
				gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 39].sprite = { 18, 4 };
			}
			if (sampleDisplay.zoomed) // Draw the zoom button.
			{
				gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 38].sprite = { 30, 7 };
				gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 39].sprite = { 30, 8 };
				gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 38].sprite = { 31, 7 };
				gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 39].sprite = { 31, 8 };
			}
			else
			{
				gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 38].sprite = { 28, 7 };
				gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 39].sprite = { 28, 8 };
				gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 38].sprite = { 29, 7 };
				gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 39].sprite = { 29, 8 };
			}
		}


		
		
		if (loadedSamples[editor.selectedSample].enabled)
		{
			float frameCount = float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) * loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC;
			if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
			{
			// Sample length display
			gui.activeUI[int(wind->position.x + 6)][int(wind->position.y + 38)].sprite = { 5, 26 };
			if (sampleDisplay.measurementSystem == 0)
				DrawGUIText("FRAMES:" + std::to_string(frameCount), wind->position.x + 7, wind->position.x + 24, wind->position.y + 38, 4, -1);
			else if (sampleDisplay.measurementSystem == 1)
			{
				float milis = (frameCount / 48000.0f) * 1000.0f;
				DrawGUIText("MS:" + std::to_string(milis), wind->position.x + 7, wind->position.x + 24, wind->position.y + 38, 4, -1);
			}
			else
			{
				float seconds = (frameCount) / 48000.0f;
				DrawGUIText("SECONDS:" + std::to_string(seconds), wind->position.x + 7, wind->position.x + 24, wind->position.y + 38, 4, -1);
			}
				
			gui.activeUI[int(wind->position.x + 24)][int(wind->position.y + 38)].sprite = { 6, 26 };


			// Loop button
				DrawGUIText("LOOP", wind->position.x + 3, wind->position.x + 7, wind->position.y + 8, 3, 0);
				gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 9)].sprite = { 5, 26 };
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType == 0)
					DrawGUIText("NONE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 9, 4, -1);
				else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType == 1)
					DrawGUIText("FORWARDS", wind->position.x + 3, wind->position.x + 12, wind->position.y + 9, 4, -1);
				else
					DrawGUIText("BOUNCE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 9, 4, -1);
				gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };


				// Snap button
				DrawGUIText("Snap:" + std::to_string(sampleDisplay.snapSubdivisions), wind->position.x + 1, wind->position.x + 9, wind->position.y + 10, 3, 0);
				if (sampleDisplay.enableSnap)
					gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 10)].sprite = { 24, 6 };
				else
					gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 10)].sprite = { 23, 6 };
				DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 10), float(sampleDisplay.snapSubdivisions) / 64.0f);


				// ASDR
				DrawGUIText("Attack:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 12, 3, 0);
				DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 12), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].attack);
				DrawGUIText("Sustain:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 13, 3, 0);
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustainForever)
					gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 13)].sprite = { 5, 22 };
				else
				{
					gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 13)].sprite = { 4, 22 };
					DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 13), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustain);
				}
				DrawGUIText("Decay:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 14, 3, 0);
				DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 14), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].decay);
				DrawGUIText("Release:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 15, 3, 0);
				DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 15), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].release);
			}



			// Glide
			DrawGUIText("Glide:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 17, 3, 0);
			DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 17), loadedSamples[editor.selectedSample].glide);

			// Stereo
			DrawGUIText("Stereo:", wind->position.x + 1, wind->position.x + 8, wind->position.y + 18, 3, 0);
			gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 18)].sprite = { 5, 26 };
			if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].stereo == 0)
				DrawGUIText("MIX", wind->position.x + 9, wind->position.x + 14, wind->position.y + 18, 4, -1);
			else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].stereo == 1)
				DrawGUIText("LEFT", wind->position.x + 9, wind->position.x + 14, wind->position.y + 18, 4, -1);
			else
				DrawGUIText("RIGHT", wind->position.x + 9, wind->position.x + 14, wind->position.y + 18, 4, -1);
			gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 18)].sprite = { 6, 26 };

			if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
			{
				// Pitch to note
				DrawGUIText("Pitch to Note:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 19, 3, 0);
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pitchToNote)
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 19)].sprite = { 24, 6 };
				else
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 19)].sprite = { 23, 6 };
				// LFO
				DrawGUIText("USE AS LFO:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 20, 3, 0);
				gui.activeUI[int(wind->position.x + 13)][int(wind->position.y + 20)].sprite = { 5, 26 };
				DrawGUIText(std::to_string(int(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].lfo)), wind->position.x + 14, wind->position.x + 16, wind->position.y + 20, 4, -1);
				gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 20)].sprite = { 6, 26 };
				// Continue note
				DrawGUIText("Continue Note:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 21, 3, 0);
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].continueNote)
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 21)].sprite = { 24, 6 };
				else
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 21)].sprite = { 23, 6 };
			}


			// Operator modulation type.
			for (int mod = 0; mod < 4; mod++)
			{
				gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 22 + mod)].sprite = { 5, 28 };
				DrawGUIText("MOD" + std::to_string(mod + 1), wind->position.x + 13, wind->position.x + 17, wind->position.y + 22 + mod, 3, 0);
				if (loadedSamples[editor.selectedSample].modulationTypes[mod] == 0)
					DrawGUIText("FM", wind->position.x + 3, wind->position.x + 12, wind->position.y + 22 + mod, 4, -1);
				else if (loadedSamples[editor.selectedSample].modulationTypes[mod] == 1)
					DrawGUIText("AM", wind->position.x + 3, wind->position.x + 12, wind->position.y + 22 + mod, 4, -1);
				else if (loadedSamples[editor.selectedSample].modulationTypes[mod] == 2)
					DrawGUIText("DELAY", wind->position.x + 3, wind->position.x + 12, wind->position.y + 22 + mod, 4, -1);
				else
				{
					if (loadedSamples[editor.selectedSample].modulationTypes[mod] == 3)
						DrawGUIText("LP-CUTOFF", wind->position.x + 3, wind->position.x + 12, wind->position.y + 22 + mod, 4, -1);
					else if (loadedSamples[editor.selectedSample].modulationTypes[mod] == 4)
						DrawGUIText("LP-RESO.", wind->position.x + 3, wind->position.x + 12, wind->position.y + 22 + mod, 4, -1);
					else
						DrawGUIText("LP-BOTH", wind->position.x + 3, wind->position.x + 12, wind->position.y + 22 + mod, 4, -1);
					if (loadedSamples[editor.selectedSample].modulationTypes[mod] < 5)
						DrawHorizontalSlider(int(wind->position.x + 13), wind->position.y + 22 + mod, loadedSamples[editor.selectedSample].lPResonances[mod]);
				}
				gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 22 + mod)].sprite = { 6, 28 };
			}
			
			gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 22)].sprite = { 5, 27 };
			gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 22)].sprite = { 6, 27 };
			gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 25)].sprite = { 5, 29 };
			gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 25)].sprite = { 6, 29 };


			// Frequency/arp toggle button.
			gui.activeUI[int(wind->position.x + 18)][int(wind->position.y + 17)].sprite = { 5, 26 };
			if (sampleDisplay.displayArp)
				DrawGUIText("ARPEGGIO", wind->position.x + 19, wind->position.x + 32, wind->position.y + 17, 4, -1);
			else
				DrawGUIText("FREQUENCIES", wind->position.x + 19, wind->position.x + 32, wind->position.y + 17, 4, -1);
			gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 17)].sprite = { 6, 26 };
			


			DrawAlgorithm({ wind->position.x + 24, wind->position.y + 18 }, loadedSamples[editor.selectedSample].algorithmType, true);

			gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 18)].sprite = { 8, 8 };
			gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 19)].sprite = { 9, 8 };
			gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 20)].sprite = { 10, 8 };
			gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 21)].sprite = { 11, 8 };

			for (int y = 0; y < 4; y++)
			{
				if (sampleDisplay.selectedOperator == y)
					gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 18 + y)].textCol = 5;
				else
					gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 18 + y)].textCol = 0;
			}
		}

		gui.activeUI[int(wind->position.x + 22)][int(wind->position.y + 24)].sprite = { 5, 26 };
		DrawGUIText("ALGORITHMS", wind->position.x + 23, wind->position.x + 33, wind->position.y + 24, 4, -1);
		gui.activeUI[int(wind->position.x + 33)][int(wind->position.y + 24)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 22)][int(wind->position.y + 25)].sprite = { 5, 26 };
		DrawGUIText("PRESETS", wind->position.x + 23, wind->position.x + 33, wind->position.y + 25, 4, -1);
		gui.activeUI[int(wind->position.x + 33)][int(wind->position.y + 25)].sprite = { 6, 26 };


		//gui.scrollBars[5].topLeft.x = wind->position.x + 1;
		//gui.scrollBars[5].topLeft.y = wind->position.y + 27;

		

		//if (sampleDisplay.offset >= sampleDisplay.pcmData.size() * 0.5f - (960 / sampleDisplay.zoomFactor))
		//	sampleDisplay.offset = sampleDisplay.pcmData.size() * 0.5f - (960 / sampleDisplay.zoomFactor) - 1;
		//if (sampleDisplay.offset < 0)
		//	sampleDisplay.offset = 0;
		

		//DrawSampleDisplay();
	}
	else if (wind->name == "Sample Selection")
	{
		std::string settingsText[8] = {
			"CUT", "RESAMPLE", "VOLUME", "SET LOOP", "FADE OUT", "FADE IN", "COPY", "PASTE"
		};
		for (int i = 0; i < 8; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);
			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Resample Sample")
	{
		DrawGUIText("Fine-tune", wind->position.x + 2, wind->position.x + 12, wind->position.y + 2, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 3)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 3)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 3)].sprite = { 10, 3 };
		DrawGUIText(std::to_string(sampleDisplay.resampleFineTune), wind->position.x + 6, wind->position.x + 16, wind->position.y + 3, 3, 0);
		DrawGUIText("Semitones", wind->position.x + 2, wind->position.x + 12, wind->position.y + 4, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 5)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 5)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 5)].sprite = { 10, 3 };
		DrawGUIText("C-4 to ", wind->position.x + 6, wind->position.x + 13, wind->position.y + 5, 3, 0);
		DrawEDONote(sampleDisplay.resampleNote, wind->position.x + 13, wind->position.y + 5, 3, 0, -1, -1);
		
		float selectedFrameSize = abs(sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);
		DrawGUIText(std::to_string(int(selectedFrameSize)) + " frames", wind->position.x + 2, wind->position.x + 20, wind->position.y + 7, 3, 0);
		DrawGUIText("to ", wind->position.x + 2, wind->position.x + 20, wind->position.y + 8, 3, 0);

		
		selectedFrameSize *= sampleDisplay.resampleMultiplier;

		DrawNum(selectedFrameSize, wind->position.x + 2, wind->position.x + 20, wind->position.y + 9, 4, 0, -1, 28);

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 11, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 11)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 11)].sprite = { 6, 26 };
	}
	else if (wind->name == "Sample Volume")
	{
		DrawGUIText("Volume", wind->position.x + 2, wind->position.x + 12, wind->position.y + 2, 4, 1);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 3)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 3)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 3)].sprite = { 10, 3 };
		DrawGUIText(std::to_string(sampleDisplay.volumeMultiplier), wind->position.x + 6, wind->position.x + 16, wind->position.y + 3, 4, 1);

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 5, 4, -2);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 5)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 5)].sprite = { 6, 26 };
	}
	else if (wind->name == "Algorithms")
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (sampleDisplay.operatorMenuSelectedOperator == x + 4 * y)
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, true);
				else
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, false);
			}
		}

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 32, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 32)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 32)].sprite = { 6, 26 };
	}
	else if (wind->name == "Presets")
	{
		for (int i = 0; i < 6; i++)
		{
			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 * i + 1)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 2 * i + 1)].sprite = { 6, 26 };
		}
		DrawGUIText("LEADS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 1, 4, -1);
		DrawGUIText("PADS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 3, 4, -1);
		DrawGUIText("BASS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 5, 4, -1);
		DrawGUIText("KEYS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 7, 4, -1);
		DrawGUIText("PERCUSSION", wind->position.x + 2, wind->position.x + 12, wind->position.y + 9, 4, -1);
		DrawGUIText("SFX", wind->position.x + 2, wind->position.x + 12, wind->position.y + 11, 4, -1);

		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 * presetMenu.instrumentType + 1)].sprite = { 5, 4 };

		for (int i = 0; i < 16; i++)
		{
			gui.activeUI[int(wind->position.x + 13)][int(wind->position.y + i + 1)].sprite = { 2, 4 };
			gui.activeUI[int(wind->position.x + 30)][int(wind->position.y + i + 1)].sprite = { 2, 4 };

			if (i == presetMenu.selectedSample)
				DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i], wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 5, 2);
			else
				DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i], wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 3, 0);
			if (i + 16 == presetMenu.selectedSample)
				DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i + 16], wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 5, 2);
			else
				DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i + 16], wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 3, 0);
		}

		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 17)].sprite = { 5, 26 };
		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 17, 4, -1);
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 17)].sprite = { 6, 26 };
	}
	else if (wind->name == "Algorithms")
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (sampleDisplay.operatorMenuSelectedOperator == x + 4 * y)
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, true);
				else
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, false);
			}
		}

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 32, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 32)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 32)].sprite = { 6, 26 };
	}
	else if (wind->name == "File")
	{
		std::string settingsText[4] = {
			"NEW", "SAVE", "LOAD", "EXPORT"
		};
		for (int i = 0; i < 4; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);
			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Phonemes")
	{
		for (int y = 0; y < 22; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				gui.activeUI[int(wind->position.x + x)][int(wind->position.y + 2 + y)].bgCol = 0;
				gui.activeUI[int(wind->position.x + x)][int(wind->position.y + 2 + y)].textCol = 13;
			}

			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + y)].sprite = { 4 + y, 17 };

			if (voiceSynth.phonemes[y].loop)
				gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 2 + y)].sprite = { 5, 22 };
			else
				gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 2 + y)].sprite = { 4, 22 };

			gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 2 + y)].sprite = { 5, 26 };
			DrawGUIText("SET", wind->position.x + 4, wind->position.x + 7, wind->position.y + 2 + y, 4, -1);
			gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 2 + y)].sprite = { 6, 26 };

			gui.activeUI[int(wind->position.x + 9)][int(wind->position.y + 2 + y)].sprite = { 5, 26 };
			DrawGUIText("SET", wind->position.x + 10, wind->position.x + 13, wind->position.y + 2 + y, 4, -1);
			gui.activeUI[int(wind->position.x + 13)][int(wind->position.y + 2 + y)].sprite = { 6, 26 };

			if (voiceSynth.phonemes[y + 22].loop)
				gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 2 + y)].sprite = { 5, 22 };
			else
				gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 2 + y)].sprite = { 4, 22 };

			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + y)].sprite = { 4 + y, 18 };
		}

		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 25)].sprite = { 5, 26 };
		DrawGUIText("SAVE", wind->position.x + 2, wind->position.x + 6, wind->position.y + 25, 4, -1);
		gui.activeUI[int(wind->position.x + 6)][int(wind->position.y + 25)].sprite = { 6, 26 };
	}
	else if (wind->name == "Channel")
	{
		std::string settingsText[1] = {
			"VOICE",
		};
		for (int i = 0; i < 1; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);
			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}

		if (channels[editor.selectedChannel].hasVoiceColumns)
			gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 2)].sprite = { 24, 6 };
		else
			gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 2)].sprite = { 23, 6 };
	}



	return;
}



void DrawWaveTypeButton(int type, int xVal, int yVal)
{
	gui.activeUI[int(xVal - 1)][int(yVal)].sprite = { 5, 26 };
	switch (type)
	{
		case -1:
		{
			DrawGUIText("---------", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 1:
		{
			DrawGUIText("SQUARE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 2:
		{
			DrawGUIText("TRIANGLE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 3:
		{
			DrawGUIText("SAW", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 4:
		{
			DrawGUIText("NOISE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 5:
		{
			DrawGUIText("WAVE A", xVal, xVal + 9, yVal, 4, -1);
			break;
		}

		case 6:
		{
			DrawGUIText("WAVE B", xVal, xVal + 9, yVal, 4, -1);
			break;
		}

		default:
		{
			DrawGUIText("SINE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
	}
	gui.activeUI[int(xVal + 9)][int(yVal)].sprite = { 6, 26 };


	
	if (loadedSamples[editor.selectedSample].enabled)
	{

		if (type != 4) // Noise has neither property.
		{
			DrawGUIText("Duty:" + std::to_string(int(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle * 100)), xVal - 2, xVal + 8, yVal + 1, 3, 0);
			DrawHorizontalSlider(int(xVal + 6), int(yVal + 1), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle);
		}

		if (type > 3) // Bell
		{
			DrawGUIText("Waves:" + std::to_string(int(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves)), xVal - 2, xVal + 8, yVal + 2, 3, 0);
			DrawHorizontalSlider(int(xVal + 6), int(yVal + 2), float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves) / 16.0f);
		}
		else if (type > 0 && type != 2) // There is no smoothness effect for: sine, triangle, noise.
		{
			if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].generateFromSines)
			{
				DrawGUIText("Waves:" + std::to_string(int(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves)), xVal - 2, xVal + 8, yVal + 2, 3, 0);
				DrawGUIText("FROM SINE", xVal, xVal + 9, yVal + 3, 4, -1);

				DrawHorizontalSlider(int(xVal + 6), int(yVal + 2), float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves) / 16.0f);
			}
			else
			{
				DrawGUIText("Soft:" + std::to_string(int(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness * 100)), xVal - 2, xVal + 8, yVal + 2, 3, 0);
				DrawGUIText("EXACT", xVal, xVal + 9, yVal + 3, 4, -1);

				DrawHorizontalSlider(int(xVal + 6), int(yVal + 2), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness);
			}

			gui.activeUI[int(xVal - 1)][int(yVal + 3)].sprite = { 5, 26 };
			gui.activeUI[int(xVal + 9)][int(yVal + 3)].sprite = { 6, 26 };
		}
	}


	return;
}



void DrawHorizontalSlider(int xVal, int yVal, float value)
{
	value *= 8;

	for (int i = 0; i < 8; i++)
	{
		if (i < value)
		{
			if (value - float(i) <= 0.5f)
				gui.activeUI[xVal + i][yVal].sprite = { 12, 21 };
			else
				gui.activeUI[xVal + i][yVal].sprite = { 13, 21 };
		}
		else
		{
			gui.activeUI[xVal + i][yVal].sprite = { 11, 21 };
		}
	}

	//if (value > 7.0f)
	//	gui.activeUI[xVal + 7][yVal].sprite = { 15, 21 };
	//else
	//	gui.activeUI[xVal + 7][yVal].sprite = { 14, 21 };

	return;
}



void DrawDial(Vector2 pos, float val, int style)
{

	
	gui.activeUI[int(pos.x)][int(pos.y)].sprite = { 2 + style * 2, 7 };
	gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 3 + style * 2, 7 };

	gui.activeUI[int(pos.x)][int(pos.y + 1)].sprite = { 2 + style * 2, 8 };
	gui.activeUI[int(pos.x + 1)][int(pos.y + 1)].sprite = { 3 + style * 2, 8 };


	return;
}



void DrawEffect(Vector2 pos, int type, int value)
{
	switch (type)
	{
	case -1: // None
	{
		gui.activeUI[int(pos.x)][int(pos.y)].sprite = { 9, 23 };
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 10, 23 };
		gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 10, 23 };
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { 13, 23 };
		break;
	}

	case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
	{
		// 10s: 0=set, 1=increase, 2=decrease
		// 1s: value

		gui.activeUI[int(pos.x)][int(pos.y)].sprite = { 9, 24 + type };
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 10, 24 + type };
		int val1 = value % 10;
		int val10 = (value - val1) / 10;
		if (val10 == 0)
			gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 13, 24 }; // Set
		else if (val10 == 1)
			gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 13, 25 }; // Increase
		else
			gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 13, 26 }; // Decrease
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { val1, 0 };
		break;
	}

	case 8: case 9:
	{
		// 0=off, 1=on

		gui.activeUI[int(pos.x)][int(pos.y)].sprite = { 11, 15 + type };
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 12, 15 + type };
		if (value == 0)
			gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 13, 27 }; // off
		else
			gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 13, 28 }; // on
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { 13, 23 };
		break;
	}

	case 10: case 11: case 12: case 13:
	{
		gui.activeUI[int(pos.x)][int(pos.y)].sprite = { 11, 15 + type };
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 12, 15 + type };
		int val1 = value % 10;
		int val10 = (value - val1) / 10;
		gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { val10, 0 };
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { val1, 0 };
		break;
	}

	case 14:
	{
		gui.activeUI[int(pos.x)][int(pos.y)].sprite = { 11, 15 + type };
		int val1 = value % 10;
		int val10 = (value % 100) - val1;
		int val100 = value - val10 - val1;
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { val100 / 100, 0 };
		gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { val10 / 10, 0 };
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { val1 , 0 };
		break;
	}





	default:
		break;
	}


	return;
}



void DrawAlgorithm(Vector2i pos, int type, bool selected)
{
	int operatorWaves[4];
	for (int i = 0; i < 4; i++)
		operatorWaves[i] = loadedSamples[editor.selectedSample].operatorWavesToUse[i];

	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			gui.activeUI[pos.x + x][pos.y + y].sprite = { 4, 7 };
		}
	}

	if (selected)
	{
		for (int x = 0; x < 7; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (gui.lightMode)
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 4;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 5;
				}
				else
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 1;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 2;
				}
			}
		}
	}
	else
	{
		for (int x = 0; x < 7; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (gui.lightMode)
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 3;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 4;
				}
				else
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 0;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 1;
				}
			}
		}
	}

	switch (type)
	{
	case 0:
	{
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };
		break;
	}

	case 1:
	{
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 8, 7 };
		break;
	}

	case 2:
	{
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 2][pos.y + 2].textCol = 4;
		break;
	}

	case 3:
	{
		gui.activeUI[pos.x + 1][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 1][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 9, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 9, 7 };
		gui.activeUI[pos.x + 5][pos.y + 3].sprite = { operatorWaves[2], 7};
		gui.activeUI[pos.x + 5][pos.y + 4].sprite = { 8, 7 };
		break;
	}

	case 4:
	{
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 8, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y + 2].textCol = 4;
		break;
	}

	case 5:
	{
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { 8, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y + 2].textCol = 4;
		gui.activeUI[pos.x + 5][pos.y + 2].sprite = { 2, 0 }; // Connectors
		gui.activeUI[pos.x + 5][pos.y + 2].textCol = 4;
		break;
	}

	case 6:
	{
		gui.activeUI[pos.x + 3][pos.y].sprite = { 6, 8 };
		gui.activeUI[pos.x + 4][pos.y].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 2][pos.y + 2].textCol = 4;
		gui.activeUI[pos.x + 2][pos.y].sprite = { 2, 0 }; // Connectors
		gui.activeUI[pos.x + 2][pos.y].textCol = 4;
		break;
	}

	case 7:
	{
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 8 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 9, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { 7, 8 };
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 8, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y + 2].textCol = 4;
		gui.activeUI[pos.x + 5][pos.y + 2].sprite = { 2, 0 }; // Connectors
		gui.activeUI[pos.x + 5][pos.y + 2].textCol = 4;
		break;
	}

	case 8:
	{
		gui.activeUI[pos.x + 4][pos.y].sprite = { 6, 8 };
		gui.activeUI[pos.x + 5][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { 6, 8 };
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 8 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y + 2].textCol = 4;
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { 2, 0 };
		gui.activeUI[pos.x + 2][pos.y + 1].textCol = 4;
		gui.activeUI[pos.x + 3][pos.y].sprite = { 3, 0 };
		gui.activeUI[pos.x + 3][pos.y].textCol = 4;
		break;
	}

	case 9:
	{
		gui.activeUI[pos.x + 2][pos.y].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { 4, 8 };
		gui.activeUI[pos.x + 4][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { 5, 8 };
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y + 1].sprite = { 2, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y + 1].textCol = 4;
		gui.activeUI[pos.x + 5][pos.y + 1].sprite = { 3, 0 };
		gui.activeUI[pos.x + 5][pos.y + 1].textCol = 4;
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 1, 0 };
		gui.activeUI[pos.x + 2][pos.y + 2].textCol = 4;
		break;
	}

	case 10:
	{
		gui.activeUI[pos.x + 2][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { 5, 7 };
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { 4, 8 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { 5, 8 };

		gui.activeUI[pos.x + 1][pos.y + 1].sprite = { 3, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y + 1].textCol = 4;
		gui.activeUI[pos.x + 1][pos.y + 3].sprite = { 1, 0 };
		gui.activeUI[pos.x + 1][pos.y + 3].textCol = 4;
		gui.activeUI[pos.x + 5][pos.y + 3].sprite = { 2, 0 };
		gui.activeUI[pos.x + 5][pos.y + 3].textCol = 4;
		break;
	}

	case 11:
	{
		gui.activeUI[pos.x + 2][pos.y].sprite = { 6, 8 };
		gui.activeUI[pos.x + 3][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 4][pos.y].sprite = { 7, 8 };
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { 8, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y].sprite = { 3, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y].textCol = 4;
		gui.activeUI[pos.x + 5][pos.y].sprite = { 4, 0 };
		gui.activeUI[pos.x + 5][pos.y].textCol = 4;
		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 };
		gui.activeUI[pos.x + 1][pos.y + 2].textCol = 4;
		gui.activeUI[pos.x + 5][pos.y + 2].sprite = { 2, 0 };
		gui.activeUI[pos.x + 5][pos.y + 2].textCol = 4;
		break;
	}

	case 12:
	{
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 6, 8 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 7, 8 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].textCol = 4;
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { 3, 0 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].textCol = 4;
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 2, 0 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].textCol = 4;
		break;
	}

	case 13:
	{
		gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 6, 8 };
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].textCol = 4;
		gui.activeUI[int(pos.x + 1)][int(pos.y + 0)].sprite = { 2, 0 };
		gui.activeUI[int(pos.x + 1)][int(pos.y + 0)].textCol = 4;
		break;
	}

	case 14:
	{
		gui.activeUI[int(pos.x + 1)][int(pos.y + 1)].sprite = { operatorWaves[1], 8};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 8, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 5, 7 };

		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { 2, 0 }; // Connectors
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].textCol = 4;
		gui.activeUI[int(pos.x)][int(pos.y + 2)].sprite = { 1, 0 };
		gui.activeUI[int(pos.x)][int(pos.y + 2)].textCol = 4;
		gui.activeUI[int(pos.x + 6)][int(pos.y + 2)].sprite = { 3, 0 };
		gui.activeUI[int(pos.x + 6)][int(pos.y + 2)].textCol = 4;
		break;
	}

	case 15:
	{
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };


		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].textCol = 4;
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 2, 0 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].textCol = 4;
		break;
	}


	case 16:
	{
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 6, 8 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 7, 8 };
		gui.activeUI[int(pos.x + 1)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 3)].sprite = { operatorWaves[2], 7};
		gui.activeUI[int(pos.x + 5)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { 2, 0 }; // Connectors
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].textCol = 4;
		gui.activeUI[int(pos.x)][int(pos.y + 2)].sprite = { 1, 0 };
		gui.activeUI[int(pos.x)][int(pos.y + 2)].textCol = 4;
		gui.activeUI[int(pos.x + 6)][int(pos.y + 2)].sprite = { 3, 0 };
		gui.activeUI[int(pos.x + 6)][int(pos.y + 2)].textCol = 4;
		break;
	}

	case 17:
	{
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 1)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 3)].sprite = { operatorWaves[2], 7};
		gui.activeUI[int(pos.x + 5)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].textCol = 4;
		break;
	}

	case 18:
	{
		gui.activeUI[int(pos.x + 1)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 8, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x)][int(pos.y + 2)].textCol = 4;
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 2, 0 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].textCol = 4;
		break;
	}

	case 19:
	{
		gui.activeUI[int(pos.x + 0)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 0)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 1)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[2], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 6)][int(pos.y + 3)].sprite = { operatorWaves[3], 7};
		gui.activeUI[int(pos.x + 6)][int(pos.y + 4)].sprite = { 8, 7 };
		break;
	}


	default:
		break;
	}

	return;
}