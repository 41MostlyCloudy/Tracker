#pragma once


#include "GlobalVariables.h"

// miniaudio
//----------------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
//----------------------------------------------------------------------------------








// Pitch lookup table
// To get different notes, the pitch of the sample is shifted. Samples should be a 'C4' note by default, and the pitch is scaled around that.
float pitchTable[85] =
{
    0.05, // C1

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



void SetUpAudioEngine();

//void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

//void playback_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);



void StartSample(int channel, int sampleNumber, float pitch);

void StopSample(int channel);

void stepSong();

void LoadSampleData();

void InitializeSample(int channel, int sampleNumber, float pitch);




//ma_engine engine;

//ma_engine_config engineConfig;

// The 4 audio channels for music.
//ma_sound channelAudio[8];




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// New Audio



ma_uint32 read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float* pOutputF32, ma_uint32 frameCount, int channel);
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);


// Decoders
ma_decoder g_pDecoders[8];
bool channelToUinit[8] = { false, false, false, false, false, false, false, false };
bool channelInitialized[8] = { false, false, false, false, false, false, false, false };
//ma_uint64 channelNoteFrames[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
//bool startNoteFlag[8] = { false, false, false, false, false, false, false, false };
ma_decoder_config decoderConfig;


// Device
ma_device_config deviceConfig;
ma_device device;


// Encoder
ma_encoder_config encoderConfig;
ma_encoder encoder;


// Resource Manager
ma_resource_manager_config ManagerConfig;
ma_resource_manager resourceManager;


// Samples
std::vector <ma_resource_manager_data_source> sampleData;


// When a new note is played, the main thread signals the data callback with a variable.
// Then, the data callback stops reading from that decoder (channelInitialized = false) and signals to the main thread.
// Then, the main thread starts the sound and signals the data callback to resume.
bool channelCallbackToStopForNote[8] = { false, false, false, false, false, false, false, false };
bool channelMainToPlayNote[8] = { false, false, false, false, false, false, false, false };


// The speed at which a note stops by fading out.
//float decoderNoteStopSpeed = 1.0f;



ma_uint32 read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float* pOutputF32, ma_uint32 frameCount, int channel)
{   
    //The way mixing works is that we just read into a temporary buffer, then take the contents of that buffer and mix it with the
    //contents of the output buffer by simply adding the samples together. You could also clip the samples to -1..+1, but I'm not
    //doing that in this example.
    ma_result result;
    float temp[4096];
    ma_uint32 tempCapInFrames = ma_countof(temp) / 2;
    ma_uint32 totalFramesRead = 0;

    //std::cout << " start -> ";

    
    

    while (totalFramesRead < frameCount) // While there are frames to read.
    {
        
        ma_uint64 iSample;
        ma_uint64 framesReadThisIteration = 0;
        ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
        ma_uint32 framesToReadThisIteration = tempCapInFrames;


        if (framesToReadThisIteration > totalFramesRemaining) // Clamp the number of frames to read to the number of frames left.
        {
            framesToReadThisIteration = totalFramesRemaining;
        }


        
        result = ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration, &framesReadThisIteration); // Read new frames to temp.
        if (result != MA_SUCCESS || framesReadThisIteration == 0)
        {
            channelToUinit[channel] = true;
            break;
        }
        

        
        float volumeLeft = channels[channel].volume;
        float volumeRight = channels[channel].volume;

        if (channels[channel].effect == 4) // Panning
        {
            float pan = (float(channels[channel].effectX)) / 255.0f;
            volumeLeft *= 1.0f - pan;
            volumeRight *= pan;
        }

        
        
        // Mix the frames together.
        for (iSample = 0; iSample < framesReadThisIteration * 2; iSample += 2) // Add the frames from temp to pOutputF32.
        {
            float newLeft = temp[iSample] * volumeLeft;
            float newRight = temp[iSample + 1] * volumeRight;
            

            

            pOutputF32[totalFramesRead * 2 + iSample] += newLeft;
            pOutputF32[totalFramesRead * 2 + iSample + 1] += newRight;
        }
        


        totalFramesRead += (ma_uint32)framesReadThisIteration; // Increment the number of frames read up.

        if (framesReadThisIteration < (ma_uint32)framesToReadThisIteration) // Break if at the end of the sample.
        {
            channelToUinit[channel] = true;
            break;  // Reached EOF.
        }
        
    }
    //std::cout << " <- end ";

    return totalFramesRead;
}



void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    float* pOutputF32 = (float*)pOutput;

    // This example assumes the device was configured to use ma_format_f32.
    for (int channel = 0; channel < 8; channel++)
    {
        if (channelCallbackToStopForNote[channel]) // Stop the sample to play a new sample.
        {
            channelMainToPlayNote[channel] = true;
        }
        else
        {
            // /////////////////////////////////////////////////////////////////////////////////////////////////////////////// THE CRASH IS HERE
            if (channelInitialized[channel] && !channelToUinit[channel])
            {
                ma_uint32 framesRead = read_and_mix_pcm_frames_f32(&g_pDecoders[channel], pOutputF32, frameCount, channel);
                if (framesRead < frameCount)
                {
                    channelToUinit[channel] = true;
                    break;
                }
            }
        }
    }
    if (playingSong && recordingSong)
    {
        ma_uint64 framesWritten;
        ma_encoder_write_pcm_frames(&encoder, pOutputF32, frameCount, &framesWritten); // Write frames to file if recording.
    }
    

    
    (void)pInput;
    (void)pDevice;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








void SetUpAudioEngine()
{
    // In this example, all decoders need to have the same output format.
    decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 48000);

    /* Create only a single device. The decoders will be mixed together in the callback. In this example the data format needs to be the same as the decoders. */
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 48000;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = NULL;

    ma_device_init(NULL, &deviceConfig, &device);

    // Now we start playback and wait for the audio thread to tell us to stop.
    ma_device_start(&device);
    



    // Set up the encoder
    encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32, 2, 48000);
    


    // Set up the resource manager.
    ManagerConfig = ma_resource_manager_config_init();
    ManagerConfig.decodedFormat = device.playback.format;
    ManagerConfig.decodedChannels = device.playback.channels;
    ManagerConfig.decodedSampleRate = device.sampleRate;
    ma_resource_manager_init(&ManagerConfig, &resourceManager);


    return;
}



void LoadSampleData()
{
    /*
    std::cout << " Samples: " << sampleData.size();

    for (int i = 0; i < sampleData.size(); i++)
    {
        ma_resource_manager_data_source_uninit(&sampleData[i]);
    }

    sampleData.clear();
    sampleData.shrink_to_fit();

    
    
    for (int i = 0; i < loadedSamples.size(); i++)
    {
        ma_resource_manager_data_source newSound;

        std::string fileName = "Samples/" + loadedSamples[i].sampleName;
        const char* name = &fileName[0];

        ma_resource_manager_data_source_init(&resourceManager, name, MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM, NULL, &newSound);

        sampleData.emplace_back(newSound);
    }*/

    return;
}



void StartSample(int channel, int sampleNumber, float pitch)
{
    channelCallbackToStopForNote[channel] = true;
    channels[channel].noteToPlaySample = sampleNumber;
    channels[channel].noteToPlayPitch = pitch;

    return;
}



void InitializeSample(int channel, int sampleNumber, float pitch)
{
    if (sampleNumber >= loadedSamples.size()) // Don't play samples that are not loaded.
        return;

    std::string fileName = "Samples/" + loadedSamples[sampleNumber].sampleName;
    const char* name = &fileName[0];



    pitch = pitchTable[int(pitch)];
    channels[channel].realPitch = pitch;


    decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 48000 / pitch);


    StopSample(channel);


    if (sampleNumber > -1) // Don't play notes with samples below zero. (-1 is used to stop notes.)
    {
        ma_decoder_init_file(name, &decoderConfig, &g_pDecoders[channel]);
        channelInitialized[channel] = true;

        if (channels[channel].volume > 1)
            channels[channel].volume = 1;
        else if (channels[channel].volume < 0)
            channels[channel].volume = 0;
    }


    channelMainToPlayNote[channel] = false;
    channelCallbackToStopForNote[channel] = false;

    return;
}



void StopSample(int channel)
{
    if (channelInitialized[channel]) // Don't uninitialized uninitialized devices.
    {
        channelInitialized[channel] = false;
        ma_decoder_uninit(&g_pDecoders[channel]);
    }
    channelToUinit[channel] = false;
    
    return;
}





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
        // The interface has changed, and must be redrawn.
        drawFrameThisFrame = true;

		loadedSong.currentNote++;

		if (loadedSong.currentNote > 8)
			frameScroll++;

		loadedSong.timeInNote = 0;

        
        //if (loadedSong.currentFrame >= loadedSong.frames.size())
        //    loadedSong.currentFrame = loadedSong.frames.size() - 1;

        //std::cout << "Start -> ";

		if (loadedSong.currentNote >= loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].rows) // Start new row
		{
            //std::cout << " start -> ";

			loadedSong.currentFrame++;
			loadedSong.currentNote = 0;

            if (loadedSong.currentFrame >= loadedSong.frameSequence.size()) // Restart song.
            {
                loadedSong.currentFrame = 0;
                loadedSong.timeInSong = 0;
                if (recordingSong) // Stop recording.
                {
                    ma_encoder_uninit(&encoder);
                    recordingSong = false;
                }
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
            
            //std::cout << " <- end ";

		}

        
        //std::cout << " <- End ";
        

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
                        //ma_sound_seek_to_pcm_frame(&channelAudio[ch], float(effectValue) * 100000.0f * (1000.0f / ma_engine_get_sample_rate(&engine)));
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

                    if (note == 255)
                        StartSample(ch, -1, 0); // Start a blank note to stop the current one.
                    else if (channels[ch].effect != 9) // Don't play delayed notes.
                    {
                        StartSample(ch, instrument, note);


                        if (channels[ch].effect == 10) // Set sample position.
                        {
                            //ma_sound_seek_to_pcm_frame(&channelAudio[ch], channels[ch].effectX * 100000.0f * (1000.0f / ma_engine_get_sample_rate(&engine)));
                        }
                    }

                    
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
                //ma_sound_set_pitch(&channelAudio[ch], pitchTable[int(channels[ch].pitch)]);
            }
            else if (channels[ch].effect == 1) // Cosmic Arp (Down)
            {
                int oldNote = channels[ch].pitch;
                channels[ch].pitch -= channels[ch].effectX;
                channels[ch].effectX = channels[ch].effectX - channels[ch].effectY;
                channels[ch].effectY = -channels[ch].effectY;
                //ma_sound_set_pitch(&channelAudio[ch], pitchTable[int(channels[ch].pitch)]);
            }
            else if (channels[ch].effect == 9) // Delayed note
            {
                channels[ch].effectX--;
                
                if (channels[ch].effectX <= 0)
                {
                    StartSample(ch, channels[ch].instrument, channels[ch].pitch);
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

            //ma_sound_set_pitch(&channelAudio[ch], pitchTable[int(channels[ch].pitch)] + volOffset);
        }
        else if (channels[ch].effect == 3) // Tremolo
        {
            float volOffset = sin((loadedSong.timeInSong / (60000.0f / (loadedSong.bpm * 4.0f))) * 3.14f * 0.125f * channels[ch].effectX) * channels[ch].effectY * 0.1;
            //channels[ch].volume += volOffset;
            //ma_sound_set_volume(&channelAudio[ch], channels[ch].volume + volOffset);
        }
        else if (channels[ch].effect == 4) // Set panning
        {
            float pan = (float(channels[ch].effectX) - 127.0f) / 128.0f;

            //ma_sound_set_pan(&channelAudio[ch], pan);
        }
        else if (channels[ch].effect == 5) // Volume Slide
        {
            float vol = float(channels[ch].effectX - channels[ch].effectY) * 0.0002f * delta;
            channels[ch].volume += vol;

            if (channels[ch].volume < 0)
                channels[ch].volume = 0;
            else if (channels[ch].volume > 1)
                channels[ch].volume = 1;

            //ma_sound_set_volume(&channelAudio[ch], float(channels[ch].volume));
        }
        else if (channels[ch].effect == 6) // Pitch Slide
        {
            float vol = float(channels[ch].effectX - channels[ch].effectY) * 0.001f * delta;
            channels[ch].realPitch += vol;

            //ma_sound_set_pitch(&channelAudio[ch], channels[ch].realPitch);
        }
        else if (channels[ch].effect == 7) // Pitch Slide (fine)
        {
            float vol = float(channels[ch].effectX - channels[ch].effectY) * 0.0001f * delta;
            channels[ch].realPitch += vol;

            //ma_sound_set_pitch(&channelAudio[ch], channels[ch].realPitch);
        }

        if (channels[ch].muted)
            channels[ch].volume = 0;

        if (channelToUinit[ch])
        {
            StopSample(ch);
        }
        if (channelMainToPlayNote[ch]) // Stop the sample to play a new sample.
        {
            InitializeSample(ch, channels[ch].noteToPlaySample, channels[ch].noteToPlayPitch);
        }
    }



	

	if (frameScroll > float(loadedFrame.rows.size()) - 40)
		frameScroll = float(loadedFrame.rows.size()) - 40;


    if (frameScroll < 0)
        frameScroll = 0;

    

    

	return;
}