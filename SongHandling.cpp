#pragma once


#include "GlobalVariables.h"

// miniaudio
//----------------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
//----------------------------------------------------------------------------------




void SetUpAudioEngine();

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void StartSample(int channel, int sampleNumber, float pitch);

void StopSample(int channel);

void stepSong();




ma_engine engine;

ma_result result;

ma_engine_config engineConfig;

// The 4 audio channels for music.
ma_sound channelAudio[8];




// Pitch lookup table
// To get different notes, the pitch of the sample is shifted. Samples should be a 'C4' note by default, and the pitch is scaled around that.
float pitchTable[60] =
{
    0.265,
    0.28,
    0.297,
    0.315,
    0.334,
    0.353,
    0.375,
    0.4,
    0.42,
    0.4454,
    0.472,
    0.5, // C3
    0.53,
    0.56,
    0.595,
    0.63,
    0.667,
    0.707,
    0.75,
    0.794,
    0.84,
    0.89,
    0.944,
    1.0, // C4
    1.06,
    1.122,
    1.19,
    1.26,
    1.335,
    1.414,
    1.5,
    1.587,
    1.68,
    1.78,
    1.888,
    2.0, // C5
    2.119,
    2.245,
    2.378,
    2.52,
    2.67,
    2.828,
    2.997,
    3.1748,
    3.364,
    3.564,
    3.775,
    4.0, // C6
    4.238,
    4.4897,
    4.757,
    5.04,
    5.34,
    5.66,
    5.99,
    6.35,
    6.727,
    7.127,
    7.55,
    8.0 // C7
};


void SetUpAudioEngine()
{
    result = ma_engine_init(NULL, &engine);

    engineConfig = ma_engine_config_init();
}


void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    // Reading PCM frames will loop based on what we specified when called ma_data_source_set_looping().
    ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}


void StartSample(int channel, int sampleNumber, float pitch)
{
    if (sampleNumber >= loadedSamples.size()) // Don't play samples that are not loaded.
        return;

    std::string fileName = "Samples/" + loadedSamples[sampleNumber].sampleName;

    const char* name = &fileName[0];


    ma_sound_uninit(&channelAudio[channel]);
    ma_sound_init_from_file(&engine, name, MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &channelAudio[channel]);
    ma_sound_start(&channelAudio[channel]);


    //ma_sound_set_fade_in_milliseconds(&channelAudio[channel], 0, 1, 20);
    pitch = pitchTable[int(pitch)];

    ma_sound_set_pitch(&channelAudio[channel], pitch);
    // Pith shift by interpolating pitch every frame.
    //ma_sound_set_pan(&channelAudio[channel], 1);
    //ma_sound_set_looping(&channels[channel], true);
    //ma_sound_set_volume(&channelAudio[channel], songPlaying.channels[channel].channelVolume);

    ma_sound_set_volume(&channelAudio[channel], channels[channel].volume);
    return;
}


void StopSample(int channel)
{
    //ma_sound_set_fade_in_milliseconds(&channelAudio[channel], 1, 0, 800);
    ma_sound_stop(&channelAudio[channel]);
    return;
}


/*
void SetBackgroundVolume(float volume)
{
    ma_sound_set_volume(&bgMusic, volume);
}*/


void stepSong()
{
	// 1B = 1/BPM;

	// 1B = 60/BPS;

	// 1B = 60,000/BPMS;

	// 1B = 1ms
	// 1000D / 1s


	loadedSong.timeInNote += delta;

	

	if (loadedSong.timeInNote > (60000.0f / (loadedSong.bpm * 4.0f)))
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

            // Reset frame values.
            loadedSong.currentNote = 0;
            frameScroll = 0.0f;
            for (int ch = 0; ch < 8; ch++)
            {
                loadedSong.noteChannelIndex[ch] = 0;
                loadedSong.volumeChannelIndex[ch] = 0;
                loadedSong.effectChannelIndex[ch] = 0;
                loadedSong.toNextChannelNote[ch] = 0;
                loadedSong.toNextChannelVolume[ch] = 0;
                loadedSong.toNextChannelEffect[ch] = 0;
            }
		}


        // Read note data an display notes.
        for (int ch = 0; ch < 8; ch++)
        {
            

            // Find distance to first note, volume and effect.
            int noteIndex = loadedSong.noteChannelIndex[ch];
            int volumeIndex = loadedSong.volumeChannelIndex[ch];
            int effectIndex = loadedSong.effectChannelIndex[ch];

            if (noteIndex == 0)
            {
                loadedSong.toNextChannelNote[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[0];
                loadedSong.noteChannelIndex[ch]++;
                noteIndex++;
                loadedSong.toNextChannelVolume[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes[0];
                loadedSong.volumeChannelIndex[ch]++;
                volumeIndex++;
                loadedSong.toNextChannelEffect[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects[0];
                loadedSong.effectChannelIndex[ch]++;
                effectIndex++;
            }


            loadedSong.toNextChannelNote[ch]--;
            loadedSong.toNextChannelVolume[ch]--;
            loadedSong.toNextChannelEffect[ch]--;

            
            if (loadedSong.toNextChannelNote[ch] < 0) // Read next note.
            {
                if (noteIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes.size())
                {
                    int note = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[noteIndex];
                    loadedSong.noteChannelIndex[ch]++;
                    noteIndex++;
                    int instrument = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[noteIndex];
                    loadedSong.noteChannelIndex[ch]++;
                    noteIndex++;

                    if (note == 255)
                        StopSample(ch);
                    else
                        StartSample(ch, instrument, note);

                    // Set distance to next note.
                    if (noteIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes.size())
                    {
                        loadedSong.toNextChannelNote[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[noteIndex];
                        loadedSong.noteChannelIndex[ch]++;
                    }
                    else
                        loadedSong.toNextChannelNote[ch] = 255; // No more notes in this channel in the frame.
                }
            }
            
            if (loadedSong.toNextChannelVolume[ch] < 0) // Read next volume.
            {
                if (volumeIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes.size())
                {
                    int volume = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes[volumeIndex];
                    loadedSong.volumeChannelIndex[ch]++;
                    volumeIndex++;

                    channels[ch].volume = float(volume) / 255.0f;
                    ma_sound_set_volume(&channelAudio[ch], float(volume) / 255.0f);

                    // Set distance to next note.
                    if (volumeIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes.size())
                    {
                        loadedSong.toNextChannelVolume[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes[volumeIndex];
                        loadedSong.volumeChannelIndex[ch]++;
                    }
                    else
                        loadedSong.toNextChannelVolume[ch] = 255; // No more notes in this channel in the frame.
                }
            }
            
        }

        /*
        for (int ch = 0; ch < 8; ch++) // Play notes.
        {
            if (loadedSong.currentNote % 4 == 0)
                StartSample(ch, 0, 32);
        }*/
	}

	

	if (frameScroll > float(loadedFrame.rows.size()) - 40)
		frameScroll = float(loadedFrame.rows.size()) - 40;


    if (frameScroll < 0)
        frameScroll = 0;


	return;
}