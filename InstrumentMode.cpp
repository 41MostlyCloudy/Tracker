// Dual Tracker has many unique features to inspire new sounds and musical styles.
// Everything works to make writing with interesting songs fun and straightforward.


// --------------------------------------------------------------------------------------------------
 

// Remove the swap operators menu and have operator changable by clicking on them.
// You can even use the same operator twice.


//
// Add voice to frame actions, saving and loading.
// 
// Add voice inputs for channels.
//		voice inputs are drawn in blue.
//
// Finish the voice top GUI with selection glow and letter pronunciation.
// 
// Fix compressed channel effect drawing.
// Add voice to compressed channel effect drawing with another horizontal tile. It looks like a long oval.
// 
// Fix deleting, copying and pasting notes to include voice samples.
// 
// Record voice samples.
// Add the voice file object, which can be given samples.
// Add playing the voice samples in songs.
// Add saving and loading the voice file.
// Add voice mode to instrument operators.


// Add a stereo property to instrument operators, only accessible to carriers.
// This can be set to L, R or MIX.
// This will require changing how carriers are added together, so that the combined signal will have 2 channels.


// Add options to the right of the Load File menu, with options depending on the file type.
// For samples, there is an option to load as stereo.
//		-If enabled, you can give the operator numbers that will be used for each of the 2 channels.
//		-The selected operators will have their stereo property set to left and right.
//		-It is then up to the user to set these operators as carriers as they want.


// Add 2 bytes for the version number of a song to the start of its file. (Version byte1.byte2).
// When loading a song, it will give an error window if the  version number is higher than the program version. (This song uses a newer version of Dual Tracker).
// If the version number is less that the program version, a window will appear. (This song uses an older version of Dual Tracker. Would you like to update it from (version1) to (version2)? Warning: the update cannot be reversed. You may want to save a copy of the song.)
// When loading the song, if statements are used for version changes.
// Ex:
//		if (versionNum < 2.5)
//			loadedSong.newVar = 0.0f;
// If the song was from an old version, it will be saved after loading, overwriting it with its new version.


// Fix C command stopping the end of frame. (and clipping)


// Add compress as a sample waveform edit.
// Add an undo button for the sample editor.


// Find a good way to synchronize threads.


// Smooth out timestretching. It seems to take longer on sometimes. (On note start?)

// Fix delay note effect causing clipping sound with other channels playing.




// Fix end of line crash.

// Fix crash while playing. (Most likely has to do with drawing or updating information of beat.) (Most likely has to do with synchronizing the audio reading with changes from updates or player input.)

// Create synth presets.

// Rework help page.

// Create a demo song.

// Release version 1.0. ////////////////////////////////////////////////////////////////////


















// Add starting partway through a frame by reading until that point is reached.

// Add play from start of song.

// Add song volume.

// Update how scroll bars work using the new sprites and their own drawing method.

// -Add question marks for a random sound.
// -If the random sound is the last on the line, it is selected from the looping samples.

// Add a slider to see all effects in the top GUI.
// Use only one effect per line, so that each can be longer.

// Add an undo button for the track.