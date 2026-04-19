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

void WriteInstrument(std::ofstream* instrumentFile, Instrument* instrument);

Instrument ReadInstrument(std::ifstream *instrumentFile);

void SaveCurrentInstrument();

void LoadCurrentInstrument(std::string name);






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
            {
                for (int ch = 0; ch < channels.size(); ch++)
                {
                    if (loadedSong.patterns[i].channels[ch].notes.size() > 0 || loadedSong.patterns[i].channels[ch].volumes.size() > 0 || loadedSong.patterns[i].channels[ch].effects.size() > 0)
                        loadedSong.unusedFrames = true;
                }
            }
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
    //      1 byte: Version number.
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
        

        uint8_t version = int(programVersionNumber); // Version
        songFile.write((char*)&version, 1);

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
            uint8_t channelAttributes = 0;
            if (channels[j].compressed)
                channelAttributes += 1;
            songFile.write((char*)&channelAttributes, 1);

            uint8_t channelMixVol = channels[j].mixVolume * 16.0f;
            songFile.write((char*)&channelMixVol, 1);
        }
        uint8_t bpm = int(loadedSong.startingBPM); // BPM
        songFile.write((char*)&bpm, 1);
        uint8_t edo = int(loadedSong.edo); // EDO
        songFile.write((char*)&edo, 1);


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
                std::string sampleName = loadedInstruments[i].name;
                uint8_t sampleNameNum = sampleName.length();
                songFile.write((char*)&sampleNameNum, 1);

                for (int j = 0; j < sampleName.length(); j++)
                {
                    uint8_t sampleChar = sampleName.at(j);
                    songFile.write((char*)&sampleChar, 1);
                }

                WriteInstrument(&songFile, &loadedInstruments[i]);
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

    if (songFile.is_open())
    {
    // Song file format.

    // Song Body:
    //      1 byte: Version number.
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

        uint8_t version;
        songFile.read((char*)&version, 1);

        // Song uses incompatible program version.
        if (version > programVersionNumber)
        {
            windowController.InitializeWindow("Newer Format", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 20, 16 });
            songFile.close();
            return;
        }



        loadedSong.songName = name;

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


            uint8_t channelMixVol;
            songFile.read((char*)&channelMixVol, 1);
            channels[ch].mixVolume = float(channelMixVol) / 16.0f;
        }

        uint8_t bpm;
        songFile.read((char*)&bpm, 1);
        loadedSong.startingBPM = bpm;
        loadedSong.bpm = loadedSong.startingBPM;
        uint8_t edo;
        songFile.read((char*)&edo, 1);
        loadedSong.edo = edo;
        

        // Frame sequence
        uint8_t frameSeqNum;
        songFile.read((char*)&frameSeqNum, 1);
        loadedSong.patternSequence.clear();
        loadedSong.patternSequence.resize(frameSeqNum);
        for (int i = 0; i < frameSeqNum; i++)
        {
            songFile.read((char*)&loadedSong.patternSequence[i], 1);
        }



        // Patterns
        uint8_t patternNum;
        songFile.read((char*)&patternNum, 1);
        loadedSong.patterns.clear();
        loadedSong.patterns.resize(patternNum);
        for (int i = 0; i < patternNum; i++)
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

                loadedInstruments[i] = ReadInstrument(&songFile);

                loadedInstruments[i].name = sampleName;
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
                newTheme.uiColors[x] = { pixelOffset[0], pixelOffset[1], pixelOffset[2] };
            }
            
            gui.themes.emplace_back(newTheme);
        }
    }

    stbi_image_free(data);

    return;
}




void SaveSettings()
{
    std::ofstream settingsFile("SuperSound Settings.bin", std::ios::binary | std::ios::out);

    if (settingsFile.is_open())
    {
        // Samples
        uint8_t themeNum = uint8_t(gui.uiColorTheme);
        settingsFile.write((char*)&themeNum, 1); // Theme number.

        uint8_t chFoc = editor.channelFocus;
        settingsFile.write((char*)&chFoc, 1); // Channel focus.

        uint8_t background = gui.background;
        settingsFile.write((char*)&background, 1); // Background.

        uint8_t light = gui.lightMode;
        settingsFile.write((char*)&light, 1); // Light mode.
    }

    settingsFile.close();

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
        gui.background = background;

        uint8_t light;
        settingsFile.read((char*)&light, 1); // Light mode.
        gui.lightMode = bool(light);
    }

    settingsFile.close();

    return;
}



void WriteInstrument(std::ofstream* instrumentFile, Instrument* instrument)
{

    if (instrument->enabled)
    {
        uint8_t jumpPoints = instrument->jumpPoints.size();
        instrumentFile->write((char*)&jumpPoints, 1);

        for (int j = 0; j < jumpPoints; j++)
        {
            int point = instrument->jumpPoints[j];
            instrumentFile->write((char*)&point, 4);
        }

        uint8_t opWaves = instrument->operatorWavesToUse[3] * 64.0f + instrument->operatorWavesToUse[2] * 16.0f + instrument->operatorWavesToUse[1] * 4.0f + instrument->operatorWavesToUse[0];
        instrumentFile->write((char*)&opWaves, 1);
        uint8_t volume = instrument->volume * 16.0f;
        instrumentFile->write((char*)&volume, 1);
        uint8_t glide = instrument->glide * 16.0f;
        instrumentFile->write((char*)&glide, 1);
        uint8_t algo = (uint8_t)instrument->interpolation * 4 + instrument->algorithmType;
        instrumentFile->write((char*)&algo, 1);

        // Arp
        if (instrument->arpSpeed > 0.9375f) instrument->arpSpeed = 0.9375f;
        uint8_t arpVar = int((instrument->arpSpeed * 16.0f) * 16.0f) + int(instrument->arpLength);
        instrumentFile->write((char*)&arpVar, 1);

        for (int arp = 0; arp < 15; arp++)
        {
            arpVar = instrument->arpPitches[arp] * 4.0f;
            instrumentFile->write((char*)&arpVar, 1);
        }

        // Modulation paths
        for (int mod = 0; mod < 4; mod++)
        {
            if (instrument->lPResonances[mod] > 0.9375f) instrument->lPResonances[mod] = 0.9375f;
            uint8_t modPath = int((instrument->modulationTypes[mod]) * 16.0f) + int(instrument->lPResonances[mod] * 16.0f);
            instrumentFile->write((char*)&modPath, 1);
        }

        bool modsUsed[4] = { false, false, false, false };

        for (int j = 0; j < 4; j++)
        {
            modsUsed[instrument->operatorWavesToUse[j]] = true;
        }


        // For each used sample.
        for (int j = 0; j < 4; j++)
        {
            if (modsUsed[j])
            {
                int frameCount = instrument->waveforms[j].pcmFrames.size();
                instrumentFile->write((char*)&frameCount, 4);

                for (int fr = 0; fr < frameCount; fr++)
                {
                    float scaledFrame = ((instrument->waveforms[j].pcmFrames[fr] * 128.0f) + 128.0f);
                    uint8_t frameVal = uint8_t(scaledFrame);
                    instrumentFile->write((char*)&frameVal, 1);
                }

                uint8_t isWaveform = 0;
                int periods = int(instrument->waveforms[j].fuzz * 16.0f);
                isWaveform = periods * 16.0f + instrument->waveforms[j].operatorType;
                instrumentFile->write((char*)&isWaveform, 1);

                uint8_t waveVar = 0;

                if (instrument->waveforms[j].operatorType != 1) // Waveform-specific data.
                {
                    waveVar = int(instrument->waveforms[j].dutyCycle * 16.0f);
                    instrumentFile->write((char*)&waveVar, 1);
                    waveVar = int(instrument->waveforms[j].smoothness * 16.0f);
                    instrumentFile->write((char*)&waveVar, 1);
                    waveVar = instrument->waveforms[j].numOfSineWaves;
                    instrumentFile->write((char*)&waveVar, 1);
                }



                waveVar = instrument->waveforms[j].waveType * 16.0f + instrument->waveforms[j].loopType;
                instrumentFile->write((char*)&waveVar, 1);
                if (instrument->waveforms[j].offset > 0.9375f) instrument->waveforms[j].offset = 0.9375f;
                waveVar = int((instrument->waveforms[j].offset * 16.0f) * 16.0f) + int(instrument->waveforms[j].lfo);
                instrumentFile->write((char*)&waveVar, 1);
                if (instrument->waveforms[j].attack > 0.9375f) instrument->waveforms[j].attack = 0.9375f;
                if (instrument->waveforms[j].sustain > 0.9375f) instrument->waveforms[j].sustain = 0.9375f;
                waveVar = int(instrument->waveforms[j].attack * 16.0f) * 16.0f + int(instrument->waveforms[j].sustain * 16.0f);
                instrumentFile->write((char*)&waveVar, 1);
                if (instrument->waveforms[j].decay > 0.9375f) instrument->waveforms[j].decay = 0.9375f;
                if (instrument->waveforms[j].release > 0.9375f) instrument->waveforms[j].release = 0.9375f;
                waveVar = int(instrument->waveforms[j].decay * 16.0f) * 16.0f + int(instrument->waveforms[j].release * 16.0f);
                instrumentFile->write((char*)&waveVar, 1);



                // Boolean flags
                bool stereo1 = false;
                bool stereo2 = false;
                if (instrument->waveforms[j].stereo == 1)
                    stereo1 = true;
                else if (instrument->waveforms[j].stereo == 2)
                    stereo2 = true;


                waveVar = (int)instrument->waveforms[j].useArp * 128.0f + (int)stereo1 * 64.0f + (int)stereo2 * 32.0f + (int)instrument->waveforms[j].generateFromSines * 16.0f + (int)instrument->waveforms[j].reverseFrames * 8.0f + (int)instrument->waveforms[j].sustainForever * 4.0f + (int)instrument->waveforms[j].pitchToNote * 2.0f + (int)instrument->waveforms[j].continueNote;
                instrumentFile->write((char*)&waveVar, 1);

                //std::cout << " Write: " << int(waveVar);

                int loopStart = instrument->waveforms[j].loopStart;
                instrumentFile->write((char*)&loopStart, 4);
                int loopEnd = instrument->waveforms[j].loopEnd;
                instrumentFile->write((char*)&loopEnd, 4);

                for (int f = 0; f < 11; f++)
                {
                    waveVar = instrument->waveforms[j].frequencies[f];
                    instrumentFile->write((char*)&waveVar, 1);
                }
            }

        }
    }

    return;
}



Instrument ReadInstrument(std::ifstream* instrumentFile)
{
    Instrument newInstrument;

    newInstrument.enabled = true;


    // Jump points
    uint8_t jumpPoints;
    instrumentFile->read((char*)&jumpPoints, 1);
    newInstrument.jumpPoints.clear();

    for (int j = 0; j < jumpPoints; j++)
    {
        int newJumpPoint = 0;
        instrumentFile->read((char*)&newJumpPoint, 4);
        newInstrument.jumpPoints.emplace_back(newJumpPoint);
    }


    uint8_t opWaves;
    instrumentFile->read((char*)&opWaves, 1);
    float op1 = int(opWaves / 64.0f);
    float op2 = int(opWaves / 16.0f) - (op1 * 4.0f);
    float op3 = int(opWaves / 4.0f) - (op1 * 16.0f + op2 * 4.0f);
    float op4 = int(opWaves) - (op1 * 64.0f + op2 * 16.0f + op3 * 4.0f);

    newInstrument.operatorWavesToUse[0] = op4;
    newInstrument.operatorWavesToUse[1] = op3;
    newInstrument.operatorWavesToUse[2] = op2;
    newInstrument.operatorWavesToUse[3] = op1;

    uint8_t volume;
    instrumentFile->read((char*)&volume, 1);
    newInstrument.volume = float(volume) / 16.0f;
    uint8_t glide;
    instrumentFile->read((char*)&glide, 1);
    newInstrument.glide = float(glide) / 16.0f;
    uint8_t algo;
    instrumentFile->read((char*)&algo, 1);
    int interp = algo / 4;
    if (interp == 0)
        newInstrument.interpolation = false;
    else
        newInstrument.interpolation = true;
    algo -= (interp * 4);
    newInstrument.algorithmType = algo;

    uint8_t readVar = 0;
    float var1 = 0.0f;
    float var2 = 0.0f;


    // Arp
    instrumentFile->read((char*)&readVar, 1);
    var1 = int(readVar / 16.0f);
    var2 = readVar - var1 * 16.0f;
    newInstrument.arpSpeed = var1 / 16.0f;
    newInstrument.arpLength = var2;

    for (int arp = 0; arp < 15; arp++)
    {
        instrumentFile->read((char*)&readVar, 1);
        newInstrument.arpPitches[arp] = float(readVar) * 0.25f;
    }


    // Modulation paths
    for (int mod = 0; mod < 4; mod++)
    {
        instrumentFile->read((char*)&readVar, 1);
        var1 = int(readVar / 16.0f);
        var2 = readVar - var1 * 16.0f;
        newInstrument.modulationTypes[mod] = var1;
        newInstrument.lPResonances[mod] = var2 / 16.0f;
    }

    bool modUsed[4] = { false, false, false, false };

    for (int j = 0; j < 4; j++)
    {
        modUsed[newInstrument.operatorWavesToUse[j]] = true;
    }

    // For each used sample.
    for (int j = 0; j < 4; j++)
    {
        if (modUsed[j])
        {
            int frameCount = 0;
            instrumentFile->read((char*)&frameCount, 4);
            newInstrument.waveforms[j].pcmFrames.clear();

            for (int fr = 0; fr < frameCount; fr++)
            {
                int frameVal = 0;
                instrumentFile->read((char*)&frameVal, 1);
                float scaledFrame = float(frameVal) - 128.0f;
                scaledFrame /= 128.0f;
                newInstrument.waveforms[j].pcmFrames.emplace_back(scaledFrame);
            }

            uint8_t waveform;
            instrumentFile->read((char*)&waveform, 1);
            var1 = int(waveform / 16.0f);
            var2 = waveform - var1 * 16.0f;
            newInstrument.waveforms[j].fuzz = float(var1) / 16.0f;
            newInstrument.waveforms[j].operatorType = var2;


            uint8_t waveVar = 0;

            if (newInstrument.waveforms[j].operatorType != 1) // Waveform-specific data.
            {
                instrumentFile->read((char*)&waveVar, 1);
                newInstrument.waveforms[j].dutyCycle = float(waveVar) / 16.0f;
                instrumentFile->read((char*)&waveVar, 1);
                newInstrument.waveforms[j].smoothness = float(waveVar) / 16.0f;
                instrumentFile->read((char*)&waveVar, 1);
                newInstrument.waveforms[j].numOfSineWaves = float(waveVar);
            }


            instrumentFile->read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            newInstrument.waveforms[j].waveType = var1;
            newInstrument.waveforms[j].loopType = var2;
            instrumentFile->read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            newInstrument.waveforms[j].offset = float(var1) / 16.0f;
            newInstrument.waveforms[j].lfo = var2;
            instrumentFile->read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            newInstrument.waveforms[j].attack = float(var1) / 16.0f;
            newInstrument.waveforms[j].sustain = float(var2) / 16.0f;
            instrumentFile->read((char*)&readVar, 1);
            var1 = int(readVar / 16.0f);
            var2 = readVar - var1 * 16.0f;
            newInstrument.waveforms[j].decay = float(var1) / 16.0f;
            newInstrument.waveforms[j].release = float(var2) / 16.0f;



            // Boolean flags
            instrumentFile->read((char*)&readVar, 1);
            int varF = int(readVar / 128.0f);
            int stereo1 = int(readVar - (varF * 128.0f)) / 64.0f;
            int stereo2 = int(readVar - (varF * 128.0f + stereo1 * 64.0f)) / 32.0f;
            int varE = int(readVar - (varF * 128.0f + stereo1 * 64.0f + stereo2 * 32.0f)) / 16.0f;
            int varD = int(readVar - (varF * 128.0f + stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f)) / 8.0f;
            int varC = int(readVar - (varF * 128.0f + stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f)) / 4.0f;
            int varB = int(readVar - (varF * 128.0f + stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f + varC * 4.0f)) / 2.0f;
            int varA = int(readVar - (varF * 128.0f + stereo1 * 64.0f + stereo2 * 32.0f + varE * 16.0f + varD * 8.0f + varC * 4.0f + varB * 2.0f));
            newInstrument.waveforms[j].useArp = (bool)varF;
            newInstrument.waveforms[j].generateFromSines = (bool)varE;
            newInstrument.waveforms[j].reverseFrames = (bool)varD;
            newInstrument.waveforms[j].sustainForever = (bool)varC;
            newInstrument.waveforms[j].pitchToNote = (bool)varB;
            newInstrument.waveforms[j].continueNote = (bool)varA;




            if (stereo2 == 1)
                newInstrument.waveforms[j].stereo = 2;
            else if (stereo1 == 1)
                newInstrument.waveforms[j].stereo = 1;
            else
                newInstrument.waveforms[j].stereo = 0;

            int loopStart = 0;
            instrumentFile->read((char*)&loopStart, 4);
            newInstrument.waveforms[j].loopStart = loopStart;
            int loopEnd = 0;
            instrumentFile->read((char*)&loopEnd, 4);
            newInstrument.waveforms[j].loopEnd = loopEnd;

            for (int f = 0; f < 11; f++)
            {
                instrumentFile->read((char*)&waveVar, 1);
                newInstrument.waveforms[j].frequencies[f] = waveVar;
            }
        }
    }


    return newInstrument;
}


void SaveCurrentInstrument()
{
    std::string instrumentFilePath = "C:/" + fileNavigator.currentFilePath.std::filesystem::path::string() + "/";

    std::ofstream instrumentFile(instrumentFilePath + loadedInstruments[editor.selectedInstrument].name + ".inst", std::ios::binary | std::ios::out);

    if (instrumentFile.is_open())
    {
        WriteInstrument(&instrumentFile, &loadedInstruments[editor.selectedInstrument]);
    }

    instrumentFile.close();

    return;
}



void LoadCurrentInstrument(std::string name)
{
    Instrument newInstrument;

    std::ifstream instrumentFile("C:/" + fileNavigator.currentFilePath.std::filesystem::path::string() + "/" + name, std::ios::binary | std::ios::in);

    name.erase(name.length() - 5, 6);

    if (instrumentFile.is_open())
    {
        

        newInstrument = ReadInstrument(&instrumentFile);
        
        loadedInstruments[editor.selectedInstrument] = newInstrument;
        loadedInstruments[editor.selectedInstrument].name = name;
        DrawSampleDisplay();
    }

    

    instrumentFile.close();

    return;
}