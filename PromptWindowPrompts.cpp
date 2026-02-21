#pragma once



#include "GlobalVariables.h"
#include "FileHandling.cpp"

 

//void AnswerQuestion(int question, int answer, GLFWwindow* wind);

void ChangeTheme(int theme);

void ClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void RightClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void HoldClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void RightHoldFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void selectAlgorithmOperator(Vector2 pos);






void ClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{
	std::lock_guard<std::mutex> lock(mtx);

	clickPos.x = int(clickPos.x);
	clickPos.y = int(clickPos.y);




	if (clickPos.y == 0)
	{
		if (clickPos.x == wind->size.x - 2 || clickPos.x == wind->size.x - 1) // Exit window.
		{
			if (wind->name == "Instrument Editor")
				sampleDisplay.visible = false;
			windowController.windows.erase(windowController.windows.begin() + windowIndex);
			windowController.windows.shrink_to_fit();
			gui.drawUIThisFrame = true;
			gui.drawFrameThisFrame = true;
			gui.scrollBars[4].topLeft = { -1,-1 }; // Reset the position of the file scrollbar to hide it.
			gui.scrollBars[4].position = 0;
			gui.scrollBars[4].length = 0;
			screen.mouseDown = false;
			return;
		}
		else // Drag window.
		{
			wind->dragWindow = true;
			wind->dragPoint.x = clickPos.x;
			wind->dragPoint.y = clickPos.y;
		}
		// Exit
	}
	
	{

		if (wind->name == "Themes")
		{
			if (clickPos.y > 0 && clickPos.y < 33)
			{
				ChangeTheme((clickPos.y - 1));
				SaveSettings();
			}
			else if (clickPos.y == 34)
			{
				gui.lightMode = !gui.lightMode;
				LoadGUIThemes();
				ChangeTheme(gui.uiColorTheme);
				SaveSettings();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			
		}
		else if (wind->name == "Save and Exit")
		{
			if (clickPos.y == 6)
			{
				SaveSong();
				glfwSetWindowShouldClose(windMain, true);
			}
			else if (clickPos.y == 8)
				glfwSetWindowShouldClose(windMain, true);
		}
		else if (wind->name == "Save and Load")
		{
			if (clickPos.y == 7)
			{
				SaveSong();
				LoadSong(editor.fileToLoad);
				ChangeTheme(gui.uiColorTheme);
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)

			{
				loadedSong.unsavedChanges = false;
				LoadSong(editor.fileToLoad);
				ChangeTheme(gui.uiColorTheme);
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Delete Frames")
		{
			if (clickPos.y == 7)
			{
				// Check for unused frames.
				std::vector <bool> frameUsed;
				for (int i = 0; i < loadedSong.frames.size(); i++)
					frameUsed.emplace_back(false);

				for (int i = 0; i < loadedSong.frameSequence.size(); i++)
				{
					frameUsed[loadedSong.frameSequence[i]] = true;
				}

				std::vector <Frame> newFrames;
				for (int i = 0; i < frameUsed.size(); i++)
				{
					if (frameUsed[i] == true)
						newFrames.emplace_back(loadedSong.frames[i]);
				}

				loadedSong.frames = newFrames;

				SaveSong();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)
			{
				SaveSong();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Overwrite Song")
		{
			if (clickPos.y == 7)
			{
				loadedSong.overWriteOldSong = true;
				SaveSong();
				loadedSong.overWriteOldSong = false;
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Load File")
		{
			gui.scrollBars[4].length = 17;
			if (clickPos.y == 1 && clickPos.x == 1)
			{
				fileNavigator.ExitFile();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 19 && clickPos.x > 28 && clickPos.x < 35) // Load.
			{
				if (fileNavigator.fileNames[editor.selectedFile].at(0) == '1') // Load Song
				{
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					std::string fileN = fileNavigator.fileNames[editor.selectedFile];
					fileN.erase(0, 1);
					LoadSong(fileN);
					ChangeTheme(gui.uiColorTheme);
				}
				else if (fileNavigator.fileNames[editor.selectedFile].at(0) == '2') // Load Sample
				{
					std::string fileN = fileNavigator.fileNames[editor.selectedFile];
					fileN.erase(0, 1);
					std::string filePath = "C:/" + fileNavigator.currentFilePath.std::filesystem::path::string() + "/" + fileN;
					fileN.erase(fileN.size() - 4, 4);
					loadedSamples[editor.selectedSample].sampleName = fileN;
					loadedSamples[editor.selectedSample].enabled = true;



					///////////////////////////////////////////////////////////// Load in the sample frames.
					// Reset all of the frame reading positions since the wave may have changed sizes.
					for (int ch = 0; ch < channels.size(); ch++)
					{
						for (int op = 0; op < 4; op++)
							channels[ch].waveforms[op].frameReadPos = 0;
					}

					ma_decoder loadingDecoder;
					const char* fPath = &filePath[0];
					ma_decoder_init_file(fPath, &decoderConfig, &loadingDecoder);

					bool reading = true;

					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.clear();
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.shrink_to_fit();
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType = 1;

					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

					while (reading)
					{
						float frames[2048]; // For stereo f32 output, this would be 2048 samples
						ma_uint64 framesRead;

						ma_result result;

						result = ma_decoder_read_pcm_frames(&loadingDecoder, frames, 1024, &framesRead);



						for (int i = 0; i < framesRead * 2; i += 2) // Mix the frames together into 1 channel.
						{
							float mixedFrame = (frames[i] + frames[i + 1]) * 0.5f;
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.emplace_back(mixedFrame);
						}

						if (framesRead < 1024)
							reading = false;
					}

					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart = 0;
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();
					// Usually, samples are not continuous waves. They are retriggered every time a note is played.
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].continueNote = false;
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType = 0;

					ma_decoder_uninit(&loadingDecoder);

					///////////////////////////////////////////////////////////// End.

					/*
					if (editor.selectedSample >= loadedSamples.size())
					{
						loadedSamples.emplace_back(newSample);
						loadedSamples[loadedSamples.size() - 1].LoadSampleData();
					}
					else
					{
						editor.selectedSample++;
						loadedSamples.emplace(loadedSamples.begin() + editor.selectedSample, newSample);
						loadedSamples[editor.selectedSample].LoadSampleData();
						//LoadSampleData(editor.selectedSample);
					}*/
					if (sampleDisplay.visible)
					{
						DrawSampleDisplay();
					}
					loadedSong.unsavedChanges = true;
				}
			}
			else if (clickPos.y > 0 && clickPos.x > 0 && clickPos.x < 39)
			{
				if (clickPos.y - 2 + gui.fileListScroll < fileNavigator.fileNames.size())
				{
					if (clickPos.x == 1)
					{
						if (fileNavigator.fileNames[clickPos.y - 2 + gui.fileListScroll].at(0) == '0')
						{
							std::string fileN = fileNavigator.fileNames[clickPos.y - 2 + gui.fileListScroll];
							fileN.erase(0, 1);
							fileNavigator.EnterFile(fileN);
						}
					}
					else
					{
						editor.selectedFile = clickPos.y - 2 + gui.fileListScroll;
					}
				}
			}
		}
		else if (wind->name == "Save Song")
		{
			gui.scrollBars[4].length = 17;
			if (clickPos.y == 1 && clickPos.x == 1)
			{
				fileNavigator.ExitFile();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 19 && clickPos.x > 28 && clickPos.x < 35) // Save.
			{
				//std::string fileN = fileNavigator.fileNames[editor.selectedFile];
				//fileN.erase(0, 1);
				SaveSong();
			}
			else if (clickPos.y > 0 && clickPos.x > 0 && clickPos.x < 39)
			{
				if (clickPos.y - 2 + gui.fileListScroll < fileNavigator.fileNames.size())
				{
					if (clickPos.x == 1)
					{
						if (fileNavigator.fileNames[clickPos.y - 2 + gui.fileListScroll].at(0) == '0')
						{
							std::string fileN = fileNavigator.fileNames[clickPos.y - 2 + gui.fileListScroll];
							fileN.erase(0, 1);
							fileNavigator.EnterFile(fileN);
						}
					}
					else
					{
						editor.selectedFile = clickPos.y - 2 + gui.fileListScroll;
					}
				}
			}
		}
		else if (wind->name == "Settings")
		{
			if (clickPos.y == 2)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				windowController.InitializeWindow("Themes", wind->position, { 16, 36 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 4)
			{
				editor.channelFocus = !editor.channelFocus;
				if (editor.channelFocus)
				{
					for (int ch = 0; ch < channels.size(); ch++)
						channels[ch].compressed = true;
				}
				SaveSettings();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			if (clickPos.y == 6)
			{
				gui.background = !gui.background;
				SaveSettings();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			if (clickPos.y == 8)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				windowController.InitializeWindow("Phonemes", wind->position, { 16, 26 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Selection")
		{
			if (clickPos.y == 2)
			{
				copyNotes();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 4)
			{
				pasteNotes();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 6)
			{
				deleteNotes();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 8)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				editor.transposeValue = 0;
				windowController.InitializeWindow("Transpose", wind->position, { 16, 10 });
			}
			else if (clickPos.y == 10)
			{
				setNoteSamples();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Transpose")
		{
			if (clickPos.y == 3)
			{
				if (clickPos.x == 2)
					editor.transposeValue += loadedSong.edo;
				if (clickPos.x == 4)
					editor.transposeValue -= loadedSong.edo;
			}
			else if (clickPos.y == 5)
			{
				if (clickPos.x == 2)
					editor.transposeValue++;
				if (clickPos.x == 4)
					editor.transposeValue--;
			}
			else if (clickPos.y == 9)
			{
				if (clickPos.x > 7 && clickPos.x < 15)
				{
					transposeNotes();
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
				}
			}
		}
		else if (wind->name == "Help")
		{
			gui.scrollBars[4].length = 37;
		}
		else if (wind->name == "Export as .WAV")
		{
			gui.scrollBars[4].length = 17;
			if (clickPos.y == 1 && clickPos.x == 1)
			{
				fileNavigator.ExitFile();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 19 && clickPos.x > 28 && clickPos.x < 37) // Save.
			{
				//std::string fileN = fileNavigator.fileNames[editor.selectedFile];
				//fileN.erase(0, 1);
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				for (int name = 0; name < fileNavigator.fileNames.size(); name++)
				{
					std::string indexName = fileNavigator.fileNames[name];
					indexName.erase(0, 1);
					if (indexName == loadedSong.songName + ".wav")
					{
						windowController.InitializeWindow("Overwrite WAV", wind->position, { 20, 16 });
						return;
					}
				}
				RecordSong();
				//SaveSong();
			}
			else if (clickPos.y > 0 && clickPos.x > 0 && clickPos.x < 39)
			{
				if (clickPos.y - 2 + gui.fileListScroll < fileNavigator.fileNames.size())
				{
					if (clickPos.x == 1)
					{
						if (fileNavigator.fileNames[clickPos.y - 2 + gui.fileListScroll].at(0) == '0')
						{
							std::string fileN = fileNavigator.fileNames[clickPos.y - 2 + gui.fileListScroll];
							fileN.erase(0, 1);
							fileNavigator.EnterFile(fileN);
						}
					}
					else
					{
						editor.selectedFile = clickPos.y - 2 + gui.fileListScroll;
					}
				}
			}
		}
		else if (wind->name == "Overwrite WAV")
		{
			if (clickPos.y == 7)
			{
				RecordSong();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Instrument Editor")
		{

			// Synth UI.
			if (clickPos.x > 1 && clickPos.x < 13)
			{
				if (clickPos.y == 2) // Toggle waveform or sample.
				{
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType++;
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType > 2)
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType = 0;
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
						GenerateAdditiveWave();
					sampleDisplay.drawing = false; // Stop sample drawing.
					sampleDisplay.zoomed = false; // Reset zoom.
					loadedSong.unsavedChanges = true;
					return;
				}
				if (clickPos.y == 4) // Change wave shape type
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType++;
						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType > 6)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType = 0;
						GenerateAdditiveWave();
						loadedSong.unsavedChanges = true;
					}
					return;
				}
				if (clickPos.y == 7 && (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType == 1 || loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType == 3)) // Toggle generate from sine waves.
				{
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].generateFromSines = !loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].generateFromSines;
					GenerateAdditiveWave();
					loadedSong.unsavedChanges = true;
					return;
				}
				else if (clickPos.y == 9) // Set sample to loop
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType++;
						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType > 2)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopType = 0;
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart = 0;

						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();
						DrawSampleDisplay();
						loadedSong.unsavedChanges = true;
					}
					return;
				}
				else if (clickPos.y > 21 && clickPos.y < 26) // Toggle modulator input type
				{
					loadedSamples[editor.selectedSample].modulationTypes[int(clickPos.y - 22)]++;
					if (loadedSamples[editor.selectedSample].modulationTypes[int(clickPos.y - 22)] > 5)
						loadedSamples[editor.selectedSample].modulationTypes[int(clickPos.y - 22)] = 0;
					loadedSong.unsavedChanges = true;
					return;
				}
			}



			if (clickPos.y == 10) // Grid snap settings
			{
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
				{
					if (clickPos.x == 8)
						sampleDisplay.enableSnap = !sampleDisplay.enableSnap;

					DrawSampleDisplay();
				}
			}
			else if (clickPos.y == 11) // Toggle reverse sample
			{
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
				{
					if (clickPos.x == 9)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].reverseFrames = !loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].reverseFrames;
						GenerateAdditiveWave();
					}

					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
			}
			else if (clickPos.y == 13) // Toggle sustain forever
			{
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
				{
					if (clickPos.x == 8)
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustainForever = !loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustainForever;

					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
			}
			else if (clickPos.y == 19) // Toggle pitch to note
			{
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
				{
					if (clickPos.x == 15)
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pitchToNote = !loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pitchToNote;

					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
			}
			else if (clickPos.y == 18) // Toggle stereo type
			{
				if (clickPos.x > 7 && clickPos.x < 15)
				{
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].stereo++;
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].stereo > 2)
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].stereo = 0;
					loadedSong.unsavedChanges = true;
				}
			}
			else if (clickPos.y == 20) // Toggle lfo amount
			{
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
				{
					if (clickPos.x > 12 && clickPos.x < 17)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].lfo++;
						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].lfo > 9)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].lfo = 0;
						loadedSong.unsavedChanges = true;
					}
				}
			}
			else if (clickPos.y == 21) // Toggle continue note
			{
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
				{
					if (clickPos.x == 15)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].continueNote = !loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].continueNote;
						loadedSong.unsavedChanges = true;
					}
				}
			}
			

			


			if (loadedSamples[editor.selectedSample].enabled)
			{
				if (clickPos.y > 17 && clickPos.y < 22)
				{
					if (clickPos.x == 32) // Select operator.
					{
						sampleDisplay.drawing = false; // Stop sample drawing.
						sampleDisplay.zoomed = false; // Reset zoom.
						sampleDisplay.selectedOperator = clickPos.y - 18;
						DrawSampleDisplay();
					}
				}

				if (clickPos.x > 17 && clickPos.x < 33) // Toggle displaying frequencies and arpeggios.
				{
					if (clickPos.y == 17)
					{
						sampleDisplay.displayArp = !sampleDisplay.displayArp;
					}
				}

				if (clickPos.y > 17 && clickPos.y < 23 && clickPos.x > 21 && clickPos.x < 30)
				{
					selectAlgorithmOperator({ clickPos.x - 24, clickPos.y - 18});
					return;
				}

				if (clickPos.x > 21 && clickPos.x < 34)
				{
					if (clickPos.y == 24) // Open algorithm menu.
					{
						sampleDisplay.operatorMenuSelectedOperator = loadedSamples[editor.selectedSample].algorithmType;
						windowController.InitializeWindow("Algorithms", wind->position, { 34, 33 });
						return;
					}
				}

				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
				{
					if (clickPos.y > 25 && clickPos.y < 38 && editor.selectedSample > -1) // Sample display
					{
						if (clickPos.y < 27) // Set jump points.
						{
							float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x - 1.0f) * 16.0f;

							if (sampleDisplay.enableSnap)
							{
								posX /= 528.0f;
								posX *= sampleDisplay.snapSubdivisions;
								posX += 0.5f;
								posX = int(posX);
								posX /= sampleDisplay.snapSubdivisions;
								posX *= 528.0f;
							}

							int frameIndex = (posX / 528.0f) * loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

							if (loadedSamples[editor.selectedSample].jumpPoints.size() == 0)
								loadedSamples[editor.selectedSample].jumpPoints.emplace_back(frameIndex);
							else
							{
								bool afterLastJump = false;
								int addPos = 0;
								while (!afterLastJump)
								{
									if (addPos >= loadedSamples[editor.selectedSample].jumpPoints.size())
										afterLastJump = true;
									else if (loadedSamples[editor.selectedSample].jumpPoints[addPos] <= frameIndex)
									{
										afterLastJump = true;
									}
									else
									{
										addPos++;
									}
								}

								loadedSamples[editor.selectedSample].jumpPoints.emplace(loadedSamples[editor.selectedSample].jumpPoints.begin() + addPos, frameIndex);
							}
							loadedSong.unsavedChanges = true;
						}
						else if (clickPos.y > 35) // Move loop points.
						{
							float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x) * 16.0f - 8.0f;
							float lStartPos = float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart) * (528.0f / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()));
							if (abs(lStartPos - posX) < 16)
							{
								sampleDisplay.dragLoopStart = true;
								return;
							}
							else
							{
								float lEndPos = float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd) * (528.0f / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()));
								if (abs(lEndPos - posX) < 16)
								{
									sampleDisplay.dragLoopEnd = true;
									return;
								}
							}
							loadedSong.unsavedChanges = true;
						}
						else
						{
							float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x - 0.5f) / 33.0f;

							if (posX > 1.0f)
								posX = 1.0f;
							else if (posX < 0)
								posX = 0;


							if (sampleDisplay.enableSnap)
							{
								posX *= sampleDisplay.snapSubdivisions;
								posX = int(posX);
								posX /= sampleDisplay.snapSubdivisions;
							}

							int frameIndex = (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) * posX;

							if (sampleDisplay.zoomed)
							{
								float zoomStart = sampleDisplay.zoomStart;
								float zoomEnd = sampleDisplay.zoomEnd;
								float zoomScale = (zoomEnd - zoomStart) / loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

								frameIndex *= zoomScale;
								frameIndex += zoomStart;
							}

							sampleDisplay.sampleStartPos = frameIndex;
							sampleDisplay.sampleSelectionEnd = frameIndex;
						}
						DrawSampleDisplay();
					}
					else if (clickPos.y > 37)
					{
						if (clickPos.x == 1 || clickPos.x == 2) // Play sample
						{
							if (!editor.playingSong) // Play the note sound.
							{
								channels[0].volume = 64.0f;
								StartSample(0, editor.selectedSample, 48, 0);
								for (int wave = 0; wave < 4; wave++)
									channels[0].waveforms[wave].note = 48;
								sampleDisplay.playingSample = true;
								for (int wave = 0; wave < 4; wave++)
									channels[0].waveforms[wave].frameReadPos = sampleDisplay.sampleStartPos;
							}
						}
						else if (clickPos.x == 3 || clickPos.x == 4) // Pause sample
						{
							channels[0].toUninitialize = true;
							sampleDisplay.playingSample = false;
							DrawSampleDisplay();
						}
						else if (clickPos.x == 26 || clickPos.x == 27) // Toggle draw sample
						{
							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
							{
								sampleDisplay.drawing = !sampleDisplay.drawing;
								DrawSampleDisplay();
								loadedSong.unsavedChanges = true;
							}
						}
						else if (clickPos.x == 28 || clickPos.x == 29) // Toggle zoom sample
						{
							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
							{
								if (!sampleDisplay.zoomed && sampleDisplay.sampleStartPos == sampleDisplay.sampleSelectionEnd) // You cannot zoom if there is no selection.
									return;

								sampleDisplay.zoomed = !sampleDisplay.zoomed;

								if (sampleDisplay.zoomed)
								{
									sampleDisplay.zoomStart = sampleDisplay.sampleStartPos;
									sampleDisplay.zoomEnd = sampleDisplay.sampleSelectionEnd;
								}
								else
								{
									sampleDisplay.zoomStart = 0;
									sampleDisplay.zoomEnd = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() - 1;
								}

								DrawSampleDisplay();
								loadedSong.unsavedChanges = true;
							}
						}
						else if (clickPos.y == 38)
						{
							if (clickPos.x > 5 && clickPos.x < 25) // Change sample length display units
							{
								sampleDisplay.measurementSystem++;
								if (sampleDisplay.measurementSystem > 2)
									sampleDisplay.measurementSystem = 0;
							}

							DrawSampleDisplay();
						}
					}
				}
			}
			if (clickPos.x > 21 && clickPos.x < 34)
			{
				if (clickPos.y == 25) // Open preset menu.
				{
					windowController.InitializeWindow("Presets", wind->position, { 48, 18 });
					return;
				}
			}
		}
		else if (wind->name == "Sample Selection")
		{
			if (clickPos.y == 2) // Cut
			{
				int frameDiff = (sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);

				if (frameDiff < 1) // No area selected.
				{
					DrawSampleDisplay();
					windowController.windows.erase(windowController.windows.begin() + windowIndex);
					windowController.windows.shrink_to_fit();
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					loadedSong.unsavedChanges = true;
					return;
				}

				for (int i = sampleDisplay.sampleSelectionEnd; i < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size(); i++)
				{
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[i - frameDiff] = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[i];
				}


				
				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.resize(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() - (sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos));
				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.shrink_to_fit();
				

				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart > sampleDisplay.sampleStartPos)
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart -= (sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd > sampleDisplay.sampleStartPos)
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd -= (sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);
				
				if (sampleDisplay.sampleStartPos > loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())
					sampleDisplay.sampleStartPos = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();


				sampleDisplay.sampleSelectionEnd = sampleDisplay.sampleStartPos;

				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

				DrawSampleDisplay();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				loadedSong.unsavedChanges = true;
			}
			else if (clickPos.y == 4) // Open sample volume window.
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				editor.transposeValue = 0;
				sampleDisplay.resampleNote = 48.0f - loadedSong.edo * 4;
				sampleDisplay.resampleFineTune = 0.0f;
				sampleDisplay.resampleMultiplier = 1.0f;
				windowController.InitializeWindow("Resample Sample", wind->position, { 16, 12 });
			}
			else if (clickPos.y == 6) // Dtlete frames
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				editor.transposeValue = 0;
				windowController.InitializeWindow("Sample Volume", wind->position, { 16, 6 });
				sampleDisplay.volumeMultiplier = 1.0f;
			}
			else if (clickPos.y == 8) // Set loop
			{
				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart = sampleDisplay.sampleStartPos;
				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd = sampleDisplay.sampleSelectionEnd;
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd == loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart) // Make sure that the loop ends are not the same.
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd++;
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				DrawSampleDisplay();
				loadedSong.unsavedChanges = true;
			}
			else if (clickPos.y == 10) // Fade out
			{
				for (int i = sampleDisplay.sampleStartPos; i < sampleDisplay.sampleSelectionEnd; i++)
				{
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[i] *= 1.0f - float(i - sampleDisplay.sampleStartPos) / float(sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);
				}

				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				DrawSampleDisplay();
				loadedSong.unsavedChanges = true;
			}
			else if (clickPos.y == 12) // Fade in
			{
				for (int i = sampleDisplay.sampleStartPos; i < sampleDisplay.sampleSelectionEnd; i++)
				{
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[i] *= float(i - sampleDisplay.sampleStartPos) / float(sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);
				}

				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				DrawSampleDisplay();
				loadedSong.unsavedChanges = true;
			}
			else if (clickPos.y == 14) // Copy frames
			{
				sampleDisplay.copyFrames.clear();
				for (int i = sampleDisplay.sampleStartPos; i < sampleDisplay.sampleSelectionEnd; i++)
					sampleDisplay.copyFrames.emplace_back(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[i]);

				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				DrawSampleDisplay();
				loadedSong.unsavedChanges = true;
			}
			else if (clickPos.y == 16) // Paste frames
			{
				for (int i = 0; i < sampleDisplay.copyFrames.size(); i++)
				{
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.emplace(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.begin() + sampleDisplay.sampleStartPos + i, sampleDisplay.copyFrames[i]);
				}
				
				loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				DrawSampleDisplay();
				loadedSong.unsavedChanges = true;
			}
		}
		else if (wind->name == "Resample Sample")
		{
			bool valUpdated = false;
			if (clickPos.y == 3)
			{
				if (clickPos.x == 2)
				{
					sampleDisplay.resampleFineTune += 0.01f;
					valUpdated = true;
				}
				if (clickPos.x == 4)
				{
					sampleDisplay.resampleFineTune -= 0.01f;
					valUpdated = true;
				}
			}
			else if (clickPos.y == 5)
			{
				if (clickPos.x == 2)
				{
					sampleDisplay.resampleNote++;
					valUpdated = true;
				}
				if (clickPos.x == 4)
				{
					sampleDisplay.resampleNote--;
					valUpdated = true;
				}
			}
			else if (clickPos.y == 9)
			{
				editor.selectedButton = 28;
				gui.selectedWindowTile = clickPos;
				gui.selectedTile.x = clickPos.x + wind->position.x;

			}
			else if (clickPos.y == 11) // Apply the resampling.
			{
				if (clickPos.x > 7 && clickPos.x < 15)
				{
					float multiplier = sampleDisplay.resampleMultiplier;

					std::vector <float> newPcmFrames;
					float selectionSize = (sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos) * sampleDisplay.resampleMultiplier;
					float selectionEndPos = (sampleDisplay.sampleStartPos) + selectionSize;
					float sampleSize = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() + selectionSize - float(sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);

					//newPcmFrames.assign(sampleSize, 0.0f);

					
					float newIndex = 0.0f;
					for (int i = 0; i < sampleSize; i++)
					{
						ma_uint32 index1 = newIndex;
						ma_uint32 index2 = index1 + 1;

						float t = newIndex - index1;  // Fractional part

						if (index1 >= loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())
							index1 = 0;
						if (index2 >= loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())
							index2 = 0;

						float frameVol = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[index1] * (1.0f - t) + loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[index2] * t;

						newPcmFrames.emplace_back(frameVol);


						if (i > sampleDisplay.sampleStartPos && i < selectionEndPos)
							newIndex += 1.0f / multiplier;
						else
							newIndex++;
					}

					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.resize(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames = newPcmFrames;

					loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].fineTuneToC = 1.0f;

					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart > sampleDisplay.sampleStartPos)
					{
						float inSelection = (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart - (sampleDisplay.sampleStartPos)) * sampleDisplay.resampleMultiplier;
						float afterSelection = (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart - (sampleDisplay.sampleSelectionEnd));
						if (afterSelection > 0)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart = selectionEndPos + afterSelection;
						else
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart = (sampleDisplay.sampleStartPos) + inSelection;
					}

					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd > sampleDisplay.sampleStartPos)
					{
						float inSelection = (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd - (sampleDisplay.sampleStartPos)) * sampleDisplay.resampleMultiplier;
						float afterSelection = (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd - (sampleDisplay.sampleSelectionEnd));
						if (afterSelection > 0)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd = selectionEndPos + afterSelection + 1;
						else
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd = (sampleDisplay.sampleStartPos) + inSelection + 1;
					}

					
					sampleDisplay.sampleSelectionEnd = selectionEndPos;

					windowController.windows.erase(windowController.windows.begin() + windowIndex);
					windowController.windows.shrink_to_fit();
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
			}
			if (valUpdated)
			{
				sampleDisplay.resampleMultiplier = (1.0f / pow(2, sampleDisplay.resampleNote / float(loadedSong.edo))) + sampleDisplay.resampleFineTune;
			}
		}
		else if (wind->name == "Sample Volume")
		{
			if (clickPos.y == 3)
			{
				if (clickPos.x == 2)
				{
					sampleDisplay.volumeMultiplier += 0.1f;
				}
				if (clickPos.x == 4)
				{
					sampleDisplay.volumeMultiplier -= 0.1f;
				}
			}
			else if (clickPos.y == 5) // Apply the volume change.
			{
				if (clickPos.x > 7 && clickPos.x < 15)
				{
					for (int i = sampleDisplay.sampleStartPos; i < sampleDisplay.sampleSelectionEnd; i++)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[i] *= sampleDisplay.volumeMultiplier;
					}

					windowController.windows.erase(windowController.windows.begin() + windowIndex);
					windowController.windows.shrink_to_fit();
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
			}
		}
		else if (wind->name == "Algorithms")
		{
			if (clickPos.x > 0 && clickPos.y > 0 && clickPos.x < 34 && clickPos.y < 32)
			{
				int algoX = int(clickPos.x - 2) / 8;
				int algoY = int(clickPos.y - 2) / 6;
				int algo = algoX + algoY * 4;
				sampleDisplay.operatorMenuSelectedOperator = algo;
				return;
			}
			else if (clickPos.y == 32 && clickPos.x > 7 && clickPos.x < 15)
			{
				loadedSamples[editor.selectedSample].algorithmType = sampleDisplay.operatorMenuSelectedOperator;
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				loadedSong.unsavedChanges = true;
				return;
			}

		}
		else if (wind->name == "Presets")
		{
			if (clickPos.x > 0 && clickPos.y > 0 && clickPos.x < 13 && clickPos.y < 13)
			{
				if (int(clickPos.y) % 2 == 1)
				{
					presetMenu.instrumentType = (clickPos.y - 1) / 2;
				}

				return;
			}
			if (clickPos.x > 14 && clickPos.y > 0 && clickPos.y < 17)
			{
				if (clickPos.x < 30)
				{
					presetMenu.selectedSample = clickPos.y - 1;
					return;
				}
				else if (clickPos.x < 48)
				{
					presetMenu.selectedSample = clickPos.y - 1 + 16;
					return;
				}
			}
			else if (clickPos.y == 17 && clickPos.x > 7 && clickPos.x < 15)
			{
				LoadInstrumentPreset(presetMenu.selectedSample, &loadedSamples[editor.selectedSample]);
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				loadedSong.unsavedChanges = true;
				return;
			}
		}
		else if (wind->name == "File")
		{
			if (clickPos.y == 2) // New
			{
				// New song
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				std::string newSongName = "NewSong";
				int songNameNumber = 1;

				for (int i = 0; i < fileNavigator.fileNames.size(); i++)
				{
					std::string songName = fileNavigator.fileNames[i];
					if (songName.at(0) == '1')
					{
						songName.erase(0, 1);
						if (songName == "NewSong" + std::to_string(songNameNumber) + ".song") // Only show .wav files.
							songNameNumber++;
					}
				}

				newSongName.append(std::to_string(songNameNumber));
				newSongName.append(".song");

				LoadSong(newSongName);

				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 4) // Save
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Save Song", wind->position, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 6) // Load
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Load File", wind->position, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 8) // Export
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Export as .WAV", wind->position, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Channel")
		{
			if (clickPos.y == 2)
			{
				channels[editor.selectedChannel].hasVoiceColumns = !channels[editor.selectedChannel].hasVoiceColumns;
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				loadedSong.unsavedChanges = true;
			}
		}
		else if (wind->name == "Phonemes")
		{
			if (clickPos.y > 1 && clickPos.y < 24)
			{
				if (clickPos.x == 2)
				{
					voiceSynth.phonemes[int(clickPos.y - 2)].loop = !voiceSynth.phonemes[int(clickPos.y - 2)].loop;
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					loadedSong.unsavedChanges = true;
				}
				else if (clickPos.x == 14)
				{
					voiceSynth.phonemes[int(clickPos.y - 2) + 22].loop = !voiceSynth.phonemes[int(clickPos.y - 2) + 22].loop;
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					loadedSong.unsavedChanges = true;
				}
				else if (clickPos.x > 2 && clickPos.x < 8)
				{
					voiceSynth.phonemes[int(clickPos.y - 2)].pcmFrames.clear();
					for (int fr = 0; fr < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size(); fr++)
					{
						voiceSynth.phonemes[int(clickPos.y - 2)].pcmFrames.emplace_back(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[fr]);
					}
				}
				else if (clickPos.x > 8 && clickPos.x < 14)
				{
					voiceSynth.phonemes[int(clickPos.y - 2) + 22].pcmFrames.clear();
					for (int fr = 0; fr < loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size(); fr++)
					{
						voiceSynth.phonemes[int(clickPos.y - 2) + 22].pcmFrames.emplace_back(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames[fr]);
					}
				}
			}
		}
	}



	//if (windowIndex > 0) // Move the dragged window to front.
	//{
	//	FloatingWindow moveWind = windowController.windows[windowIndex];
	//	windowController.windows.erase(windowController.windows.begin() + windowIndex);
	//	windowController.windows.emplace(windowController.windows.begin(), moveWind);
	//	windowController.windows.shrink_to_fit();
	//	windowIndex = 0;
	//}

	return;
}



void RightClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{
	clickPos.x = int(clickPos.x);
	clickPos.y = int(clickPos.y);

	std::lock_guard<std::mutex> lock(mtx);



	if (clickPos.y > 0)
	{

		if (wind->name == "Instrument Editor")
		{
			if (loadedSamples[editor.selectedSample].enabled)
			{
				if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
				{
					if (clickPos.y > 25 && clickPos.y < 38 && editor.selectedSample > -1)
					{
						if (clickPos.y < 27) // Delete jump points.
						{
							float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x) * 16.0f - 8.0f;
							for (int i = 0; i < loadedSamples[editor.selectedSample].jumpPoints.size(); i++)
							{
								float iPos = float(loadedSamples[editor.selectedSample].jumpPoints[i]) * (528.0f / float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()));
								//int iPos = (loadedSamples[editor.selectedSample]->jumpPoints[i]) * (1.0f / loadedSamples[editor.selectedSample]->pcmFrames.size()) * 800.0f;
								if (abs(iPos - posX) < 16)
								{
									loadedSamples[editor.selectedSample].jumpPoints.erase(loadedSamples[editor.selectedSample].jumpPoints.begin() + i);
									loadedSamples[editor.selectedSample].jumpPoints.shrink_to_fit();
									DrawSampleDisplay();
									loadedSong.unsavedChanges = true;
									return;
								}
							}
						}
						else // Open the selection menu.
						{
							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
								windowController.InitializeWindow("Sample Selection", wind->position, { 16, 19 });
						}
						DrawSampleDisplay();
					}
				}
			}
		}
	}


	if (windowIndex > 0) // Move the dragged window to front.
	{
		FloatingWindow moveWind = windowController.windows[windowIndex];
		windowController.windows.erase(windowController.windows.begin() + windowIndex);
		windowController.windows.emplace(windowController.windows.begin(), moveWind);
		windowController.windows.shrink_to_fit();
		windowIndex = 0;
	}

	return;
}




void HoldClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{
	//clickPos.x = int(clickPos.x);
	//clickPos.y = int(clickPos.y);
	std::lock_guard<std::mutex> lock(mtx);

	


	if (clickPos.y > 0)
	{

		if (wind->name == "Instrument Editor")
		{
			if (int(clickPos.y) == 2)
			{
				if (clickPos.x > 24 && clickPos.x < 34) // Volume/Arp speed.
				{
					if (sampleDisplay.displayArp)
					{
						loadedSamples[editor.selectedSample].arpSpeed = (float(int((clickPos.x - 25) * 2.0f)) / 16.0f);

						if (loadedSamples[editor.selectedSample].arpSpeed < 0.0f)
							loadedSamples[editor.selectedSample].arpSpeed = 0.0f;
						else if (loadedSamples[editor.selectedSample].arpSpeed > 0.9375f)
							loadedSamples[editor.selectedSample].arpSpeed = 0.9375f;
					}
					else
					{
						loadedSamples[editor.selectedSample].volume = (float(int((clickPos.x - 25) * 2.0f)) / 16.0f);

						if (loadedSamples[editor.selectedSample].volume < 0.0f)
							loadedSamples[editor.selectedSample].volume = 0.0f;
						else if (loadedSamples[editor.selectedSample].volume > 1.0f)
							loadedSamples[editor.selectedSample].volume = 1.0f;
					}
					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
					return;
				}
			}
			if (clickPos.x > 9 && clickPos.x < 18)
			{
				if (int(clickPos.y) == 5) // Edit duty cycle.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
					{
						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType != 4) // Noise has no duty cycle.
						{
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle < 0.0f)
								loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle = 0.0f;
							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle > 1.0f)
								loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].dutyCycle = 1.0f;

							GenerateAdditiveWave();
							loadedSong.unsavedChanges = true;
						}
					}
					return;
				}
				else if (int(clickPos.y) == 6) // Edit smoothness / # of sine waves to generate.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
					{
						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].generateFromSines || loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].waveType > 3)
						{
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f) * 16;

							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves < 1)
								loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves = 1;
							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves > 15)
								loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].numOfSineWaves = 15;
						}
						else
						{
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness < 0.0f)
								loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness = 0.0f;
							if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness > 1.0f)
								loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].smoothness = 1.0f;
						}
						loadedSong.unsavedChanges = true;
						GenerateAdditiveWave();
					}
					return;

				}
				else if (int(clickPos.y) == 10) // Set sample display subdivisions.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
					{
						sampleDisplay.snapSubdivisions = (float(int((clickPos.x - 9) * 4.0f)) / 32.0f) * 64.0f;

						if (sampleDisplay.snapSubdivisions < 0.0f)
							sampleDisplay.snapSubdivisions = 0.0f;
						if (sampleDisplay.snapSubdivisions > 64.0f)
							sampleDisplay.snapSubdivisions = 64.0f;


						DrawSampleDisplay();
					}
					return;
				}
				else if (int(clickPos.y) == 12) // Set sample attack.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].attack = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);;

						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].attack < 0)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].attack = 0;
						else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].attack > 0.9375)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].attack = 0.9375;

						loadedSong.unsavedChanges = true;
						DrawSampleDisplay();
					}
					return;
				}
				else if (int(clickPos.y) == 13 && !loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustainForever) // Set sample sustain.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustain = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustain < 0)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustain = 0;
						else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustain > 0.9375)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].sustain = 0.9375;

						loadedSong.unsavedChanges = true;
						DrawSampleDisplay();
					}
					return;
				}
				else if (int(clickPos.y) == 14) // Set sample decay.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].decay = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].decay < 0)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].decay = 0;
						else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].decay > 0.9375)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].decay = 0.9375;

						loadedSong.unsavedChanges = true;
						DrawSampleDisplay();
					}
					return;
				}
				else if (int(clickPos.y) == 15) // Set sample release.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].release = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].release < 0)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].release = 0;
						else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].release > 0.9375)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].release = 0.9375;

						loadedSong.unsavedChanges = true;
						DrawSampleDisplay();
					}
					return;
				}
				else if (int(clickPos.y) == 16) // Set wave offset.
				{
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset < 0)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset = 0;
						else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset > 0.9375)
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].offset = 0.9375;

						loadedSong.unsavedChanges = true;
						GenerateAdditiveWave();
						DrawSampleDisplay();
					}
					return;
				}
				else if (int(clickPos.y) == 17) // Set sample glide.
				{
					loadedSamples[editor.selectedSample].glide = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

					if (loadedSamples[editor.selectedSample].glide < 0)
						loadedSamples[editor.selectedSample].glide = 0;
					else if (loadedSamples[editor.selectedSample].glide > 0.99)
						loadedSamples[editor.selectedSample].glide = 0.99;

					loadedSong.unsavedChanges = true;
					DrawSampleDisplay();
					return;
				}
			}

			if (clickPos.x > 13 && clickPos.x < 22) // Low-pass filter resonance.
			{
				if (clickPos.y > 22 && clickPos.y < 26)
				{
					if (loadedSamples[editor.selectedSample].modulationTypes[int(clickPos.y) - 22] > 2 && loadedSamples[editor.selectedSample].modulationTypes[int(clickPos.y) - 22] < 5)
					{
						loadedSamples[editor.selectedSample].lPResonances[int(clickPos.y) - 22] = (float(int((clickPos.x - 13) * 2.0f)) / 16.0f);

						if (loadedSamples[editor.selectedSample].lPResonances[int(clickPos.y) - 22] < 0)
							loadedSamples[editor.selectedSample].lPResonances[int(clickPos.y) - 22] = 0;
						else if (loadedSamples[editor.selectedSample].lPResonances[int(clickPos.y) - 22] > 0.9375)
							loadedSamples[editor.selectedSample].lPResonances[int(clickPos.y) - 22] = 0.9375;

						loadedSong.unsavedChanges = true;
						DrawSampleDisplay();
						return;
					}
				}
			}

			if (clickPos.x > 18 && clickPos.x < 33 && clickPos.y > 3 && clickPos.y < 18.0f)
			{
				if (sampleDisplay.displayArp) // Set arp pitches.
				{
					// 0 - 14
					if (clickPos.y > 3.25f && clickPos.y < 15.5f)
					{
						loadedSamples[editor.selectedSample].arpPitches[int(clickPos.x) - 18] = 17.0f - float(int(clickPos.y * 4.0f)) * 0.25f;

						if (loadedSamples[editor.selectedSample].arpPitches[int(clickPos.x) - 18] < 0.0f)
							loadedSamples[editor.selectedSample].arpPitches[int(clickPos.x) - 18] = 0.0f;

						// Reset arpeggiation in channels using this instrument.
						for (int ch = 0; ch < channels.size(); ch++)
						{
							if (channels[ch].instrument == editor.selectedSample)
							{
								channels[ch].arpIndex = -1;
								channels[ch].arpTimer = 0.0f;

								for (int i = 0; i < loadedSamples[editor.selectedSample].arpLength + 1; i++)
								{
									channels[ch].arpP[i] = loadedSamples[editor.selectedSample].arpPitches[i];
								}
							}
						}
						
						loadedSong.unsavedChanges = true;
					}
					else if (clickPos.y > 15.5f && clickPos.y < 17.0f) // Set arp length.
					{
						loadedSamples[editor.selectedSample].arpLength = int(clickPos.x - 17.5f);
						if (loadedSamples[editor.selectedSample].arpLength > 14)
							loadedSamples[editor.selectedSample].arpLength = 14;
						else if (loadedSamples[editor.selectedSample].arpLength < 0)
							loadedSamples[editor.selectedSample].arpLength = 0;
						loadedSong.unsavedChanges = true;
					}
				}
				else if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType == 0) // Change frequencies.
				{
					if (int(clickPos.x) > 21)
					{
						float newFreqVal = 17.0f - float(int(clickPos.y * 4.0f)) * 0.25f;
						if (newFreqVal < 0.0f) newFreqVal = 0.0f;

						if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[int(clickPos.x) - 18 - 4] != newFreqVal)
						{
							loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].frequencies[int(clickPos.x) - 18 - 4] = newFreqVal;
							GenerateAdditiveWave();
							loadedSong.unsavedChanges = true;
						}
					}
				}
			}

			if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].operatorType != 2)
			{
				// Drag loop points.
				if (loadedSamples[editor.selectedSample].enabled)
				{
					float posX = ((gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x) * 16.0f - 8.0f) * (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() / 528.0f);

					if (sampleDisplay.enableSnap)
					{
						posX /= float(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
						posX *= sampleDisplay.snapSubdivisions;
						posX += 0.5f;
						posX = int(posX);
						posX /= sampleDisplay.snapSubdivisions;
						posX *= (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
					}


					if (posX > loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())
						posX = loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();
					else if (posX < 0)
						posX = 0;

					if (sampleDisplay.dragLoopStart)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart = posX;
						DrawSampleDisplay();
						loadedSong.unsavedChanges = true;
					}
					else if (sampleDisplay.dragLoopEnd)
					{
						loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd = posX;
						DrawSampleDisplay();
						loadedSong.unsavedChanges = true;
					}
					if (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart > loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd)
					{
						std::swap(sampleDisplay.dragLoopStart, sampleDisplay.dragLoopEnd);
						std::swap(loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopStart, loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].loopEnd);
					}

					if (clickPos.y > 26 && clickPos.y < 38) // Move sample selection / draw.
					{
						/////////////////////////////////////////////////////////////////////////////////////////////////////////

						float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x - 0.5f) / 33.0f;

						if (posX > 1.0f)
							posX = 1.0f;
						else if (posX < 0)
							posX = 0;

						//////////////////////////////////////////////////////////////////////////////////////////////////////////


						if (sampleDisplay.drawing)
						{
							float posY = -(gui.floatHoveredTile.y - windowController.windows[windowIndex].position.y - 32.0f) / 6.0f;

							int frameIndex = (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) * posX;

							if (sampleDisplay.zoomed)
							{
								float zoomStart = sampleDisplay.zoomStart;
								float zoomEnd = sampleDisplay.zoomEnd;
								float zoomScale = (zoomEnd - zoomStart) / loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

								frameIndex *= zoomScale;
								frameIndex += zoomStart;
							}

							sampleDisplay.drawWavePos = { float(frameIndex), posY };
							DrawSamplePoint(sampleDisplay.drawWavePos);
						}
						else
						{
							if (sampleDisplay.enableSnap)
							{
								posX *= sampleDisplay.snapSubdivisions;
								posX = int(posX);
								posX /= sampleDisplay.snapSubdivisions;
							}

							int frameIndex = (loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) * posX;

							if (sampleDisplay.zoomed)
							{
								float zoomStart = sampleDisplay.zoomStart;
								float zoomEnd = sampleDisplay.zoomEnd;
								float zoomScale = (zoomEnd - zoomStart) / loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

								frameIndex *= zoomScale;
								frameIndex += zoomStart;
							}

							sampleDisplay.sampleSelectionEnd = frameIndex;
							DrawSampleDisplay();
						}
						loadedSong.unsavedChanges = true;
					}
				}
			}
		}
	}



	return;
}



void RightHoldFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{
	clickPos.x = int(clickPos.x);
	clickPos.y = int(clickPos.y);





	



	return;
}




void ChangeTheme(int theme)
{
	int sizeX, sizeY, comps;
	glBindTexture(GL_TEXTURE_2D, gui.uiTexture);
	unsigned char* data;

	if (gui.lightMode)
		data = stbi_load("TilesLight.png", &sizeX, &sizeY, &comps, 3);
	else
		data = stbi_load("Tiles.png", &sizeX, &sizeY, &comps, 3);

	GUITheme currentTheme = gui.themes[theme];
	/*
	if (gui.uiBrightMode)
	{
		for (int i = 0; i < 6; i++)
			currentTheme.uiColors[i] = gui.themes[theme].uiColors[5 - i];
	}*/

	for (int i = 0; i < 512 * 512; i++)
	{
		unsigned char* colPos = data + i * 3;
		if (colPos[0] == 0 && colPos[1] == 0 && colPos[2] == 0)
		{
			data[i * 3] = currentTheme.uiColors[0].r;
			data[i * 3 + 1] = currentTheme.uiColors[0].g;
			data[i * 3 + 2] = currentTheme.uiColors[0].b;
		}
		else if (colPos[0] == 0 && colPos[1] == 0 && colPos[2] == 40)
		{
			data[i * 3] = gui.themes[theme].uiColors[1].r * 0.5f + gui.themes[theme].uiColors[0].r * 0.5f;
			data[i * 3 + 1] = gui.themes[theme].uiColors[1].g * 0.5f + gui.themes[theme].uiColors[0].g * 0.5f;
			data[i * 3 + 2] = gui.themes[theme].uiColors[1].b * 0.5f + gui.themes[theme].uiColors[0].b * 0.5f;
		}
		else if (colPos[0] == 30)
		{
			data[i * 3] = currentTheme.uiColors[1].r;
			data[i * 3 + 1] = currentTheme.uiColors[1].g;
			data[i * 3 + 2] = currentTheme.uiColors[1].b;
		}
		else if (colPos[0] == 50)
		{
			data[i * 3] = currentTheme.uiColors[2].r;
			data[i * 3 + 1] = currentTheme.uiColors[2].g;
			data[i * 3 + 2] = currentTheme.uiColors[2].b;
		}
		else if (colPos[0] == 90)
		{
			data[i * 3] = currentTheme.uiColors[3].r;
			data[i * 3 + 1] = currentTheme.uiColors[3].g;
			data[i * 3 + 2] = currentTheme.uiColors[3].b;
		}
		else if (colPos[0] == 150)
		{
			data[i * 3] = currentTheme.uiColors[4].r;
			data[i * 3 + 1] = currentTheme.uiColors[4].g;
			data[i * 3 + 2] = currentTheme.uiColors[4].b;
		}
		else if (colPos[0] == 255 && colPos[1] == 255 && colPos[2] == 255)
		{
			data[i * 3] = currentTheme.uiColors[5].r;
			data[i * 3 + 1] = currentTheme.uiColors[5].g;
			data[i * 3 + 2] = currentTheme.uiColors[5].b;
		}
		// Accent colors
		else if (colPos[0] == 63 && colPos[1] == 100 && colPos[2] == 100)
		{
			data[i * 3] = currentTheme.uiColors[6].r;
			data[i * 3 + 1] = currentTheme.uiColors[6].g;
			data[i * 3 + 2] = currentTheme.uiColors[6].b;
		}
		else if (colPos[0] == 127 && colPos[1] == 190 && colPos[2] == 0)
		{
			data[i * 3] = currentTheme.uiColors[7].r;
			data[i * 3 + 1] = currentTheme.uiColors[7].g;
			data[i * 3 + 2] = currentTheme.uiColors[7].b;
		}
		else if (colPos[0] == 255 && colPos[1] == 255 && colPos[2] == 0)
		{
			data[i * 3] = currentTheme.uiColors[8].r;
			data[i * 3 + 1] = currentTheme.uiColors[8].g;
			data[i * 3 + 2] = currentTheme.uiColors[8].b;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	// Set the ui colors.
	gui.uiColors[0] = gui.themes[theme].uiColors[0].r / 255.0f;
	gui.uiColors[1] = gui.themes[theme].uiColors[0].g / 255.0f;
	gui.uiColors[2] = gui.themes[theme].uiColors[0].b / 255.0f;

	gui.uiColors[3] = gui.themes[theme].uiColors[1].r / 255.0f;
	gui.uiColors[4] = gui.themes[theme].uiColors[1].g / 255.0f;
	gui.uiColors[5] = gui.themes[theme].uiColors[1].b / 255.0f;

	gui.uiColors[6] = gui.themes[theme].uiColors[2].r / 255.0f;
	gui.uiColors[7] = gui.themes[theme].uiColors[2].g / 255.0f;
	gui.uiColors[8] = gui.themes[theme].uiColors[2].b / 255.0f;

	gui.uiColors[9] = gui.themes[theme].uiColors[3].r / 255.0f;
	gui.uiColors[10] = gui.themes[theme].uiColors[3].g / 255.0f;
	gui.uiColors[11] = gui.themes[theme].uiColors[3].b / 255.0f;

	gui.uiColors[12] = gui.themes[theme].uiColors[4].r / 255.0f;
	gui.uiColors[13] = gui.themes[theme].uiColors[4].g / 255.0f;
	gui.uiColors[14] = gui.themes[theme].uiColors[4].b / 255.0f;

	gui.uiColors[15] = gui.themes[theme].uiColors[5].r / 255.0f;
	gui.uiColors[16] = gui.themes[theme].uiColors[5].g / 255.0f;
	gui.uiColors[17] = gui.themes[theme].uiColors[5].b / 255.0f;


	gui.uiColors[45] = gui.themes[theme].uiColors[7].r / 255.0f;
	gui.uiColors[46] = gui.themes[theme].uiColors[7].g / 255.0f;
	gui.uiColors[47] = gui.themes[theme].uiColors[7].b / 255.0f;

	gui.uiColors[48] = gui.themes[theme].uiColors[7].r / 255.0f;
	gui.uiColors[49] = gui.themes[theme].uiColors[7].g / 255.0f;
	gui.uiColors[50] = gui.themes[theme].uiColors[7].b / 255.0f;

	gui.uiColors[51] = gui.themes[theme].uiColors[8].r / 255.0f;
	gui.uiColors[52] = gui.themes[theme].uiColors[8].g / 255.0f;
	gui.uiColors[53] = gui.themes[theme].uiColors[8].b / 255.0f;

	gui.uiColorTheme = theme;

	if (sampleDisplay.visible)
	{
		DrawSampleDisplay();
		
		gui.drawFrameThisFrame = true;
		DrawFrameBorder();
		//DrawEverything();
		
	}
}


void selectAlgorithmOperator(Vector2 pos)
{
	int selectedOp = -1;

	switch (loadedSamples[editor.selectedSample].algorithmType)
	{
	case 0:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		break;
	}

	case 1:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		break;
	}

	case 2:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 1;
		break;
	}

	case 3:
	{
		if (pos.x == 1 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 5 && pos.y == 3)
			selectedOp = 2;
		break;
	}

	case 4:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 2;
		break;
	}

	case 5:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 2;
		break;
	}

	case 6:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 0)
			selectedOp = 2;
		break;
	}

	case 7:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 2;
		break;
	}

	case 8:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 2)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 5 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 9:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 0)
			selectedOp = 2;
		else if (pos.x == 4 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 10:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 2)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 2)
			selectedOp = 2;
		else if (pos.x == 2 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 11:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 12:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 3 && pos.y == 2)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 13:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 14:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 1 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 5 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 15:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 16:
	{
		if (pos.x == 1 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 5 && pos.y == 3)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 17:
	{
		if (pos.x == 1 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 5 && pos.y == 3)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 18:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 1 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 19:
	{
		if (pos.x == 0 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 2;
		else if (pos.x == 6 && pos.y == 3)
			selectedOp = 3;
		break;
	}

	default:
		break;
	}

	

	if (selectedOp > -1)
	{
		

		loadedSamples[editor.selectedSample].operatorWavesToUse[selectedOp]++;
		if (loadedSamples[editor.selectedSample].operatorWavesToUse[selectedOp] > 3)
			loadedSamples[editor.selectedSample].operatorWavesToUse[selectedOp] = 0;



		
	}


	return;
}