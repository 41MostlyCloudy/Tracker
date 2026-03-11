#pragma once


#include "GlobalVariables.h"


Pattern rollPattern(UnrolledPattern pattern); // Rolls and returns the frame given.

UnrolledPattern unrollPattern(Pattern pattern); // Unrolls and returns the unrolled frame given. (for viewing and editing).

void saveCurrentPattern();

void loadCurrentPattern();

UnrolledPattern resizeUnrolledPatternRows(UnrolledPattern pattern, int newSize);

UnrolledPattern resizeSongPatternEffectColumns(UnrolledPattern pattern);

void copyNotes();

void pasteNotes();

void deleteNotes();

void transposeNotes();

void setNoteSamples();




Pattern rollPattern(UnrolledPattern pattern)
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

	Pattern newPattern;

	newPattern.rows = pattern.rows.size();
	newPattern.beatsPerMeasure = pattern.beatsPerMeasure;
	newPattern.channels.resize(loadedSong.numberOfChannels);

	for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
	{
		newPattern.channels[ch].notes.clear();
		newPattern.channels[ch].volumes.clear();
		newPattern.channels[ch].effects.clear();
		int toNextNote = 0;
		int toNextVolume = 0;
		int toNextVoiceSample = 0;
		int toNextEffect = 0;
		for (int i = 0; i < pattern.rows.size(); i++)
		{
			if (pattern.rows[i].note[ch] > -1)
			{
				newPattern.channels[ch].notes.emplace_back(toNextNote);
				newPattern.channels[ch].notes.emplace_back(pattern.rows[i].note[ch]);
				newPattern.channels[ch].notes.emplace_back(pattern.rows[i].instrument[ch]);
				toNextNote = 0;
			}
			else
				toNextNote++;
			if (pattern.rows[i].volume[ch] > -1)
			{
				newPattern.channels[ch].volumes.emplace_back(toNextVolume);
				newPattern.channels[ch].volumes.emplace_back(pattern.rows[i].volume[ch]);
				toNextVolume = 0;
			}
			else
				toNextVolume++;


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
			for (int ef = 0; ef < pattern.rows[i].effects[ch].cEffect.size(); ef++)
			{
				if (pattern.rows[i].effects[ch].cEffect[ef] > -1)
				{
					newPattern.channels[ch].effects.emplace_back(toNextEffect);
					newPattern.channels[ch].effects.emplace_back(pattern.rows[i].effects[ch].cEffect[ef]);
					newPattern.channels[ch].effects.emplace_back(pattern.rows[i].effects[ch].cEffectValue[ef]);
					toNextEffect = 255;
					effectOnBeat = true;
				}
			}
			if (effectOnBeat)
				toNextEffect = 0;
			else
				toNextEffect++;
				

			if (i == pattern.rows.size() - 1)
			{
				if (newPattern.channels[ch].notes.size() == 0)
					newPattern.channels[ch].notes.emplace_back(toNextNote);
				if (newPattern.channels[ch].volumes.size() == 0)
					newPattern.channels[ch].volumes.emplace_back(toNextVolume);
				if (newPattern.channels[ch].effects.size() == 0)
					newPattern.channels[ch].effects.emplace_back(toNextEffect);
			}
		}
	}




	return newPattern;
}



UnrolledPattern unrollPattern(Pattern pattern)
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
	

	UnrolledPattern newPattern;

	newPattern = resizeUnrolledPatternRows(newPattern, int(pattern.rows));
	newPattern = resizeSongPatternEffectColumns(newPattern);
	newPattern.beatsPerMeasure = pattern.beatsPerMeasure;
	


	for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
	{
		channels[ch].effectCountPerRow = 1;

		int unrolledNoteIndex = 0;

		for (int i = 0; i < pattern.channels[ch].notes.size(); i += 3)
		{
			int toNextNote = pattern.channels[ch].notes[i];

			for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
			{
				newPattern.rows[j].note[ch] = -1;
				newPattern.rows[j].instrument[ch] = -1;
			}

			unrolledNoteIndex += toNextNote;
			

			if (i < pattern.channels[ch].notes.size() - 1)
			{
				int nextNote = pattern.channels[ch].notes[i + 1];
				newPattern.rows[unrolledNoteIndex].note[ch] = nextNote;
				nextNote = pattern.channels[ch].notes[i + 2];
				newPattern.rows[unrolledNoteIndex].instrument[ch] = nextNote;
			}

			unrolledNoteIndex++;
		}


		unrolledNoteIndex = 0;

		for (int i = 0; i < pattern.channels[ch].volumes.size(); i += 2)
		{
			int toNextNote = pattern.channels[ch].volumes[i];

			for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
			{
				newPattern.rows[j].volume[ch] = -1;
			}

			unrolledNoteIndex += toNextNote;


			if (i < pattern.channels[ch].volumes.size() - 1)
			{
				int nextNote = pattern.channels[ch].volumes[i + 1];
				newPattern.rows[unrolledNoteIndex].volume[ch] = nextNote;
			}

			unrolledNoteIndex++;
		}


		unrolledNoteIndex = 0;
		int effectNum = 0;

		for (int i = 0; i < pattern.channels[ch].effects.size(); i += 3)
		{
			int toNextNote = pattern.channels[ch].effects[i];

			
			if (toNextNote == 255)
			{
				effectNum++;
				if (effectNum >= channels[ch].effectCountPerRow)
				{
					channels[ch].effectCountPerRow++;
					newPattern = resizeSongPatternEffectColumns(newPattern);
				}
				unrolledNoteIndex--;
				int nextNote = pattern.channels[ch].effects[i + 1];
				newPattern.rows[unrolledNoteIndex].effects[ch].cEffect[effectNum] = nextNote;
				nextNote = pattern.channels[ch].effects[i + 2];
				newPattern.rows[unrolledNoteIndex].effects[ch].cEffectValue[effectNum] = nextNote;
				unrolledNoteIndex++;
			}
			else
			{
				effectNum = 0;
				for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
				{
					newPattern.rows[j].effects[ch].cEffect[0] = -1;
					newPattern.rows[j].effects[ch].cEffectValue[0] = -1;
				}



				unrolledNoteIndex += toNextNote;


				if (i < pattern.channels[ch].effects.size() - 1)
				{
					int nextNote = pattern.channels[ch].effects[i + 1];
					newPattern.rows[unrolledNoteIndex].effects[ch].cEffect[0] = nextNote;
					nextNote = pattern.channels[ch].effects[i + 2];
					newPattern.rows[unrolledNoteIndex].effects[ch].cEffectValue[0] = nextNote;
				}

				unrolledNoteIndex++;
			}
		}
	}

	

	return newPattern;
}



void saveCurrentPattern()
{
	loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]] = rollPattern(loadedPattern);
	gui.drawUIThisFrame = true;
	gui.drawFrameThisFrame = true;
}



void loadCurrentPattern()
{
	loadedPattern = unrollPattern(loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]]);
	gui.frameScroll.y = 0.0f;

	// The interface has changed, and must be redrawn.
	gui.drawUIThisFrame = true;
	gui.drawFrameThisFrame = true;
	loadedPattern = resizeSongPatternEffectColumns(loadedPattern);
}



UnrolledPattern resizeUnrolledPatternRows(UnrolledPattern pattern, int newSize)
{
	if (newSize > pattern.rows.size())
	{
		for (int fr = 0; fr < newSize; fr++)
		{
			PatternRow newRow;
			for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
			{
				newRow.note.emplace_back(-1);
				newRow.instrument.emplace_back(-1);
				newRow.volume.emplace_back(-1);


				UnrolledEffects newEffects;
				newRow.effects.emplace_back(newEffects);
				//newRow.effect.emplace_back(-1);
				//newRow.effectValue.emplace_back(-1);
			}
			pattern.rows.emplace_back(newRow);
		}
	}
	else
	{
		pattern.rows.resize(newSize);
		pattern.rows.shrink_to_fit();
	}

	return pattern;
}



UnrolledPattern resizeSongPatternEffectColumns(UnrolledPattern pattern)
{
	for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
	{
		for (int fr = 0; fr < pattern.rows.size(); fr++)
		{
			int oldEffectNum = pattern.rows[fr].effects[ch].cEffect.size();
			pattern.rows[fr].effects[ch].cEffect.resize(channels[ch].effectCountPerRow);
			pattern.rows[fr].effects[ch].cEffectValue.resize(channels[ch].effectCountPerRow);
			for (int i = oldEffectNum; i < channels[ch].effectCountPerRow; i++)
			{
				pattern.rows[fr].effects[ch].cEffect[i] = -1;
				pattern.rows[fr].effects[ch].cEffectValue[i] = -1;
			}
		}
	}

	return pattern;
}




void copyNotes()
{
	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	// Create mouse selection frame.
	patternSelection.rows.clear();
	patternSelection = resizeUnrolledPatternRows(patternSelection, int(editor.noteSelectionEnd.y + 1 - editor.noteSelectionStart.y));
	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;

	for (int y = 0; y < patternSelection.rows.size(); y++)
	{
		for (int x = 0; x < loadedSong.numberOfChannels; x++)
		{
			patternSelection.rows[y].note[x] = -2;
			patternSelection.rows[y].instrument[x] = -2;
			patternSelection.rows[y].volume[x] = -2;
			patternSelection.rows[y].effects[x].cEffect.resize(channels[x].effectCountPerRow);
			patternSelection.rows[y].effects[x].cEffectValue.resize(channels[x].effectCountPerRow);
			for (int i = 0; i < channels[x].effectCountPerRow; i++)
			{
				patternSelection.rows[y].effects[x].cEffect[i] = -2;
				patternSelection.rows[y].effects[x].cEffectValue[i] = -2;
			}
		}

		for (int x = editor.noteSelectionStart.x; x < editor.noteSelectionEnd.x; x++)
		{
			Vector2 selection = findFrameTileByPosition(x);
			int selectedChannel = selection.x;
			int selectedPart = selection.y;

			if (selectedPart == -2) // Entire channel selected.
			{
				patternSelection.rows[y].note[selectedChannel - leftMostChannel] = loadedPattern.rows[y + editor.noteSelectionStart.y].note[selectedChannel];
				patternSelection.rows[y].instrument[selectedChannel - leftMostChannel] = loadedPattern.rows[y + editor.noteSelectionStart.y].instrument[selectedChannel];
				patternSelection.rows[y].volume[selectedChannel - leftMostChannel] = loadedPattern.rows[y + editor.noteSelectionStart.y].volume[selectedChannel];
				for (int ef = 0; ef < patternSelection.rows[y].effects[0].cEffect.size(); ef++)
				{
					patternSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffect[ef] = loadedPattern.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffect[ef];
					patternSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffectValue[ef] = loadedPattern.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffectValue[ef];
				}
			}
			else if (selectedPart < 6)
			{
				patternSelection.rows[y].note[selectedChannel - leftMostChannel] = loadedPattern.rows[y + editor.noteSelectionStart.y].note[selectedChannel];
				patternSelection.rows[y].instrument[selectedChannel - leftMostChannel] = loadedPattern.rows[y + editor.noteSelectionStart.y].instrument[selectedChannel];
			}
			else if (selectedPart < 8)
				patternSelection.rows[y].volume[selectedChannel - leftMostChannel] = loadedPattern.rows[y + editor.noteSelectionStart.y].volume[selectedChannel];

			if (selectedPart > 6)
			{
				selectedPart -= 7;

				if (selectedPart >= 0) // Effecct
				{
					int effectPart = (selectedPart) % 4;
					int effectNum = (selectedPart) / 4;

					patternSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffect[effectNum] = loadedPattern.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffect[effectNum];
					patternSelection.rows[y].effects[selectedChannel - leftMostChannel].cEffectValue[effectNum] = loadedPattern.rows[y + editor.noteSelectionStart.y].effects[selectedChannel].cEffectValue[effectNum];
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

	for (int y = 0; y < patternSelection.rows.size(); y++)
	{
		if (y + int(editor.noteSelectionStart.y) < loadedPattern.rows.size())
		{
			for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
			{
				if (ch + leftMostChannel < loadedSong.numberOfChannels)
				{
					if (patternSelection.rows[y].note[ch] > -2)
						loadedPattern.rows[y + int(editor.noteSelectionStart.y)].note[ch + leftMostChannel] = patternSelection.rows[y].note[ch];
					if (patternSelection.rows[y].instrument[ch] > -2)
						loadedPattern.rows[y + int(editor.noteSelectionStart.y)].instrument[ch + leftMostChannel] = patternSelection.rows[y].instrument[ch];
					if (patternSelection.rows[y].volume[ch] > -2)
						loadedPattern.rows[y + int(editor.noteSelectionStart.y)].volume[ch + leftMostChannel] = patternSelection.rows[y].volume[ch];



					for (int i = 0; i < channels[ch + leftMostChannel].effectCountPerRow; i++)
					{
						if (i < patternSelection.rows[y].effects[ch].cEffect.size())
						{
							if (patternSelection.rows[y].effects[ch].cEffect[i] > -2)
								loadedPattern.rows[y + int(editor.noteSelectionStart.y)].effects[ch + leftMostChannel].cEffect[i] = patternSelection.rows[y].effects[ch].cEffect[i];
							if (patternSelection.rows[y].effects[ch].cEffectValue[i] > -2)
								loadedPattern.rows[y + int(editor.noteSelectionStart.y)].effects[ch + leftMostChannel].cEffectValue[i] = patternSelection.rows[y].effects[ch].cEffectValue[i];
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
					loadedPattern.rows[y].note[selectedChannel] = -1;
					loadedPattern.rows[y].instrument[selectedChannel] = -1;
					loadedPattern.rows[y].volume[selectedChannel] = -1;
					for (int ef = 0; ef < loadedPattern.rows[y].effects[0].cEffect.size(); ef++)
					{
						loadedPattern.rows[y].effects[selectedChannel].cEffect[ef] = -1;
						loadedPattern.rows[y].effects[selectedChannel].cEffectValue[ef] = -1;
					}
					x += 2;
				}
				else if (selectedPart < 5)
				{
					loadedPattern.rows[y].note[selectedChannel] = -1;
					loadedPattern.rows[y].instrument[selectedChannel] = -1;
				}
				else if (selectedPart < 7)
					loadedPattern.rows[y].volume[selectedChannel] = -1;
				else
				{
					selectedPart -= 7;

					if (selectedPart >= 0) // Effect
					{
						int effectPart = (selectedPart) % 4;
						int effectNum = (selectedPart) / 4;

						loadedPattern.rows[y].effects[selectedChannel].cEffect[effectNum] = -1;
						loadedPattern.rows[y].effects[selectedChannel].cEffectValue[effectNum] = -1;
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
					if (loadedPattern.rows[y].note[selectedChannel] != -1)
						loadedPattern.rows[y].note[selectedChannel] += editor.transposeValue;
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
					if (loadedPattern.rows[y].instrument[selectedChannel] != -1)
						loadedPattern.rows[y].instrument[selectedChannel] = editor.selectedSample;
				}
			}
		}
	}
	loadedSong.unsavedChanges = true;

	return;
}