#pragma once


#include "GlobalVariables.h"

// For searching file directories. New as of c++ 17.
#include <filesystem>




#include <cstdlib>


void LoadSamples(); // Load all samples



void LoadSamples()
{

    for (auto const& dir_entry : std::filesystem::directory_iterator("Samples/"))
    {
        std::string sampleName = dir_entry.path().generic_string();
        sampleName.erase(0, 8);
        if (sampleName.at(sampleName.length() - 3) == 'w' && sampleName.at(sampleName.length() - 2) == 'a' && sampleName.at(sampleName.length() - 1) == 'v') // Only show .wav files.
        {
            Sample newSample;
            newSample.sampleName = sampleName;
            fileSamples.emplace_back(newSample);
        }
    }
}