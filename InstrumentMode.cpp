




// Replace song theme with song program version number.


// Fix playing line looks strange when playing.


// Fix save and keep frames crash.


// Fix mouse click while exporting causes crash.


// Make sure notes are in tune.


// Fix first theme not saving when selected.



// Fix problem with loading fine-tune.





// Fix copying and pasting multiple channels.

// Make windowed mode movable.

// Fix C command stopping the end of frame. (and clipping)

// Fix sample glide not coming into effect until notes are played in the song.

// Fix editing over a note with another sample causes crash.




// Make sure that all modulator types work correctly.

// Make sure that transpose selection works.

// Fix file folders sometimes not appearing in file select.

// Fix loading then playing a song from another song sometimes causes a crash.


// Add 2 bytes for the version number of a song to the start of its file. (Version byte1.byte2).
// When loading a song, it will give an error window if the  version number is higher than the program version. (This song uses a newer version of Dual Tracker).
// If the version number is less that the program version, a window will appear. (This song uses an older version of Dual Tracker. Would you like to update it from (version1) to (version2)? Warning: the update cannot be reversed. You may want to save a copy of the song.)
// When loading the song, if statements are used for version changes.
// Ex:
//		if (versionNum < 2.5)
//			loadedSong.newVar = 0.0f;
// If the song was from an old version, it will be saved after loading, overwriting it with its new version.

// Fix delay note effect causing clipping sound with other channels playing.





// Create synth presets.

// Rework help page.

// Create a demo song.

// Release version 1.0. ////////////////////////////////////////////////////////////////////




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