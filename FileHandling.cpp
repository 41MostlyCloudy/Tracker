#pragma once


#include "GlobalVariables.h"
#include "FrameHandling.cpp"
#include "SongHandling.cpp"

// For searching file directories. New as of c++ 17.
#include <filesystem>




#include <cstdlib>


void LoadSamples(); // Load all samples.

void LoadSongFiles(); // Load all song files.

void SaveSong(); // Save the currently loaded song.

void LoadSong(std::string name); // Load the song file with the given name.




void SaveSong() // Save the currently loaded song.
{
    // Song file format.

    // Samples:
    // 1 byte: # of samples.
    // For each sample:
    //      1 byte: Length of name.
    //      For each character:
    //          1 byte value.

    // Song Body:
    //      1 byte: BPM
    //      1 byte: TPB
    //      1 byte: # Of frames in frame sequence.
    //      For each index in the frame sequence:
    //          1 byte: Frame index number.
    //      1 byte: # Of frames in song.
    //      For each frame:
    //          1 byte: # of rows.
    //          For each channel:
    //              1 byte: # of notes.
    //              For each note:
    //                  1 byte: note value.
    //              1 byte: # of volumes.
    //              For each volume:
    //                  1 byte: volume value.
    //              1 byte: # of effects.
    //              For each effect:
    //                  1 byte: note effect.

    saveCurrentFrame();

    std::ofstream songFile("Songs/" + loadedSong.songName + ".song", std::ios::binary | std::ios::out);


    if (songFile.is_open())
    {
        // Samples
        uint8_t sampleNum = loadedSamples.size();
        songFile.write((char*)&sampleNum, 1); // # of samples.
        
        for (int i = 0; i < loadedSamples.size(); i++) // Sample
        {
            std::string sampleName = "Samples/" + loadedSamples[i].sampleName;

            uint8_t sampleNameNum = sampleName.length();
            songFile.write((char*)&sampleNameNum, 1);

            for (int j = 0; j < sampleName.length(); j++)
            {
                uint8_t sampleChar = sampleName.at(j);
                songFile.write((char*)&sampleChar, 1);
            }
        }
       

        // Song Body
        uint8_t bpm = int(loadedSong.bpm);
        songFile.write((char*)&bpm, 1);
        uint8_t tpb = int(loadedSong.ticksPerBeat);
        songFile.write((char*)&tpb, 1);
        uint8_t seqSize = loadedSong.frameSequence.size();
        songFile.write((char*)&seqSize, 1);

        for (int i = 0; i < loadedSong.frameSequence.size(); i++) // Frame sequence.
        {
            uint8_t seqId = loadedSong.frameSequence[i];
            songFile.write((char*)&seqId, 1);
        }


        uint8_t frameNum = loadedSong.frames.size();
        songFile.write((char*)&frameNum, 1);

        for (int i = 0; i < loadedSong.frames.size(); i++) // Frame.
        {
            uint8_t frameRows = loadedSong.frames[i].rows;
            songFile.write((char*)&frameRows, 1);

            for (int ch = 0; ch < 8; ch++)
            {
                uint8_t noteSize = loadedSong.frames[i].channels[ch].notes.size();
                songFile.write((char*)&noteSize, 1);
                for (int j = 0; j < noteSize; j++)
                {
                    uint8_t note = loadedSong.frames[i].channels[ch].notes[j];
                    songFile.write((char*)&note, 1);
                }

                uint8_t volumeSize = loadedSong.frames[i].channels[ch].volumes.size();
                songFile.write((char*)&volumeSize, 1);
                for (int j = 0; j < volumeSize; j++)
                {
                    uint8_t volume = loadedSong.frames[i].channels[ch].volumes[j];
                    songFile.write((char*)&volume, 1);
                }

                uint8_t effectSize = loadedSong.frames[i].channels[ch].effects.size();
                songFile.write((char*)&effectSize, 1);
                for (int j = 0; j < effectSize; j++)
                {
                    uint8_t effect = loadedSong.frames[i].channels[ch].effects[j];
                    songFile.write((char*)&effect, 1);
                }
            }
        }

        songFile.close();
    }

    

    return;
}


void LoadSong(std::string name) // Load the song file with the given name.
{
    playingSong = false;
    loadedSong.currentFrame = 0;
    loadedSong.currentNote = 0;

    std::ifstream songFile("Songs/" + name, std::ios::binary | std::ios::in);

    name.erase(name.length() - 5, 6);
    loadedSong.songName = name;

    if (songFile.is_open())
    {
        // Samples

        uint8_t sampleSize;
        songFile.read((char*)&sampleSize, 1);

        loadedSamples.clear();

        for (int i = 0; i < sampleSize; i++)
        {
            Sample newSample;
            newSample.sampleName = "";

            uint8_t nameSize;
            songFile.read((char*)&nameSize, 1);

            for (int j = 0; j < nameSize; j++)
            {
                uint8_t newChar;
                songFile.read((char*)&newChar, 1);
                newSample.sampleName = newSample.sampleName + char(newChar);
            }
            newSample.sampleName.erase(0, 8);
            loadedSamples.emplace_back(newSample);
        }

        // Song Body

        uint8_t bpm;
        songFile.read((char*)&bpm, 1);
        loadedSong.bpm = bpm;
        uint8_t tpb;
        songFile.read((char*)&tpb, 1);
        loadedSong.ticksPerBeat = tpb;

        // Frame sequence
        uint8_t frameSeqNum;
        songFile.read((char*)&frameSeqNum, 1);
        loadedSong.frameSequence.clear();
        loadedSong.frameSequence.resize(frameSeqNum);
        for (int i = 0; i < frameSeqNum; i++)
        {
            songFile.read((char*)&loadedSong.frameSequence[i], 1);
        }

        // Frames
        uint8_t frameNum;
        songFile.read((char*)&frameNum, 1);
        loadedSong.frames.clear();
        loadedSong.frames.resize(frameNum);
        for (int i = 0; i < frameNum; i++)
        {
            uint8_t rows;
            songFile.read((char*)&rows, 1);
            loadedSong.frames[i].rows = rows;

            for (int ch = 0; ch < 8; ch++)
            {
                uint8_t noteNum;
                songFile.read((char*)&noteNum, 1);
                loadedSong.frames[i].channels[ch].notes.clear();
                loadedSong.frames[i].channels[ch].notes.resize(noteNum);
                for (int j = 0; j < noteNum; j++)
                {
                    uint8_t note;
                    songFile.read((char*)&note, 1);
                    loadedSong.frames[i].channels[ch].notes[j] = note;
                }

                uint8_t volumeNum;
                songFile.read((char*)&volumeNum, 1);
                loadedSong.frames[i].channels[ch].volumes.clear();
                loadedSong.frames[i].channels[ch].volumes.resize(volumeNum);
                for (int j = 0; j < volumeNum; j++)
                {
                    uint8_t volume;
                    songFile.read((char*)&volume, 1);
                    loadedSong.frames[i].channels[ch].volumes[j] = volume;
                }

                uint8_t effectNum;
                songFile.read((char*)&effectNum, 1);
                loadedSong.frames[i].channels[ch].effects.clear();
                loadedSong.frames[i].channels[ch].effects.resize(effectNum);
                for (int j = 0; j < effectNum; j++)
                {
                    uint8_t effect;
                    songFile.read((char*)&effect, 1);
                    loadedSong.frames[i].channels[ch].effects[j] = effect;
                }
            }
        }

        songFile.close();
    }

    // Load the sample data
    LoadSampleData();


    loadCurrentFrame();

    return;
}


void LoadSamples()
{
    fileNameList.clear();
    for (auto const& dir_entry : std::filesystem::directory_iterator("Samples/"))
    {
        std::string sampleName = dir_entry.path().generic_string();
        sampleName.erase(0, 8);
        if (sampleName.at(sampleName.length() - 3) == 'w' && sampleName.at(sampleName.length() - 2) == 'a' && sampleName.at(sampleName.length() - 1) == 'v') // Only show .wav files.
        {
            fileNameList.emplace_back(sampleName);
        }
    }

    return;
}


void LoadSongFiles()
{
    fileNameList.clear();
    for (auto const& dir_entry : std::filesystem::directory_iterator("Songs/"))
    {
        std::string songName = dir_entry.path().generic_string();
        songName.erase(0, 6);
        if (songName.at(songName.length() - 4) == 's' && songName.at(songName.length() - 3) == 'o' && songName.at(songName.length() - 2) == 'n' && songName.at(songName.length() - 1) == 'g') // Only show .wav files.
        {
            fileNameList.emplace_back(songName);
        }
    }

    return;
}