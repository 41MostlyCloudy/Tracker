#pragma once


#include "GlobalVariables.h"
#include "PatternHandling.cpp"
#include "SongHandling.cpp"

// For searching file directories. New as of c++ 17.
#include <filesystem>

//#include <cstdlib>






void SaveSong(); // Save the currently loaded song.

void LoadSong(std::string name); // Load the song file with the given name.

void LoadGUIThemes();

void SaveSettings(); // Save the setting for the SuperSound program.

void LoadSettings(); // Load the setting for the SuperSound program.

void SaveInstrumentPreset(int presetNum, Instrument* sample);

void LoadInstrumentPreset(int presetNum, Instrument* sample);





void SaveInstrumentPreset(int presetNum, Instrument* sample)
{
    // Preset file format:
    // 
    // // Sample info:
    // 
    // Sample name is tied to the sample number.
    // Clear jump points
    // Set waveform to false;
    // 
    // (1 byte) opWave1 + opWave2 + opWave3 + opWave4
    // 
    // (1 byte) Glide
    // (1 byte) Algorithm type
    // 
    // (1 byte) Arp speed + Arp length
    // (15 bytes) Arp pitches
    // 
    // For all for modulation paths:
    //      -1 byte: modulation type + modulation resonance
    // 
    // // Wave info: (up to x 4)
    //  (1 byte) WaveType + LoopType
    //  (1 byte) Duty cycle
    //  (1 byte) Smoothness
    //  (1 byte) Offset + LFO
    //  (1 byte) # of sine waves
    //  (1 byte) Attack + Sustain
    //  (1 byte) Decay + Release
    //  (1 byte) Boolean flags
    //  Set loop start to sample start (3 bytes) Loop start
    //  Set loop end to sample end (3 bytes) Loop end
    //  (11 bytes) frequencies

    
    if (!std::filesystem::exists("Presets.bin"))
    {
        std::ofstream newPFile("Presets.bin", std::ios::out | std::ios::binary);

        int fillVar = 0;
        for (int s = 0; s < 192; s++)
        {
            fillVar = 0;
            newPFile.write((char*)&fillVar, 1);
            newPFile.write((char*)&fillVar, 1);
            newPFile.write((char*)&fillVar, 1);
            fillVar = 75;
            newPFile.write((char*)&fillVar, 1);
            fillVar = 7.75f * 4.0f;
            for (int arp = 0; arp < 15; arp++)
                newPFile.write((char*)&fillVar, 1);

            for (int mod = 0; mod < 4; mod++)
                newPFile.write((char*)&fillVar, 1);

            for (int wave = 0; wave < 4; wave++)
            {
                fillVar = 1;
                newPFile.write((char*)&fillVar, 1);
                fillVar = 16;
                newPFile.write((char*)&fillVar, 1);
                fillVar = 0;
                newPFile.write((char*)&fillVar, 1);
                fillVar = 1 * 16.0f;
                newPFile.write((char*)&fillVar, 1);
                fillVar = 1;
                newPFile.write((char*)&fillVar, 1);
                fillVar = 0;
                newPFile.write((char*)&fillVar, 1);
                newPFile.write((char*)&fillVar, 1);
                fillVar = 7.0f;
                newPFile.write((char*)&fillVar, 1);
                for (int i = 0; i < 11; i++)
                {
                    fillVar = 0;
                    newPFile.write((char*)&fillVar, 1);
                }
            }
        }
    }

    


    std::ofstream presetFile("Presets.bin", std::ios::in | std::ios::out | std::ios::binary);


    if (presetFile.is_open())
    {
        // (1 byte) opWave1 + opWave2 + opWave3 + opWave4
        // 
        // (1 byte) Glide
        // (1 byte) Algorithm type
        // 
        // (1 byte) Arp speed + Arp length
        // (15 bytes) Arp pitches
        // 
        // For all for modulation paths:
        //      -1 byte: modulation type + modulation resonance
        // 
        // // Wave info: (up to x 4)
        //  (1 byte) WaveType + LoopType
        //  (1 byte) Duty cycle
        //  (1 byte) Smoothness
        //  (1 byte) Offset + LFO
        //  (1 byte) # of sine waves
        //  (1 byte) Attack + Sustain
        //  (1 byte) Decay + Release
        //  (1 byte) Boolean flags
        //  Set loop start to sample start (3 bytes) Loop start
        //  Set loop end to sample end (3 bytes) Loop end
        //  (11 bytes) frequencies

        int readPos = 99 * presetNum;
        presetFile.seekp(readPos);

        uint8_t opWaves = sample->operatorWavesToUse[3] * 64.0f + sample->operatorWavesToUse[2] * 16.0f + sample->operatorWavesToUse[1] * 4.0f + sample->operatorWavesToUse[0];
        presetFile.write((char*)&opWaves, 1);
        uint8_t writeVar = sample->glide * 16.0f;
        presetFile.write((char*)&writeVar, 1);
        writeVar = sample->algorithmType;
        presetFile.write((char*)&writeVar, 1);

        if (sample->arpSpeed > 0.9375f) sample->arpSpeed = 0.9375f;
        writeVar = int((sample->arpSpeed * 16.0f) * 16.0f) + int(sample->arpLength);

        presetFile.write((char*)&writeVar, 1);
        for (int i = 0; i < 15; i++)
        {
            writeVar = sample->arpPitches[i] * 4.0f;
            presetFile.write((char*)&writeVar, 1);
        }

        for (int mod = 0; mod < 4; mod++)
        {
            if (sample->lPResonances[mod] > 0.9375f) sample->lPResonances[mod] = 0.9375f;
            writeVar = int((sample->modulationTypes[mod]) * 16.0f) + int(sample->lPResonances[mod] * 16.0f);
            presetFile.write((char*)&writeVar, 1);
        }

        for (int wave = 0; wave < 4; wave++)
        {
            writeVar = sample->waveforms[wave].waveType * 16.0f + sample->waveforms[wave].loopType;
            presetFile.write((char*)&writeVar, 1);
            writeVar = int(sample->waveforms[wave].dutyCycle * 16.0f);
            presetFile.write((char*)&writeVar, 1);
            writeVar = int(sample->waveforms[wave].smoothness * 16.0f);
            presetFile.write((char*)&writeVar, 1);
            if (sample->waveforms[wave].offset > 0.9375f) sample->waveforms[wave].offset = 0.9375f;
            writeVar = int((sample->waveforms[wave].offset * 16.0f) * 16.0f) + int(sample->waveforms[wave].lfo);
            presetFile.write((char*)&writeVar, 1);
            writeVar = sample->waveforms[wave].numOfSineWaves;
            presetFile.write((char*)&writeVar, 1);
            if (sample->waveforms[wave].attack > 0.9375f) sample->waveforms[wave].attack = 0.9375f;
            if (sample->waveforms[wave].sustain > 0.9375f) sample->waveforms[wave].sustain = 0.9375f;
            writeVar = int(sample->waveforms[wave].attack * 16.0f) * 16.0f + int(sample->waveforms[wave].sustain * 16.0f);
            presetFile.write((char*)&writeVar, 1);
            if (sample->waveforms[wave].decay > 0.9375f) sample->waveforms[wave].decay = 0.9375f;
            if (sample->waveforms[wave].release > 0.9375f) sample->waveforms[wave].release = 0.9375f;
            writeVar = int(sample->waveforms[wave].decay * 16.0f) * 16.0f + int(sample->waveforms[wave].release * 16.0f);
            presetFile.write((char*)&writeVar, 1);

            // Boolean flags.
            bool stereo1 = false;
            bool stereo2 = false;
            if (sample->waveforms[wave].stereo == 1)
                stereo1 = true;
            else if (sample->waveforms[wave].stereo == 2)
                stereo2 = true;

            writeVar = (int)stereo1 * 64.0f + (int)stereo2 * 32.0f + (int)sample->waveforms[wave].generateFromSines * 16.0f + (int)sample->waveforms[wave].reverseFrames * 8.0f + (int)sample->waveforms[wave].sustainForever * 4.0f + (int)sample->waveforms[wave].pitchToNote * 2.0f + (int)sample->waveforms[wave].continueNote;
            presetFile.write((char*)&writeVar, 1);

            for (int i = 0; i < 11; i++)
            {
                writeVar = sample->waveforms[wave].frequencies[i];
                presetFile.write((char*)&writeVar, 1);
            }
        }
    }


    presetFile.close();


    return;
}



void LoadInstrumentPreset(int presetNum, Instrument* sample)
{

    std::ifstream presetFile("Presets.bin", std::ios::binary | std::ios::in);

    if (presetFile.is_open())
    {

        // (1 byte) opWave1 + opWave2 + opWave3 + opWave4
        // 
        //      (1 byte) Glide
        //      (1 byte) Algorithm type
        // 
        //      (1 byte) Arp speed + Arp length
        //      (15 bytes) Arp pitches
        // 
        // For all for modulation paths:
        //      -1 byte: modulation type + modulation resonance
        // 
        // // Wave info: (up to x 4)
        //  (1 byte) WaveType + LoopType
        //  (1 byte) Duty cycle
        //  (1 byte) Smoothness
        //  (1 byte) Offset + LFO
        //  (1 byte) # of sine waves
        //  (1 byte) Attack + Sustain
        //  (1 byte) Decay + Release
        //  (1 byte) Boolean flags
        //  Set loop start to sample start (3 bytes) Loop start
        //  Set loop end to sample end (3 bytes) Loop end
        //  (15 bytes) frequencies


        int begin = presetFile.tellg();
        presetFile.seekg(0, std::ios::end);
        int end = presetFile.tellg();

        int readPos = 99 * presetNum;

        if (readPos >= end) // The sample is beyond the file length.
        {
            return;
        }

        presetFile.seekg(readPos);



        uint8_t readVar;
        presetFile.read((char*)&readVar, 1);
        float op1 = int(readVar / 64.0f);
        float op2 = int(readVar / 16.0f) - (op1 * 4.0f);
        float op3 = int(readVar / 4.0f) - (op1 * 16.0f + op2 * 4.0f);
        float op4 = int(readVar) - (op1 * 64.0f + op2 * 16.0f + op3 * 4.0f);

        sample->operatorWavesToUse[0] = op4;
        sample->operatorWavesToUse[1] = op3;
        sample->operatorWavesToUse[2] = op2;
        sample->operatorWavesToUse[3] = op1;


        presetFile.read((char*)&readVar, 1);
        sample->glide = float(readVar) / 16.0f;
        presetFile.read((char*)&readVar, 1);
        sample->algorithmType = readVar;

        float var1 = 0.0f;
        float var2 = 0.0f;

        presetFile.read((char*)&readVar, 1);
        var1 = int(readVar / 16.0f);
        var2 = readVar - var1 * 16.0f;
        sample->arpSpeed = var1 / 16.0f;
        sample->arpLength = var2;

        for (int i = 0; i < 15; i++)
        {
            presetFile.read((char*)&readVar, 1);
            sample->arpPitches[i] = float(readVar) * 0.25f;
        }


        for (int mod = 0; mod < 4; mod++)
        {
            presetFile.read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            sample->modulationTypes[mod] = var1;
            sample->lPResonances[mod] = var2 / 16.0f;
        }

        
        for (int wave = 0; wave < 4; wave++)
        {
            presetFile.read((char*)&readVar, 1);
            float var1 = int(readVar / 16.0f);
            float var2 = readVar - var1 * 16.0f;
            sample->waveforms[wave].waveType = var1;
            sample->waveforms[wave].loopType = var2;
            presetFile.read((char*)&readVar, 1);
            sample->waveforms[wave].dutyCycle = float(readVar) / 16.0f;
            presetFile.read((char*)&readVar, 1);
            sample->waveforms[wave].smoothness = float(readVar) / 16.0f;

            presetFile.read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            sample->waveforms[wave].offset = var1 / 16.0f;
            sample->waveforms[wave].lfo = var2;
            presetFile.read((char*)&readVar, 1);
            sample->waveforms[wave].numOfSineWaves = readVar;
            presetFile.read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            sample->waveforms[wave].attack = var1 / 16.0f;
            sample->waveforms[wave].sustain = var2 / 16.0f;
            presetFile.read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            sample->waveforms[wave].decay = var1 / 16.0f;
            sample->waveforms[wave].release = var2 / 16.0f;
            presetFile.read((char*)&readVar, 1);

            float stereo1 = int(readVar / 64.0f);
            float stereo2 = int(readVar / 32.0f) - (stereo1 * 64.0f);
            float varE = int(readVar / 16.0f) - (stereo1 * 64.0f + stereo2 * 32.0f);
            float varD = int(readVar / 8.0f) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f);
            float varC = int(readVar / 4.0f) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f);
            float varB = int(readVar / 2.0f) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f + varC * 4.0f);
            float varA = int(readVar) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f + varC * 4.0f + varB * 2.0f);
            sample->waveforms[wave].generateFromSines = (bool)varE;
            sample->waveforms[wave].reverseFrames = (bool)varD;
            sample->waveforms[wave].sustainForever = (bool)varC;
            sample->waveforms[wave].pitchToNote = (bool)varB;
            sample->waveforms[wave].continueNote = (bool)varA;

            if (stereo2 == 1)
                sample->waveforms[wave].stereo = 2;
            else if (stereo1 == 1)
                sample->waveforms[wave].stereo = 1;
            else
                sample->waveforms[wave].stereo = 0;


            //////////////////////////////////////////////////////
            // Boolean flags.
            //bool stereo1 = false;
            //bool stereo2 = false;
            //if (sample->waveforms[wave].stereo == 1)
            //    stereo1 = true;
            //else if (sample->waveforms[wave].stereo == 2)
            //    stereo2 = true;
            //
            //writeVar = (int)stereo1 * 64.0f + (int)stereo2 * 32.0f + (int)sample->waveforms[wave].generateFromSines * 16.0f + (int)sample->waveforms[wave].reverseFrames * 8.0f + (int)sample->waveforms[wave].sustainForever * 4.0f + (int)sample->waveforms[wave].pitchToNote * 2.0f + (int)sample->waveforms[wave].continueNote;
            //presetFile.write((char*)&writeVar, 1);
            //////////////////////////////////////////////////////

            
            for (int i = 0; i < 11; i++)
            {
                presetFile.read((char*)&readVar, 1);
                sample->waveforms[wave].frequencies[i] = readVar;
            }
        }
        


    }

    // The preset is set to full volume.
    loadedInstruments[editor.selectedSample].volume = 1.0f;



    GenerateAllInstrumentWaves();

    presetFile.close();

    return;
}



void SaveSong() // Save the currently loaded song.
{
    if (!loadedSong.unusedFrames)
    {
        // Check for unused frames.
        std::vector <bool> frameUsed;
        for (int i = 0; i < loadedSong.patterns.size(); i++)
            frameUsed.emplace_back(false);

        for (int i = 0; i < loadedSong.patternSequence.size(); i++)
        {
            frameUsed[loadedSong.patternSequence[i]] = true;
        }


        for (int i = 0; i < frameUsed.size(); i++)
        {
            if (frameUsed[i] == false)
                loadedSong.unusedFrames = true;
        }

        if (loadedSong.unusedFrames)
        {
            windowController.InitializeWindow("Delete Frames", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 20, 16 });
            return;
        }
    }
    else
        loadedSong.unusedFrames = false;

    std::string songFilePath = "C:/" + fileNavigator.currentFilePath.std::filesystem::path::string() + "/";

    

    // See if any songs have the same name.
    if (!loadedSong.overWriteOldSong)
    {
        //std::string pathName = "C:/" + fileNavigator.currentFilePath.std::filesystem::path::string();

        //std::string songName = fileNavigator.fileNames[editor.selectedFile];
        //songName.erase(0, 1);
        
        for (int name = 0; name < fileNavigator.fileNames.size(); name++)
        {
            std::string indexName = fileNavigator.fileNames[name];
            indexName.erase(0, 1);
            if (indexName == loadedSong.songName + ".song")
            {
                windowController.InitializeWindow("Overwrite Song", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 20, 16 });
                return;
            }
        }
    }




    loadedSong.unsavedChanges = false;

    // Song file format.

    // Song Body:
    //      1 byte: Length of artist name.
    //      For each character:
    //          1 byte value.
    // 
    //      1 byte: # Of channels.
    //      For each channel:
    //          1 byte: compressed.
    //          1 byte: voice samples enables.
    //      1 byte: BPM
    //      1 byte: EDO
    //      1 byte: Theme
    //      1 byte: # Of frames in frame sequence.
    //      For each index in the frame sequence:
    //          1 byte: Frame index number.
    //      1 byte: # Of frames in song.
    //      For each frame:
    //          1 byte: # of rows.
    //          1 byte: beats per measure.
    //          For each channel:
    //              1 byte: # of notes.
    //              For each note:
    //                  1 byte: note value.
    //              1 byte: # of volumes.
    //              For each volume:
    //                  1 byte: volume value.
    //              1 byte: # of voice samples.
    //              For each voice sample:
    //                  1 bytes: sample value.
    //              1 byte: # of effects.
    //              For each effect:
    //                  1 byte: note effect.
    //              1 byte: # of voice samples.
    //              For each voice sample:
    //                  1 byte: voice sample.
    


    // For each sample:
    //      1 bytes: enabled.
    // 
    // For each enabled sample:
    //      1 byte: Length of name.
    //      For each character:
    //          1 byte value.
    // 
    //      (1 byte) # of jump points.
    //      For each jump point:
    //           4 bytes: Jump point position.
    // 
    //      (1 byte) Operator waves
    //      (1 byte) Volume
    //      (1 byte) Glide
    //      (1 byte) Algorithm type
    // 
    //      (1 byte) Arp speed + Arp length
    //      (15 bytes) Arp pitches
    // 
    //      For all for modulation paths:
    //           -1 byte: modulation type + modulation resonance
    // 
    //      // Wave info: (For each wave used in the selected modulator algorithm type)
    // 
    //          4 bytes: Number of frames.
    //          For each frame:
    //              1 byte: Frame value.
    // 
    //          (1 byte) waveform + pitch fine-tune
    //          if (waveform):
    //              -(1 byte) Duty cycle
    //              -(1 byte) Smoothness
    //              -(1 byte) # of sine waves
    // 
    //          (1 byte) WaveType + LoopType
    //          (1 byte) Offset + LFO
    //          (1 byte) Attack + Sustain
    //          (1 byte) Decay + Release
    //          (1 byte) Boolean flags
    //          (4 bytes) Loop start
    //          (4 bytes) Loop end
    //          (11 bytes) frequencies
    

    saveCurrentPattern();

    std::ofstream songFile(songFilePath + loadedSong.songName + ".song", std::ios::binary | std::ios::out);


    if (songFile.is_open())
    {
        ////////////////////// SONG
        // 
        // Artist Name
        std::string artistName = loadedSong.artistName;

        uint8_t artistNameNum = artistName.length();
        songFile.write((char*)&artistNameNum, 1);

        for (int j = 0; j < artistName.length(); j++)
        {
            uint8_t artistChar = artistName.at(j);
            songFile.write((char*)&artistChar, 1);
        }

        uint8_t channelCount = int(loadedSong.numberOfChannels); // Channel count
        songFile.write((char*)&channelCount, 1);
        for (int j = 0; j < channelCount; j++)
        {
            uint8_t compressed = 0;
            if (channels[j].compressed)
                compressed = 1;
            songFile.write((char*)&compressed, 1);
        }
        uint8_t bpm = int(loadedSong.startingBPM); // BPM
        songFile.write((char*)&bpm, 1);
        uint8_t edo = int(loadedSong.edo); // EDO
        songFile.write((char*)&edo, 1);
        uint8_t theme = int(gui.uiColorTheme); // Theme
        songFile.write((char*)&theme, 1);


        uint8_t seqSize = loadedSong.patternSequence.size();
        songFile.write((char*)&seqSize, 1);

        for (int i = 0; i < loadedSong.patternSequence.size(); i++) // Frame sequence.
        {
            uint8_t seqId = loadedSong.patternSequence[i];
            songFile.write((char*)&seqId, 1);
        }

        uint8_t frameNum = loadedSong.patterns.size();
        songFile.write((char*)&frameNum, 1);

        for (int i = 0; i < loadedSong.patterns.size(); i++) // Frame.
        {
            uint8_t frameRows = loadedSong.patterns[i].rows;
            songFile.write((char*)&frameRows, 1);

            uint8_t frameMeasureBeats = loadedSong.patterns[i].beatsPerMeasure;
            songFile.write((char*)&frameMeasureBeats, 1);

            for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
            {
                uint8_t noteSize = loadedSong.patterns[i].channels[ch].notes.size();
                songFile.write((char*)&noteSize, 1);
                for (int j = 0; j < noteSize; j++)
                {
                    uint8_t note = loadedSong.patterns[i].channels[ch].notes[j];
                    songFile.write((char*)&note, 1);
                }

                uint8_t volumeSize = loadedSong.patterns[i].channels[ch].volumes.size();
                songFile.write((char*)&volumeSize, 1);
                for (int j = 0; j < volumeSize; j++)
                {
                    uint8_t volume = loadedSong.patterns[i].channels[ch].volumes[j];
                    songFile.write((char*)&volume, 1);
                }

                uint8_t effectSize = loadedSong.patterns[i].channels[ch].effects.size();
                songFile.write((char*)&effectSize, 1);
                for (int j = 0; j < effectSize; j++)
                {
                    uint8_t effect = loadedSong.patterns[i].channels[ch].effects[j];
                    songFile.write((char*)&effect, 1);
                }
            }
        }

        ////////////////////// SAMPLES

        for (int i = 0; i < 256; i++)
        {
            uint8_t enabled = loadedInstruments[i].enabled;
            songFile.write((char*)&enabled, 1);
        }
        
        for (int i = 0; i < 256; i++)
        {
            if (loadedInstruments[i].enabled)
            {
                // Name
                std::string sampleName = loadedInstruments[i].sampleName;
                uint8_t sampleNameNum = sampleName.length();
                songFile.write((char*)&sampleNameNum, 1);

                for (int j = 0; j < sampleName.length(); j++)
                {
                    uint8_t sampleChar = sampleName.at(j);
                    songFile.write((char*)&sampleChar, 1);
                }

                uint8_t jumpPoints = loadedInstruments[i].jumpPoints.size();
                songFile.write((char*)&jumpPoints, 1);
                
                for (int j = 0; j < jumpPoints; j++)
                {
                    int point = loadedInstruments[i].jumpPoints[j];
                    songFile.write((char*)&point, 4);
                }

                uint8_t opWaves = loadedInstruments[i].operatorWavesToUse[3] * 64.0f + loadedInstruments[i].operatorWavesToUse[2] * 16.0f + loadedInstruments[i].operatorWavesToUse[1] * 4.0f + loadedInstruments[i].operatorWavesToUse[0];
                songFile.write((char*)&opWaves, 1);
                uint8_t volume = loadedInstruments[i].volume * 16.0f;
                songFile.write((char*)&volume, 1);
                uint8_t glide = loadedInstruments[i].glide * 16.0f;
                songFile.write((char*)&glide, 1);
                uint8_t algo = loadedInstruments[i].algorithmType;
                songFile.write((char*)&algo, 1);

                // Arp
                if (loadedInstruments[i].arpSpeed > 0.9375f) loadedInstruments[i].arpSpeed = 0.9375f;
                uint8_t arpVar = int((loadedInstruments[i].arpSpeed * 16.0f) * 16.0f) + int(loadedInstruments[i].arpLength);
                songFile.write((char*)&arpVar, 1);

                for (int arp = 0; arp < 15; arp++)
                {
                    arpVar = loadedInstruments[i].arpPitches[arp] * 4.0f;
                    songFile.write((char*)&arpVar, 1);
                }

                // Modulation paths
                for (int mod = 0; mod < 4; mod++)
                {
                    if (loadedInstruments[i].lPResonances[mod] > 0.9375f) loadedInstruments[i].lPResonances[mod] = 0.9375f;
                    uint8_t modPath = int((loadedInstruments[i].modulationTypes[mod]) * 16.0f) + int(loadedInstruments[i].lPResonances[mod] * 16.0f);
                    songFile.write((char*)&modPath, 1);
                }

                bool modsUsed[4] = { false, false, false, false };

                for (int j = 0; j < 4; j++)
                {
                    modsUsed[loadedInstruments[i].operatorWavesToUse[j]] = true;
                }
                

                // For each used sample.
                for (int j = 0; j < 4; j++)
                {
                    if (modsUsed[j])
                    {
                        int frameCount = loadedInstruments[i].waveforms[j].pcmFrames.size();
                        songFile.write((char*)&frameCount, 4);

                        for (int fr = 0; fr < frameCount; fr++)
                        {
                            float scaledFrame = ((loadedInstruments[i].waveforms[j].pcmFrames[fr] * 128.0f) + 128.0f);
                            uint8_t frameVal = uint8_t(scaledFrame);
                            songFile.write((char*)&frameVal, 1);
                        }

                        uint8_t isWaveform = 0;
                        int fineTune = loadedInstruments[i].waveforms[j].getFineTuneLookupIndex();
                        isWaveform = fineTune * 16.0f + loadedInstruments[i].waveforms[j].operatorType;
                        songFile.write((char*)&isWaveform, 1);

                        uint8_t waveVar = 0;

                        if (loadedInstruments[i].waveforms[j].operatorType == 0) // Waveform-specific data.
                        {
                            waveVar = int(loadedInstruments[i].waveforms[j].dutyCycle * 16.0f);
                            songFile.write((char*)&waveVar, 1);
                            waveVar = int(loadedInstruments[i].waveforms[j].smoothness * 16.0f);
                            songFile.write((char*)&waveVar, 1);
                            waveVar = loadedInstruments[i].waveforms[j].numOfSineWaves;
                            songFile.write((char*)&waveVar, 1);
                        }



                        waveVar = loadedInstruments[i].waveforms[j].waveType * 16.0f + loadedInstruments[i].waveforms[j].loopType;
                        songFile.write((char*)&waveVar, 1);
                        if (loadedInstruments[i].waveforms[j].offset > 0.9375f) loadedInstruments[i].waveforms[j].offset = 0.9375f;
                        waveVar = int((loadedInstruments[i].waveforms[j].offset * 16.0f) * 16.0f) + int(loadedInstruments[i].waveforms[j].lfo);
                        songFile.write((char*)&waveVar, 1);
                        if (loadedInstruments[i].waveforms[j].attack > 0.9375f) loadedInstruments[i].waveforms[j].attack = 0.9375f;
                        if (loadedInstruments[i].waveforms[j].sustain > 0.9375f) loadedInstruments[i].waveforms[j].sustain = 0.9375f;
                        waveVar = int(loadedInstruments[i].waveforms[j].attack * 16.0f) * 16.0f + int(loadedInstruments[i].waveforms[j].sustain * 16.0f);
                        songFile.write((char*)&waveVar, 1);
                        if (loadedInstruments[i].waveforms[j].decay > 0.9375f) loadedInstruments[i].waveforms[j].decay = 0.9375f;
                        if (loadedInstruments[i].waveforms[j].release > 0.9375f) loadedInstruments[i].waveforms[j].release = 0.9375f;
                        waveVar = int(loadedInstruments[i].waveforms[j].decay * 16.0f) * 16.0f + int(loadedInstruments[i].waveforms[j].release * 16.0f);
                        songFile.write((char*)&waveVar, 1);



                        // Boolean flags
                        bool stereo1 = false;
                        bool stereo2 = false;
                        if (loadedInstruments[i].waveforms[j].stereo == 1)
                            stereo1 = true;
                        else if (loadedInstruments[i].waveforms[j].stereo == 2)
                            stereo2 = true;

                        waveVar = (int)stereo1 * 64.0f + (int)stereo2 * 32.0f + (int)loadedInstruments[i].waveforms[j].generateFromSines * 16.0f + (int)loadedInstruments[i].waveforms[j].reverseFrames * 8.0f + (int)loadedInstruments[i].waveforms[j].sustainForever * 4.0f + (int)loadedInstruments[i].waveforms[j].pitchToNote * 2.0f + (int)loadedInstruments[i].waveforms[j].continueNote;
                        songFile.write((char*)&waveVar, 1);

                        int loopStart = loadedInstruments[i].waveforms[j].loopStart;
                        songFile.write((char*)&loopStart, 4);
                        int loopEnd = loadedInstruments[i].waveforms[j].loopEnd;
                        songFile.write((char*)&loopEnd, 4);

                        for (int f = 0; f < 11; f++)
                        {
                            waveVar = loadedInstruments[i].waveforms[j].frequencies[f];
                            songFile.write((char*)&waveVar, 1);
                        }
                    }

                }
            }
        }

        


        songFile.close();
    }

    

    return;
}



void LoadSong(std::string name) // Load the song file with the given name.
{
    if (loadedSong.unsavedChanges)
    {
        editor.fileToLoad = name;
        windowController.InitializeWindow("Save and Load", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 20, 16 });
        //promptWind.AskSaveBeforeLoadingSong();
        return;
    }
    

    editor.playingSong = false;
    loadedSong.currentPattern = 0;
    loadedSong.currentNote = 0;

    std::ifstream songFile("C:/" + fileNavigator.currentFilePath.std::filesystem::path::string() + "/" + name, std::ios::binary | std::ios::in);

    name.erase(name.length() - 5, 6);
    loadedSong.songName = name;

    if (songFile.is_open())
    {
    // Song file format.

    // Song Body:
    //      1 byte: Length of artist name.
    //      For each character:
    //          1 byte value.
    // 
    //      1 byte: # Of channels.
    //      For each channel:
    //          1 byte: compressed.
    //          1 byte: voice samples enables.
    //      1 byte: BPM
    //      1 byte: EDO
    //      1 byte: Theme
    //      1 byte: # Of frames in frame sequence.
    //      For each index in the frame sequence:
    //          1 byte: Frame index number.
    //      1 byte: # Of frames in song.
    //      For each frame:
    //          1 byte: # of rows.
    //          1 byte: beats per measure.
    //          For each channel:
    //              1 byte: # of notes.
    //              For each note:
    //                  1 byte: note value.
    //              1 byte: # of volumes.
    //              For each volume:
    //                  1 byte: volume value.
    //              1 byte: # of voice samples.
    //              For each voice sample:
    //                  1 bytes: sample value.
    //              1 byte: # of effects.
    //              For each effect:
    //                  1 byte: note effect.
    //              1 byte: # of voice samples.
    //              For each voice sample:
    //                  1 byte: voice sample.


    // For each sample:
    //      1 bytes: enabled.
    // 
    // For each enabled sample:
    //      1 byte: Length of name.
    //      For each character:
    //          1 byte value.
    // 
    //      (1 byte) # of jump points.
    //      For each jump point:
    //           4 bytes: Jump point position.
    //    
    //      (1 byte) Operator waves
    //      (1 byte) Volume
    //      (1 byte) Glide
    //      (1 byte) Algorithm type
    // 
    //      (1 byte) Arp speed + Arp length
    //      (15 bytes) Arp pitches
    // 
    //      (1 byte) Arp speed + Arp length
    //      (15 bytes) Arp pitches
    // 
    //      For all for modulation paths:
    //           -1 byte: modulation type + modulation resonance
    // 
    //      // Wave info: (For each wave used in the selected modulator algorithm type)
    // 
    //          4 bytes: Number of frames.
    //          For each frame:
    //              1 byte: Frame value.
    // 
    //          (1 byte) waveform + pitch fine-tune
    //          if (waveform):
    //              -(1 byte) Duty cycle
    //              -(1 byte) Smoothness
    //              -(1 byte) # of sine waves
    // 
    //          (1 byte) WaveType + LoopType
    //          (1 byte) Offset + LFO
    //          (1 byte) Attack + Sustain
    //          (1 byte) Decay + Release
    //          (1 byte) Boolean flags
    //          (4 bytes) Loop start
    //          (4 bytes) Loop end
    //          (11 bytes) frequencies


        ////////////////////// SONG

        uint8_t artistNameNum;
        songFile.read((char*)&artistNameNum, 1);

        // Artist name
        std::string artistName = "";
        for (int j = 0; j < artistNameNum; j++)
        {
            uint8_t newChar;
            songFile.read((char*)&newChar, 1);
            artistName = artistName + char(newChar);
        }
        loadedSong.artistName = artistName;

        // Number of Channels
        uint8_t channelCount;
        songFile.read((char*)&channelCount, 1);
        ResizeDecoders(channelCount); // Resize decoder array to match channels.

        for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
        {
            uint8_t compressed;
            songFile.read((char*)&compressed, 1);
            if (compressed == 0) channels[ch].compressed = false;
            else channels[ch].compressed = true;
        }

        uint8_t bpm;
        songFile.read((char*)&bpm, 1);
        loadedSong.startingBPM = bpm;
        loadedSong.bpm = loadedSong.startingBPM;
        uint8_t edo;
        songFile.read((char*)&edo, 1);
        loadedSong.edo = edo;
        uint8_t theme;
        songFile.read((char*)&theme, 1);
        gui.uiColorTheme = theme;

        // Frame sequence
        uint8_t frameSeqNum;
        songFile.read((char*)&frameSeqNum, 1);
        loadedSong.patternSequence.clear();
        loadedSong.patternSequence.resize(frameSeqNum);
        for (int i = 0; i < frameSeqNum; i++)
        {
            songFile.read((char*)&loadedSong.patternSequence[i], 1);
        }



        // Frames
        uint8_t frameNum;
        songFile.read((char*)&frameNum, 1);
        loadedSong.patterns.clear();
        loadedSong.patterns.resize(frameNum);
        for (int i = 0; i < frameNum; i++)
        {
            loadedSong.patterns[i].channels.resize(loadedSong.numberOfChannels); // Resize frame to match the number of channels.

            uint8_t rows;
            songFile.read((char*)&rows, 1);
            loadedSong.patterns[i].rows = rows;

            uint8_t measureBeats;
            songFile.read((char*)&measureBeats, 1);
            loadedSong.patterns[i].beatsPerMeasure = measureBeats;

            for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
            {
                uint8_t noteNum;
                songFile.read((char*)&noteNum, 1);
                loadedSong.patterns[i].channels[ch].notes.clear();
                loadedSong.patterns[i].channels[ch].notes.resize(noteNum);
                for (int j = 0; j < noteNum; j++)
                {
                    uint8_t note;
                    songFile.read((char*)&note, 1);
                    loadedSong.patterns[i].channels[ch].notes[j] = note;
                }

                uint8_t volumeNum;
                songFile.read((char*)&volumeNum, 1);
                loadedSong.patterns[i].channels[ch].volumes.clear();
                loadedSong.patterns[i].channels[ch].volumes.resize(volumeNum);
                for (int j = 0; j < volumeNum; j++)
                {
                    uint8_t volume;
                    songFile.read((char*)&volume, 1);
                    loadedSong.patterns[i].channels[ch].volumes[j] = volume;
                }


                uint8_t effectNum;
                songFile.read((char*)&effectNum, 1);
                loadedSong.patterns[i].channels[ch].effects.clear();
                loadedSong.patterns[i].channels[ch].effects.resize(effectNum);
                for (int j = 0; j < effectNum; j++)
                {
                    uint8_t effect;
                    songFile.read((char*)&effect, 1);
                    loadedSong.patterns[i].channels[ch].effects[j] = effect;
                }
            }
        }


        ////////////////////// SAMPLES

        

        for (int i = 0; i < 256; i++) // Enable/disable samples.
        {
            uint8_t enabled;
            songFile.read((char*)&enabled, 1);
            if (enabled > 0) loadedInstruments[i].enabled = true;
            else loadedInstruments[i].enabled = false;
        }
        
        for (int i = 0; i < 256; i++)
        {
            if (loadedInstruments[i].enabled)
            {
                // Sample name
                uint8_t sampleNameNum;
                songFile.read((char*)&sampleNameNum, 1);

                std::string sampleName = "";
                for (int j = 0; j < sampleNameNum; j++)
                {
                    uint8_t newChar;
                    songFile.read((char*)&newChar, 1);
                    sampleName = sampleName + char(newChar);
                }
                loadedInstruments[i].sampleName = sampleName;


                // Jump points
                uint8_t jumpPoints;
                songFile.read((char*)&jumpPoints, 1);
                loadedInstruments[i].jumpPoints.clear();

                for (int j = 0; j < jumpPoints; j++)
                {
                    int newJumpPoint = 0;
                    songFile.read((char*)&newJumpPoint, 4);
                    loadedInstruments[i].jumpPoints.emplace_back(newJumpPoint);
                }


                uint8_t opWaves;
                songFile.read((char*)&opWaves, 1);
                float op1 = int(opWaves / 64.0f);
                float op2 = int(opWaves / 16.0f) - (op1 * 4.0f);
                float op3 = int(opWaves / 4.0f) - (op1 * 16.0f + op2 * 4.0f);
                float op4 = int(opWaves) - (op1 * 64.0f + op2 * 16.0f + op3 * 4.0f);

                loadedInstruments[i].operatorWavesToUse[0] = op4;
                loadedInstruments[i].operatorWavesToUse[1] = op3;
                loadedInstruments[i].operatorWavesToUse[2] = op2;
                loadedInstruments[i].operatorWavesToUse[3] = op1;

                uint8_t volume;
                songFile.read((char*)&volume, 1);
                loadedInstruments[i].volume = float(volume) / 16.0f;
                uint8_t glide;
                songFile.read((char*)&glide, 1);
                loadedInstruments[i].glide = float(glide) / 16.0f;
                uint8_t algo;
                songFile.read((char*)&algo, 1);
                loadedInstruments[i].algorithmType = algo;

                uint8_t readVar = 0;
                float var1 = 0.0f;
                float var2 = 0.0f;


                // Arp
                songFile.read((char*)&readVar, 1);
                var1 = int(readVar / 16.0f);
                var2 = readVar - var1 * 16.0f;
                loadedInstruments[i].arpSpeed = var1 / 16.0f;
                loadedInstruments[i].arpLength = var2;

                for (int arp = 0; arp < 15; arp++)
                {
                    songFile.read((char*)&readVar, 1);
                    loadedInstruments[i].arpPitches[arp] = float(readVar) * 0.25f;
                }


                // Modulation paths
                for (int mod = 0; mod < 4; mod++)
                {
                    songFile.read((char*)&readVar, 1);
                    var1 = int(readVar / 16.0f);
                    var2 = readVar - var1 * 16.0f;
                    loadedInstruments[i].modulationTypes[mod] = var1;
                    loadedInstruments[i].lPResonances[mod] = var2 / 16.0f;
                }

                bool modUsed[4] = { false, false, false, false };
                
                for (int j = 0; j < 4; j++)
                {
                    modUsed[loadedInstruments[i].operatorWavesToUse[j]] = true;
                }

                // For each used sample.
                for (int j = 0; j < 4; j++)
                {
                    if (modUsed[j])
                    {
                        int frameCount = 0;
                        songFile.read((char*)&frameCount, 4);
                        loadedInstruments[i].waveforms[j].pcmFrames.clear();

                        for (int fr = 0; fr < frameCount; fr++)
                        {
                            int frameVal = 0;
                            songFile.read((char*)&frameVal, 1);
                            float scaledFrame = float(frameVal) - 128.0f;
                            scaledFrame /= 128.0f;
                            loadedInstruments[i].waveforms[j].pcmFrames.emplace_back(scaledFrame);
                        }

                        uint8_t waveform;
                        songFile.read((char*)&waveform, 1);
                        var1 = int(waveform / 16.0f);
                        var2 = waveform - var1 * 16.0f;
                        loadedInstruments[i].waveforms[j].setFineTuneFromLookupValue(int(var1));
                        loadedInstruments[i].waveforms[j].operatorType = var2;


                        uint8_t waveVar = 0;

                        if (loadedInstruments[i].waveforms[j].operatorType == 0) // Waveform-specific data.
                        {
                            songFile.read((char*)&waveVar, 1);
                            loadedInstruments[i].waveforms[j].dutyCycle = float(waveVar) / 16.0f;
                            songFile.read((char*)&waveVar, 1);
                            loadedInstruments[i].waveforms[j].smoothness = float(waveVar) / 16.0f;
                            songFile.read((char*)&waveVar, 1);
                            loadedInstruments[i].waveforms[j].numOfSineWaves = float(waveVar);
                        }


                        songFile.read((char*)&readVar, 1);
                        var1 = int(readVar / 16.0f);
                        var2 = readVar - var1 * 16.0f;
                        loadedInstruments[i].waveforms[j].waveType = var1;
                        loadedInstruments[i].waveforms[j].loopType = var2;
                        songFile.read((char*)&readVar, 1);
                        var1 = int(readVar / 16.0f);
                        var2 = readVar - var1 * 16.0f;
                        loadedInstruments[i].waveforms[j].offset = float(var1) / 16.0f;
                        loadedInstruments[i].waveforms[j].lfo = var2;
                        songFile.read((char*)&readVar, 1);
                        var1 = int(readVar / 16.0f);
                        var2 = readVar - var1 * 16.0f;
                        loadedInstruments[i].waveforms[j].attack = float(var1) / 16.0f;
                        loadedInstruments[i].waveforms[j].sustain = float(var2) / 16.0f;
                        songFile.read((char*)&readVar, 1);
                        var1 = int(readVar / 16.0f);
                        var2 = readVar - var1 * 16.0f;
                        loadedInstruments[i].waveforms[j].decay = float(var1) / 16.0f;
                        loadedInstruments[i].waveforms[j].release = float(var2) / 16.0f;


                        
                        // Boolean flags
                        songFile.read((char*)&readVar, 1);
                        float stereo1 = int(readVar / 64.0f);
                        float stereo2 = int(readVar / 32.0f) - (stereo1 * 64.0f);
                        float varE = int(readVar / 16.0f) - (stereo1 * 64.0f + stereo2 * 32.0f);
                        float varD = int(readVar / 8.0f) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f);
                        float varC = int(readVar / 4.0f) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f);
                        float varB = int(readVar / 2.0f) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f + varC * 4.0f);
                        float varA = int(readVar) - (stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f + varC * 4.0f + varB * 2.0f);
                        loadedInstruments[i].waveforms[j].generateFromSines = (bool)varE;
                        loadedInstruments[i].waveforms[j].reverseFrames = (bool)varD;
                        loadedInstruments[i].waveforms[j].sustainForever = (bool)varC;
                        loadedInstruments[i].waveforms[j].pitchToNote = (bool)varB;
                        loadedInstruments[i].waveforms[j].continueNote = (bool)varA;

                        if (stereo2 == 1)
                            loadedInstruments[i].waveforms[j].stereo = 2;
                        else if (stereo1 == 1)
                            loadedInstruments[i].waveforms[j].stereo = 1;
                        else
                            loadedInstruments[i].waveforms[j].stereo = 0;

                        int loopStart = 0;
                        songFile.read((char*)&loopStart, 4);
                        loadedInstruments[i].waveforms[j].loopStart = loopStart;
                        int loopEnd = 0;
                        songFile.read((char*)&loopEnd, 4);
                        loadedInstruments[i].waveforms[j].loopEnd = loopEnd;

                        for (int f = 0; f < 11; f++)
                        {
                            songFile.read((char*)&waveVar, 1);
                            loadedInstruments[i].waveforms[j].frequencies[f] = waveVar;
                        }
                    }
                }
            }
        }

        

        songFile.close();
    }
    else // Create new song.
    {
        for (int i = 0; i < 256; i++)
        {
            loadedInstruments[i].enabled = false;
        }
        loadedSong.artistName = "Me";
        loadedSong.bpm = 120;
        //loadedSong.ticksPerBeat = 6;
        loadedSong.patternSequence.clear();
        loadedSong.patterns.clear();
        loadedSong.numberOfChannels = 1;

        // Initialize channels.
        channels.resize(loadedSong.numberOfChannels);
        // Initialize decoders.
        ResizeDecoders(loadedSong.numberOfChannels);

        Pattern firstFrame;
        firstFrame.channels.resize(loadedSong.numberOfChannels);
        loadedSong.patterns.emplace_back(firstFrame);
        loadedSong.patternSequence.emplace_back(0);


        loadedPattern = resizeUnrolledPatternRows(loadedPattern, 32);

        loadCurrentPattern();
        saveCurrentPattern();
    }

    


    loadedSong.currentPattern = 0; // Current frame in frameSequence.
    loadedSong.currentNote = 0;
    loadedSong.timeInNote = 0.0f;
    loadedSong.timeInSong = 0.0f;


    for (int i = 0; i < loadedSong.numberOfChannels; i++)
    {
        loadedSong.toNextChannelNote[i] = 0;
        loadedSong.toNextChannelVolume[i] = 0;
        loadedSong.toNextChannelEffect[i] = 0;
    }



    loadCurrentPattern();


    DrawSampleDisplay();

    return;
}





void LoadGUIThemes()
{
    int sizeX, sizeY, comps;
    unsigned char* data;

    if (gui.lightMode)
        data = stbi_load("LightColorThemes.png", &sizeX, &sizeY, &comps, 3);
    else
        data = stbi_load("ThemeColors.png", &sizeX, &sizeY, &comps, 3);
    

    if (data)
    {
        gui.themes.clear();

        for (int y = 0; y < sizeY; y++)
        {
            GUITheme newTheme;
            for (int x = 0; x < sizeX; x++)
            {
                unsigned char* pixelOffset = data + ((9 * y) + x) * 3;
                //if (gui.uiBrightMode)
                //    newTheme.uiColors[8 - x] = { pixelOffset[0], pixelOffset[1], pixelOffset[2] };
                //else
                newTheme.uiColors[x] = { pixelOffset[0], pixelOffset[1], pixelOffset[2] };
            }
            
            

            gui.themes.emplace_back(newTheme);
        }
    }

    stbi_image_free(data);

    return;
}

/*
void SwapLightDarkMode()
{
    gui.uiBrightMode = !gui.uiBrightMode;
    LoadGUIThemes();
    SaveSettings();

    return;
}*/


void SaveSettings()
{
    std::ofstream settingsFile("SuperSound Settings.bin", std::ios::binary | std::ios::out);

    if (settingsFile.is_open())
    {
        // Samples
        uint8_t themeNum = gui.uiColorTheme;
        settingsFile.write((char*)&themeNum, 1); // Theme number.

        uint8_t chFoc = editor.channelFocus;
        settingsFile.write((char*)&chFoc, 1); // Channel focus.

        uint8_t background = gui.background;
        settingsFile.write((char*)&background, 1); // Background.

        uint8_t light = gui.lightMode;
        settingsFile.write((char*)&light, 1); // Light mode.

        settingsFile.close();
    }

    return;
}


void LoadSettings()
{
    std::ifstream settingsFile("SuperSound Settings.bin", std::ios::binary | std::ios::out);

    if (settingsFile.is_open())
    {
        uint8_t themeNum;
        settingsFile.read((char*)&themeNum, 1); // Theme number.
        gui.uiColorTheme = themeNum;

        uint8_t chFoc;
        settingsFile.read((char*)&chFoc, 1); // Channel focus.
        editor.channelFocus = bool(chFoc);

        uint8_t background;
        settingsFile.read((char*)&background, 1); // Background.
        gui.background = bool(background);

        uint8_t light;
        settingsFile.read((char*)&light, 1); // Light mode.
        gui.lightMode = bool(light);

        settingsFile.close();
    }

    return;
}