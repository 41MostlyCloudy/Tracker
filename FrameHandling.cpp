#pragma once


#include "GlobalVariables.h"


Frame rollFrame(UnrolledFrame frame); // Rolls and returns the frame given.

UnrolledFrame unrollFrame(Frame frame); // Unrolls and returns the unrolled frame given. (for viewing and editing).

void saveCurrentFrame();

void loadCurrentFrame();

UnrolledFrame resizeUnrolledFrameRows(UnrolledFrame frame, int newSize);

UnrolledFrame resizeSongFrameEffectColumns(UnrolledFrame frame);

void copyNotes();

void pasteNotes();

void deleteNotes();

void transposeNotes();

void setNoteSamples();




Frame rollFrame(UnrolledFrame frame)
{
	// Frame Format:

	// For each channel
	//	1 byte: Note
	//	1 byte: Instrument
	//	1 byte: To next note

	//	1 byte:	Volume
	//	1 byte: To next volume

	//	5 bytes: Voice sample
	//	1 byte: To next voice sample

	//	1 byte: Effect
	//	1 byte: Effect value
	//	1 byte: To next effect (This is set to 0 for multiple effects per beat.)

	Frame newFrame;

	newFrame.rows = frame.rows.size();
	newFrame.beatsPerMeasure = frame.beatsPerMeasure;
	newFrame.channels.resize(loadedSong.numberOfChannels);

	for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
	{
		newFrame.channels[ch].notes.clear();
		newFrame.channels[ch].volumes.clear();
		newFrame.channels[ch].voiceSamples.clear();
		newFrame.channels[ch].effects.clear();
		int toNextNote = 0;
		int toNextVolume = 0;
		int toNextVoiceSample = 0;
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

			bool rowHasVoiceSample = false;
			for (int vSample = 0; vSample < 5; vSample++)
			{
				if (frame.rows[i].voiceSamples[ch].sample[vSample] != 44)
					rowHasVoiceSample = true;
			}
			if (rowHasVoiceSample)
			{
				newFrame.channels[ch].voiceSamples.emplace_back(toNextVoiceSample);
				for (int vSample = 0; vSample < 5; vSample++)
				{
					newFrame.channels[ch].voiceSamples.emplace_back(frame.rows[i].voiceSamples[ch].sample[vSample]);
				}
				toNextVoiceSample = 0;
			}
			else
				toNextVoiceSample++;
			/*
			if (frame.rows[i].effects[ch].cEffect[0] > -1)
			{
				newFrame.channels[ch].effects.emplace_back(toNextEffect);
				newFrame.channels[ch].effects.emplace_back(frame.rows[i].effects[ch].cEffect[0]);
				newFrame.channels[ch].effects.emplace_back(frame.rows[i].effects[ch].cEffectValue[0]);
				toNextEffect = 0;
			}
			else
				toNextEffect++;
				*/


			
			bool effectOnBeat = false;
			for (int ef = 0; ef < frame.rows[i].effects[ch].cEffect.size(); ef++)
			{
				if (frame.rows[i].effects[ch].cEffect[ef] > -1)
				{
					newFrame.channels[ch].effects.emplace_back(toNextEffect);
					newFrame.channels[ch].effects.emplace_back(frame.rows[i].effects[ch].cEffect[ef]);
					newFrame.channels[ch].effects.emplace_back(frame.rows[i].effects[ch].cEffectValue[ef]);
					toNextEffect = 255;
					effectOnBeat = true;
				}
			}
			if (effectOnBeat)
				toNextEffect = 0;
			else
				toNextEffect++;
				

			if (i == frame.rows.size() - 1)
			{
				if (newFrame.channels[ch].notes.size() == 0)
					newFrame.channels[ch].notes.emplace_back(toNextNote);
				if (newFrame.channels[ch].volumes.size() == 0)
					newFrame.channels[ch].volumes.emplace_back(toNextVolume);
				if (newFrame.channels[ch].voiceSamples.size() == 0)
					newFrame.channels[ch].voiceSamples.emplace_back(toNextVoiceSample);
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
	//	1 byte: To next note
	//	1 byte: Note
	//	1 byte: Instrument

	//	1 byte: To next volume
	//	1 byte:	Volume
	
	//	1 byte: To next effect
	//	1 byte: Effect
	//	1 byte: Effect value
	

	UnrolledFrame newFrame;

	newFrame = resizeUnrolledFrameRows(newFrame, int(frame.rows));
	newFrame = resizeSongFrameEffectColumns(newFrame);
	newFrame.beatsPerMeasure = frame.beatsPerMeasure;
	


	for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
	{
		channels[ch].effectCountPerRow = 1;

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

		for (int i = 0; i < frame.channels[ch].voiceSamples.size(); i += 6)
		{
			int toNextNote = frame.channels[ch].voiceSamples[i];

			for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
			{
				newFrame.rows[j].voiceSamples[ch] = { 44, 44, 44, 44, 44 };
			}

			unrolledNoteIndex += toNextNote;



			if (i + 5 < frame.channels[ch].voiceSamples.size())
			{
				for (int vSample = 0; vSample < 5; vSample++)
				{
					int nextNote = frame.channels[ch].voiceSamples[i + vSample + 1];
					newFrame.rows[unrolledNoteIndex].voiceSamples[ch].sample[vSample] = nextNote;
				}
			}

			unrolledNoteIndex++;
		}


		unrolledNoteIndex = 0;
		int effectNum = 0;

		for (int i = 0; i < frame.channels[ch].effects.size(); i += 3)
		{
			int toNextNote = frame.channels[ch].effects[i];

			
			if (toNextNote == 255)
			{
				effectNum++;
				if (effectNum >= channels[ch].effectCountPerRow)
				{
					channels[ch].effectCountPerRow++;
					newFrame = resizeSongFrameEffectColumns(newFrame);
				}
				unrolledNoteIndex--;
				int nextNote = frame.channels[ch].effects[i + 1];
				newFrame.rows[unrolledNoteIndex].effects[ch].cEffect[effectNum] = nextNote;
				nextNote = frame.channels[ch].effects[i + 2];
				newFrame.rows[unrolledNoteIndex].effects[ch].cEffectValue[effectNum] = nextNote;
				unrolledNoteIndex++;
			}
			else
			{
				effectNum = 0;
				for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
				{
					newFrame.rows[j].effects[ch].cEffect[0] = -1;
					newFrame.rows[j].effects[ch].cEffectValue[0] = -1;
				}



				unrolledNoteIndex += toNextNote;


				if (i < frame.channels[ch].effects.size() - 1)
				{
					int nextNote = frame.channels[ch].effects[i + 1];
					newFrame.rows[unrolledNoteIndex].effects[ch].cEffect[0] = nextNote;
					nextNote = frame.channels[ch].effects[i + 2];
					newFrame.rows[unrolledNoteIndex].effects[ch].cEffectValue[0] = nextNote;
				}

				unrolledNoteIndex++;
			}
		}
	}

	

	return newFrame;
}



void saveCurrentFrame()
{
	loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]] = rollFrame(loadedFrame);
	gui.drawUIThisFrame = true;
	gui.drawFrameThisFrame = true;
}



void loadCurrentFrame()
{
	loadedFrame = unrollFrame(loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]]);
	gui.frameScroll.y = 0.0f;

	// The interface has changed, and must be redrawn.
	gui.drawUIThisFrame = true;
	gui.drawFrameThisFrame = true;
	loadedFrame = resizeSongFrameEffectColumns(loadedFrame);
}



UnrolledFrame resizeUnrolledFrameRows(UnrolledFrame frame, int newSize)
{
	if (newSize > frame.rows.size())
	{
		for (int fr = 0; fr < newSize; fr++)
		{
			FrameRow newRow;
			for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
			{
				newRow.note.emplace_back(-1);
				newRow.instrument.emplace_back(-1);
				newRow.volume.emplace_back(-1);

				UnrolledVoiceSamples vSamples;
				for (int sample = 0; sample < 5; sample++)
					newRow.voiceSamples.emplace_back(vSamples);

				UnrolledEffects newEffects;
				newRow.effects.emplace_back(newEffects);
				//newRow.effect.emplace_back(-1);
				//newRow.effectValue.emplace_back(-1);
			}
			frame.rows.emplace_back(newRow);
		}
	}
	else
	{
		frame.rows.resize(newSize);
		frame.rows.shrink_to_fit();
	}

	return frame;
}



UnrolledFrame resizeSongFrameEffectColumns(UnrolledFrame frame)
{
	for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
	{
		for (int fr = 0; fr < frame.rows.size(); fr++)
		{
			int oldEffectNum = frame.rows[fr].effects[ch].cEffect.size();
			frame.rows[fr].effects[ch].cEffect.resize(channels[ch].effectCountPerRow);
			frame.rows[fr].effects[ch].cEffectValue.resize(channels[ch].effectCountPerRow);
			for (int i = oldEffectNum; i < channels[ch].effectCountPerRow; i++)
			{
				frame.rows[fr].effects[ch].cEffect[i] = -1;
				frame.rows[fr].effects[ch].cEffectValue[i] = -1;
			}
		}
	}

	return frame;
}




void copyNotes()
{
	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	// Create mouse selection frame.
	frameSelection.rows.clear();
	frameSelection = resizeUnrolledFrameRows(frameSelection, int(editor.noteSelectionEnd.y + 1 - editor.noteSelectionStart.y));
	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;

	for (int y = 0; y < frameSelection.rows.size(); y++)
	{
		for (int x = 0; x < loadedSong.numberOfChannels; x++)
		{
			frameSelection.rows[y].note[x] = -2;
			frameSelection.rows[y].instrument[x] = -2;
			frameSelection.rows[y].volume[x] = -2;
			frameSelection.rows[y].voiceSamples[x] = { -2, -2, -2, -2, -2 };
			frameSelection.rows[y].effects[x].cEffect.resize(channels[x].effectCountPerRow);
			frameSelection.rows[y].effects[x].cEffectValue.resize(channels[x].effectCountPerRow);
			for (int i = 0; i < channels[x].effectCountPerRow; i++)
			{
				frameSelection.rows[y].effects[x].cEffect[i] = -2;
				frameSelection.rows[y].effects[x].cEffectValue[i] = -2;
			}
		}

		for (int x = editor.noteSelectionStart.x; x < editor.noteSelectionEnd.x; x++)
		{
			Vector2 selection = findFrameTileByPosition(x);
			int selectedChannel = selection.x;
			int selectedPart = selection.y;

			if (selectedPart == -2) // Entire channel selected.
			{
				frameSelection.rows[y].note[selectedChannel - leftMostChannel] = loadedFrame.rows[y + editor.noteSelectionStart.y].note[selectedChannel];
				frameSelection.rows[y].instrument[selectedChannel - leftMostChannel] = loadedFrame.rows[y + editor.noteSelectionStart.y].instrument[selectedChannel];
				frameSelection.rows[y].volume[selectedChannel - leftMostChannel] = loadedFrame.rows[y + editor.noteSelectionStart.y].volume[selectedChannel];
				for (int ef = 0; ef < frameSelection.rows[y].effects[0].cEffect.size(); ef++)
				{
					frameSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffect[ef] = loadedFrame.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffect[ef];
					frameSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffectValue[ef] = loadedFrame.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffectValue[ef];
				}
			}
			else if (selectedPart < 6)
			{
				frameSelection.rows[y].note[selectedChannel - leftMostChannel] = loadedFrame.rows[y + editor.noteSelectionStart.y].note[selectedChannel];
				frameSelection.rows[y].instrument[selectedChannel - leftMostChannel] = loadedFrame.rows[y + editor.noteSelectionStart.y].instrument[selectedChannel];
			}
			else if (selectedPart < 8)
				frameSelection.rows[y].volume[selectedChannel - leftMostChannel] = loadedFrame.rows[y + editor.noteSelectionStart.y].volume[selectedChannel];

			if (selectedPart > 6)
			{
				selectedPart -= 7;

				if (channels[selectedChannel].hasVoiceColumns)
					selectedPart -= 5;

				if (selectedPart < 0) // Voice sample
				{
					if (channels[selectedChannel].hasVoiceColumns)
					{
						for (int voiceNum = 0; voiceNum < 5; voiceNum++)
							frameSelection.rows[y].voiceSamples[selectedChannel - leftMostChannel].sample[voiceNum] = loadedFrame.rows[y + editor.noteSelectionStart.y].voiceSamples[selectedChannel].sample[voiceNum];
					}
				}
				else // Effect
				{
					int effectPart = (selectedPart) % 4;
					int effectNum = (selectedPart) / 4;

					frameSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffect[effectNum] = loadedFrame.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffect[effectNum];
					frameSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffectValue[effectNum] = loadedFrame.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffectValue[effectNum];
				}
			}
		}
	}

	return;
}




void pasteNotes()
{
	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;

	for (int y = 0; y < frameSelection.rows.size(); y++)
	{
		if (y + int(editor.noteSelectionStart.y) < loadedFrame.rows.size())
		{
			for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
			{
				if (ch + leftMostChannel < loadedSong.numberOfChannels)
				{
					if (frameSelection.rows[y].note[ch] > -2)
						loadedFrame.rows[y + int(editor.noteSelectionStart.y)].note[ch + leftMostChannel] = frameSelection.rows[y].note[ch];
					if (frameSelection.rows[y].instrument[ch] > -2)
						loadedFrame.rows[y + int(editor.noteSelectionStart.y)].instrument[ch + leftMostChannel] = frameSelection.rows[y].instrument[ch];
					if (frameSelection.rows[y].volume[ch] > -2)
						loadedFrame.rows[y + int(editor.noteSelectionStart.y)].volume[ch + leftMostChannel] = frameSelection.rows[y].volume[ch];

					if (channels[ch].hasVoiceColumns)
					{
						if (frameSelection.rows[y].voiceSamples[ch].sample[0] != -2)
						{
							for (int voiceNum = 0; voiceNum < 5; voiceNum++)
								loadedFrame.rows[y + int(editor.noteSelectionStart.y)].voiceSamples[ch + leftMostChannel] = frameSelection.rows[y].voiceSamples[ch];
						}
					}

					for (int i = 0; i < channels[ch + leftMostChannel].effectCountPerRow; i++)
					{
						if (i < frameSelection.rows[y].effects[ch].cEffect.size())
						{
							if (frameSelection.rows[y].effects[ch].cEffect[i] > -2)
								loadedFrame.rows[y + int(editor.noteSelectionStart.y)].effects[ch + leftMostChannel].cEffect[i] = frameSelection.rows[y].effects[ch].cEffect[i];
							if (frameSelection.rows[y].effects[ch].cEffectValue[i] > -2)
								loadedFrame.rows[y + int(editor.noteSelectionStart.y)].effects[ch + leftMostChannel].cEffectValue[i] = frameSelection.rows[y].effects[ch].cEffectValue[i];
						}
					}
				}
			}
		}
	}
	loadedSong.unsavedChanges = true;

	return;
}


void deleteNotes()
{
	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	for (int y = editor.noteSelectionStart.y; y <= editor.noteSelectionEnd.y; y++)
	{
		for (int x = editor.noteSelectionStart.x; x < editor.noteSelectionEnd.x; x++)
		{
			Vector2 selection = findFrameTileByPosition(x);

			if (selection.x != -1)
			{
				int selectedChannel = selection.x;
				int selectedPart = selection.y;

				if (selectedPart == -2) // Entire channel selected.
				{
					loadedFrame.rows[y].note[selectedChannel] = -1;
					loadedFrame.rows[y].instrument[selectedChannel] = -1;
					loadedFrame.rows[y].volume[selectedChannel] = -1;
					for (int ef = 0; ef < loadedFrame.rows[y].effects[0].cEffect.size(); ef++)
					{
						loadedFrame.rows[y].effects[selectedChannel].cEffect[ef] = -1;
						loadedFrame.rows[y].effects[selectedChannel].cEffectValue[ef] = -1;
					}
					x += 2;
				}
				else if (selectedPart < 5)
				{
					loadedFrame.rows[y].note[selectedChannel] = -1;
					loadedFrame.rows[y].instrument[selectedChannel] = -1;
				}
				else if (selectedPart < 7)
					loadedFrame.rows[y].volume[selectedChannel] = -1;
				else
				{
					selectedPart -= 7;

					if (channels[selectedChannel].hasVoiceColumns)
						selectedPart -= 5;

					if (selectedPart < 0) // Voice sample
					{
						int voiceNum = (selectedPart + 5);
						loadedFrame.rows[y].voiceSamples[selectedChannel].sample[voiceNum] = 44;
					}
					else // Effect
					{
						int effectPart = (selectedPart) % 4;
						int effectNum = (selectedPart) / 4;

						loadedFrame.rows[y].effects[selectedChannel].cEffect[effectNum] = -1;
						loadedFrame.rows[y].effects[selectedChannel].cEffectValue[effectNum] = -1;
					}
				}
			}
		}
	}
	loadedSong.unsavedChanges = true;

	return;
}



void transposeNotes()
{
	for (int y = editor.noteSelectionStart.y; y <= editor.noteSelectionEnd.y; y++)
	{
		for (int x = editor.noteSelectionStart.x; x < editor.noteSelectionEnd.x; x++)
		{
			Vector2 selection = findFrameTileByPosition(x);

			if (selection.x != -1)
			{
				int selectedChannel = selection.x;
				int selectedPart = selection.y;

				if (selectedPart < 5)
				{
					if (loadedFrame.rows[y].note[selectedChannel] != -1)
						loadedFrame.rows[y].note[selectedChannel] += editor.transposeValue;
				}
			}
		}
	}
	loadedSong.unsavedChanges = true;

	return;
}



void setNoteSamples()
{
	for (int y = editor.noteSelectionStart.y; y <= editor.noteSelectionEnd.y; y++)
	{
		for (int x = editor.noteSelectionStart.x; x < editor.noteSelectionEnd.x; x++)
		{
			Vector2 selection = findFrameTileByPosition(x);

			if (selection.x != -1)
			{
				int selectedChannel = selection.x;
				int selectedPart = selection.y;

				if (selectedPart < 5)
				{
					if (loadedFrame.rows[y].instrument[selectedChannel] != -1)
						loadedFrame.rows[y].instrument[selectedChannel] = editor.selectedSample;
				}
			}
		}
	}
	loadedSong.unsavedChanges = true;

	return;
}