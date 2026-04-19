




// Crash while playing and trying to adjust frequencies.
// Crash while playing and trying to adjust attack. (Could the crash be in the sliders?).
// Crash while pressing key to add note while stopping song.
// Crash while playing song Test3. (Possible problem with playing mod?)
// Crash while stopping song.


// Rework help page.
//	-About
//	-Top GUI
//	-Writing Patterns
//	-Instruments
//	-Effects































// Release version 1.0. ////////////////////////////////////////////////////////////////////

// Add pattern channel split.

// Fix different instruments cause clipping in interpolation.

// Add 16 bit samples.

// Add loading sterio.

// Fix wave A and B subharmonics.

// Find a better way to add and delete channels.

// Only save sample frames if using.

// Rework the envelope system.

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



// Add starting partway through a frame by reading until that point is reached.

// Add play from start of song.

// Add song volume.

// Update how scroll bars work using the new sprites and their own drawing method.

// -Add question marks for a random sound.
// -If the random sound is the last on the line, it is selected from the looping samples.

// Add a slider to see all effects in the top GUI.
// Use only one effect per line, so that each can be longer.

// Add an undo button for the track.