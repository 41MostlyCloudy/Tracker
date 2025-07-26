#pragma once


#include "GlobalVariables.h"


Frame rollFrame(UnrolledFrame frame); // Rolls and returns the frame given.

UnrolledFrame unrollFrame(Frame frame); // Unrolls and returns the unrolled frame given. (for viewing and editing).

void saveCurrentFrame();

void loadCurrentFrame();




Frame rollFrame(UnrolledFrame frame)
{
	// Frame Format:

	// For each channel
	//	1 byte: Note
	//	1 byte: Instrument
	//	1 byte: To next note

	//	1 byte:	Volume
	//	1 byte: To next volume

	//	1 byte: Effect
	//	1 byte: Effect value
	//	1 byte: To next effect

	Frame newFrame;

	newFrame.rows = frame.rows.size();

	for (int ch = 0; ch < 8; ch++)
	{
		newFrame.channels[ch].notes.clear();
		newFrame.channels[ch].volumes.clear();
		newFrame.channels[ch].effects.clear();
		int toNextNote = 0;
		int toNextVolume = 0;
		int toNextEffect = 0;
		for (int i = 0; i < frame.rows.size(); i++)
		{
			if (frame.rows[i].note[ch] > -1)
			{
				newFrame.channels[ch].notes.emplace_back(toNextNote);
				newFrame.channels[ch].notes.emplace_back(frame.rows[i].note[ch]);
				newFrame.channels[ch].notes.emplace_back(frame.rows[i].instrument[ch]);
				toNextNote = 0;
			}
			else
				toNextNote++;
			if (frame.rows[i].volume[ch] > -1)
			{
				newFrame.channels[ch].volumes.emplace_back(toNextVolume);
				newFrame.channels[ch].volumes.emplace_back(frame.rows[i].volume[ch]);
				toNextVolume = 0;
			}
			else
				toNextVolume++;
			if (frame.rows[i].effect[ch] > -1)
			{
				newFrame.channels[ch].effects.emplace_back(toNextEffect);
				newFrame.channels[ch].effects.emplace_back(frame.rows[i].effect[ch]);
				newFrame.channels[ch].effects.emplace_back(frame.rows[i].effectValue[ch]);
				toNextEffect = 0;
			}
			else
				toNextEffect++;

			if (i == frame.rows.size() - 1)
			{
				if (newFrame.channels[ch].notes.size() == 0)
					newFrame.channels[ch].notes.emplace_back(toNextNote);
				if (newFrame.channels[ch].volumes.size() == 0)
					newFrame.channels[ch].volumes.emplace_back(toNextVolume);
				if (newFrame.channels[ch].effects.size() == 0)
					newFrame.channels[ch].effects.emplace_back(toNextEffect);
			}
		}
	}




	return newFrame;
}



UnrolledFrame unrollFrame(Frame frame)
{
	// Frame Format:

	// Each channel
	//	1 byte: Note
	//	1 byte: Instrument
	//	1 byte: To next note

	//	1 byte:	Volume
	//	1 byte: To next volume

	//	1 byte: Effect
	//	1 byte: Effect value
	//	1 byte: To next effect

	UnrolledFrame newFrame;

	newFrame.rows.resize(frame.rows);


	for (int ch = 0; ch < 8; ch++)	
	{
		int unrolledNoteIndex = 0;

		for (int i = 0; i < frame.channels[ch].notes.size(); i += 3)
		{
			int toNextNote = frame.channels[ch].notes[i];

			for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
			{
				newFrame.rows[j].note[ch] = -1;
				newFrame.rows[j].instrument[ch] = -1;
			}

			unrolledNoteIndex += toNextNote;
			

			if (i < frame.channels[ch].notes.size() - 1)
			{
				int nextNote = frame.channels[ch].notes[i + 1];
				newFrame.rows[unrolledNoteIndex].note[ch] = nextNote;
				nextNote = frame.channels[ch].notes[i + 2];
				newFrame.rows[unrolledNoteIndex].instrument[ch] = nextNote;
			}

			unrolledNoteIndex++;
		}


		unrolledNoteIndex = 0;

		for (int i = 0; i < frame.channels[ch].volumes.size(); i += 2)
		{
			int toNextNote = frame.channels[ch].volumes[i];

			for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
			{
				newFrame.rows[j].volume[ch] = -1;
			}

			unrolledNoteIndex += toNextNote;


			if (i < frame.channels[ch].volumes.size() - 1)
			{
				int nextNote = frame.channels[ch].volumes[i + 1];
				newFrame.rows[unrolledNoteIndex].volume[ch] = nextNote;
			}

			unrolledNoteIndex++;
		}


		unrolledNoteIndex = 0;

		for (int i = 0; i < frame.channels[ch].effects.size(); i += 3)
		{
			int toNextNote = frame.channels[ch].effects[i];

			for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
			{
				newFrame.rows[j].effect[ch] = -1;
				newFrame.rows[j].effectValue[ch] = -1;
			}

			unrolledNoteIndex += toNextNote;


			if (i < frame.channels[ch].effects.size() - 1)
			{
				int nextNote = frame.channels[ch].effects[i + 1];
				newFrame.rows[unrolledNoteIndex].effect[ch] = nextNote;
				nextNote = frame.channels[ch].effects[i + 2];
				newFrame.rows[unrolledNoteIndex].effectValue[ch] = nextNote;
			}

			unrolledNoteIndex++;
		}
	}

	return newFrame;
}



void saveCurrentFrame()
{
	loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]] = rollFrame(loadedFrame);
}



void loadCurrentFrame()
{
	loadedFrame = unrollFrame(loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]]);
	frameScroll = 0.0f;
}