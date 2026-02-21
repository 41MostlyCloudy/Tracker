#pragma once


#include "GlobalVariables.h"






void SetUpAudioEngine();

void StartSample(int channel, int sampleNumber, float pitch, int startFrame);

void StopSample(int channel);

void PlayChannels(float* pOutputF32, ma_uint32 frameCount, ma_uint32 frameOffset);

void updateSongOnBeat();

int updateChannelOnBeat(int ch);

void ResizeDecoders(int newSize); // When a new song is loaded, decoders need to be created or destroyed to match the number of channels.

void RecordSong();

void StartOrStopSong();

void DrawSampleDisplay();

void GenerateAdditiveWave();

void GenerateAllInstrumentWaves();

void ConstructWave(int waveType, float frequencies[16], float framesToWrite, float periodLength, int frequency, float* inputWave);

void DrawSamplePoint(Vector2 drawWavePos);







////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// New Audio

void readModulator(float* pOutputF32, ma_uint64 frameCount, int channel, int waveForm, float* mods[4]);

void applySubtractiveFilters(float* pOutputF32, ma_uint64 frameCount, int channel, float* input, ma_uint32 frameOffset);

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void readWithFMAlgorithm(float* pOutputF32, ma_uint64 frameCount, int channel, ma_uint32 frameOffset);

// New audio reading method:
// -The mixer is given the decoder, output, frameCount, channel and point between frames.

// -It reads the number of frames into a buffer.
// -Then it resamples them.
// -While there is still empty space in the buffer, read more frames.
// -Do this before running any filters or effects.












// Decoders
std::vector <bool> channelInitialized = { false };
ma_decoder_config decoderConfig;


// Device
ma_device_config deviceConfig;
ma_device device;


// Encoder
ma_encoder_config encoderConfig;
ma_encoder encoder;




// The speed at which a note stops by fading out.
//float decoderNoteStopSpeed = 1.0f;



void readModulator(float* pOutputF32, ma_uint64 frameCount, int channel, int op, float* mods[4])
{
    int waveForm = loadedSamples[channels[channel].instrument].operatorWavesToUse[op];


    float notePitch = channels[channel].waveforms[op].pitch;

    if (channels[channel].arpIndex > -1) // Arpeggiate note and find pitch.
    {
        float arpNote = ((channels[channel].arpP[channels[channel].arpIndex]) - 7.75f) * 4.0f;
        arpNote /= loadedSong.edo;
        arpNote = pow(2, arpNote);
        notePitch *= arpNote;
    }

    int lfo = loadedSamples[channels[channel].instrument].waveforms[waveForm].lfo;
    float lfoMultiplier = pow(0.5f, lfo);

    notePitch *= lfoMultiplier;

    notePitch *= loadedSamples[channels[channel].instrument].waveforms[waveForm].fineTuneToC;

    if (notePitch > 12.0f) notePitch = 12.0f;
    else if (notePitch < 0.00001f) notePitch = 0.00001f;


    if (channels[channel].waveforms[op].reverse)
        notePitch *= -1.0f;


    // Glide value.
    float glide = (loadedSamples[channels[channel].instrument].glide) * 20.0f;
    if (glide > 0.0f)
        glide = (1.0f / glide);
    else
        glide = 1.0f;
    float interp = pow(0.99f, glide);



    float resampleIndex = channels[channel].waveforms[op].frameReadPos;

    // Make sure that the frame reading position is inside the sample.
    if (resampleIndex < 0.0f)
        resampleIndex = 0.0f;
    if (resampleIndex >= loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames.size() - 1)
        resampleIndex = loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames.size() - 1;

    for (int i = 0; i < frameCount; i++)
    {
        // Retrigger note.
        if (channels[channel].retrigger > 0)
        {
            float retrigFrameMultiple = 16.0f;
            if (channels[channel].waveforms[op].frameReadPos > channels[channel].jumpPoint + channels[channel].retrigger * retrigFrameMultiple)
            {
                //lastFrameReadPos = int(channels[channel].waveforms[op].frameReadPos);
                //float reaBetweenFrames = channels[channel].waveforms[op].frameReadPos - lastFrameReadPos;
                channels[channel].waveforms[op].frameReadPos = channels[channel].jumpPoint;
                //channels[channel].waveforms[op].frameReadPos = channels[channel].jumpPoint + reaBetweenFrames;
                if (channels[channel].waveforms[op].frameReadPos > loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames.size())
                    channels[channel].waveforms[op].frameReadPos = loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames.size() - 1;

                //nextFrameReadPos = int(channels[channel].waveforms[op].frameReadPos) + 1.0f;
                resampleIndex = channels[channel].waveforms[op].frameReadPos;
            }
        }

        

        // Envelope
        channels[channel].waveforms[op].envelopePos += 0.00004f;
        float envPos = channels[channel].waveforms[op].envelopePos;
        if (envPos < loadedSamples[channels[channel].instrument].waveforms[waveForm].attack) // In attack section.
        {
            channels[channel].waveforms[op].envelopeVol = channels[channel].waveforms[op].envelopePos / loadedSamples[channels[channel].instrument].waveforms[waveForm].attack;
            channels[channel].waveforms[op].envelopeReleasePointVol = channels[channel].waveforms[op].envelopeVol;
        }
        else
        {
            // Sustain/decay.
            envPos -= loadedSamples[channels[channel].instrument].waveforms[waveForm].attack;
            if ((loadedSamples[channels[channel].instrument].waveforms[waveForm].sustainForever && !channels[channel].noteStopped) || envPos < loadedSamples[channels[channel].instrument].waveforms[waveForm].sustain * 2.0f)
            {
                channels[channel].waveforms[op].envelopeVol -= loadedSamples[channels[channel].instrument].waveforms[waveForm].decay * 0.00005f;
                channels[channel].waveforms[op].envelopeReleasePointVol = channels[channel].waveforms[op].envelopeVol;
            }
            else // Release.
            {
                envPos -= loadedSamples[channels[channel].instrument].waveforms[waveForm].sustain * 2.0f;
                if (envPos < loadedSamples[channels[channel].instrument].waveforms[waveForm].release)
                {
                    channels[channel].waveforms[op].envelopeVol = channels[channel].waveforms[op].envelopeReleasePointVol * (1.0f - (envPos / loadedSamples[channels[channel].instrument].waveforms[waveForm].release));
                }
                else
                    channels[channel].waveforms[op].envelopeVol = 0.0f;
            }
        }

        if (channels[channel].waveforms[op].envelopeVol <= 0.0f)
        {
            channels[channel].waveforms[op].envelopeVol = 0.0f;


            //StopSample(channel);
            //channels[channel].toUninitialize = true;
        }


        // Glide to pitch.
        channels[channel].waveforms[op].pitch = channels[channel].waveforms[op].pitch * interp + channels[channel].waveforms[op].glideDest * (1.0f - interp);
        channels[channel].waveforms[op].glideVolume = channels[channel].waveforms[op].glideVolume * interp + channels[channel].waveforms[op].envelopeVol * (1.0f - interp);

        
        notePitch = channels[channel].waveforms[op].pitch;

        if (channels[channel].arpIndex > -1) // Arpeggiate note and find pitch.
        {
            float arpNote = ((channels[channel].arpP[channels[channel].arpIndex]) - 7.75f) * 4.0f;
            arpNote /= loadedSong.edo;
            arpNote = pow(2, arpNote);
            notePitch *= arpNote;
        }

        notePitch *= lfoMultiplier;

        notePitch *= loadedSamples[channels[channel].instrument].waveforms[waveForm].fineTuneToC;

        if (notePitch > 12.0f) notePitch = 12.0f;
        else if (notePitch < 0.00001f) notePitch = 0.00001f;

        if (channels[channel].waveforms[op].reverse)
            notePitch *= -1.0f;


        ////////////////////////////////////////////////////////////////////////////////////////////////////////// Read frame data.
        float sampleRateIndex = int(resampleIndex / channels[channel].sampleRate) * channels[channel].sampleRate;
        int index1 = int(sampleRateIndex);
        int index2 = int(sampleRateIndex) + 1;
        

        float t = resampleIndex - index1;  // Fractional part


        if (index2 >= loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames.size())
            index2 = 0;

        float frameVol = loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames[index1] * (1.0f - t) + loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames[index2] * t;

        float frameVolStereo[2] = { frameVol, frameVol };
        

        resampleIndex += notePitch;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int modNum = 0; modNum < 4; modNum++)
        {
            if (mods[modNum] != nullptr) // Modulate
            {
                for (int stereoOffset = 0; stereoOffset < 2; stereoOffset++)
                {
                    if (loadedSamples[channels[channel].instrument].modulationTypes[modNum] == 0) // FM
                    {
                        resampleIndex += mods[modNum][i * 2 + stereoOffset] * 2.0f;
                        if ((notePitch > 0 && resampleIndex < 0) || (notePitch < 0 && resampleIndex > 0))
                        {
                            notePitch *= -1;
                        }
                    }
                    else if (loadedSamples[channels[channel].instrument].modulationTypes[modNum] == 1) // AM
                    {
                        frameVolStereo[stereoOffset] *= abs(mods[modNum][i * 2 + stereoOffset]);
                    }
                    else if (loadedSamples[channels[channel].instrument].modulationTypes[modNum] == 2) // Apply delay.
                    {
                        float delayIndex = resampleIndex + mods[modNum][i * 2 + stereoOffset] * 100.0f;
                        int sampleLen = loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames.size();
                        while (delayIndex >= sampleLen)
                            delayIndex -= sampleLen;
                        while (delayIndex < 0)
                            delayIndex += sampleLen;

                        ma_uint32 dIndex1 = delayIndex;
                        ma_uint32 dIndex2 = delayIndex + 1;

                        float t2 = delayIndex - dIndex1;  // Fractional part
                        if (dIndex2 >= sampleLen)
                            dIndex2 = 0;

                        frameVolStereo[stereoOffset] += loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames[dIndex1] * (1.0f - t2) + loadedSamples[channels[channel].instrument].waveforms[waveForm].pcmFrames[dIndex2] * t2;
                    }
                    else if (loadedSamples[channels[channel].instrument].modulationTypes[modNum] > 2) // Apply low-pass.
                    {
                        int modType = loadedSamples[channels[channel].instrument].modulationTypes[modNum];

                        // Set initial parameters
                        if (modType == 4)
                            channels[channel].waveforms[op].cutoff = (loadedSamples[channels[channel].instrument].lPResonances[modNum]) * 4000.0f * abs(notePitch);
                        else
                            channels[channel].waveforms[op].cutoff = (abs(mods[modNum][i * 2 + stereoOffset])) * 4000.0f * abs(notePitch);

                        if (channels[channel].waveforms[op].cutoff < 200.0f) channels[channel].waveforms[op].cutoff = 200.0f;
                        else if (channels[channel].waveforms[op].cutoff > 24000.0f) channels[channel].waveforms[op].cutoff = 24000.0f;

                        // Update parameters.
                        // Normalize cutoff frequency (0 to 1, where 1 = Nyquist)
                        channels[channel].waveforms[op].f = 2.0f * channels[channel].waveforms[op].cutoff / 48000.0f;

                        // Clamp to valid range
                        if (channels[channel].waveforms[op].f > 0.99) channels[channel].waveforms[op].f = 0.99;

                        // Calculate feedback amount from resonance
                        float resonance;
                        if (modType == 3)
                            resonance = loadedSamples[channels[channel].instrument].lPResonances[modNum] * 2.0f;
                        else
                            resonance = abs(mods[modNum][i * 2 + stereoOffset]) * 2.0f;

                        channels[channel].waveforms[op].q = 1.0 - channels[channel].waveforms[op].f;
                        channels[channel].waveforms[op].fb = resonance + resonance / (1.0f - channels[channel].waveforms[op].f);

                        // Process audio samples
                        frameVolStereo[stereoOffset] = channels[channel].waveforms[op].process(frameVol);
                    }
                }
            }
        }

        

        for (int j = 0; j < 2; j++)
        {
            frameVolStereo[j] *= channels[channel].waveforms[op].volume;
            frameVolStereo[j] *= channels[channel].waveforms[op].glideVolume;
        }

        //if (frameVol > 1.0f)
        //    frameVol = 1.0f;
        //else if (frameVol < -1.0f)
        //    frameVol = -1.0f;

        

        if (loadedSamples[channels[channel].instrument].waveforms[waveForm].stereo == 0)
        {
            pOutputF32[i * 2] += frameVolStereo[0];
            pOutputF32[i * 2 + 1] += frameVolStereo[1];
        }
        else if (loadedSamples[channels[channel].instrument].waveforms[waveForm].stereo == 1)
        {
            pOutputF32[i * 2] += frameVolStereo[0];
        }
        else
        {
            pOutputF32[i * 2 + 1] += frameVolStereo[1];
        }


        ////////////////////////////////////////////

        // Wave Volume Slide
        if (channels[channel].waveforms[op].volumeSlide != 0.0f)
        {
            channels[channel].waveforms[op].volume += channels[channel].waveforms[op].volumeSlide * 0.0001f * 120.0f;
            if (channels[channel].waveforms[op].volume > 1.0f) channels[channel].waveforms[op].volume = 1.0f;
            else if (channels[channel].waveforms[op].volume < 0.0f) channels[channel].waveforms[op].volume = 0.0f;
        }

        // Pitch Slide
        if (channels[channel].pitchSlide != 0)
        {
            channels[channel].waveforms[op].glideDest += channels[channel].pitchSlide * 0.00001f * 120.0f;
            if (channels[channel].waveforms[op].glideDest > 4)
                channels[channel].waveforms[op].glideDest = 4;
            else if (channels[channel].waveforms[op].glideDest < 0)
                channels[channel].waveforms[op].glideDest = 0;
        }


        /////////////////////////////////////////////// End of sample.
        if (notePitch < 0.0f)
        {
            if (resampleIndex < loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart)
            {
                if (loadedSamples[channels[channel].instrument].waveforms[waveForm].loopType == 0)
                {
                    resampleIndex -= notePitch;
                    StopSample(channel);
                }
                else if (loadedSamples[channels[channel].instrument].waveforms[waveForm].loopType == 1)
                {
                    float loopLen = float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd - loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart);
                    while (resampleIndex < loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart)
                        resampleIndex += loopLen;
                }
                else
                {
                    float loopLen = float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd - loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart);
                    float overLoop = float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart) - resampleIndex;
                    resampleIndex = float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart) + overLoop;
                    while (resampleIndex >= loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd)
                        resampleIndex -= loopLen;

                    channels[channel].waveforms[op].reverse = !channels[channel].waveforms[op].reverse;
                }
            }
        }
        else
        {
            if (resampleIndex >= loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd)
            {
                if (loadedSamples[channels[channel].instrument].waveforms[waveForm].loopType == 0)
                {
                    resampleIndex -= notePitch;
                    StopSample(channel);
                }
                else if (loadedSamples[channels[channel].instrument].waveforms[waveForm].loopType == 1)
                {
                    float loopLen = float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd - loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart);
                    while (resampleIndex >= loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd)
                        resampleIndex -= loopLen;
                }
                else
                {
                    float loopLen = float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd - loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart);
                    float overLoop = resampleIndex - float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd);
                    resampleIndex = float(loadedSamples[channels[channel].instrument].waveforms[waveForm].loopEnd) - overLoop - loopLen;
                    while (resampleIndex < loadedSamples[channels[channel].instrument].waveforms[waveForm].loopStart)
                        resampleIndex += loopLen;

                    channels[channel].waveforms[op].reverse = !channels[channel].waveforms[op].reverse;
                }

            }
        }
    }

    channels[channel].waveforms[op].frameReadPos = resampleIndex;


    return;
}



void applySubtractiveFilters(float* pOutputF32, ma_uint64 frameCount, int channel, float* input, ma_uint32 frameOffset)
{
    float volumeL = 0;
    float volumeR = 0;

    // Panning
    float pan = channels[channel].panValue * 2;
    volumeL += 1.0f + pan;
    volumeR += 1.0f - pan;

    if (volumeL > 1.0f) volumeL = 1.0f;
    else if (volumeL < 0.0f) volumeL = 0.0f;
    if (volumeR > 1.0f) volumeR = 1.0f;
    else if (volumeR < 0.0f) volumeR = 0.0f;


    // Lowpass filter
    float alpha = 1.0f - channels[channel].lowPass;
    float alpha2 = channels[channel].alphaHigh;
    

    for (int i = 0; i < frameCount; i++)
    {
        float noteVolume = channels[channel].volume * loadedSamples[editor.selectedSample].volume;
        if (noteVolume > 1.0f) noteVolume = 1.0f;
        else if (noteVolume < 0.0f) noteVolume = 0.0f;

        float oldInputL = 0;
        float oldInputR = 0;

        float frameL = input[i * 2] * 0.5f;
        float frameR = input[i * 2 + 1] * 0.5f;

        
        // Add the fuzz effect.
        float fuzz = channels[channel].fuzzLevel;
        frameL *= fuzz;
        frameR *= fuzz;


        // Clamp the volume to a normal range.
        if (frameL > 2.0f) frameL = 2.0f;
        else if (frameL < -2.0f) frameL = -2.0f;

        if (frameR > 2.0f) frameR = 2.0f;
        else if (frameR < -2.0f) frameR = -2.0f;

        frameL /= fuzz * 0.5f;
        frameR /= fuzz * 0.5f;
        

        // Lowpass
        frameL = alpha * frameL + (1.0f - alpha) * channels[channel].prevLowPassSampleL;
        channels[channel].prevLowPassSampleL = frameL;
        frameR = alpha * frameR + (1.0f - alpha) * channels[channel].prevLowPassSampleR;
        channels[channel].prevLowPassSampleR = frameR;

        // High-pass
        oldInputL = frameL;
        oldInputR = frameR;
        frameL = alpha2 * (channels[channel].prevHighPassSampleL + frameL - channels[channel].prevHighPassSampleLI);
        channels[channel].prevHighPassSampleLI = oldInputL;
        channels[channel].prevHighPassSampleL = frameL;
        frameR = alpha2 * (channels[channel].prevHighPassSampleR + frameR - channels[channel].prevHighPassSampleRI);
        channels[channel].prevHighPassSampleRI = oldInputR;
        channels[channel].prevHighPassSampleR = frameR;



        // Reverb
        /*
        int combDelays[4] = { 20, 50, 90, 127 };
        float combDecays[4] = { 1.0f, 0.75f, 0.5f, 0.25f };

        float combinedCombL = 0.0f;
        float combinedCombR = 0.0f;

        for (int comb = 0; comb < 4; comb++)
        {
            combinedCombL += channels[channel].pastVolsL[combDelays[comb]] * combDecays[comb];
            combinedCombR += channels[channel].pastVolsR[combDelays[comb]] * combDecays[comb];
        }

        //combinedCombL /= 4.0f;
        //combinedCombR /= 4.0f;

        float decay = 0.8f;

        //channels[channel].reverbCombVolL *= decay;
        //channels[channel].reverbCombVolR *= decay;

        //channels[channel].reverbCombVolL += combinedCombL;
        //channels[channel].reverbCombVolR += combinedCombR;


        for (int oldVols = 127; oldVols > 0; oldVols--)
        {
            channels[channel].pastVolsL[oldVols] = channels[channel].pastVolsL[oldVols - 1];
            channels[channel].pastVolsR[oldVols] = channels[channel].pastVolsR[oldVols - 1];
        }

        channels[channel].pastVolsL[0] = frameL;
        channels[channel].pastVolsR[0] = frameR;


        frameL += combinedCombL;
        frameR += combinedCombR;
        */

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        

        // Write the frames.

        pOutputF32[(i + frameOffset) * 2] += frameL * noteVolume * volumeL;
        pOutputF32[(i + frameOffset) * 2 + 1] += frameR * noteVolume * volumeR;

        // Update the volume visualizer bar.
        channels[channel].averageVolL += abs(pOutputF32[i * 2]);
        channels[channel].averageVolR += abs(pOutputF32[i * 2 + 1]);


        /////////////////////////////////////////////// Update note parameters.

        // Volume Slide
        if (channels[channel].volumeSlide != 0)
        {
            channels[channel].volume += channels[channel].volumeSlide * 0.0001f * 120.0f;
            if (channels[channel].volume > 1) channels[channel].volume = 1;
            else if (channels[channel].volume < 0) channels[channel].volume = 0;
        }

        // Lowpass Slide
        if (channels[channel].lowPassSlide != 0)
        {
            channels[channel].lowPass += channels[channel].lowPassSlide * 0.0001f * 12.0f;
            if (channels[channel].lowPass > 1.0f) channels[channel].lowPass = 1.0f;
            if (channels[channel].lowPass < 0.0f) channels[channel].lowPass = 0.0f;
        }

        // High-pass Slide
        if (channels[channel].highPassSlide != 0)
        {
            channels[channel].highPass += channels[channel].highPassSlide * 0.0001f * 12.0f;
            if (channels[channel].highPass > 1.0f) channels[channel].highPass = 1.0f;
            else if (channels[channel].highPass < 0.0f) channels[channel].highPass = 0.0f;
            float cutoffFreq = channels[channel].highPass * 2000.0f;
            if (cutoffFreq <= 0)
                cutoffFreq = 1.0f;
            float RC = 1.0f / (2.0f * 3.14159265f * cutoffFreq);
            channels[channel].alphaHigh = RC / (RC + (1.0f / 48000.0f));
        }

        // Jump Slide
        if (channels[channel].jumpSlide != 0)
        {
            channels[channel].jumpPoint += channels[channel].jumpSlide * 0.005f;
            if (channels[channel].jumpPoint < 0.0f) channels[channel].jumpPoint = 0.0f;
        }

        // Fuzz Slide
        if (channels[channel].fuzzSlide != 0)
        {
            channels[channel].fuzzLevel += channels[channel].fuzzSlide * 0.0001f;
            if (channels[channel].fuzzLevel < 1.0f) channels[channel].fuzzLevel = 1.0f;
            else if (channels[channel].fuzzLevel > 32.0f) channels[channel].fuzzLevel = 32.0f;
        }

        // Retrigger Slide
        if (channels[channel].retriggerSlide != 0)
        {
            channels[channel].retrigger += channels[channel].retriggerSlide * 0.01f;
            if (channels[channel].retrigger < 1.0f) channels[channel].retrigger = 1.0f;
            else if (channels[channel].retrigger > 255.0f) channels[channel].retrigger = 255.0f;
        }

        // Sample Rate Slide
        if (channels[channel].sampleRateSlide != 0)
        {
            channels[channel].sampleRate += channels[channel].sampleRateSlide * 0.0001f;
            if (channels[channel].sampleRate < 1.0f) channels[channel].sampleRate = 1.0f;
            else if (channels[channel].sampleRate > 32.0f) channels[channel].sampleRate = 32.0f;
        }
    }


    return;
}



void readWithFMAlgorithm(float* pOutputF32, ma_uint64 frameCount, int channel, ma_uint32 frameOffset)
{
    float frames[480 * 2];
    std::fill(frames, frames + 480 * 2, 0.0f);
    float* framePointer = &frames[0];


    int modTypes[4] = { 0,0,0,0 };
    for (int i = 0; i < 4; i++)
        modTypes[i] = loadedSamples[channels[channel].instrument].modulationTypes[i];


    float* nullPointer[4] = { NULL, NULL, NULL, NULL };

    switch (loadedSamples[channels[channel].instrument].algorithmType)
    {

    case 1:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer);
        readModulator(framePointer, frameCount, channel, 1, nullPointer);
        break;
    }

    case 2:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float* modPointer[4] = {&mod[0], NULL, NULL, NULL};

        readModulator(modPointer[0], frameCount, channel, 1, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);

        break;
    }

    case 3:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer);
        readModulator(framePointer, frameCount, channel, 1, nullPointer);
        readModulator(framePointer, frameCount, channel, 2, nullPointer);
        
        break;
    }

    case 4:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };

        readModulator(modPointer[0], frameCount, channel, 2, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);
        readModulator(framePointer, frameCount, channel, 1, nullPointer);

        break;
    }

    case 5:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], &mod2[0], NULL, NULL };

        readModulator(modPointer[0], frameCount, channel, 1, nullPointer);
        readModulator(modPointer[1], frameCount, channel, 2, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);

        break;
    }

    case 6:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };
        float* modPointer2[4] = { NULL, &mod2[0], NULL, NULL };

        readModulator(modPointer2[1], frameCount, channel, 2, nullPointer);
        readModulator(modPointer[0], frameCount, channel, 1, modPointer2);
        readModulator(framePointer, frameCount, channel, 0, modPointer);

        break;
    }

    case 7:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };
        float* modPointer2[4] = { NULL, &mod[0], NULL, NULL };

        readModulator(modPointer[0], frameCount, channel, 2, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);
        readModulator(framePointer, frameCount, channel, 1, modPointer2);

        break;
    }

    case 8:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float mod3[480 * 2];
        std::fill(mod3, mod3 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };
        float* modPointer2[4] = { NULL, &mod2[0], NULL, NULL };
        float* modPointer3[4] = { NULL, NULL, &mod3[0], NULL };

        readModulator(modPointer3[2], frameCount, channel, 3, nullPointer);
        readModulator(modPointer2[1], frameCount, channel, 2, modPointer3);
        readModulator(modPointer[0], frameCount, channel, 1, modPointer2);
        readModulator(framePointer, frameCount, channel, 0, modPointer);

        break;
    }

    case 9:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float mod3[480 * 2];
        std::fill(mod3, mod3 + 480 * 2, 0.0f);
        float* modPointer[4] = { NULL, &mod[0], &mod2[0], NULL };
        float* modPointer2[4] = { &mod3[0], NULL, NULL, NULL};

        readModulator(modPointer[1], frameCount, channel, 2, nullPointer);
        readModulator(modPointer[2], frameCount, channel, 3, nullPointer);
        readModulator(modPointer2[0], frameCount, channel, 1, modPointer);

        readModulator(framePointer, frameCount, channel, 0, modPointer2);

        break;
    }

    case 10:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float mod3[480 * 2];
        std::fill(mod3, mod3 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], &mod2[0], NULL, NULL };
        float* modPointer2[4] = { NULL, NULL, &mod3[0], NULL };

        readModulator(modPointer2[2], frameCount, channel, 3, nullPointer);
        readModulator(modPointer[0], frameCount, channel, 1, modPointer2);
        readModulator(modPointer[1], frameCount, channel, 2, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);

        break;
    }

    case 11:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float mod3[480 * 2];
        std::fill(mod3, mod3 + 480 * 2, 0.0f);
        float* modPointer[4] = { NULL, NULL, &mod[0], NULL };
        float* modPointer2[4] = { NULL, NULL, NULL, &mod[0] };
        float* modPointer3[4] = { &mod2[0], NULL, NULL, NULL };
        float* modPointer4[4] = { NULL, &mod3[0], NULL, NULL };
        float* modPointer5[4] = { &mod2[0], &mod3[0], NULL, NULL };

        readModulator(modPointer[2], frameCount, channel, 3, nullPointer);
        readModulator(modPointer3[0], frameCount, channel, 1, modPointer);
        readModulator(modPointer4[1], frameCount, channel, 2, modPointer2);
        readModulator(framePointer, frameCount, channel, 0, modPointer5);

        break;
    }

    case 12:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };
        float* modPointer2[4] = { NULL, &mod[0], NULL, NULL };
        float* modPointer3[4] = { NULL, NULL, &mod2[0], NULL };

        readModulator(modPointer3[2], frameCount, channel, 2, nullPointer);
        readModulator(modPointer[0], frameCount, channel, 2, modPointer3);
        readModulator(framePointer, frameCount, channel, 0, modPointer);
        readModulator(framePointer, frameCount, channel, 1, modPointer2);

        break;
    }

    case 13:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };
        float* modPointer2[4] = { NULL, &mod2[0], NULL, NULL };

        readModulator(modPointer2[1], frameCount, channel, 3, nullPointer);
        readModulator(modPointer[0], frameCount, channel, 2, modPointer2);
        readModulator(framePointer, frameCount, channel, 0, modPointer);
        readModulator(framePointer, frameCount, channel, 1, nullPointer);

        break;
    }

    case 14:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float mod3[480 * 2];
        std::fill(mod3, mod3 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], &mod2[0], &mod3[0], NULL };

        readModulator(modPointer[0], frameCount, channel, 1, nullPointer);
        readModulator(modPointer[1], frameCount, channel, 2, nullPointer);
        readModulator(modPointer[2], frameCount, channel, 3, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);

        break;
    }

    case 15:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };
        float* modPointer2[4] = { NULL, &mod2[0], NULL, NULL };

        readModulator(modPointer[0], frameCount, channel, 2, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);

        readModulator(modPointer2[1], frameCount, channel, 3, nullPointer);
        readModulator(framePointer, frameCount, channel, 1, modPointer2);


        break;
    }

    case 16:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };
        float* modPointer2[4] = { NULL, &mod[0], NULL, NULL };
        float* modPointer3[4] = { NULL, NULL, &mod[0], NULL };

        readModulator(modPointer[0], frameCount, channel, 3, nullPointer);

        readModulator(framePointer, frameCount, channel, 0, modPointer);
        readModulator(framePointer, frameCount, channel, 1, modPointer2);
        readModulator(framePointer, frameCount, channel, 2, modPointer3);

        break;
    }

    case 17:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], NULL, NULL, NULL };

        readModulator(modPointer[0], frameCount, channel, 3, nullPointer);

        readModulator(framePointer, frameCount, channel, 0, nullPointer);
        readModulator(framePointer, frameCount, channel, 1, modPointer);
        readModulator(framePointer, frameCount, channel, 2, nullPointer);

        break;
    }

    case 18:
    {
        float mod[480 * 2];
        std::fill(mod, mod + 480 * 2, 0.0f);
        float mod2[480 * 2];
        std::fill(mod2, mod2 + 480 * 2, 0.0f);
        float* modPointer[4] = { &mod[0], &mod2[0], NULL, NULL };

        readModulator(modPointer[0], frameCount, channel, 2, nullPointer);
        readModulator(modPointer[1], frameCount, channel, 3, nullPointer);
        readModulator(framePointer, frameCount, channel, 0, modPointer);
        readModulator(framePointer, frameCount, channel, 1, nullPointer);

        break;
    }

    case 19:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer);
        readModulator(framePointer, frameCount, channel, 1, nullPointer);
        readModulator(framePointer, frameCount, channel, 2, nullPointer);
        readModulator(framePointer, frameCount, channel, 3, nullPointer);

        break;
    }

    default:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer);
        break;
    }
    }

    applySubtractiveFilters(pOutputF32, frameCount, channel, framePointer, frameOffset);
    


    return;
}



void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    std::lock_guard<std::mutex> lock(mtx);

    

    
    float* pOutputF32 = (float*)pOutput;
    std::fill(pOutputF32, pOutputF32 + 960, 0.0f);

    

    if (editor.playingSong)
    {
        ma_uint32 frameOffset = 0;
        ma_uint32 framesToRead = 480;

        ma_uint32 fInOldBeat = loadedSong.timeInNote * 48.0f;

        float elapsedMS = float(frameCount) / 48000.0f * 1000.0f;

        loadedSong.timeInNote += elapsedMS;
        loadedSong.timeInSong += elapsedMS;


        ma_uint32 fInNote = loadedSong.timeInNote * 48.0f;

        ma_uint32 fInBeat = (60000.0f / (loadedSong.bpm * 4.0f)) * 48.0f;

        while (fInNote >= fInBeat)
        {
            float fInThisBeat = fInBeat - fInOldBeat;
            PlayChannels(pOutputF32, fInThisBeat, frameOffset);
            frameOffset += fInThisBeat;
            fInOldBeat = 0.0f;
            fInNote -= fInBeat;
            loadedSong.timeInNote -= fInBeat / 48.0f;
            framesToRead -= fInThisBeat;
            updateSongOnBeat();
        }

        PlayChannels(pOutputF32, framesToRead, frameOffset);
        
    }
    else // if (sampleDisplay.playingSample)
    {
        ma_uint32 frameOffset = 0;
        // For each channel, read out frames of data in 4000 frame groups.
        PlayChannels(pOutputF32, frameCount, frameOffset);
    }


    
    
    (void)pInput;
    (void)pDevice;
}


void PlayChannels(float* pOutputF32, ma_uint32 frameCount, ma_uint32 frameOffset)
{
    for (int channel = 0; channel < loadedSong.numberOfChannels; channel++)
    {
        if (channels[channel].toUninitialize)
        {
            channels[channel].volume = 0;
            StopSample(channel);
        }
        else if (!channels[channel].muted)
        {
            if (loadedSamples[channels[channel].instrument].arpLength >= 0) // Arpeggiate note.
            {
                channels[channel].arpTimer++;

                float arpAmount = loadedSamples[channels[channel].instrument].arpSpeed * 16.0f;

                if (channels[channel].arpTimer > arpAmount)
                {
                    channels[channel].arpTimer = 0;
                    channels[channel].arpIndex++;
                    if (channels[channel].arpIndex > loadedSamples[channels[channel].instrument].arpLength)
                        channels[channel].arpIndex = -1;
                }
            }


            if (channels[channel].frameOffset > 0) // Start offset note.
            {
                channels[channel].frameOffset -= frameCount;
                int framesLeft = -channels[channel].frameOffset;

                if (framesLeft > 0) // Start the new note.
                {
                    if (channelInitialized[channel])
                        readWithFMAlgorithm(pOutputF32, frameCount - framesLeft, channel, frameOffset);
                    frameOffset += frameCount - framesLeft;
                    StartSample(channel, channels[channel].offsetInstrument, channels[channel].offsetNote, channels[channel].offsetPos);
                    readWithFMAlgorithm(pOutputF32, framesLeft, channel, frameOffset);
                }
                else if(channelInitialized[channel])
                    readWithFMAlgorithm(pOutputF32, frameCount, channel, frameOffset);
            }
            /*
            else if (channels[channel].retrigger > 0) // Retrigger note.
            {
                channels[channel].retriggerTimer++;

                int retrigAmount = channels[channel].retrigger;
                
                if (channels[channel].retriggerTimer > retrigAmount)
                {
                    channels[channel].retriggerTimer = 0;
                    StartSample(channel, channels[channel].offsetInstrument, channels[channel].offsetNote, channels[channel].jumpPoint);
                }

                readWithFMAlgorithm(pOutputF32, frameCount, channel, frameOffset);
            }*/
            else if (channelInitialized[channel])
                readWithFMAlgorithm(pOutputF32, frameCount, channel, frameOffset);

            channels[channel].averageVolL *= 0.0625f;
            channels[channel].averageVolR *= 0.0625f;
        }
        else
        {
            channels[channel].averageVolL *= 0.9;
            channels[channel].averageVolR *= 0.9;
        }
    }

    return;
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
    



    return;
}



void StartSample(int channel, int sampleNumber, float pitch, int startFrame)
{
    if (!loadedSamples[sampleNumber].enabled) // Don't play samples that are not loaded.
        return;

    if (sampleNumber < 0) // Don't play notes with samples below zero. (-1 is used to stop notes.)
        return;

    StopSample(channel);

    channels[channel].instrument = sampleNumber;


    // Set pitch.
    pitch -= loadedSong.edo * 4;
    pitch = pow(2, pitch / loadedSong.edo);

    // Reset arpeggiation.
    channels[channel].arpIndex = -1;
    channels[channel].arpTimer = 0.0f;
    for (int i = 0; i < loadedSamples[sampleNumber].arpLength + 1; i++)
    {
        channels[channel].arpP[i] = loadedSamples[sampleNumber].arpPitches[i];
    }



    for (int wave = 0; wave < 4; wave++)
    {
        int opWave = loadedSamples[channels[channel].instrument].operatorWavesToUse[wave];

        if (loadedSamples[sampleNumber].waveforms[opWave].pitchToNote)
        {
            channels[channel].waveforms[wave].glideDest = pitch;

            if (loadedSong.currentNote == 0) // Don't glide on the first note in a frame.
            {
                channels[channel].waveforms[wave].pitch = pitch;
                channels[channel].waveforms[wave].glideVolume = channels[channel].waveforms[wave].envelopeVol;
            }
        }
        else
            channels[channel].waveforms[wave].pitch = 1.0f;


        channels[channel].waveforms[wave].reverse = false;

        // Fill any empty waves.
        if (loadedSamples[sampleNumber].waveforms[wave].pcmFrames.size() == 0)
        {
            loadedSamples[sampleNumber].waveforms[wave].pcmFrames.assign(480 * 2, 0.0f);
            loadedSamples[sampleNumber].waveforms[wave].loopStart = 0.0f;
            loadedSamples[sampleNumber].waveforms[wave].loopEnd = 480 * 2;
        }


        // Set frame reading position.
        if (!loadedSamples[sampleNumber].waveforms[opWave].continueNote || (loadedSong.currentNote == 0 && editor.playingSong))
        {
            if (channels[channel].waveforms[wave].reverse)
                channels[channel].waveforms[wave].frameReadPos = loadedSamples[sampleNumber].waveforms[opWave].pcmFrames.size() - 1;
            else
                channels[channel].waveforms[wave].frameReadPos = 0;


            if (channels[channel].waveforms[wave].reverse)
                channels[channel].waveforms[wave].frameReadPos -= startFrame;
            else
                channels[channel].waveforms[wave].frameReadPos += startFrame;


            // Make sure that the sample reading position is inside the sample.
            while (channels[channel].waveforms[wave].frameReadPos < 0)
                channels[channel].waveforms[wave].frameReadPos += loadedSamples[channels[channel].instrument].waveforms[opWave].pcmFrames.size();
            while (channels[channel].waveforms[wave].frameReadPos >= loadedSamples[channels[channel].instrument].waveforms[opWave].pcmFrames.size())
                channels[channel].waveforms[wave].frameReadPos -= loadedSamples[channels[channel].instrument].waveforms[opWave].pcmFrames.size();
        }

        // Set envelope position.
        channels[channel].waveforms[wave].envelopePos = 0.0f;
        channels[channel].waveforms[wave].envelopeVol = 1.0f;
        channels[channel].waveforms[wave].envelopeReleasePointVol = 1.0f; // If releasing from the start of the note, use full volume.
        channels[channel].noteStopped = false;
    }


    

    channelInitialized[channel] = true;

    if (channels[channel].volume > 1)
        channels[channel].volume = 1;
    else if (channels[channel].volume < 0)
        channels[channel].volume = 0;


    // Set the high-pass filter.
    float cutoffFreq = channels[channel].highPass * 2000.0f;
    if (cutoffFreq <= 0)
        cutoffFreq = 1.0f;
    float RC = 1.0f / (2.0f * 3.14159265f * cutoffFreq);
    channels[channel].alphaHigh = RC / (RC + (1.0f / 48000.0f));


    return;
}



void StopSample(int channel)
{
    if (channelInitialized[channel]) // Don't uninitialized uninitialized devices.
    {
        channelInitialized[channel] = false;
        //ma_decoder_uninit(&g_pDecoders[channel]);
    }

    channels[channel].toUninitialize = false;

    sampleDisplay.playingSample = false; // This flag is used to stop the sample played in the sample menu if any sample is stopped.



    //channels[channel].highPass = 100.0f; // High-pass test.


    
    return;
}








void ResizeDecoders(int newSize) // When a new song is loaded, decoders need to be created or destroyed to match the number of channels.
{
    // Set the number of channels.
    for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
        StopSample(ch);

    // Resize sound objects
    //g_pDecoders.resize(newSize);
    channelInitialized.resize(newSize);
    //decoderAudioData.resize(newSize);
    for (int i = loadedSong.numberOfChannels; i < newSize; i++)
        channelInitialized[i] = false;

    // Resize frame
    for (int i = 0; i < loadedSong.frames.size(); i++)
    {
        loadedSong.frames[i].channels.resize(newSize); // Resize frame to match the number of channels.
        for (int j = loadedSong.numberOfChannels; j < newSize; j++)
        {
            loadedSong.frames[i].channels[j].notes = { loadedSong.frames[i].rows };
            loadedSong.frames[i].channels[j].volumes = { loadedSong.frames[i].rows };
            loadedSong.frames[i].channels[j].voiceSamples = { loadedSong.frames[i].rows };
            loadedSong.frames[i].channels[j].effects = { loadedSong.frames[i].rows };
        }
    }
    for (int i = 0; i < loadedFrame.rows.size(); i++)
    {
        loadedFrame.rows[i].note.resize(newSize);
        loadedFrame.rows[i].instrument.resize(newSize);
        loadedFrame.rows[i].volume.resize(newSize);
        loadedFrame.rows[i].voiceSamples.resize(newSize);
        loadedFrame.rows[i].effects.resize(newSize);

        for (int j = loadedSong.numberOfChannels; j < newSize; j++)
        {
            loadedFrame.rows[i].note[j] = -1;
            loadedFrame.rows[i].instrument[j] = -1;
            loadedFrame.rows[i].volume[j] = -1;
            loadedFrame.rows[i].voiceSamples[j] = { 44, 44, 44, 44, 44 };
        }
    }

    // Resize song objects
    loadedSong.noteChannelIndex.resize(newSize);
    loadedSong.volumeChannelIndex.resize(newSize);
    loadedSong.effectChannelIndex.resize(newSize);
    loadedSong.toNextChannelNote.resize(newSize);
    loadedSong.toNextChannelVolume.resize(newSize);
    loadedSong.toNextChannelEffect.resize(newSize);

    for (int i = 0; i < newSize; i++)
    {
        loadedSong.noteChannelIndex[i] = 0;
        loadedSong.volumeChannelIndex[i] = 0;
        loadedSong.effectChannelIndex[i] = 0;
        loadedSong.toNextChannelNote[i] = 0;
        loadedSong.toNextChannelVolume[i] = 0;
        loadedSong.toNextChannelEffect[i] = 0;
    }


    channels.resize(newSize);


    loadedSong.unsavedChanges = false;

    loadedSong.numberOfChannels = newSize;
    gui.drawFrameThisFrame = true;

    return;
}







void updateSongOnBeat()
{
    // The interface has changed, and must be redrawn.
    gui.drawFrameThisFrame = true;

    loadedSong.currentNote++;



    int frameToJumpTo = -1; // The frame to jump to if jumping to a frame this cycle. This allows all effects to be played before a frame jump happens.


    if (loadedSong.currentNote >= loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].rows) // Start new frame.
    {

        loadedSong.currentFrame++;
        loadedSong.currentNote = 0;

        if (loadedSong.currentFrame >= loadedSong.frameSequence.size()) // Restart song.
        {
            loadedSong.currentFrame = 0;
            loadedSong.timeInSong = 0;
            if (editor.recordingSong) // Stop recording.
            {
                editor.recordingSong = false;
            }
        }


        loadCurrentFrame();



        for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
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
    for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
    {
        int newFrame = updateChannelOnBeat(ch);
        if (newFrame != -1)
            frameToJumpTo = newFrame;
    }

    /*
    for (int ch = 0; ch < 8; ch++) // Play notes.
    {
        if (loadedSong.currentNote % 4 == 0)
            StartSample(ch, 0, 32);
    }*/

    if (frameToJumpTo > -1)
    {
        if (frameToJumpTo >= loadedSong.frameSequence.size())
            frameToJumpTo = loadedSong.frameSequence.size() - 1;
        loadedSong.currentFrame = frameToJumpTo;
        loadedSong.currentNote = 0;

        loadCurrentFrame();

        // Reset frame values.
        loadedSong.timeInNote = 0;
        loadedSong.timeInSong = 0;
        gui.frameScroll.y = 0.0f;


        for (int ch2 = 0; ch2 < loadedSong.numberOfChannels; ch2++)
        {
            loadedSong.noteChannelIndex[ch2] = 0;
            loadedSong.volumeChannelIndex[ch2] = 0;
            loadedSong.effectChannelIndex[ch2] = 0;
            loadedSong.toNextChannelNote[ch2] = 0;
            loadedSong.toNextChannelVolume[ch2] = 0;
            loadedSong.toNextChannelEffect[ch2] = 0;
        }
    }

    return;
}


int updateChannelOnBeat(int ch)
{
    int frameToJumpTo = -1;

    bool restartFrame = false;

    // Find distance to first note, volume and effect.
    int noteIndex = loadedSong.noteChannelIndex[ch];
    int volumeIndex = loadedSong.volumeChannelIndex[ch];
    int effectIndex = loadedSong.effectChannelIndex[ch];

    if (loadedSong.noteChannelIndex[ch] == 0) // Initial "to next note" at the start of each frame.
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

            channels[ch].volume = float(volume) / 255.0f;


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

    bool readingEffects = true;
    int effectNum = 0;
    ma_uint64 sampleStartOffset = 0;

    while (readingEffects)
    {
        effectIndex = loadedSong.effectChannelIndex[ch];
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


                if (effect == 21) // Set Volume.
                    channels[ch].volume = float(effectValue) / 255.0f;
                else if (effect == 121) // Decrease Volume.
                    channels[ch].volumeSlide = float(effectValue) / -25500.0f;
                else if (effect == 221) // Increase Volume.
                    channels[ch].volumeSlide = float(effectValue) / 25500.0f;
                else if (effect == 15) // Set pitch.
                {
                    int pitchNote = int(effectValue) % loadedSong.edo;

                    for (int wave = 0; wave < 4; wave++)
                    {
                        if (loadedSamples[channels[ch].instrument].waveforms[wave].pitchToNote)
                        {
                            channels[ch].waveforms[wave].glideDest = pow(2, float(effectValue - loadedSong.edo * 4) / int(loadedSong.edo));
                            channels[ch].waveforms[wave].note = pitchNote;
                        }
                    }
                }
                else if (effect == 115) // Decrease pitch.
                    channels[ch].pitchSlide = float(effectValue) / -1000.0f;
                else if (effect == 215) // Increase pitch.
                    channels[ch].pitchSlide = float(effectValue) / 1000.0f;
                else if (effect == 11) // Set low-pass.
                    channels[ch].lowPass = float(effectValue) / 255.0f;
                else if (effect == 111) // Decrease low-pass.
                    channels[ch].lowPassSlide = float(effectValue) / -255.0f;
                else if (effect == 211) // Increase low-pass.
                    channels[ch].lowPassSlide = float(effectValue) / 255.0f;
                else if (effect == 7) // Set high-pass.
                {
                    channels[ch].highPass = float(effectValue) / 255.0f;
                    float cutoffFreq = channels[ch].highPass * 2000.0f;
                    if (cutoffFreq <= 0)
                        cutoffFreq = 1.0f;
                    float RC = 1.0f / (2.0f * 3.14159265f * cutoffFreq);
                    channels[ch].alphaHigh = RC / (RC + (1.0f / 48000.0f));
                }
                else if (effect == 107) // Decrease high-pass.
                    channels[ch].highPassSlide = float(effectValue) / -255.0f;
                else if (effect == 207) // Increase high-pass.
                    channels[ch].highPassSlide = float(effectValue) / 255.0f;
                else if (effect == 18) // Set pan.
                    channels[ch].panValue = (float(effectValue) / 127.0f) - 1.0f;
                else if (effect == 5) // Jump to frame.
                    frameToJumpTo = effectValue;
                else if (effect == 19) // Set tempo.
                {
                    loadedSong.bpm = effectValue;
                    if (loadedSong.bpm < 1)
                        loadedSong.bpm = 1;
                }
                else if (effect == 12) // Toggle mute
                {
                    if (effectValue > 0)
                        channels[ch].muted = true;
                    else
                        channels[ch].muted = false;
                }
                else if (effect == 9) // Sample jump
                {
                    if (effectValue < loadedSamples[channels[ch].instrument].jumpPoints.size())
                    {
                        effectValue = loadedSamples[channels[ch].instrument].jumpPoints.size() - effectValue - 1;

                        for (int wave = 0; wave < 4; wave++)
                        {
                            channels[ch].waveforms[wave].frameReadPos = loadedSamples[channels[ch].instrument].jumpPoints[effectValue];
                            sampleStartOffset = channels[ch].waveforms[wave].frameReadPos;
                            channels[ch].jumpPoint = sampleStartOffset;
                        }
                    }
                }
                else if (effect == 109) // Decrease jump
                    channels[ch].jumpSlide = -effectValue;
                else if (effect == 209) // Increase jump
                    channels[ch].jumpSlide = effectValue;
                else if (effect == 14) // Set operator volume. (1st param = operator, 2nd param = volume)
                {
                    int p1 = effectValue / 16;
                    if (p1 > 3)
                        p1 = 3;
                    int p2 = effectValue % 16;
                    channels[ch].waveforms[p1].volume = float(p2) / 15.0f;
                }
                else if (effect == 114) // Decrease operator volume.
                {
                    int p1 = effectValue / 16;
                    if (p1 > 3)
                        p1 = 3;
                    int p2 = effectValue % 16;
                    channels[ch].waveforms[p1].volumeSlide = float(p2) / -1700.0f;
                }
                else if (effect == 214) // Increase operator volume.
                {
                    int p1 = effectValue / 16;
                    if (p1 > 3)
                        p1 = 3;
                    int p2 = effectValue % 16;
                    channels[ch].waveforms[p1].volumeSlide = float(p2) / 1700.0f;
                }
                else if (effect == 3) // Delay note.
                {
                    float fInBeat = (60000.0f / (loadedSong.bpm * 4.0f)) * 48.0f;
                    channels[ch].frameOffset = fInBeat * (float(effectValue) / 255.0f);
                }
                else if (effect == 17) // Retrigger.
                {
                    channels[ch].retrigger = float(effectValue);
                }
                else if (effect == 117) // Decrease retrigger.
                {
                    channels[ch].retriggerSlide = float(effectValue) / -100.0f;
                }
                else if (effect == 217) // Increase retrigger.
                {
                    channels[ch].retriggerSlide = float(effectValue) / 100.0f;
                }
                else if (effect == 100) // Decrease arp value.
                {
                    int p1 = effectValue / 16; // Note
                    p1--;
                    if (p1 > loadedSamples[channels[ch].instrument].arpLength)
                        p1 = loadedSamples[channels[ch].instrument].arpLength;
                    int p2 = effectValue % 16; // Value

                    if (p1 < 0)
                    {
                        for (int wave = 0; wave < 4; wave++)
                        {
                            if (loadedSamples[channels[ch].instrument].waveforms[wave].pitchToNote)
                            {
                                int pitchNote = channels[ch].waveforms[wave].note - float(p2);
                                channels[ch].waveforms[wave].glideDest = pow(2, float(pitchNote - loadedSong.edo * 4) / int(loadedSong.edo));
                                channels[ch].waveforms[wave].note = pitchNote;
                            }
                        }

                    }
                    else
                        channels[ch].arpP[p1] -= float(p2) * 0.25f;
                }
                else if (effect == 200) // Increase arp value.
                {
                    int p1 = effectValue / 16; // Note
                    p1--;
                    if (p1 > loadedSamples[channels[ch].instrument].arpLength)
                        p1 = loadedSamples[channels[ch].instrument].arpLength;
                    int p2 = effectValue % 16; // Value

                    if (p1 < 0)
                    {
                        for (int wave = 0; wave < 4; wave++)
                        {
                            if (loadedSamples[channels[ch].instrument].waveforms[wave].pitchToNote)
                            {
                                int pitchNote = channels[ch].waveforms[wave].note + float(p2);
                                channels[ch].waveforms[wave].glideDest = pow(2, float(pitchNote - loadedSong.edo * 4) / int(loadedSong.edo));
                                channels[ch].waveforms[wave].note = pitchNote;
                            }
                        }
                    }
                    else
                        channels[ch].arpP[p1] += float(p2) * 0.25f;
                }
                else if (effect == 2) // Channel jump to row.
                {
                    restartFrame = true;
                }
                if (effect == 25) // Set fuzz level.
                    channels[ch].fuzzLevel = float(effectValue + 1) / 8.0f;
                else if (effect == 125) // Decrease fuzz level.
                    channels[ch].fuzzSlide = float(effectValue) / -100.0f;
                else if (effect == 225) // Increase fuzz level.
                    channels[ch].fuzzSlide = float(effectValue) / 100.0f;
                if (effect == 1) // Set sample rate.
                    channels[ch].sampleRate = float(effectValue + 1) / 8.0f;
                else if (effect == 101) // Decrease sample rate.
                    channels[ch].sampleRateSlide = float(effectValue) / -100.0f;
                else if (effect == 201) // Increase sample rate.
                    channels[ch].sampleRateSlide = float(effectValue) / 100.0f;







                if (effectIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects.size())
                {
                    loadedSong.toNextChannelEffect[ch] = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].effects[effectIndex];
                    loadedSong.effectChannelIndex[ch]++;
                    if (loadedSong.toNextChannelEffect[ch] == 255) // Read multiple effects on one beat.
                    {
                        loadedSong.toNextChannelEffect[ch] = -1;
                        effectNum++;
                        //loadedSong.effectChannelIndex[ch]--;
                    }
                    else
                        readingEffects = false;
                }
                else
                {
                    loadedSong.toNextChannelEffect[ch] = 256; // No more notes in this channel in the frame.
                    readingEffects = false;
                }
            }
            else
                readingEffects = false;
        }
        else
            readingEffects = false;
    }

    if (loadedSong.toNextChannelNote[ch] < 0) // Read next note.
    {
        if (noteIndex < loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes.size())
        {
            int note = loadedSong.frames[loadedSong.frameSequence[loadedSong.currentFrame]].channels[ch].notes[noteIndex];
            loadedSong.noteChannelIndex[ch]++;
            noteIndex++;
            //channels[ch].pitch = note;

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

            if (note == 255) // Stop note.
            {
                channels[ch].noteStopped = true;
                for (int wave = 0; wave <4; wave++)
                    channels[ch].waveforms[wave].envelopePos = loadedSamples[channels[ch].instrument].waveforms[wave].attack + loadedSamples[channels[ch].instrument].waveforms[wave].sustain * 2.0f;
            }
            else
            {
                channels[ch].offsetNote = note;
                channels[ch].offsetInstrument = instrument;
                channels[ch].offsetPos = sampleStartOffset;
                if (channels[ch].frameOffset <= 0) // Don't play delayed notes.
                {
                    for (int wave = 0; wave < 4; wave++)
                    {
                        channels[ch].waveforms[wave].note = note;
                    }

                    StartSample(ch, instrument, note, sampleStartOffset);
                }
            }
        }
    }

    if (restartFrame)
    {
        loadedSong.noteChannelIndex[ch] = 0;
        loadedSong.volumeChannelIndex[ch] = 0;
        loadedSong.effectChannelIndex[ch] = 0;
        loadedSong.toNextChannelNote[ch] = 0;
        loadedSong.toNextChannelVolume[ch] = 0;
        loadedSong.toNextChannelEffect[ch] = 0;

        updateChannelOnBeat(ch);
    }

    return frameToJumpTo;
}



void RecordSong()
{
    if (editor.playingSong)
        StartOrStopSong();
    saveCurrentFrame();
    while (loadedSong.frameSequence[loadedSong.currentFrame] >= loadedSong.frames.size()) // Create a new frame when changed to one not yet used.
    {
        Frame newFrame;
        newFrame.channels.resize(loadedSong.numberOfChannels);
        newFrame.beatsPerMeasure = loadedFrame.beatsPerMeasure;
        loadedSong.frames.emplace_back(newFrame);
    }
    loadedSong.currentFrame = 0;
    loadCurrentFrame();


    StartOrStopSong();


    std::string fileName = "C:/" + fileNavigator.currentFilePath.std::filesystem::path::string() + "/" + loadedSong.songName + ".wav";
    const char* name = &fileName[0];
    ma_encoder_init_file(name, &encoderConfig, &encoder);

    

    editor.recordingSong = true;

    while (editor.recordingSong)
    {
        float frameCount = 480;

        float pOut[960] = { 0.0f };
        float* pOutputF32 = (float*)pOut;
        std::fill(pOutputF32, pOutputF32 + 960, 0.0f);


        ma_uint32 frameOffset = 0;
        ma_uint32 framesToRead = 480;

        float fInOldBeat = loadedSong.timeInNote * 48.0f;

        float elapsedMS = float(frameCount) / 48000.0f * 1000.0f;

        loadedSong.timeInNote += elapsedMS;
        loadedSong.timeInSong += elapsedMS;


        float fInNote = loadedSong.timeInNote * 48.0f;

        float fInBeat = (60000.0f / (loadedSong.bpm * 4.0f)) * 48.0f;

        while (fInNote >= fInBeat)
        {
            float fInThisBeat = fInBeat - fInOldBeat;
            PlayChannels(pOutputF32, fInThisBeat, frameOffset);
            frameOffset += fInThisBeat;
            fInOldBeat = 0.0f;
            fInNote -= fInBeat;
            loadedSong.timeInNote -= fInBeat / 48.0f;
            framesToRead -= fInThisBeat;
            updateSongOnBeat();
        }

        PlayChannels(pOutputF32, framesToRead, frameOffset);


        ma_uint64 framesWritten;
        ma_encoder_write_pcm_frames(&encoder, pOutputF32, 480, &framesWritten); // Write frames to file if recording.


    }

    
    
    StartOrStopSong();

    //ma_device_start(&device);

    ma_encoder_uninit(&encoder);

    

    return;
}




void StartOrStopSong()
{


    /*
    if (editor.recordingSong && !editor.playingSong) // Set up recording.
    {
        std::string fileName = "Export/" + loadedSong.songName + ".wav";
        const char* name = &fileName[0];
        ma_encoder_init_file(name, &encoderConfig, &encoder);
    }*/

    editor.playingSong = !editor.playingSong;
    loadedSong.bpm = loadedSong.startingBPM;


    saveCurrentFrame();

    for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
    {
        channels[ch].toUninitialize = true;
    }

    if (editor.playingSong) // Restart the frame when playing the song.
    {
        loadedSong.currentNote = 0;
        loadedSong.timeInNote = (60000.0f / (loadedSong.bpm * 4.0f));
        loadedSong.timeInSong = 0.0f;
        for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
        {
            StopSample(ch);
            loadedSong.noteChannelIndex[ch] = 0;
            loadedSong.volumeChannelIndex[ch] = 0;
            loadedSong.effectChannelIndex[ch] = 0;
            loadedSong.toNextChannelNote[ch] = 0;
            loadedSong.toNextChannelVolume[ch] = 0;
            loadedSong.toNextChannelEffect[ch] = 0;



            for (int wave = 0; wave < 4; wave++)
            {
                channels[ch].waveforms[wave].glideDest = 0.0f;
                channels[ch].waveforms[wave].glideVolume = 1.0f;
                channels[ch].waveforms[wave].note = 0;
                channels[ch].waveforms[wave].volume = 1.0f;
                channels[ch].waveforms[wave].volumeSlide = 0.0f;
            }


            channels[ch].panValue = 0.0f;
            channels[ch].panSlide = 0.0f;

            channels[ch].pitchSlide = 0.0f;

            channels[ch].volume = 1.0f;
            channels[ch].volumeSlide = 0.0f;

            channels[ch].highPass = 0.0f;
            channels[ch].highPassSlide = 0.0f;

            float cutoffFreq = channels[ch].highPass * 2000.0f;
            if (cutoffFreq <= 0)
                cutoffFreq = 1.0f;
            float RC = 1.0f / (2.0f * 3.14159265f * cutoffFreq);
            channels[ch].alphaHigh = RC / (RC + (1.0f / 48000.0f));

            channels[ch].frameOffset = 0;

            channels[ch].lowPass = 0.0f;
            channels[ch].lowPassSlide = 0.0f;

            channels[ch].retrigger = 0.0f;
            channels[ch].retriggerSlide = 0.0f;

            channels[ch].jumpPoint = -1.0f;
            channels[ch].jumpSlide = 0.0f;

            channels[ch].fuzzLevel = 1.0f;
            channels[ch].fuzzSlide = 0.0f;

            channels[ch].sampleRate = 1.0f;
            channels[ch].sampleRateSlide = 0.0f;

        }


    }
}




void DrawSampleDisplay()
{
    float loopStartPos = 0;
    float loopEndPos = 0;
    float startPos = 0;
    float selectionEndPos = 0;

    if (loadedSamples[editor.selectedSample].enabled)
    {
        loopStartPos = float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart) / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
        loopEndPos = float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd) / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
        startPos = float(sampleDisplay.sampleStartPos) / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
        selectionEndPos = float(sampleDisplay.sampleSelectionEnd) / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
        
        
        if (sampleDisplay.zoomed)
        {
            float zoomStart = float(sampleDisplay.zoomStart) / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
            float zoomEnd = float(sampleDisplay.zoomEnd) / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
            float zoomScale = (zoomEnd - zoomStart);

            loopStartPos -= zoomStart;
            loopStartPos /= zoomScale;
            
            loopEndPos -= zoomStart;
            loopEndPos /= zoomScale;

            startPos -= zoomStart;
            startPos /= zoomScale;

            selectionEndPos -= zoomStart;
            selectionEndPos /= zoomScale;
        }

        loopStartPos *= 528.0f;
        loopEndPos *= 528.0f;
        startPos *= 528.0f;
        selectionEndPos *= 528.0f;


        if (selectionEndPos < startPos)
            std::swap(startPos, selectionEndPos);


        if (loopStartPos > 527)
            loopStartPos = 527;
        else if (loopStartPos < 0)
            loopStartPos = 0;

        if (loopEndPos > 527)
            loopEndPos = 527;
        else if (loopEndPos < 0)
            loopEndPos = 0;

        if (startPos > 527)
            startPos = 527;
        else if (startPos < 0)
            startPos = 0;

        if (selectionEndPos > 527)
            selectionEndPos = 527;
        else if (selectionEndPos < 0)
            selectionEndPos = 0;
    }

    


    for (int x = 0; x < 528; x++)
    {

        float brightness = 1.0f;

        if (loadedSamples[editor.selectedSample].enabled)
        {
            if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType > 0)
            {
                if (x > loopStartPos && x < loopEndPos)
                    brightness *= 0.25f;
            }
        }
        
        int subdivision = int((float(x) / 528.0f) * (sampleDisplay.snapSubdivisions));
        bool lighten = subdivision % 2 == 0;

        for (int y = 0; y < 192 - 16; y++)
        {
            sampleDisplay.pixelData[x + 528 * y] = { 0, 0, 0 };

            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;

            if (x > startPos && x < selectionEndPos)
            {
                if (lighten)
                {
                    sampleDisplay.pixelData[x + 528 * y].r *= 0.8f;
                    sampleDisplay.pixelData[x + 528 * y].g *= 0.8f;
                    sampleDisplay.pixelData[x + 528 * y].b *= 0.8f;
                }
            }
            else
            {
                if (lighten)
                {
                    sampleDisplay.pixelData[x + 528 * y].r *= 0.5f;
                    sampleDisplay.pixelData[x + 528 * y].g *= 0.5f;
                    sampleDisplay.pixelData[x + 528 * y].b *= 0.5f;
                }
                else
                {
                    sampleDisplay.pixelData[x + 528 * y].r *= 0.25f;
                    sampleDisplay.pixelData[x + 528 * y].g *= 0.25f;
                    sampleDisplay.pixelData[x + 528 * y].b *= 0.25f;
                }
            }
        }
        for (int y = 192 - 16; y < 192; y++)
        {
            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;
        }
        for (int y = 192 - 16 + 4; y < 192 - 4; y++)
        {
            sampleDisplay.pixelData[x + 528 * y] = { 100, 100, 0 };
        }
        for (int y = 0; y < 16; y++)
        {
            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;
        }
        for (int y = 4; y < 16 - 4; y++)
        {
            if (loadedSamples[editor.selectedSample].enabled && x > float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart) * (528.0f / loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) && x < float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd) * (528.0f / loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()))
            {
                sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[39] * 255.0f;
                sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[40] * 255.0f;
                sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[41] * 255.0f;
            }
        }
    }

    if (editor.selectedSample < 0 || !loadedSamples[editor.selectedSample].enabled)
        return;



    if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() > 0)
    {

        float lastFrameVal = 0;


        for (int x = 0; x < 528; x++)
        {
            int frameIndex = (float(x) / 528.0f) * (((loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())));

            
            if (sampleDisplay.zoomed)
            {
                float zoomStart = sampleDisplay.zoomStart;
                float zoomEnd = sampleDisplay.zoomEnd;
                float zoomScale = (zoomEnd - zoomStart) / loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

                frameIndex *= zoomScale;
                frameIndex += zoomStart;
            }


            if (frameIndex >= loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())
                break;

            int frameVal = int(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[frameIndex] * 96.0f) + 96.0f;

            if (frameVal > 191)
                frameVal = 191;
            else if (frameVal < 0)
                frameVal = 0;

            

            float brightness = 127.0f;

            if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType > 0)
            {
                if (x > loopStartPos && x < loopEndPos)
                    brightness *= 0.5f;
            }

            if (frameVal > lastFrameVal)
            {
                for (int i = lastFrameVal; i <= frameVal; i++)
                {
                    sampleDisplay.pixelData[x + 528 * i].r = 1.0f * 127.0f;
                    sampleDisplay.pixelData[x + 528 * i].g = 1.0f * 127.0f;
                    sampleDisplay.pixelData[x + 528 * i].b = 1.0f * 127.0f;
                    if (sampleDisplay.drawing)
                    {
                        sampleDisplay.pixelData[x + 528 * i].r = 0.0f;
                        sampleDisplay.pixelData[x + 528 * i].b = 255.0f;
                        sampleDisplay.pixelData[x + 528 * i].b = 0.0f;
                    }
                }
            }
            else
            {
                for (int i = frameVal; i <= lastFrameVal; i++)
                {
                    sampleDisplay.pixelData[x + 528 * i].r = 1.0f * 127.0f;
                    sampleDisplay.pixelData[x + 528 * i].g = 1.0f * 127.0f;
                    sampleDisplay.pixelData[x + 528 * i].b = 1.0f * 127.0f;
                    if (sampleDisplay.drawing)
                    {
                        sampleDisplay.pixelData[x + 528 * i].r = 0.0f;
                        sampleDisplay.pixelData[x + 528 * i].g = 255.0f;
                        sampleDisplay.pixelData[x + 528 * i].b = 0.0f;
                    }
                }
            }

            lastFrameVal = frameVal;
        }


        // Draw jump points.
        for (int i = 0; i < loadedSamples[editor.selectedSample].jumpPoints.size(); i++)
        {
            int samplePos = (loadedSamples[editor.selectedSample].jumpPoints[i]) * (528.0f / loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());

            for (int y = 0; y < 16; y++)
            {
                for (int x = samplePos - abs(y * 0.5); x < samplePos + abs(y * 0.5) + 1; x++)
                {
                    if (x > 0 && x < 528)
                        sampleDisplay.pixelData[x + 528 * (y + 175)] = { 255, 255, 0 };
                }
            }
            for (int y = 0; y < 175; y++)
            {
                sampleDisplay.pixelData[samplePos + 528 * y] = { 255, 255, 0 };
            }
        }

        // Draw loop points.
        if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType > 0)
        {
            for (int y = 0; y < 16; y++)
            {
                for (int x = loopStartPos - abs(y * 0.5); x < loopStartPos + abs(y * 0.5) + 1; x++)
                {
                    if (x > 0 && x < 528)
                        sampleDisplay.pixelData[x + 528 * (15 - y)] = { 0, 255, 255 };
                }
            }
            if (loopStartPos < 528)
            {
                for (int y = 0; y < 175; y++)
                {
                    sampleDisplay.pixelData[int(loopStartPos) + 528 * y] = { 0, 255, 255 };
                }
            }

            for (int y = 0; y < 16; y++)
            {
                for (int x = loopEndPos - abs(y * 0.5); x < loopEndPos + abs(y * 0.5) + 1; x++)
                {
                    if (x > 0 && x < 528)
                        sampleDisplay.pixelData[x + 528 * (15 - y)] = { 0, 255, 255 };
                }
            }
            if (loopEndPos < 528)
            {
                for (int y = 0; y < 175; y++)
                {
                    sampleDisplay.pixelData[int(loopEndPos) + 528 * y] = { 0, 255, 255 };
                }
            }
        }


        // Draw the position marker.
        for (int y = 0; y < 8; y++)
        {
            for (int x = startPos - abs(y * 0.5); x < startPos + abs(y * 0.5) + 1; x++)
            {
                if (x > 0 && x < 528)
                {
                    sampleDisplay.pixelData[x + 528 * (y + 175)] = { 255, 255, 255 };
                    sampleDisplay.pixelData[x + 528 * (15 - y)] = { 255, 255, 255 };
                }
            }

            for (int x = selectionEndPos - abs(y * 0.5); x < selectionEndPos + abs(y * 0.5) + 1; x++)
            {
                if (x > 0 && x < 528)
                {
                    sampleDisplay.pixelData[x + 528 * (y + 175)] = { 255, 255, 255 };
                    sampleDisplay.pixelData[x + 528 * (15 - y)] = { 255, 255, 255 };
                }
            }
        }
        for (int y = 0; y < 192; y++)
        {
            sampleDisplay.pixelData[int(startPos) + 528 * y] = { 255, 255, 255 };
            sampleDisplay.pixelData[int(selectionEndPos) + 528 * y] = { 255, 255, 255 };
        }
    }
    else
    {
        for (int x = 0; x < 528; x++)
        {
            for (int y = 192 - 16; y < 192; y++)
            {
                sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * (y - 192 + 16) * 16;
                sampleDisplay.pixelData[x + 528 * y].g = 0;
                sampleDisplay.pixelData[x + 528 * y].b = 0;
            }
        }
    }

    // Draw center point.
    for (int x = 0; x < 528; x++)
    {
        sampleDisplay.pixelData[x + 528 * 96] = { 255, 255, 0 };
    }

    return;
}



void GenerateAdditiveWave()
{
    if (!loadedSamples[editor.selectedSample].enabled) // Create a new sample.
    {
        loadedSamples[editor.selectedSample].enabled = true;
    }


    if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType == 4) // Generate noise
    {
        loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.clear();
        for (int x = 0; x < 480 * 4 * loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves; x++)
            loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.emplace_back(0.0f);

        loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

        for (int wave = 0; wave < 11; wave++)
        {
            if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[wave] > 0)
            {
                srand(0);

                float noiseVol1 = float((rand() % 256) - 127) / 128.0f;
                float noiseVol2 = float((rand() % 256) - 127) / 128.0f;
                float index = 0.0f;

                for (int x = 0; x < 480 * 4 * loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves; x++)
                {
                    if (int(index + 1.0f / (float(16 - wave))) != int(index))
                    {
                        noiseVol1 = noiseVol2;
                        noiseVol2 = float((rand() % 256) - 127) / 128.0f;
                    }

                    index += 1.0f / (float(16 - wave));
                    float interp = index - int(index);

                    float interpVol = noiseVol1 * (1.0f - interp) + noiseVol2 * interp;

                    interpVol *= (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[wave] + 2.0f) / 16.0f;

                    loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[x] += interpVol;
                }
            }
        }


        //loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.data()

        //float vol = 0;

        //float index = float(x) / (float(16) * 1.0f);
        //float index2 = index + 1;
        //while (index2 >= noiseFrames.size())
        //    index2 -= noiseFrames.size();

        //float interp = index - int(index);

        //float interpVol = noiseFrames[index] * (1.0f - interp) + noiseFrames[index2] * interp;

        //interpVol *= frequencies[frequency] * 0.1f;

        //vol += interpVol;

        //inputWave[x] += vol;
    }
    else
    {
        // Find LCM of waves.
        // Then divide by 6720.0f and multiply by the size of the fundamental frequency to find the number of frames to read.


        float freqSizes[11] = {
            3360.0f, 2520.0f, 1680.0f, 840.0f, 420.0f, 280.0f, 210.0f, 168.0f, 140.0f, 120.0f, 105.0f
        };

        int factor7[11] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 };

        int factor5[11] = { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 };

        int factor3[11] = { 1, 2, 1, 1, 1, 0, 1, 1, 0, 1, 1 };

        int factor2[11] = { 8, 6, 7, 6, 5, 6, 4, 6, 5, 6, 3 };

        // Multiply the wave value of all included waves.
        float fact7 = 0.0f;
        float fact5 = 0.0f;
        float fact3 = 0.0f;
        float fact2 = 0.0f;


        for (int i = 0; i < 11; i++)
        {
            if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[i] > 0)
            {
                if (fact7 < factor7[i]) fact7 = factor7[i];
                if (fact5 < factor5[i]) fact5 = factor5[i];
                if (fact3 < factor3[i]) fact3 = factor3[i];
                if (fact2 < factor2[i]) fact2 = factor2[i];
            }
        }

        float waveTotal = pow(7, fact7) * pow(5, fact5) * pow(3, fact3) * pow(2, fact2);
        waveTotal /= 480.0f;
        waveTotal *= (183.46519f / 16.0f);

        // Make sure the waves are not too small, which can cause problems with the tuninig.
        if (waveTotal < 40.0f) waveTotal *= 2.0f;

        ma_uint64 framesToWrite = ma_uint64(waveTotal);


        

        if (framesToWrite == 0) // If there are no waves, create empty frames.
        {
            loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;
            loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.clear();

            for (int fr = 0; fr < 480; fr++)
                loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.emplace_back(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset);
        }
        else
        {
            float waveScale = waveTotal / float(framesToWrite);
            loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = waveScale;
            loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.clear();
            loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.assign(framesToWrite, 0.0f);

            for (int freq = 0; freq < 11; freq++)
            {
                if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[freq] > 0)
                    ConstructWave(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType, loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies, framesToWrite, 183.46519f * (freqSizes[freq] / 480.0f), freq, loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.data());
            }
        }
    }




    //loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset
    /*
    for (int i = 0; i < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveCyclesToGenerate - 1; i++)
    {
        int sampleLen = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();
        for (int j = 0; j < sampleLen; j++)
        {
            loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.emplace_back(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[j]);
        }
    }*/
    
    loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart = 0;
    loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

    if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].reverseFrames)
        std::reverse(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.begin(), loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.end());
    
    
    DrawSampleDisplay();




    // Reset all of the frame reading positions since the wave may have changed sizes.
    for (int ch = 0; ch < channels.size(); ch++)
    {
        for (int op = 0; op < 4; op++)
            channels[ch].waveforms[op].frameReadPos = 0;
    }


    return;
}



void ConstructWave(int waveType, float frequencies[16], float framesToWrite, float periodLength, int frequency, float* inputWave)
{

    float periodLen = periodLength;



    for (int x = 0; x < framesToWrite; x++)
    {
        if (waveType == -1) // Empty wave
        {
            inputWave[x] += 1.0f;
        }
        else if (waveType == 0) // Sine wave
        {
            float vol = 0;

            float waveLen = float(periodLen * 0.5f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos <= loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle)
                vol += sin(float(x) * 2.0f * 6.28312 / periodLen) * frequencies[frequency] * 0.07f;

            inputWave[x] += vol;
        }
        else if (waveType == 1) // Square wave
        {
            float vol = 0;

            float duty = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle;

            if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].generateFromSines)
            {
                float waveLen = float(periodLen * 0.5f);
                float periodPos = float(x);
                while (periodPos > waveLen)
                    periodPos -= waveLen;
                periodPos /= waveLen;

                if (periodPos <= duty)
                {
                    bool addSign = true;
                    for (int w = 1; w < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves * 2; w += 2)
                    {
                        vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                    }
                }
            }
            else
            {
                float round = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness;

                float waveLen = float(periodLen * 0.5f);

                float periodPos = float(x);

                while (periodPos > waveLen)
                    periodPos -= waveLen;

                periodPos /= waveLen;


                float volume = 1.0f * frequencies[frequency] * 0.07f;


                float c1 = round * (duty * 0.5f);
                float c2 = duty - (round * (duty * 0.5f));
                float c3 = duty;
                float c4 = (round * ((1.0f - duty) * 0.5f)) + duty;
                float c5 = 1.0f - (round * ((1.0f - duty) * 0.5f));

                // Rounded corners
                if (periodPos < c1) // corner
                {
                    float x1 = periodPos - c1;
                    float r1 = c1;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol += yVal;
                }
                else if (periodPos <= c2) // flat
                {
                    vol += volume;
                }
                else if (periodPos < c3) // corner
                {
                    float x1 = periodPos - c2;
                    float r1 = c1;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol += yVal;
                }
                else if (periodPos < c4) // corner
                {
                    float x1 = periodPos - c4;
                    float r1 = c4 - duty;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol -= yVal;
                }
                else if (periodPos <= c5) // flat
                {
                    vol -= volume;
                }
                else // corner
                {
                    float x1 = periodPos - c5;
                    float r1 = 1.0f - c5;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol -= yVal;
                }
            }


            inputWave[x] += vol;
        }
        else if (waveType == 2) // Triangle wave
        {
            float vol = 0;

            float waveLen = float(periodLen * 0.5f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos <= loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle)
            {
                if (periodPos > 0.5)
                    vol += float(periodPos - 0.5f - 0.25f) * frequencies[frequency] * 0.07f * 4.0f;
                else
                    vol += (0.25f - float(periodPos)) * frequencies[frequency] * 0.07f * 4.0f;
            }

            inputWave[x] += vol;
        }
        else if (waveType == 3) // Saw wave
        {
            float vol = 0;

            float duty = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle;

            if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].generateFromSines)
            {
                float waveLen = float(periodLen * 0.5f);
                float periodPos = float(x);
                while (periodPos > waveLen)
                    periodPos -= waveLen;
                periodPos /= waveLen;

                if (periodPos * 1.0f <= duty)
                {
                    bool addSign = true;
                    for (int w = 1; w < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves + 1; w++)
                    {
                        if (addSign)
                            vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.05f) / float(w);
                        else
                            vol -= (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.05f) / float(w);
                        addSign = !addSign;
                    }
                }
            }
            else
            {
                float round = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness * 0.5f;

                float waveLen = float(periodLen * 0.5f);
                float periodPos = float(x);
                while (periodPos > waveLen)
                    periodPos -= waveLen;
                periodPos /= waveLen;

                float amplitude = frequencies[frequency] * 0.2f;

                if (periodPos <= duty)
                {

                    // Approximate y in a Bezier curve.
                    if (periodPos < round)
                    {
                        float t = 0.5f;
                        float interval = 0.5f;
                        for (int j = 0; j < 32; j++)
                        {
                            float estimate = (t * t) * round;
                            interval *= 0.5f;

                            float posInCurve = periodPos;

                            if (estimate > posInCurve)
                                t -= interval;
                            else
                                t += interval;
                        }
                        float roundedVol = -(t * t) * (amplitude * round + amplitude * 0.5f) + t * amplitude + amplitude * 0.5f;


                        vol -= (roundedVol)-amplitude * 0.5f;
                    }
                    else if (periodPos > 1.0f - round)
                    {
                        float t = 0.5f;
                        float interval = 0.5f;
                        for (int j = 0; j < 32; j++)
                        {
                            float estimate = 1.0f - round + 2 * round * t - (t * t) * round;
                            interval *= 0.5f;

                            float posInCurve = periodPos;

                            if (estimate > posInCurve)
                                t -= interval;
                            else
                                t += interval;
                        }
                        float roundedVol = -(t * t) * (amplitude * round + amplitude * 0.5f) + 2 * t * amplitude * round + amplitude - (amplitude * round);

                        vol += (roundedVol)-amplitude * 0.5f;
                    }
                    else
                    {
                        vol += (periodPos * amplitude) - amplitude * 0.5f;
                    }
                }
            }

            inputWave[x] += vol;
        }
        else if (waveType == 5) // Bell
        {
            float vol = 0;

            float duty = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle;

            float waveLen = float(periodLen * 1.0f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos * 2.0f <= duty)
            {
                bool addSign = true;
                for (int w = 1; w < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves * 4; w += 4)
                {
                    if (addSign)
                        vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                    else
                        vol -= (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                    addSign = !addSign;
                }
            }

            inputWave[x] += vol;
        }
        else if (waveType == 6) // Organ
        {
            float vol = 0;

            float duty = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle;

            float waveLen = float(periodLen * 1.0f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos * 2.0f <= duty)
            {
                bool addSign = true;
                for (int w = 1; w < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves * 6; w += 2)
                {
                    if (addSign)
                    {
                        vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                        vol -= abs(cos(periodPos * float(w) * 6.283f * 2.0f) * frequencies[frequency] * 0.07f) / float(w);
                    }
                    else
                    {
                        vol -= (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                        vol += abs(cos(periodPos * float(w) * 6.283f * 2.0f) * frequencies[frequency] * 0.07f) / float(w);
                    }
                    addSign = !addSign;
                }
            }

            inputWave[x] += vol;
        }

        inputWave[x] = float((int(inputWave[x] * 128.0f)) / 128.0f) + loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset;
    }


    return;
}



void GenerateAllInstrumentWaves()
{
    sampleDisplay.drawing = false; // Stop sample drawing.
    sampleDisplay.zoomed = false; // Reset zoom.
    for (int wave = 0; wave < 4; wave++)
    {
        sampleDisplay.selectedOperator = wave;
        GenerateAdditiveWave();
    }
    sampleDisplay.selectedOperator = 0;
    DrawSampleDisplay();

    return;
}


void DrawSamplePoint(Vector2 drawWavePos)
{
    if (drawWavePos.x < 0.0f)
        drawWavePos.x = 0.0f;
    else if (drawWavePos.x > loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() - 1)
        drawWavePos.x = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() - 1;
    if (drawWavePos.y < -1.0f)
        drawWavePos.y = -1.0f;
    else if (drawWavePos.y > 1.0f)
        drawWavePos.y = 1.0f;

    loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[drawWavePos.x] = drawWavePos.y;
    loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[int(drawWavePos.x)] = int(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[int(drawWavePos.x)] * 128.0f) / 128.0f;
    DrawSampleDisplay();
    return;
}