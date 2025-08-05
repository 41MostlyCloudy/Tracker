#pragma once


#include "GlobalVariables.h"

// miniaudio
//----------------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
//----------------------------------------------------------------------------------




void SetUpAudioEngine();

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void StartSample(int channel, int sampleNumber, float pitch, float time);

void StopSample(int channel);

void stepSong();




ma_engine engine;

ma_result result;

ma_engine_config engineConfig;

// The 4 audio channels for music.
ma_sound channelAudio[8];


///////////////////////////////////////////////////// For recording






// Pitch lookup table
// To get different notes, the pitch of the sample is shifted. Samples should be a 'C4' note by default, and the pitch is scaled around that.
float pitchTable[84] =
{
    0.066,
    0.07,
    0.074,
    0.0787,
    0.0834,
    0.0884,
    0.0936,
    0.0992,
    0.105,
    0.111,
    0.118,
    0.125, // C1

    0.132,
    0.14,
    0.149,
    0.157,
    0.167,
    0.177,
    0.187,
    0.198,
    0.21,
    0.223,
    0.236,
    0.25, // C2

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


void StartSample(int channel, int sampleNumber, float pitch, float time)
{
    if (sampleNumber >= loadedSamples.size()) // Don't play samples that are not loaded.
        return;

    std::string fileName = "Samples/" + loadedSamples[sampleNumber].sampleName;

    const char* name = &fileName[0];


    ma_sound_uninit(&channelAudio[channel]);

    

    ma_sound_init_from_file(&engine, name, MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &channelAudio[channel]);

    //ma_sound_stop(&channelAudio[channel]);

    //ma_sound_set_start_time_in_milliseconds(&channelAudio[channel], float(time) * 100.0f);
    //ma_sound_set_start_time_in_pcm_frames(&channelAudio[channel], float(time) * 10000.0f);
    //ma_sound_set_start_time_in_milliseconds(&channelAudio[channel], time);

    

    ma_sound_start(&channelAudio[channel]);


    //ma_sound_set_fade_in_milliseconds(&channelAudio[channel], 0, 1, 20);
    pitch = pitchTable[int(pitch)];
    channels[channel].realPitch = pitch;

    ma_sound_set_pitch(&channelAudio[channel], pitch);


    
    
    

    // Pith shift by interpolating pitch every frame.
    //ma_sound_set_pan(&channelAudio[channel], 1);
    //ma_sound_set_looping(&channels[channel], true);
    //ma_sound_set_volume(&channelAudio[channel], songPlaying.channels[channel].channelVolume);

    if (channels[channel].volume > 1)
        channels[channel].volume = 1;


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
    loadedSong.timeInTick += delta;
    loadedSong.timeInSong += delta;

	

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
            {
                loadedSong.currentFrame = 0;
                loadedSong.timeInSong = 0;
            }

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

            if (noteIndex == 0) // Initial "to next note" at the start of each frame.
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
                loadedSong.currentNote = 0;
            }


            loadedSong.toNextChannelNote[ch]--;
            loadedSong.toNextChannelVolume[ch]--;
            loadedSong.toNextChannelEffect[ch]--;




            if (loadedSong.toNextChannelVolume[ch] < 0) // Read next volume.
            {
                if (volumeIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes.size())
                {
                    int volume = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes[volumeIndex];
                    loadedSong.volumeChannelIndex[ch]++;
                    volumeIndex++;

                    channels[ch].volume = float(volume) / 64.0f;
                    ma_sound_set_volume(&channelAudio[ch], channels[ch].volume);

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

            if (loadedSong.toNextChannelEffect[ch] < 0) // Read next effect.
            {
                if (effectIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects.size())
                {
                    int effect = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects[effectIndex];
                    loadedSong.effectChannelIndex[ch]++;
                    effectIndex++;
                    int effectValue = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects[effectIndex];
                    loadedSong.effectChannelIndex[ch]++;
                    effectIndex++;


                    channels[ch].effect = effect;
                    if (effect == 0 || effect == 1 || effect == 2 || effect == 3 || effect == 5 || effect == 6 || effect == 7) // 2-parameter effects.
                    {
                        channels[ch].effectX = effectValue / 16;
                        channels[ch].effectY = effectValue % 16;
                    }
                    else if (effect == 8) // Jump to frame.
                    {
                        if (effectValue >= loadedSong.frameSequence.size())
                            effectValue = loadedSong.frameSequence.size() - 1;
                        loadedSong.currentFrame = effectValue;
                        loadedSong.currentNote = 0;

                        loadCurrentFrame();

                        // Reset frame values.
                        loadedSong.timeInNote = 0;
                        loadedSong.timeInTick = 0;
                        loadedSong.timeInSong = 0;
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

                        loadedSong.toNextChannelNote[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[0];
                        loadedSong.noteChannelIndex[ch]++;
                        noteIndex++;
                        loadedSong.toNextChannelVolume[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].volumes[0];
                        loadedSong.volumeChannelIndex[ch]++;
                        volumeIndex++;
                        loadedSong.toNextChannelEffect[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects[0];
                        loadedSong.effectChannelIndex[ch]++;
                        effectIndex++;
                        loadedSong.currentNote = 0;
                    }
                    else if (effect == 10) // Set sample position.
                    {
                        ma_sound_seek_to_pcm_frame(&channelAudio[ch], float(effectValue) * 100000.0f * (1000.0f / ma_engine_get_sample_rate(&engine)));
                        channels[ch].effectX = effectValue;
                    }
                    else if (effect == 11) // Set BPM.
                    {
                        loadedSong.bpm = effectValue;
                    }
                    else if (effect == 12) // Set TPB.
                    {
                        loadedSong.ticksPerBeat = effectValue;
                    }
                    else // 1-parameter effects.
                        channels[ch].effectX = effectValue;


                    if (effectIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects.size())
                    {
                        loadedSong.toNextChannelEffect[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects[effectIndex];
                        loadedSong.effectChannelIndex[ch]++;
                    }
                    else
                        loadedSong.toNextChannelEffect[ch] = 255; // No more notes in this channel in the frame.
                }
            }

            if (loadedSong.toNextChannelNote[ch] < 0) // Read next note.
            {
                if (noteIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes.size())
                {
                    int note = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[noteIndex];
                    loadedSong.noteChannelIndex[ch]++;
                    noteIndex++;
                    channels[ch].pitch = note;

                    int instrument = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[noteIndex];
                    channels[ch].instrument = instrument;
                    loadedSong.noteChannelIndex[ch]++;
                    noteIndex++;

                    if (note == 255)
                        StopSample(ch);
                    else if (channels[ch].effect != 9) // Don't play delayed notes.
                    {
                        StartSample(ch, instrument, note, 0);


                        if (channels[ch].effect == 10) // Set sample position.
                        {
                            ma_sound_seek_to_pcm_frame(&channelAudio[ch], channels[ch].effectX * 100000.0f * (1000.0f / ma_engine_get_sample_rate(&engine)));
                        }
                    }

                    // Set distance to next note.
                    if (noteIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes.size())
                    {
                        float pastNote = loadedSong.toNextChannelNote[ch];
                        loadedSong.toNextChannelNote[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[noteIndex];
                        loadedSong.toNextChannelNote[ch] += pastNote + 1;
                        loadedSong.noteChannelIndex[ch]++;
                    }
                    else
                        loadedSong.toNextChannelNote[ch] = 255; // No more notes in this channel in the frame.
                }
            }




            /*
            channels[ch].soundVolume = 4.0f - ma_sound_get_time_in_milliseconds(&channelAudio[ch]) * 0.01;

            if (channels[ch].soundVolume < 0)
                channels[ch].soundVolume = 0.0f;

            if (channels[ch].soundVolume > 5.0)
                channels[ch].soundVolume = 5.0f;
            */
            //channels[ch].soundVolume = ma_sound_get_data_source(&channelAudio[ch]);
        }

        /*
        for (int ch = 0; ch < 8; ch++) // Play notes.
        {
            if (loadedSong.currentNote % 4 == 0)
                StartSample(ch, 0, 32);
        }*/
	}


    if (loadedSong.timeInTick > (60000.0f / (loadedSong.ticksPerBeat * 4.0f * loadedSong.bpm))) // New tick
    {
        loadedSong.timeInTick = 0;
        for (int ch = 0; ch < 8; ch++)
        {
            if (channels[ch].effect == 0) // Cosmic Arp (Up)
            {
                int oldNote = channels[ch].pitch;
                channels[ch].pitch += channels[ch].effectX;
                channels[ch].effectX = channels[ch].effectY - channels[ch].effectX;
                channels[ch].effectY = -channels[ch].effectY;
                ma_sound_set_pitch(&channelAudio[ch], pitchTable[int(channels[ch].pitch)]);
            }
            else if (channels[ch].effect == 1) // Cosmic Arp (Down)
            {
                int oldNote = channels[ch].pitch;
                channels[ch].pitch -= channels[ch].effectX;
                channels[ch].effectX = channels[ch].effectX - channels[ch].effectY;
                channels[ch].effectY = -channels[ch].effectY;
                ma_sound_set_pitch(&channelAudio[ch], pitchTable[int(channels[ch].pitch)]);
            }
            else if (channels[ch].effect == 9) // Delayed note
            {
                channels[ch].effectX--;
                
                if (channels[ch].effectX <= 0)
                {
                    StartSample(ch, channels[ch].instrument, channels[ch].pitch, 0);
                    channels[ch].effect = -1;
                }
            }
        }
    }


    for (int ch = 0; ch < 8; ch++) // Constant checks
    {
        if (channels[ch].effect == 2) // Vibrato
        {
            float volOffset = sin((loadedSong.timeInSong / (60000.0f / (loadedSong.bpm * 4.0f))) * 3.14f * 0.125f * channels[ch].effectX) * channels[ch].effectY * 0.01;

            ma_sound_set_pitch(&channelAudio[ch], pitchTable[int(channels[ch].pitch)] + volOffset);
        }
        else if (channels[ch].effect == 3) // Vibrato
        {
            float volOffset = sin((loadedSong.timeInSong / (60000.0f / (loadedSong.bpm * 4.0f))) * 3.14f * 0.125f * channels[ch].effectX) * channels[ch].effectY * 0.1;

            ma_sound_set_volume(&channelAudio[ch], channels[ch].volume + volOffset);
        }
        else if (channels[ch].effect == 4) // Set panning
        {
            float pan = (float(channels[ch].effectX) - 127.0f) / 128.0f;

            ma_sound_set_pan(&channelAudio[ch], pan);
        }
        else if (channels[ch].effect == 5) // Volume Slide
        {
            float vol = float(channels[ch].effectX - channels[ch].effectY) * 0.0002f * delta;
            channels[ch].volume += vol;

            if (channels[ch].volume < 0)
                channels[ch].volume = 0;
            else if (channels[ch].volume > 1)
                channels[ch].volume = 1;

            ma_sound_set_volume(&channelAudio[ch], float(channels[ch].volume));
        }
        else if (channels[ch].effect == 6) // Pitch Slide
        {
            float vol = float(channels[ch].effectX - channels[ch].effectY) * 0.001f * delta;
            channels[ch].realPitch += vol;

            ma_sound_set_pitch(&channelAudio[ch], channels[ch].realPitch);
        }
        else if (channels[ch].effect == 7) // Pitch Slide (fine)
        {
            float vol = float(channels[ch].effectX - channels[ch].effectY) * 0.0001f * delta;
            channels[ch].realPitch += vol;

            ma_sound_set_pitch(&channelAudio[ch], channels[ch].realPitch);
        }

        if (channels[ch].muted)
            ma_sound_set_volume(&channelAudio[ch], 0.0);
    }



	

	if (frameScroll > float(loadedFrame.rows.size()) - 40)
		frameScroll = float(loadedFrame.rows.size()) - 40;


    if (frameScroll < 0)
        frameScroll = 0;


	return;
}