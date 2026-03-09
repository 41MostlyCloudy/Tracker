




// Fix save and keep frames crash.


// Fix mouse click while exporting causes crash.


// Fix playing line looks strange when playing.


// Make sure notes are in tune.


// Fix first theme not saving when selected.







// posInPoint is increased.


// If posInPoint is more than speechSpeed, increase phonemePos.
// 
//		posInPoint is reduced by speechSpeed.
//
// 
//		If phonemePos is past the sample length (in points) - mix, set nextPhoneme to the next one.
//		 
//			-If nextPhoneme is 44, and the phoneme has the looping property, set it to currentPhoneme.
//
//			-If nextPhoneme is 45, set it to 44.
//
// 
//		If phonemePos is past the sample length (in points), set currentPhoneme to the next and nextPhoneme to 44.
//	
//			-Reset phonemePos.
//
//			-Set phonemeInterpolation to 0.


// If phonemePos is past the sample length (in points) - mix:
//
//	-Set phonemeInterpolation to (phonemePos + (posInPoint / speechSpeed)) / sample length.
//
// else:
//
//	-Set phonemeInterpolation to 0.


// For each frequency:
// 
//	-Read the next frame
// 
//	-Increase the frequency framePos
// 
//	-Loop at end (or start)
// 
//



























// Make a diagram of how reading phonemes will work.

// Sample reading pos changes are applied for each frequency in their instances.


// Every frame, increment the sample read pos and add the two voice samples.
// It does this using the channel voice notes.
// Set a default voice speed and blend (how quickly it goes from one phoneme to another).











// The voice menu should include the sample display, since it uses the sample for the voice.
// Improver selecting operator types.


// Use Fourier transform to break voice samples into frequencies.
// Each sample is made up of a series of points, each containing volumes for all frequencies.
// The points are interpolated between over time and the sine waves are played at the corresponding volumes.


// Write a FourierTransform function that takes a sample point as input and outputs a vector of the transform.

// Write a DeconstructSample function that takes a sample as input, breaks it into sample points, uses FourierTransform on them,
//		then uses the output to find each point's frequency volumes, it outputs this as a 2D vector.

// Start with { 1/8, 1/7, 1/6, 1/5, 1/4, 1/3, 1/2, 1, 2, 3, 4, 5, 6, 7, 8 } the the frequencies and 480*100 frames for the sample point size.

// Use it when reading samples into the phonemes object.
// First test the function with a spoken sentence.

// When playing the voice samples, use a special set of sine wave samples in the voice samples object. They are added and read like a normal single wave operator.

// First deconstruct voice samples.
// 
// -Apply the Fourier transform to the sample.
// 
// -Take the values of the output vector at the frequency points as the frequency volumes.
// -Test to find a good set of frequencies and a good size of each sample point to test.
// 
// -When playing voice samples, all of the frequencies are played at their respective volumes and pitched up or down to match the note.
// 
// 
// Then add playing voice samples.
// Then add blending between voice samples.



///////////////////////////////////// Voice synthesis
// Record voice samples.
//	-Edit the samples in Audacity.

// Look into sub-types for objects so that all sample operators do not have voice variables.

// Add voice variables
//		-Voice speed (at attack)
//		-Phoneme blend (at sustain)
//		-Time stretch speed (at attack)
//		-Time stretch length (at attack)

// Add playing the voice samples in songs.

// Use time-stretching to give higher and lower frequencies similar speeds.
/////////////////////////////////////





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