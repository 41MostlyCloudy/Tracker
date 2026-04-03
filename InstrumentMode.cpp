




// Computer Volume

// Fix EDO transpose.



// Make presets.


// Fix the playing crash.




// Keyboard Song

// Stepping on Snow

// Lunar Memories

// Fatal Error








// Create a demo song using keyboard samples and samples around the house.

// Create a demo song using a different EDO.

// Create a demo song around saw waves and a lot of effects.

// Create a demo song with high use of additive synth instruments.

// Create a demo song with drum breaks, dynamic synths and samples.









// Fix set LFO crash?

// Is there a crash in the regular program loop?

// Is the crash caused by starting and stopping the song?



// Create 10 3 minute demo songs.


// Create synth presets.


// Rework help page.
//	-About
//	-Top GUI
//	-Writing Patterns
//	-Instruments
//	-Effects


// Create names for light themes.


// Release version 1.0. ////////////////////////////////////////////////////////////////////



// Only save sample frames if using.

// Delete additive synth completely.



// What is causing the file reading problem?
//	-It only happens when multiple operators are set to additive waves, regardless of whether they are read. The wave corresponding to the operator is removed.
//	-Check all references to additive wave frequencies in relation to regular sample frequencies.

// Is the additive crash in the onBeat reading or in modulator reading?


// Does delay cause the crash?

////////////////////////////


// Rework the envelope system.


// Fix wave A and B subharmonics.


// Fix retrigger effect only working with looping samples and not stopping at stop note.


// Re-implement and fix C effect.
// Fix C command stopping the end of frame. (and clipping)




// (WHEN MAKING CHANGES TO THE SONG FORMAT AFTER RELEASE)
// If the version number is less that the program version, a window will appear. (This song uses an older version of Dual Tracker. Would you like to update it from (version1) to (version2)? Warning: the update cannot be reversed. You may want to save a copy of the song.)
// When loading the song, if statements are used for version changes.
// Ex:
//		if (versionNum < 2.5)
//			loadedSong.newVar = 0.0f;
// If the song was from an old version, it will be saved after loading, overwriting it with its new version.


// Add channel start parameters, which are calculated every time a pattern is loaded.
// They go through the song from the start to find the current volume, pitch, exc.


// Fix first theme not saving when selected.


// Make windowed mode movable.


// Fix low-pass clicking.


// Add options to the right of the Load File menu, with options depending on the file type.
// For samples, there is an option to load as stereo.
//		-If enabled, you can give the operator numbers that will be used for each of the 2 channels.
//		-The selected operators will have their stereo property set to left and right.
//		-It is then up to the user to set these operators as carriers as they want.


// Add reverb effect. (Computer in mixed)
// Schroeder reverberator:
// -Combine the results of 4 copies of the sample, delayed back by different amounts. (0, 2, 3, 5)
// -The old combFilterVolume is reduced and the new combFilters are added to it.
// -Allpass filters.


// Add compress as a sample waveform edit.
// Add an undo button for the sample editor.


// Add voice to compressed channel effect drawing with another horizontal tile. It looks like a long oval.


// Add starting partway through a frame by reading until that point is reached.

// Add play from start of song.

// Add song volume.

// Update how scroll bars work using the new sprites and their own drawing method.

// -Add question marks for a random sound.
// -If the random sound is the last on the line, it is selected from the looping samples.

// Add a slider to see all effects in the top GUI.
// Use only one effect per line, so that each can be longer.

// Add an undo button for the track.