#pragma once


#include "GlobalVariables.h"


void stepSong();




void stepSong()
{
	// 1B = 1/BPM;

	// 1B = 60/BPS;

	// 1B = 60,000/BPMS;

	// 1B = 1ms
	// 1000D / 1s


	loadedSong.timeInNote += delta;

	

	if (loadedSong.timeInNote > loadedSong.bpm)
	{
		loadedSong.currentNote++;

		if (loadedSong.currentNote > 8)
			frameScroll++;

		loadedSong.timeInNote = 0;

		if (loadedSong.currentNote >= loadedSong.frames[loadedSong.currentFrame].rows)
		{
			loadedSong.currentFrame++;
			loadedSong.currentNote = 0;

			if (loadedSong.currentFrame >= loadedSong.frameSequence.size())
				loadedSong.currentFrame = 0;

			loadCurrentFrame();			
		}
	}

	if (frameScroll < 0)
		frameScroll = 0;

	if (frameScroll >= loadedFrame.rows.size() - 40)
		frameScroll = loadedFrame.rows.size() - 39;


	return;
}