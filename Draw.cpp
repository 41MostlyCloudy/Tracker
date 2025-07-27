#pragma once





void DrawEverything();

void  DrawBorder();

void DrawMute();

void DrawFrameBorder();

void DrawChannel(int channelNum);

void DrawText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawNote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);




void DrawEverything()
{
	drawScreen = true;

	DrawFrameBorder();
	DrawMute();
	DrawBorder();
	for (int i = 0; i < 8; i++)
	{
		DrawChannel(i);
	}

	return;
}



void  DrawBorder()
{
	// Song editor
	DrawText("Song", 1, 5, 0, 3, 0, -1, -1);
	DrawText("Num", 7, 10, 4, 3, 0, -1, -1);
	DrawText("Add", 7, 10, 7, 3, 0, -1, -1);

	// Frame list
	activeUI[0][1].sprite = { 0, 3 };
	activeUI[1][1].sprite = { 2, 3 };
	activeUI[2][1].sprite = { 2, 3 };
	activeUI[3][1].sprite = { 3, 3 };
	activeUI[4][1].sprite = { 2, 3 };
	activeUI[5][1].sprite = { 2, 3 };
	activeUI[6][1].sprite = { 1, 3 };

	activeUI[0][11].sprite = { 0, 4 };
	activeUI[1][11].sprite = { 2, 3 };
	activeUI[2][11].sprite = { 2, 3 };
	activeUI[3][11].sprite = { 4, 4 };
	activeUI[4][11].sprite = { 2, 3 };
	activeUI[5][11].sprite = { 2, 3 };
	activeUI[6][11].sprite = { 1, 4 };

	for (int y = 2; y < 11; y++)
	{
		activeUI[0][y].sprite = { 2, 4 };
		activeUI[3][y].sprite = { 2, 4 };
	}
	for (int y = 3; y < 10; y++)
		activeUI[6][y].sprite = { 2, 4 };

	for (int y = 0; y < 9; y++)
	{
		if (y + frameListScroll < loadedSong.frameSequence.size())
		{
			
			if (y + frameListScroll == loadedSong.currentFrame)
			{
				DrawHex(loadedSong.frameSequence[y + frameListScroll], 4, 2 + y, 5, 3, -1, -1);
				DrawHex(y + int(frameListScroll), 1, 2 + y, 4, 3, -1, -1);
				activeUI[3][y + 2].sprite = { 5, 4 };
			}
			else
			{
				DrawHex(loadedSong.frameSequence[y + frameListScroll], 4, 2 + y, 5, 0, -1, -1);
				DrawHex(y + int(frameListScroll), 1, 2 + y, 3, 0, -1, -1);
			}
		}
		else
		{
			activeUI[4][y + 2].sprite = { 15, 0 };
			activeUI[4][y + 2].bgCol = 0;
			activeUI[5][y + 2].sprite = { 15, 0 };
			activeUI[5][y + 2].bgCol = 0;
			DrawHex(y + int(frameListScroll), 1, 2 + y, 3, 0, -1, -1);
		}
	}

	

	// Edit button box
	for (int x = 7; x < 10; x++)
	{
		activeUI[x][3].sprite = { 2, 3 };
		activeUI[x][9].sprite = { 2, 3 };
	}
	for (int y = 4; y < 9; y++)
	{
		activeUI[10][y].sprite = { 2, 4 };
	}
	activeUI[10][3].sprite = { 1, 3 };
	activeUI[10][9].sprite = { 1, 4 };


	// Song/Frame settings box
	DrawText("Settings", 13, 21, 0, 3, 0, -1, -1);
	activeUI[12][1].sprite = { 0, 3 };
	activeUI[20][1].sprite = { 1, 3 };
	activeUI[12][11].sprite = { 0, 4 };
	activeUI[20][11].sprite = { 1, 4 };
	for (int x = 13; x < 20; x++)
	{
		activeUI[x][1].sprite = { 2, 3 };
		activeUI[x][11].sprite = { 2, 3 };
	}
	for (int y = 2; y < 11; y++)
	{
		activeUI[12][y].sprite = { 2, 4 };
		activeUI[20][y].sprite = { 2, 4 };
	}

	// Buttons
	DrawText("BPM", 13, 16, 2, 3, 0, -1, -1);
	DrawText(std::to_string(loadedSong.bpm), 17, 20, 2, 4, 0, -1, -1);
	DrawText("TPB", 13, 16, 4, 3, 0, -1, -1);
	DrawText(std::to_string(loadedSong.ticksPerBeat), 17, 19, 4, 4, 0, -1, -1);
	DrawText("OCT", 13, 16, 6, 3, 0, -1, -1);
	DrawText(std::to_string(selectedOctave), 17, 20, 6, 4, 0, -1, -1);	
	DrawText("ROW", 13, 16, 8, 3, 0, -1, -1);
	DrawHex(loadedFrame.rows.size(), 17, 8, 4, 0, -1, -1);



	// Song name box
	DrawText("Song Name", 45, 21 + 44, 0, 3, 0, -1, -1);
	DrawText("SAVE", 61, 65, 0, 4, 2, -1, -1);
	if (filesSampleNotSong)
		DrawText("FILES(Samples)", 67, 80, 0, 4, 2, -1, -1);
	else
		DrawText("FILES(Songs)", 67, 80, 0, 4, 2, -1, -1);
	DrawText("SAMPLES", 79 + 4, 86 + 4, 0, 4, 2, -1, -1);
	//DrawText("CLOSE", 87, 92, 0, 4, 2, -1, -1);
	activeUI[44][1].sprite = { 0, 3 };
	activeUI[44][2].sprite = { 2, 4 };
	activeUI[44][3].sprite = { 0, 4 };
	activeUI[21 + 44][1].sprite = { 1, 3 };
	activeUI[21 + 44][2].sprite = { 2, 4 };
	activeUI[21 + 44][3].sprite = { 1, 4 };
	for (int x = 0; x < 20; x++)
	{
		activeUI[45 + x][1].sprite = { 2, 3 };
		activeUI[45 + x][3].sprite = { 2, 3 };
	}
	DrawText(loadedSong.songName, 45, 21 + 44, 2, 5, 0, -1, 0);

	// File select box
	for (int y = 3; y < 11; y++)
	{
		activeUI[66][y].sprite = { 2, 4 };
		activeUI[77][y].sprite = { 2, 4 };
		activeUI[78][y].sprite = { 2, 4 };
		activeUI[80][y].sprite = { 2, 4 };
		activeUI[91][y].sprite = { 2, 4 };
	}
	for (int x = 66; x < 91; x++)
	{
		activeUI[x][1].sprite = { 2, 3 };
	}
	activeUI[66][2].sprite = { 2, 4 };
	activeUI[66][11].sprite = { 2, 4 };
	activeUI[78][2].sprite = { 2, 4 };
	activeUI[78][11].sprite = { 2, 4 };
	activeUI[80][2].sprite = { 2, 4 };
	activeUI[80][11].sprite = { 2, 4 };
	for (int y = 2; y < 12; y++)
	{
		if (y - 2 + fileListScroll < fileNameList.size()) // Draw files
		{
			if (y - 2 + fileListScroll == selectedFile)
			{
				DrawText(fileNameList[y - 2 + fileListScroll], 67, 77, y, 5, 3, -1, -1);
				activeUI[66][y].sprite = { 5, 4 };
			}
			else
			{
				DrawText(fileNameList[y - 2 + fileListScroll], 67, 77, y, 4, 0, -1, -1);
			}
		}
		else
		{
			DrawText("", 67, 77, y, 4, 0, -1, -1);
		}

		if (y - 2 + sampleListScroll < loadedSamples.size()) // Draw samples
		{
			if (y - 2 + sampleListScroll == selectedSample)
			{
				DrawText(loadedSamples[y - 2 + sampleListScroll].sampleName, 81, 91, y, 5, 3, -1, -1);
				activeUI[80][y].sprite = { 5, 4 };
			}
			else
			{
				DrawText(loadedSamples[y - 2 + sampleListScroll].sampleName, 81, 91, y, 4, 0, -1, -1);
			}
		}
		else
		{
			DrawText("", 81, 91, y, 4, 0, -1, -1);
		}
	}
	
	
	activeUI[66][12].sprite = { 4, 4 };
	activeUI[77][12].sprite = { 4, 4 };
	activeUI[78][12].sprite = { 4, 4 };
	activeUI[80][12].sprite = { 5, 3 };
	activeUI[91][12].sprite = { 4, 3 };

	activeUI[66][1].sprite = { 0, 3 };
	activeUI[77][1].sprite = { 1, 3 };
	activeUI[78][1].sprite = { 0, 3 };
	activeUI[80][1].sprite = { 3, 3 };
	activeUI[91][1].sprite = { 1, 3 };

	// Scroll arrows
	activeUI[91][2].sprite = { 6, 3 };
	activeUI[91][11].sprite = { 6, 4 };


	// Logo
	int logoX = 22;
	int logoY = 1;
	activeUI[logoX][logoY].sprite = { 17, 3 };		activeUI[logoX][logoY + 1].sprite = { 22, 3 };		activeUI[logoX][logoY + 2].sprite = { 17, 4 };
	activeUI[logoX + 1][logoY].sprite = { 19, 3 };	activeUI[logoX + 1][logoY + 1].sprite = { 23, 3 };	activeUI[logoX + 1][logoY + 2].sprite = { 19, 4 };
	activeUI[logoX + 2][logoY].sprite = { 18, 3 };	activeUI[logoX + 2][logoY + 1].sprite = { 24, 3 };	activeUI[logoX + 2][logoY + 2].sprite = { 18, 4 };

	activeUI[logoX + 3][logoY].sprite = { 20, 3 };	activeUI[logoX + 3][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 3][logoY + 2].sprite = { 17, 4 };
																										activeUI[logoX + 4][logoY + 2].sprite = { 19, 4 };
	activeUI[logoX + 5][logoY].sprite = { 21, 3 };	activeUI[logoX + 5][logoY + 1].sprite = { 21, 4 };	activeUI[logoX + 5][logoY + 2].sprite = { 18, 4 };

	activeUI[logoX + 6][logoY].sprite = { 17, 3 };	activeUI[logoX + 6][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 6][logoY + 2].sprite = { 20, 3 };
	activeUI[logoX + 7][logoY].sprite = { 19, 3 };	activeUI[logoX + 7][logoY + 1].sprite = { 19, 4 };
	activeUI[logoX + 8][logoY].sprite = { 18, 3 };	activeUI[logoX + 8][logoY + 1].sprite = { 18, 4 };

	activeUI[logoX + 9][logoY].sprite = { 17, 3 };	activeUI[logoX + 9][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 9][logoY + 2].sprite = { 17, 4 };
	activeUI[logoX + 10][logoY].sprite = { 19, 3 };	activeUI[logoX + 10][logoY + 1].sprite = { 19, 4 };	activeUI[logoX + 10][logoY + 2].sprite = { 19, 4 };
	activeUI[logoX + 11][logoY].sprite = { 18, 3 };	activeUI[logoX + 11][logoY + 1].sprite = { 18, 4 };	activeUI[logoX + 11][logoY + 2].sprite = { 18, 4 };

	activeUI[logoX + 12][logoY].sprite = { 17, 3 };	activeUI[logoX + 12][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 12][logoY + 2].sprite = { 20, 3 };
	activeUI[logoX + 13][logoY].sprite = { 19, 3 };	activeUI[logoX + 13][logoY + 1].sprite = { 19, 4 };
	activeUI[logoX + 14][logoY].sprite = { 18, 3 };	activeUI[logoX + 14][logoY + 1].sprite = { 18, 4 };	activeUI[logoX + 14][logoY + 2].sprite = { 18, 4 };

	logoY += 3;

	activeUI[logoX][logoY].sprite = { 17, 3 };		activeUI[logoX][logoY + 1].sprite = { 22, 3 };		activeUI[logoX][logoY + 2].sprite = { 17, 4 };
	activeUI[logoX + 1][logoY].sprite = { 19, 3 };	activeUI[logoX + 1][logoY + 1].sprite = { 23, 3 };	activeUI[logoX + 1][logoY + 2].sprite = { 19, 4 };
	activeUI[logoX + 2][logoY].sprite = { 18, 3 };	activeUI[logoX + 2][logoY + 1].sprite = { 24, 3 };	activeUI[logoX + 2][logoY + 2].sprite = { 18, 4 };

	activeUI[logoX + 3][logoY].sprite = { 17, 3 };	activeUI[logoX + 3][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 3][logoY + 2].sprite = { 17, 4 };
	activeUI[logoX + 4][logoY].sprite = { 19, 3 };														activeUI[logoX + 4][logoY + 2].sprite = { 19, 4 };
	activeUI[logoX + 5][logoY].sprite = { 18, 3 };	activeUI[logoX + 5][logoY + 1].sprite = { 21, 4 };	activeUI[logoX + 5][logoY + 2].sprite = { 18, 4 };

	activeUI[logoX + 6][logoY].sprite = { 20, 3 };	activeUI[logoX + 6][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 6][logoY + 2].sprite = { 17, 4 };
																										activeUI[logoX + 7][logoY + 2].sprite = { 19, 4 };
	activeUI[logoX + 8][logoY].sprite = { 21, 3 };	activeUI[logoX + 8][logoY + 1].sprite = { 21, 4 };	activeUI[logoX + 8][logoY + 2].sprite = { 18, 4 };

	activeUI[logoX + 9][logoY].sprite = { 20, 3 };	activeUI[logoX + 9][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 9][logoY + 2].sprite = { 17, 4 };
													activeUI[logoX + 10][logoY + 1].sprite = { 23, 3 };
	activeUI[logoX + 11][logoY].sprite = { 18, 3 };	activeUI[logoX + 11][logoY + 1].sprite = { 21, 4 };	activeUI[logoX + 11][logoY + 2].sprite = { 21, 3 };

	activeUI[logoX + 12][logoY].sprite = { 20, 3 };	activeUI[logoX + 12][logoY + 1].sprite = { 20, 4 };	activeUI[logoX + 12][logoY + 2].sprite = { 20, 3 };
	activeUI[logoX + 13][logoY].sprite = { 19, 3 };														activeUI[logoX + 13][logoY + 2].sprite = { 19, 4 };
	activeUI[logoX + 14][logoY].sprite = { 18, 3 };	activeUI[logoX + 14][logoY + 1].sprite = { 21, 4 };	activeUI[logoX + 14][logoY + 2].sprite = { 18, 4 };

	logoY += 3;

	activeUI[logoX][logoY].sprite = { 22, 4 };		activeUI[logoX + 1][logoY].sprite = { 23, 4 };			activeUI[logoX + 2][logoY].sprite = { 24, 4 };
	activeUI[logoX + 3][logoY].sprite = { 22, 4 };		activeUI[logoX + 4][logoY].sprite = { 23, 4 };			activeUI[logoX + 5][logoY].sprite = { 24, 4 };
	activeUI[logoX + 6][logoY].sprite = { 22, 4 };		activeUI[logoX + 7][logoY].sprite = { 23, 4 };			activeUI[logoX + 8][logoY].sprite = { 24, 4 };
	activeUI[logoX + 9][logoY].sprite = { 22, 4 };																activeUI[logoX + 11][logoY].sprite = { 25, 4 };
	activeUI[logoX + 12][logoY].sprite = { 25, 3 };		activeUI[logoX + 13][logoY].sprite = { 23, 4 };			activeUI[logoX + 14][logoY].sprite = { 24, 4 };

	/* // Eye
	logoY -= 2;
	activeUI[logoX + 15][logoY].sprite = { 26, 1 }; activeUI[logoX + 16][logoY].sprite = { 27, 1 };
	activeUI[logoX + 15][logoY + 1].sprite = { 26, 2 }; activeUI[logoX + 16][logoY + 1].sprite = { 27, 2 };
	activeUI[logoX + 15][logoY + 2].sprite = { 28, 2 }; activeUI[logoX + 16][logoY + 2].sprite = { 29, 2 };
	*/

	return;
}


void DrawMute()
{
	for (int i = 0; i < 8; i++)
	{
		// Border
		for (int x = 0; x < 10; x++)
		{
			activeUI[4 + x + 11 * i][12].sprite = { 2, 3 };
			activeUI[4 + x + 11 * i][15].sprite = { 2, 3 };
		}
		activeUI[4 + 10 + 11 * i][12].sprite = { 3, 3 };
		activeUI[4 + 10 + 11 * i][15].sprite = { 5, 3 };

		float muteOffset = 0;

		if (channels[i].muted)
			muteOffset = 3;

		// Glass
		activeUI[4 + 11 * i][13].sprite = { 11 + muteOffset, 3 }; activeUI[4 + 11 * i][14].sprite = { 11 + muteOffset, 4 };
		activeUI[5 + 11 * i][13].sprite = { 12 + muteOffset, 3 }; activeUI[5 + 11 * i][14].sprite = { 12 + muteOffset, 4 };
		activeUI[6 + 11 * i][13].sprite = { 12 + muteOffset, 3 }; activeUI[6 + 11 * i][14].sprite = { 12 + muteOffset, 4 };
		activeUI[7 + 11 * i][13].sprite = { 12 + muteOffset, 3 }; activeUI[7 + 11 * i][14].sprite = { 12 + muteOffset, 4 };

		activeUI[10 + 11 * i][13].sprite = { 12 + muteOffset, 3 }; activeUI[10 + 11 * i][14].sprite = { 12 + muteOffset, 4 };
		activeUI[11 + 11 * i][13].sprite = { 12 + muteOffset, 3 }; activeUI[11 + 11 * i][14].sprite = { 12 + muteOffset, 4 };
		activeUI[12 + 11 * i][13].sprite = { 12 + muteOffset, 3 }; activeUI[12 + 11 * i][14].sprite = { 12 + muteOffset, 4 };
		activeUI[13 + 11 * i][13].sprite = { 13 + muteOffset, 3 }; activeUI[13 + 11 * i][14].sprite = { 13 + muteOffset, 4 };
		// Number
		activeUI[8 + 11 * i][13].sprite = { 2.0f * i + muteOffset * 16/3, 5 };
		activeUI[9 + 11 * i][13].sprite = { 2.0f * i + 1 + muteOffset * 16 / 3, 5 };
		activeUI[8 + 11 * i][14].sprite = { 2.0f * i + muteOffset * 16 / 3, 6 };
		activeUI[9 + 11 * i][14].sprite = { 2.0f * i + 1 + muteOffset * 16 / 3, 6 };

		activeUI[14 + 11 * i][13].sprite = { 2, 4 }; activeUI[14 + 11 * i][14].sprite = { 2, 4 };


		// Box color
		int color = 1;
		//color = channels[i].soundVolume * 1.0f;
		

		activeUI[4 + 11 * i][13].bgCol = color; activeUI[4 + 11 * i][14].bgCol = color;
		activeUI[5 + 11 * i][13].bgCol = color; activeUI[5 + 11 * i][14].bgCol = color;
		activeUI[6 + 11 * i][13].bgCol = color; activeUI[6 + 11 * i][14].bgCol = color;
		activeUI[7 + 11 * i][13].bgCol = color; activeUI[7 + 11 * i][14].bgCol = color;
		activeUI[8 + 11 * i][13].bgCol = color; activeUI[8 + 11 * i][14].bgCol = color;
		activeUI[9 + 11 * i][13].bgCol = color; activeUI[9 + 11 * i][14].bgCol = color;
		activeUI[10 + 11 * i][13].bgCol = color; activeUI[10 + 11 * i][14].bgCol = color;
		activeUI[11 + 11 * i][13].bgCol = color; activeUI[11 + 11 * i][14].bgCol = color;
		activeUI[12 + 11 * i][13].bgCol = color; activeUI[12 + 11 * i][14].bgCol = color;
		activeUI[13 + 11 * i][13].bgCol = color; activeUI[13 + 11 * i][14].bgCol = color;

		activeUI[4 + 11 * i][13].textCol = color + 2; activeUI[4 + 11 * i][14].textCol = color + 2;
		activeUI[5 + 11 * i][13].textCol = color + 2; activeUI[5 + 11 * i][14].textCol = color + 2;
		activeUI[6 + 11 * i][13].textCol = color + 2; activeUI[6 + 11 * i][14].textCol = color + 2;
		activeUI[7 + 11 * i][13].textCol = color + 2; activeUI[7 + 11 * i][14].textCol = color + 2;
		activeUI[8 + 11 * i][13].textCol = color + 2; activeUI[8 + 11 * i][14].textCol = color + 2;
		activeUI[9 + 11 * i][13].textCol = color + 2; activeUI[9 + 11 * i][14].textCol = color + 2;
		activeUI[10 + 11 * i][13].textCol = color + 2; activeUI[10 + 11 * i][14].textCol = color + 2;
		activeUI[11 + 11 * i][13].textCol = color + 2; activeUI[11 + 11 * i][14].textCol = color + 2;
		activeUI[12 + 11 * i][13].textCol = color + 2; activeUI[12 + 11 * i][14].textCol = color + 2;
		activeUI[13 + 11 * i][13].textCol = color + 2; activeUI[13 + 11 * i][14].textCol = color + 2;

	}
	activeUI[3][12].sprite = { 0, 3 };
	activeUI[3][13].sprite = { 2, 4 };
	activeUI[3][14].sprite = { 2, 4 };
	activeUI[3][15].sprite = { 5, 3 };

	activeUI[91][12].sprite = { 4, 3 };
	activeUI[91][13].sprite = { 2, 4 };
	activeUI[91][14].sprite = { 2, 4 };
	activeUI[91][15].sprite = { 4, 3 };

	activeUI[0][15].sprite = { 0, 3 };
	activeUI[1][15].sprite = { 2, 3 };
	activeUI[2][15].sprite = { 2, 3 };

	return;
}

void DrawFrameBorder()
{
	for (int x = 0; x < 92; x++)
		activeUI[x][56].sprite = { 2, 3 };


	for (int i = 0; i < 8; i++)
	{
		// Border
		for (int y = 16; y < 56; y++)
		{
			activeUI[3 + 11 * i][y].sprite = { 2, 4 };
		}
		activeUI[3 + 11 * i][56].sprite = { 4, 4 };
	}

	for (int y = 17; y < 55; y++)
	{
		activeUI[0][y].sprite = { 2, 4 };
		activeUI[91][y].sprite = { 2, 4 };
	}
	activeUI[0][16].sprite = { 2, 4 };
	activeUI[0][55].sprite = { 2, 4 };

	for (int y = 16; y < 56; y++)
	{
		// Beat numbering.
		if ((y + int(frameScroll)) % 4 == 0)
			DrawHex(y - 16 + frameScroll, 1, y, 4, 0, -1, -1);
		else
			DrawHex(y - 16 + frameScroll, 1, y, 2, 0, -1, -1);

		if (y - 16 + frameScroll == loadedSong.currentNote)
		{
			activeUI[1][y].bgCol = 2;
			activeUI[2][y].bgCol = 2;
			activeUI[1][y].textCol++;
			activeUI[2][y].textCol++;
			activeUI[0][y].sprite = { 5, 4 };
			activeUI[3][y].sprite = { 5, 4 };
		}
	}

	// File/sample select box corners.
	activeUI[66][12].sprite = { 4, 4 };


	// Scroll arrows
	//activeUI[91][16].sprite = { 6, 3 };
	//activeUI[91][55].sprite = { 6, 4 };

	activeUI[0][56].sprite = { 0, 4 };
	activeUI[91][56].sprite = { 1, 4 };

	return;
}



void DrawChannel(int channelNum)
{
	for (int y = 0; y < 40; y++)
	{
		int bgColor = 0;
		int textColor = 5;

		//if ((y + int(frameScroll)) % 2 == 1)
			//bgColor++;

		if (channels[channelNum].muted || y >= loadedFrame.rows.size())
		{
			bgColor--;
			if (bgColor < 0)
				bgColor = 0;
			textColor -= 2;
		}

		if (y + frameScroll == loadedSong.currentNote) // Note position marker.
		{
			bgColor = 2;
		}


		
		activeUI[4 + 11 * channelNum][y + 16].sprite = { 16, 0 };
		activeUI[4 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[4 + 11 * channelNum][y + 16].textCol = bgColor + 1;
		activeUI[5 + 11 * channelNum][y + 16].sprite = { 16, 0 };
		activeUI[5 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[5 + 11 * channelNum][y + 16].textCol = bgColor + 1;
		activeUI[6 + 11 * channelNum][y + 16].sprite = { 12, 0 };
		activeUI[6 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[6 + 11 * channelNum][y + 16].textCol = bgColor + 1;

		activeUI[7 + 11 * channelNum][y + 16].sprite = { 16, 0 };
		activeUI[7 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[7 + 11 * channelNum][y + 16].textCol = bgColor + 1;
		activeUI[8 + 11 * channelNum][y + 16].sprite = { 12, 0 };
		activeUI[8 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[8 + 11 * channelNum][y + 16].textCol = bgColor + 1;

		activeUI[9 + 11 * channelNum][y + 16].sprite = { 16, 0 };
		activeUI[9 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[9 + 11 * channelNum][y + 16].textCol = bgColor + 1;
		activeUI[10 + 11 * channelNum][y + 16].sprite = { 12, 0 };
		activeUI[10 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[10 + 11 * channelNum][y + 16].textCol = bgColor + 1;

		activeUI[11 + 11 * channelNum][y + 16].sprite = { 12, 0 };
		activeUI[11 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[11 + 11 * channelNum][y + 16].textCol = bgColor + 1;

		activeUI[12 + 11 * channelNum][y + 16].sprite = { 16, 0 };
		activeUI[12 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[12 + 11 * channelNum][y + 16].textCol = bgColor + 1;
		activeUI[13 + 11 * channelNum][y + 16].sprite = { 16, 0 };
		activeUI[13 + 11 * channelNum][y + 16].bgCol = bgColor;
		activeUI[13 + 11 * channelNum][y + 16].textCol = bgColor + 1;

		if (channels[channelNum].muted || y >= loadedFrame.rows.size())
		{
			activeUI[4 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[5 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[6 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[7 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[8 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[9 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[10 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[11 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[12 + 11 * channelNum][y + 16].textCol = 0;
			activeUI[13 + 11 * channelNum][y + 16].textCol = 0;
		}

		if (y + frameScroll < loadedFrame.rows.size())
		{
			if (loadedFrame.rows[y + frameScroll].note[channelNum] > -1) // Note
				DrawNote(uint8_t(loadedFrame.rows[y + frameScroll].note[channelNum]), 4 + 11 * channelNum, y + 16, textColor, bgColor, -1, -1);

			if (loadedFrame.rows[y + frameScroll].instrument[channelNum] > -1) // Instrument
				DrawHex(uint8_t(loadedFrame.rows[y + frameScroll].instrument[channelNum]), 7 + 11 * channelNum, y + 16, textColor + 3, bgColor, -1, -1);

			// Draw stop note
			if (loadedFrame.rows[y + frameScroll].note[channelNum] == 255) // Stop Note
			{
				activeUI[4 + 11 * channelNum][y + 16].sprite = { 18, 0 };
				activeUI[5 + 11 * channelNum][y + 16].sprite = { 19, 0 };
				activeUI[6 + 11 * channelNum][y + 16].sprite = { 19, 0 };
				activeUI[7 + 11 * channelNum][y + 16].sprite = { 19, 0 };
				activeUI[7 + 11 * channelNum][y + 16].textCol = 5;
				activeUI[8 + 11 * channelNum][y + 16].sprite = { 20, 0 };
				activeUI[8 + 11 * channelNum][y + 16].textCol = 5;
			}

			if (loadedFrame.rows[y + frameScroll].volume[channelNum] > -1) // Volume
				DrawHex(uint8_t(loadedFrame.rows[y + frameScroll].volume[channelNum]), 9 + 11 * channelNum, y + 16, textColor + 6, bgColor, -1, -1);

			if (loadedFrame.rows[y + frameScroll].effect[channelNum] > -1) // Effect
				DrawText(std::to_string(loadedFrame.rows[y + frameScroll].effect[channelNum]), 11 + 11 * channelNum, 12 + 11 * channelNum, y + 16, textColor, bgColor, -1, -1);

			if (loadedFrame.rows[y + frameScroll].effectValue[channelNum] > -1) // Volume
				DrawHex(uint8_t(loadedFrame.rows[y + frameScroll].effectValue[channelNum]), 12 + 11 * channelNum, y + 16, textColor + 3, bgColor, -1, -1);
		}


		if (y + frameScroll >= noteSelectionStart.y && y + frameScroll <= noteSelectionEnd.y) // Draw selected notes.
		{
			for (int x = 11 * channelNum; x < 10 + 11 * channelNum; x++)
			{
				if (x >= noteSelectionStart.x && x <= noteSelectionEnd.x)
				{
					activeUI[4 + x][y + 16].bgCol = 4;
					if (activeUI[4 + x][y + 16].sprite.x == 12 || activeUI[4 + x][y + 16].sprite.x == 16)
						activeUI[4 + x][y + 16].textCol = 5;

				}
			}
		}
	}

	return;
}



void DrawText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && selectedButton == buttonIndex)
	{
		selectedChar = selectedTile.x - textStart;
	}

	for (int i = 0; i < textEnd - textStart; i++)
	{
		if (i > textEnd)
			return;

		Vector2 textSprite = { 17, 0 };

		if (i < text.length())
		{
			if (text.at(i) > 39 && text.at(i) < 42) // (-)
				textSprite = { float(text.at(i) - 40) + 13, 0.0f };
			else if (text.at(i) > 47 && text.at(i) < 58) // 0-9
				textSprite = { float(text.at(i) - 48), 0.0f };
			else if (text.at(i) > 64 && text.at(i) < 91) // A-Z
				textSprite = { float(text.at(i) - 65), 1.0f };
			else if (text.at(i) > 96 && text.at(i) < 123) // a-z
				textSprite = { float(text.at(i) - 97), 2.0f };
		}

		activeUI[textStart + i][textY].sprite = textSprite;
		activeUI[textStart + i][textY].textCol = textCol;
		activeUI[textStart + i][textY].bgCol = bgCol;

		if (i == selectedChar)
			activeUI[textStart + i][textY].bgCol = 3;
	}

	if (buttonIndex > -1 && selectedButton == buttonIndex)
	{
		if (loadedSong.songName.length() == 0)
			activeUI[textStart][textY].bgCol = 3;
	}

	return;
}


void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && selectedButton == buttonIndex)
	{
		selectedChar = selectedTile.x - textStart;
	}

	Vector2 textSprite = { 15, 0 };

	int value1 = num / 16;
	int value2 = num % 16;

	//std::cout << "  " << value1 << "  " << value2;

	if (value1 < 10)
		textSprite = { float(value1), 0.0f };
	else
		textSprite = { float(value1 - 10), 1.0f };

	activeUI[textStart][textY].sprite = textSprite;
	activeUI[textStart][textY].textCol = textCol;
	activeUI[textStart][textY].bgCol = bgCol;

	if (value2 < 10)
		textSprite = { float(value2), 0.0f };
	else
		textSprite = { float(value2 - 10), 1.0f };

	activeUI[textStart + 1][textY].sprite = textSprite;
	activeUI[textStart + 1][textY].textCol = textCol;
	activeUI[textStart + 1][textY].bgCol = bgCol;


	if (selectedChar == 0)
		activeUI[textStart][textY].bgCol = 3;
	else if (selectedChar == 1)
		activeUI[textStart + 1][textY].bgCol = 3;

	return;
}


void DrawNote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	// A, A#, B, C, C#, D, D#, E, F, F#, G, G#
	int letter = num % 12;
	int octave = int(float(num) / 12.0f);

	switch (letter)
	{
		case 0: // C
		{
			activeUI[textStart][textY].sprite = { 2, 1 };
			activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}
		case 1: // C#
		{
			activeUI[textStart][textY].sprite = { 2, 1 };
			activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 2: // D
		{
			activeUI[textStart][textY].sprite = { 3, 1 };
			activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 3: // D#
		{
			activeUI[textStart][textY].sprite = { 3, 1 };
			activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 4: // E
		{
			activeUI[textStart][textY].sprite = { 4, 1 };
			activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 5: // F
		{
			activeUI[textStart][textY].sprite = { 5, 1 };
			activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 6: // F#
		{
			activeUI[textStart][textY].sprite = { 5, 1 };
			activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 7: // G
		{
			activeUI[textStart][textY].sprite = { 6, 1 };
			activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 8: // G#
		{
			activeUI[textStart][textY].sprite = { 6, 1 };
			activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 9: // A
		{
			activeUI[textStart][textY].sprite = { 0, 1 };
			activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 10: // A#
		{
			activeUI[textStart][textY].sprite = { 0, 1 };
			activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		default: // B
		{
			activeUI[textStart][textY].sprite = { 1, 1 };
			activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}
	}

	activeUI[textStart][textY].textCol = textCol;
	activeUI[textStart][textY].bgCol = bgCol;
	activeUI[textStart + 1][textY].textCol = textCol;
	activeUI[textStart + 1][textY].bgCol = bgCol;
	DrawText(std::to_string(octave), textStart + 2, textStart + 3, textY, textCol, bgCol, selectedLetter, buttonIndex);

	return;
}