#pragma once

#include "Structures.cpp"


// miniaudio
//----------------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
//----------------------------------------------------------------------------------


int calculateSongLength(); // Returns the song's length in seconds.



float delta = 0.0f; // Time between frames.



std::vector <Channel> channels;

Screen screen;
GUI gui;
Editor editor;
WindowController windowController;

Song loadedSong;
UnrolledFrame loadedFrame; // The frame currently in the editor.
std::vector <std::string> fileNameList; // The names of the samples in the "Samples" file.
//std::vector <Sample> loadedSamples; // The names of the samples used in the song.
Sample loadedSamples[256]; // The names of the samples used in the song.

UnrolledFrame frameSelection; // A frame containing the portion of the frame that is selected, with that number of rows. All values not used are set to -2.

FileNavigator fileNavigator;

SampleDisplay sampleDisplay;
PresetMenu presetMenu;

VoiceSynth voiceSynth;


Vector2 findFrameTileByPosition(int pos) // Returns the channel and channel part as a Vector2
{
	int selectedPart = pos;
	int selectedChannel = 0;
	bool inChannel = false;
	while (!inChannel)
	{
		int channelSize = 0;
		
		if (channels[selectedChannel].compressed)
			channelSize = 3;
		else
		{
			channelSize = 8 + loadedFrame.rows[0].effects[selectedChannel].cEffect.size() * 4;
			if (channels[selectedChannel].hasVoiceColumns)
				channelSize += 5;
		}

		if (selectedPart >= channelSize)
		{
			selectedPart -= channelSize;
			selectedChannel++;
			if (selectedChannel >= loadedFrame.rows[0].effects.size())
			{
				return { -1, -1 }; // Outside of the frame.
			}
		}
		else
		{
			inChannel = true;
			if (channels[selectedChannel].compressed)
				selectedPart = -2; // Represents selecting whole channel.
		}
	}

	return { float(selectedChannel), float(selectedPart) };
}


/*
size_t memory_write_callback(void* pUserData, const void* pData, size_t bytesToWrite)
{
	void* data2 = pUserData;

	void* newData = realloc(data2, bytesToWrite);

	data2 = newData;
	//buffer->capacity = newCapacity;

	memcpy((unsigned char*)data2, pData, bytesToWrite);
	//memcpy((unsigned char*)buffer->data + buffer->cursor, pData, bytesToWrite);

	//MemoryBuffer* buffer = (MemoryBuffer*)pUserData;
	return bytesToWrite;
}*/


ma_result onWrite(ma_encoder *pEncoder, const void* pBufferIn, size_t bytesToWrite, unsigned long long *pBytesWritten)
{
	


	std::vector <char>* pStream = reinterpret_cast<std::vector <char>*>(pEncoder->pUserData);


	//size_t bytesActuallyWritten = pStream->write(static_cast<const char*>(pBufferIn), bytesToWrite);

	std::memcpy(pStream, static_cast<const char*>(pBufferIn), bytesToWrite);

	*pBytesWritten = static_cast<unsigned long long>(bytesToWrite);



	//MemoryStream* pStream = reinterpret_cast<MemoryStream*>(pEncoder->pUserData);
	// Copy data to buffer
	//memcpy((unsigned char*)pBufferIn, pBytesWritten, bytesToWrite);
	//buffer->cursor += bytesToWrite;
	//buffer->size = buffer->cursor;


	/*
	void* data2 = pUserData;

	void* newData = realloc(data2, bytesToWrite);

	data2 = newData;
	//buffer->capacity = newCapacity;

	memcpy((unsigned char*)data2, pData, bytesToWrite);
	//memcpy((unsigned char*)buffer->data + buffer->cursor, pData, bytesToWrite);

	//MemoryBuffer* buffer = (MemoryBuffer*)pUserData;
	return bytesToWrite;
	*/

	return MA_SUCCESS;
}


ma_result onSeek(ma_encoder* pEncoder, ma_int64 offset, ma_seek_origin origin)
{
	if (pEncoder == nullptr || pEncoder->pUserData == nullptr) {
		return MA_INVALID_ARGS; // Ensure valid arguments.
	}

	std::vector <char>* stream = reinterpret_cast<std::vector <char>*>(pEncoder->pUserData);
	//MemoryStream* stream = reinterpret_cast<MemoryStream*>(pEncoder->pUserData);

	std::vector <char>* pStream = reinterpret_cast<std::vector <char>*>(pEncoder->pUserData);


	
	/*
	void* data2 = pUserData;

	void* newData = realloc(data2, bytesToWrite);

	data2 = newData;
	//buffer->capacity = newCapacity;

	memcpy((unsigned char*)data2, pData, bytesToWrite);
	//memcpy((unsigned char*)buffer->data + buffer->cursor, pData, bytesToWrite);

	//MemoryBuffer* buffer = (MemoryBuffer*)pUserData;
	return bytesToWrite;
	*/

	return MA_SUCCESS;
}




int calculateSongLength() // Returns the song's length in seconds.
{
	float length = 0;
	float calcTempo = loadedSong.startingBPM;

	

	for (int fr = 0; fr < loadedSong.frameSequence.size(); fr++)
	{
		std::vector <int> effectIndex = {};
		std::vector <int>  effectTime = {};

		effectIndex.resize(channels.size());
		effectTime.resize(channels.size());


		for (int ch = 0; ch < channels.size(); ch++)
		{
			effectIndex[ch] = 0;
			effectTime[ch] = 0;

			

			if (effectIndex[ch] < loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects.size())
			{
				effectTime[ch] = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[0];
				effectIndex[ch]++;
			}
			else
				return 0;
		}

		for (int i = 0; i < loadedSong.frames[loadedSong.frameSequence[fr]].rows; i++)
		{
			for (int ch = 0; ch < channels.size(); ch++)
			{
				effectTime[ch] -= 1;

				if (effectTime[ch] < 0) // Read next effect.
				{
					bool readingEffects = true;
					int effectNum = 0;


					while (readingEffects)
					{
						if (effectIndex[ch] < loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects.size())
						{
							int effect = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[effectIndex[ch]];
							effectIndex[ch]++;
							int effectValue = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[effectIndex[ch]];
							effectIndex[ch]++;

							if (effect == 19) // Set tempo.
							{
								calcTempo = effectValue;
								if (calcTempo < 1)
									calcTempo = 1;
							}

							if (effectIndex[ch] < loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects.size())
							{
								effectTime[ch] = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[effectIndex[ch]];
								effectIndex[ch]++;
								if (effectTime[ch] == 255) // Read multiple effects on one beat.
								{
									effectTime[ch] = -1;
									effectNum++;
									//loadedSong.effectChannelIndex[ch]--;
								}
								else
									readingEffects = false;
							}
							else
							{
								effectTime[ch] = 256; // No more notes in this channel in the frame.
								readingEffects = false;
							}
						}
						else
							readingEffects = false;
					}
				}
			}


			// Add row to time.
			length += ((60000.0f / (calcTempo * 4.0f)) * 48.0f / 48000.0f);
		}
	}

	return int(length);
}



int calculateSongPosition() // Returns the song's current position in seconds.
{
	float length = 0;
	float calcTempo = loadedSong.startingBPM;



	for (int fr = 0; fr < loadedSong.currentFrame + 1; fr++)
	{
		std::vector <int> effectIndex = {};
		std::vector <int>  effectTime = {};

		effectIndex.resize(channels.size());
		effectTime.resize(channels.size());


		for (int ch = 0; ch < channels.size(); ch++)
		{
			effectIndex[ch] = 0;
			effectTime[ch] = 0;



			if (effectIndex[ch] < loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects.size())
			{
				effectTime[ch] = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[0];
				effectIndex[ch]++;
			}
			else
				return 0;
		}

		for (int i = 0; i < loadedSong.frames[loadedSong.frameSequence[fr]].rows; i++)
		{
			for (int ch = 0; ch < channels.size(); ch++)
			{
				effectTime[ch] -= 1;

				if (effectTime[ch] < 0) // Read next effect.
				{
					bool readingEffects = true;
					int effectNum = 0;


					while (readingEffects)
					{
						if (effectIndex[ch] < loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects.size())
						{
							int effect = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[effectIndex[ch]];
							effectIndex[ch]++;
							int effectValue = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[effectIndex[ch]];
							effectIndex[ch]++;

							if (effect == 19) // Set tempo.
							{
								calcTempo = effectValue;
								if (calcTempo < 1)
									calcTempo = 1;
							}

							if (effectIndex[ch] < loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects.size())
							{
								effectTime[ch] = loadedSong.frames[loadedSong.frameSequence[fr]].channels[ch].effects[effectIndex[ch]];
								effectIndex[ch]++;
								if (effectTime[ch] == 255) // Read multiple effects on one beat.
								{
									effectTime[ch] = -1;
									effectNum++;
									//loadedSong.effectChannelIndex[ch]--;
								}
								else
									readingEffects = false;
							}
							else
							{
								effectTime[ch] = 256; // No more notes in this channel in the frame.
								readingEffects = false;
							}
						}
						else
							readingEffects = false;
					}
				}
			}


			// Add row to time.
			length += ((60000.0f / (calcTempo * 4.0f)) * 48.0f / 48000.0f);

			if (fr == loadedSong.currentFrame && i >= loadedSong.currentNote)
				i = loadedSong.frames[loadedSong.frameSequence[fr]].rows;
		}
	}

	return int(length);
}






std::vector <std::string> helpPageText =
{
	"WELCOME TO SUPER SOUND!",
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
	"    The sample number is in hexadecimal, and ranges from 00 to FF.",
	"",
	"    After that is the volume value.",
	"    This sets the volume for the note and all future notes until changed again.",
	"    Volume is in decimal, and ranges from 00 to 99.",
	"",
	"    Then is the effect Type.",
	"    This sets the effect for the note and all future notes until changed again.",
	"    Different effects will make different changes to the note or song.",
	"",
	"    To the furthest right is the effect value.",
	"    This specifies a parameter or two for the specified effect.",
	"    What is does depends on the effect.",
	"    The effect value is in decimal, and ranges from 00 to 99.",
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
	"        This effect sets the panning position of the channel audio,",
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
	"#",
	"KEY FUNCTIONS",
	"",
	"",
	"",
	"Certain keys are pressed to preform the actions below.",
	"",
	"    SPACE",
	"",
	"        Start or stop the song.",
	"",
	"    ENTER",
	"",
	"        Return the cursor to the beginning of the song.",
	"",
	"    ESC",
	"",
	"        Close the program.",
	"",
	"    TAB",
	"",
	"        Set the currently selected note to instead stop the sample playing.",
	"",
	"    DELETE",
	"",
	"        Delete the currently selected notes.",
	"",
	"    LEFT SHIFT",
	"",
	"        Copy the currently selected notes.",
	"",
	"    RIGHT SHIFT",
	"",
	"        Paste the currently selected notes.",
	"",
	"",
	"",
	"#",
};