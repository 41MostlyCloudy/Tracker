
// SUPERSOUND





#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "GlobalVariables.h"
#include "Shaders.h"
#define STB_IMAGE_IMPLEMENTATION // Image loading library made by Sean Barrett. Loads PNGs Modified to remove most parts having to do with other file formats.
#include "stb_image.h"
#include "Draw.cpp"
#include "FileHandling.cpp"
#include "FrameHandling.cpp"
#include "SongHandling.cpp"



// Function Prototypes
void processInput(GLFWwindow* window);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods);

void character_callback(GLFWwindow* window, unsigned int codepoint);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void RunEngine();

void pressButton();

void releaseButton();

//unsigned char* ChangeTheme();





void RunEngine()
{
    // Set up OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    screenSize.y = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
    screenSize.x = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;

    windowRatio = (736.0f / 456.0f) * (screenSize.y / screenSize.x);


    // Create the window
    GLFWwindow* window = glfwCreateWindow(screenSize.x, screenSize.y, "", glfwGetPrimaryMonitor(), nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, screenSize.x, screenSize.y);


    ////////////////////////////////////////////////////////////////////////////// Set up the sprite mesh
    float spriteScreenWidth = 1.0f / 92.0f;
    float spriteScreenHeight = 1.0f / 57.0f;

    float spriteVertices[] = {
    -spriteScreenWidth, spriteScreenHeight, 0.0, 0.0,
     spriteScreenWidth, spriteScreenHeight, 1.0 / 32, 0.0,
    -spriteScreenWidth, -spriteScreenHeight, 0.0, 1.0 / 32,
     spriteScreenWidth, -spriteScreenHeight, 1.0 / 32, 1.0 / 32,
     spriteScreenWidth, spriteScreenHeight, 1.0 / 32, 0.0,
    -spriteScreenWidth, -spriteScreenHeight, 0.0, 1.0f / 32,
    };
    ////////////////////////////////////////////////////////////////////////////// Set up the sprite VAO
    unsigned int sVBO;
    unsigned int sVAO;

    glGenBuffers(1, &sVBO);
    glGenVertexArrays(1, &sVAO);

    // 1. bind Vertex Array Object
    glBindVertexArray(sVAO);
    // Bind the buffer to the VBO
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);

    // Use the buffer to fill the VBO with the vertices
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), spriteVertices, GL_STATIC_DRAW);
    // Assign how vertex attributes should be interpreted.

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ////////////////////////////////////////////////////////////////////////////// Create and compile the vertex shaders
    unsigned int uiVertexShader;
    uiVertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Compile the shader
    glShaderSource(uiVertexShader, 1, &uiVertexShaderSource, NULL);
    glCompileShader(uiVertexShader);
    ////////////////////////////////////////////////////////////////////////////// Create and compile the fragment shaders
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Create the shader programs
    unsigned int uiShaderProgram;
    uiShaderProgram = glCreateProgram();

    glAttachShader(uiShaderProgram, uiVertexShader);
    glAttachShader(uiShaderProgram, fragmentShader);
    glLinkProgram(uiShaderProgram);
    // Use the screen shader.
    glUseProgram(uiShaderProgram);

    // Load the ui texture map.
    int sizeX, sizeY, comps;
    unsigned int uiTexture;
    glGenTextures(1, &uiTexture);
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    unsigned char* data = stbi_load("Tiles.png", &sizeX, &sizeY, &comps, 3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512,  512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



    // Initialize ui tiles.
    for (int y = 0; y < 57; y++)
    {
        for (int x = 0; x < 92; x++)
            activeUI[x][y] = {15, 0};
    }


    // Create the initial song.
    Frame firstFrame;
    loadedSong.frames.emplace_back(firstFrame);
    loadedSong.frameSequence.emplace_back(0);
    loadedFrame.rows.resize(32);

    // Set buttons to unpressed state.
    releaseButton();

    // Show the samples currently in the "Samples" file.
    LoadSamples();

    // Set up the audio engine.
    SetUpAudioEngine();
    

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Start the delta timer
        std::chrono::high_resolution_clock time;
        auto start = time.now();


        if (playingSong) // Play the song.
            stepSong();

        if (mouseDown)
        {
            mouseHoldTime += delta;
            if (mouseHoldTime > 500.0f) // Repeatedly press button after holding for a while.
            {
                if (int((mouseHoldTime - delta) * 0.01) % 2 != int((mouseHoldTime) * 0.01) % 2)
                    pressButton();
            }
        }

        drawScreen = false;

        // Draw stuff
        DrawEverything();

        // Process player input
        processInput(window);

        // Poll for and process events
        glfwPollEvents();


        // Do stuff
        
        


        // Draw the screen
        if (drawScreen)
        {
            GLint ratioInUIShader = glGetUniformLocation(uiShaderProgram, "windowRatio");
            glUniform1f(ratioInUIShader, windowRatio);

            int indexX = 0;
            int indexY = 0;

            for (int x = 0; x < (5244 / 50) + 1; x++)
            {
                float uiPos[200];

                float uiText[100];
                float uiBg[100];

                float spriteCount = 0;


                for (int i = 0; i < 50; i++)
                {
                    if (indexY > 56)
                    {
                        uiPos[i * 4] = 0;
                        uiPos[i * 4 + 1] = 0;
                        uiPos[i * 4 + 2] = 0;
                        uiPos[i * 4 + 3] = 0;

                        uiText[i] = 0;
                        uiBg[i] = 0;
                    }
                    else
                    {
                        uiPos[i * 4] = indexX;
                        uiPos[i * 4 + 1] = indexY;
                        uiPos[i * 4 + 2] = float(activeUI[indexX][indexY].sprite.x);
                        uiPos[i * 4 + 3] = float(activeUI[indexX][indexY].sprite.y);

                        uiText[i] = float(activeUI[indexX][indexY].textCol);
                        uiBg[i] = float(activeUI[indexX][indexY].bgCol);
                        spriteCount++;
                    }


                    indexX++;
                    if (indexX > 91)
                    {
                        indexX = 0;
                        indexY++;
                    }
                    
                }

                GLint posInShader2 = glGetUniformLocation(uiShaderProgram, "spriteOffsets");
                glUniform4fv(posInShader2, spriteCount, uiPos);

                GLint posInShader3 = glGetUniformLocation(uiShaderProgram, "textCol");
                glUniform1fv(posInShader3, spriteCount, uiText);

                GLint posInShader4 = glGetUniformLocation(uiShaderProgram, "bgCol");
                glUniform1fv(posInShader4, spriteCount, uiBg);

                GLint ratioInUIShader = glGetUniformLocation(uiShaderProgram, "windowRatio");
                glUniform1f(ratioInUIShader, windowRatio);


                glBindTexture(GL_TEXTURE_2D, uiTexture);
                //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, uiSpriteAtlasBuffer);
                glBindVertexArray(sVAO);
                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, spriteCount); // Draw the sprites.

                
            }
        }

        // Swap front and back buffers
        glfwSwapBuffers(window);


        // Find the frame time
        auto end = time.now();
        using ms = std::chrono::duration<float, std::milli>;
        delta = std::chrono::duration_cast<ms>(end - start).count();
    }


    glfwTerminate();
    return;
}


void processInput(GLFWwindow* window)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (mouseX < 0) glfwSetCursorPos(window, 0, mouseY);
    if (mouseX > screenSize.x) glfwSetCursorPos(window, screenSize.x, mouseY);

    if (mouseY < 0) glfwSetCursorPos(window, mouseX, 0);
    if (mouseY > screenSize.y) glfwSetCursorPos(window, mouseX, screenSize.y);

    glfwGetCursorPos(window, &mouseX, &mouseY);

    mouseCoords.x = (mouseX / screenSize.x) * 736;
    mouseCoords.y = (mouseY / screenSize.y) * 456;

    mouseCoords.x -= 368;
    mouseCoords.x /= windowRatio;
    mouseCoords.x += 368;

    hoveredTile = { float(int(mouseCoords.x / 8)), float(int(mouseCoords.y / 8)) };

    // Clamp the room selection to the map.
    if (hoveredTile.x < 0)
        hoveredTile.x = 0;
    else if (hoveredTile.x > 91)
        hoveredTile.x = 91;
    if (hoveredTile.y < 0)
        hoveredTile.y = 0;
    else if (hoveredTile.y > 56)
        hoveredTile.y = 56;


    if (mouseDown) // Select note (End selection)
    {
        if (hoveredTile.y > 15 && hoveredTile.y < 56 && hoveredTile.x > 3 && hoveredTile.x < 91)
        {
            int mouseTileX = hoveredTile.x - 4;
            int mouseTileY = hoveredTile.y - 16 + frameScroll;

            if (mouseTileX <= noteSelectionStart.x)
                noteSelectionStart.x = mouseTileX;
            else if (mouseTileX >= noteSelectionStart.x)
                noteSelectionEnd.x = mouseTileX;

            if (mouseTileY <= noteSelectionStart.y)
                noteSelectionStart.y = mouseTileY;
            else if (mouseTileY >= noteSelectionStart.y)
                noteSelectionEnd.y = mouseTileY;

            loadedSong.currentNote = mouseTileY;
        }
    }


    
    
    glfwSetKeyCallback(window, key_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetCharCallback(window, character_callback);

    glfwSetScrollCallback(window, scroll_callback);

    if (loadedSong.currentNote >= loadedFrame.rows.size()) // Snap the current selected note to the end of the frame.
        loadedSong.currentNote = loadedFrame.rows.size() - 1;
    if (noteSelectionStart.y >= loadedFrame.rows.size())
        noteSelectionStart.y = loadedFrame.rows.size() - 1;
    if (noteSelectionEnd.y >= loadedFrame.rows.size())
        noteSelectionEnd.y = loadedFrame.rows.size() - 1;

    return;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_BACKSPACE)
        {
            if (selectedButton == 0) // Delete text
            {
                int selectedChar = selectedTile.x - 45;
                if (loadedSong.songName.length() > 0 && selectedChar > -1)
                {
                    if (selectedChar == loadedSong.songName.length())
                    {
                        loadedSong.songName = loadedSong.songName.substr(0, selectedChar);
                    }
                    else
                    {
                        loadedSong.songName = loadedSong.songName.substr(0, selectedChar) + loadedSong.songName.substr(selectedChar + 1, loadedSong.songName.length());
                    }
                    
                    if (selectedChar > 0)
                        selectedTile.x--;
                }
            }
            else if (editing) // Delete note
            {
                for (int y = noteSelectionStart.y; y <= noteSelectionEnd.y; y++)
                {
                    for (int x = noteSelectionStart.x; x <= noteSelectionEnd.x; x++)
                    {
                        int selectedChannel = int((x) / 11.0f);
                        int selectedPart = int(x) % 11;

                        if (selectedPart < 3)
                            loadedFrame.rows[y].note[selectedChannel] = -1;
                        else if (selectedPart < 5)
                            loadedFrame.rows[y].instrument[selectedChannel] = -1;
                        else if (selectedPart < 7)
                            loadedFrame.rows[y].volume[selectedChannel] = -1;
                        if (selectedPart < 8)
                            loadedFrame.rows[y].effect[selectedChannel] = -1;
                        else if (selectedPart < 10)
                            loadedFrame.rows[y].effectValue[selectedChannel] = -1;
                    }
                }
            }
        }
        
        if (key == GLFW_KEY_UP || key == GLFW_KEY_KP_8) // Move up
        {
            if (editing)
            {
                if (frameScroll > 0)
                    frameScroll--;
                if (loadedSong.currentNote > 0)
                    loadedSong.currentNote--;
                noteSelectionStart.y = loadedSong.currentNote;
                noteSelectionEnd.y = loadedSong.currentNote;
                activeUI[91][16].sprite = { 7, 3 };
            }

            
        }
        if (key == GLFW_KEY_DOWN || key == GLFW_KEY_KP_5) // Move down
        {
            if (editing)
            {
                if (frameScroll < loadedFrame.rows.size() - 40)
                    frameScroll++;
                if (loadedSong.currentNote < loadedFrame.rows.size() - 1)
                    loadedSong.currentNote++;
                noteSelectionStart.y = loadedSong.currentNote;
                noteSelectionEnd.y = loadedSong.currentNote;
                activeUI[91][55].sprite = { 7, 4 };
            }
        }
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_KP_4) // Move left
        {
            if (editing)
            {
                if (noteSelectionStart.x > 0)
                {
                    int selectedPart = int(noteSelectionStart.x) % 11;

                    if (selectedPart == 0)
                    {
                        noteSelectionStart.x -= 2;
                    }
                    else
                    {
                        noteSelectionStart.x -= 1;
                    }
                    noteSelectionEnd.x = noteSelectionStart.x;
                }
            }
        }
        if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_KP_6) // Move left
        {
            if (editing)
            {
                if (noteSelectionStart.x < 86)
                {
                    int selectedPart = int(noteSelectionStart.x) % 11;

                    if (selectedPart == 9)
                    {
                        noteSelectionStart.x += 2;
                    }
                    else
                    {
                        noteSelectionStart.x += 1;
                    }
                    noteSelectionEnd.x = noteSelectionStart.x;
                }
            }
        }

        if (key == GLFW_KEY_SPACE) // Start/stop song
        {
            playingSong = !playingSong;

            if (inHelpPage) // Don't play the song if editing.
                playingSong = false;

            editing = !playingSong;
            saveCurrentFrame();

            if (playingSong) // Restart the frame when playing the song.
            {
                loadedSong.currentNote = 0;
                loadedSong.timeInNote = 1000000.0f;
                loadedSong.timeInSong = 0.0f;
                frameScroll = 0.0f;
                for (int ch = 0; ch < 8; ch++)
                {
                    channels[ch].volume = 1;
                    channels[ch].effect = -1;
                    channels[ch].effectX = 0;
                    channels[ch].effectY = 0;
                    channels[ch].pitch = 0;
                    loadedSong.noteChannelIndex[ch] = 0;
                    loadedSong.volumeChannelIndex[ch] = 0;
                    loadedSong.effectChannelIndex[ch] = 0;
                    loadedSong.toNextChannelNote[ch] = 0;
                    loadedSong.toNextChannelVolume[ch] = 0;
                    loadedSong.toNextChannelEffect[ch] = 0;
                }

                if (recordingSong) // Set up recording.
                {
                    std::string fileName = "Export/" + loadedSong.songName + ".wav";
                    const char* name = &fileName[0];
                    ma_encoder_init_file(name, &encoderConfig, &encoder);
                }
            }
            else
            {
                for (int ch = 0; ch < 8; ch++)
                {
                    channels[ch].volume = 0;
                    //ma_sound_set_volume(&channelAudio[ch], channels[ch].volume);
                }

                if (recordingSong) // Stop recording.
                {
                    ma_encoder_uninit(&encoder);
                    recordingSong = false;
                }
            }
        }

        if (key == GLFW_KEY_ENTER && editing) // Return to start of song
        {
            loadedSong.currentNote = 0;
            frameScroll = 0.0f;
        }

        if (key == GLFW_KEY_TAB) // Create stop note
        {
            int selectedChannel = int((noteSelectionStart.x) / 11.0f);
            int selectedPart = int(noteSelectionStart.x) % 11;

            if (selectedPart < 3) // Playing keys
            {
                loadedFrame.rows[loadedSong.currentNote].note[selectedChannel] = 255;

                if (loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] < 0)
                    loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] = selectedSample;
            }
        }

        if (key == GLFW_KEY_LEFT_SHIFT && editing) // Copy selected notes.
        {
            // Create mouse selection frame.
            frameSelection.rows.clear();
            frameSelection.rows.resize(int(noteSelectionEnd.y + 1 - noteSelectionStart.y));
            int leftMostChannel = int((noteSelectionStart.x) / 11.0f);

            for (int y = 0; y < frameSelection.rows.size(); y++)
            {
                for (int x = 0; x < 8; x++)
                {
                    frameSelection.rows[y].note[x] = -2;
                    frameSelection.rows[y].instrument[x] = -2;
                    frameSelection.rows[y].volume[x] = -2;
                    frameSelection.rows[y].effect[x] = -2;
                    frameSelection.rows[y].effectValue[x] = -2;
                }
                for (int x = noteSelectionStart.x; x < noteSelectionEnd.x; x++)
                {
                    int selectedChannel = int((x) / 11.0f);
                    int selectedPart = int(x) % 11;

                    if (selectedPart < 3)
                        frameSelection.rows[y].note[selectedChannel - leftMostChannel] = loadedFrame.rows[y + noteSelectionStart.y].note[selectedChannel];
                    else if (selectedPart < 5)
                        frameSelection.rows[y].instrument[selectedChannel - leftMostChannel] = loadedFrame.rows[y + noteSelectionStart.y].instrument[selectedChannel];
                    else if (selectedPart < 7)
                        frameSelection.rows[y].volume[selectedChannel - leftMostChannel] = loadedFrame.rows[y + noteSelectionStart.y].volume[selectedChannel];
                    if (selectedPart < 8)
                        frameSelection.rows[y].effect[selectedChannel - leftMostChannel] = loadedFrame.rows[y + noteSelectionStart.y].effect[selectedChannel];
                    else if (selectedPart < 10)
                        frameSelection.rows[y].effectValue[selectedChannel - leftMostChannel] = loadedFrame.rows[y + noteSelectionStart.y].effectValue[selectedChannel];
                }
            }
        }

        if (key == GLFW_KEY_RIGHT_SHIFT && editing) // Paste selected notes.
        {
            int leftMostChannel = int((noteSelectionStart.x) / 11.0f);

            for (int y = 0; y < frameSelection.rows.size(); y++)
            {
                if (y + int(noteSelectionStart.y) < loadedFrame.rows.size())
                {
                    for (int ch = 0; ch < 8; ch++)
                    {
                        if (ch + +leftMostChannel < 8)
                        {
                            if (frameSelection.rows[y].note[ch] > -2)
                                loadedFrame.rows[y + int(noteSelectionStart.y)].note[ch + leftMostChannel] = frameSelection.rows[y].note[ch];
                            if (frameSelection.rows[y].instrument[ch] > -2)
                                loadedFrame.rows[y + int(noteSelectionStart.y)].instrument[ch + leftMostChannel] = frameSelection.rows[y].instrument[ch];
                            if (frameSelection.rows[y].volume[ch] > -2)
                                loadedFrame.rows[y + int(noteSelectionStart.y)].volume[ch + leftMostChannel] = frameSelection.rows[y].volume[ch];
                            if (frameSelection.rows[y].effect[ch] > -2)
                                loadedFrame.rows[y + int(noteSelectionStart.y)].effect[ch + leftMostChannel] = frameSelection.rows[y].effect[ch];
                            if (frameSelection.rows[y].effectValue[ch] > -2)
                                loadedFrame.rows[y + int(noteSelectionStart.y)].effectValue[ch + leftMostChannel] = frameSelection.rows[y].effectValue[ch];
                        }
                    }
                }
            }
        }
    }

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);


    if (action == GLFW_PRESS || action == GLFW_RELEASE)
        keyDown = false;

    return;
}


void mouse_button_callback(GLFWwindow* window, int key, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Mouse click actions
        if (key == GLFW_MOUSE_BUTTON_LEFT)
        {
            mouseDown = true;
            pressButton();
        }
    }

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_MOUSE_BUTTON_LEFT && editing)
        {
            if (hoveredTile.y > 15 && hoveredTile.y < 56 && hoveredTile.x > 3 && hoveredTile.x < 91) // Select note
            {
                noteSelectionStart = { hoveredTile.x - 4, hoveredTile.y - 16 + frameScroll };
                noteSelectionEnd = { hoveredTile.x - 4, hoveredTile.y - 16 + frameScroll };
                loadedSong.currentNote = hoveredTile.y - 16 + frameScroll;



                int selectedChannel = int((noteSelectionStart.x) / 11.0f);
                int selectedPart = int(noteSelectionStart.x) % 11;

                if (selectedPart == 7)
                {
                    loadedFrame.rows[loadedSong.currentNote].effect[selectedChannel] = selectedEffect;
                    if (loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] < 0)
                        loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] = 0;
                    if (selectedEffect == -1) // Delete effect value when deleting effect.
                        loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] = -1;
                }
            }
        }
    }


    if (action == GLFW_RELEASE)
    {
        // Mouse click actions
        if (key == GLFW_MOUSE_BUTTON_LEFT)
        {
            releaseButton();
        }
    }

    return;
}


void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    int input = codepoint;

    bool writing = false;

    // Text writing
    if ((input > 39 && input < 42) || (input > 47 && input < 58) || (input > 64 && input < 91) || (input > 96 && input < 123) || input == 32)
    {
        if (selectedButton == 0)
        {
            writing = true;
            if (loadedSong.songName.length() < 20)
            {
                int selectedChar = selectedTile.x - 45;
                char input = codepoint;

                if (loadedSong.songName.length() == 0)
                    loadedSong.songName = input;
                else if (selectedChar >= loadedSong.songName.length() - 1)
                {
                    loadedSong.songName = loadedSong.songName + input;
                }
                else
                {
                    loadedSong.songName = loadedSong.songName.substr(0, selectedChar + 1) + input + loadedSong.songName.substr(selectedChar + 1, loadedSong.songName.length());
                }

                selectedTile.x++;
            }
        }
    }

    if (!writing && editing)
    {
        int selectedChannel = int((noteSelectionStart.x) / 11.0f);
        int selectedPart = int(noteSelectionStart.x) % 11;
        

        if (selectedPart < 3) // Playing keys
        {
            int noteNum = -1;

            if (input == 122) // z (C 1)
                noteNum += 1;
            else if (input == 115) // s (C# 1)
                noteNum += 2;
            else if (input == 120) // x (D 1)
                noteNum += 3;
            else if (input == 100) // d (D# 1)
                noteNum += 4;
            else if (input == 99) // c (E 1)
                noteNum += 5;
            else if (input == 118) // v (F 1)
                noteNum += 6;
            else if (input == 103) // g (F# 1)
                noteNum += 7;
            else if (input == 98) // b (G 1)
                noteNum += 8;
            else if (input == 104) // h (G# 1)
                noteNum += 9;
            else if (input == 110) // n (A 2)
                noteNum += 10;
            else if (input == 106) // j (A# 2)
                noteNum += 11;
            else if (input == 109) // m (B 2)
                noteNum += 12;
            else if (input == 44) // , (C 2)
                noteNum += 13;
            else if (input == 108) // l (C# 2)
                noteNum += 14;
            else if (input == 46) // . (D 2)
                noteNum += 15;
            else if (input == 59) // ; (D# 2)
                noteNum += 16;
            else if (input == 47) // / (E 2)
                noteNum += 17;

            selectedKey = noteNum;


            if (input == 113) // q (C 2)
                noteNum += 13;
            else if (input == 50) // 2 (C# 2)
                noteNum += 14;
            else if (input == 119) // w (D 2)
                noteNum += 15;
            else if (input == 51) // 3 (D# 2)
                noteNum += 16;
            else if (input == 101) // e (E 2)
                noteNum += 17;
            else if (input == 114) // r (F 2)
                noteNum += 18;
            else if (input == 53) // 5 (F# 2)
                noteNum += 19;
            else if (input == 116) // t (G 2)
                noteNum += 20;
            else if (input == 54) // 6 (G# 2)
                noteNum += 21;
            else if (input == 121) // y (A 3)
                noteNum += 22;
            else if (input == 55) // 7 (A# 3)
                noteNum += 23;
            else if (input == 117) // u (B 3)
                noteNum += 24;
            else if (input == 105) // i (C 3)
                noteNum += 25;
            else if (input == 57) // 9 (C# 3)
                noteNum += 26;
            else if (input == 111) // o (D 3)
                noteNum += 27;
            else if (input == 48) // 0 (D# 3)
                noteNum += 28;
            else if (input == 112) // p (E 3)
                noteNum += 29;
            else if (input == 91) // [ (F 3)
                noteNum += 30;
            else if (input == 61) // = (F# 3)
                noteNum += 31;
            else if (input == 93) // ] (A 3)
                noteNum += 32;

            if (selectedKey == -1)
            {
                if (noteNum == -1)
                    selectedKey = -1;
                else
                    selectedKey = noteNum + 5;
            }
            if (noteNum != -1)
            {
                noteNum += selectedOctave * 12;

                if (!keyDown)
                {
                    channels[selectedChannel].volume = 64.0f;
                    StartSample(selectedChannel, selectedSample, noteNum, 0);
                }
                loadedFrame.rows[loadedSong.currentNote].note[selectedChannel] = noteNum;

                loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] = selectedSample;
            }
        }
        else if (selectedPart == 3 || selectedPart == 4) // Instrument
        {
            int val = 0;

            if (input > 47 && input < 58)
                val = input - 48;
            else if (input > 96 && input < 103)
                val = input - 97 + 10;
            else
                return;

            if (loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] < 0)
                loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] = 0;

            if (selectedPart == 3)
            {
                int d1 = loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] % 16;
                loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] = val * 16 + d1;
            }
            else if (selectedPart == 4)
            {
                int d2 = loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] - loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] % 16;
                loadedFrame.rows[loadedSong.currentNote].instrument[selectedChannel] = d2 + val;
            }
        }
        else if (selectedPart == 5 || selectedPart == 6) // Volume
        {
            int val = 0;

            if (input > 47 && input < 58)
                val = input - 48;
            else if (input > 96 && input < 103)
                val = input - 97 + 10;
            else
                return;

            if (loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] < 0)
                loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] = 0;

            if (selectedPart == 5)
            {
                int d1 = loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] % 16;
                loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] = val * 16 + d1;

                if (loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] > 64)
                    loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] = 64;
            }
            else if(selectedPart == 6)
            {
                int d2 = loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] - loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] % 16;
                loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] = d2 + val;

                if (loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] > 64)
                    loadedFrame.rows[loadedSong.currentNote].volume[selectedChannel] = 64;
            }
        }
        else if (selectedPart == 8 || selectedPart == 9) // Effect value
        {
            int val = 0;

            if (input > 47 && input < 58)
                val = input - 48;
            else if (input > 96 && input < 103)
                val = input - 97 + 10;
            else
                return;

            if (loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] < 0)
                loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] = 0;

            if (loadedFrame.rows[loadedSong.currentNote].effect[selectedChannel] < 0)
                loadedFrame.rows[loadedSong.currentNote].effect[selectedChannel] = 0;

            if (selectedPart == 8)
            {
                int d1 = loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] % 16;
                loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] = val * 16 + d1;
            }
            else
            {
                int d2 = loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] - loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] % 16;
                loadedFrame.rows[loadedSong.currentNote].effectValue[selectedChannel] = d2 + val;
            }
        }

        

    }

    keyDown = true;

    return;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    frameScroll -= yoffset * 1.0f;

    if (loadedFrame.rows.size() > 40)
    {
        if (frameScroll > loadedFrame.rows.size() - 40)
            frameScroll = loadedFrame.rows.size() - 40;

        if (frameScroll < 0)
            frameScroll = 0;
    }
    else
        frameScroll = 0;

    frameScroll = int(frameScroll);

    return;
}


void pressButton()
{
    selectedButton = -1;

    selectedTile = hoveredTile; // Select the tile that the mouse is currently on


    if (hoveredTile.y > 9 && hoveredTile.y < 12 && hoveredTile.x > 23 && hoveredTile.x < 26 && editing) // Record
    {
        recordingSong = !recordingSong;
    }

    if (hoveredTile.y == 9 && hoveredTile.x > 29 && hoveredTile.x < 34 && editing) // Read Help Page
    {
        inHelpPage = !inHelpPage;
        frameScroll = 0.0f; // Reset frame scroll.
        editing = !inHelpPage;

        if (inHelpPage)
        {
            saveCurrentFrame();
            loadedFrame.rows.resize(helpPageText.size());
        }
        else
            loadCurrentFrame();
    }


    if (hoveredTile.y > 12 && hoveredTile.y < 15 && hoveredTile.x > 3) // Mute/unmute
    {
        int selectedChannel = int((hoveredTile.x - 4.0f) / 11.0f);
        channels[selectedChannel].muted = !channels[selectedChannel].muted;
    }


    if (hoveredTile.y == 2 && hoveredTile.x > 44 && hoveredTile.x < 65) // Edit Song Name
    {
        selectedButton = 0;
        if (selectedTile.x - 45 >= loadedSong.songName.length()) // Snap to end of text.
            selectedTile.x = 44 + loadedSong.songName.length();
    }

    if (hoveredTile.y > 0 && hoveredTile.y < 5) // Select effect.
    {
        if (hoveredTile.x > 38 && hoveredTile.x < 43)
        {
            int newEffect = int(hoveredTile.x - 39) + int(hoveredTile.y - 1) * 4;
            if (newEffect == selectedEffect)
                selectedEffect = -1;
            else
                selectedEffect = int(hoveredTile.x - 39) + int(hoveredTile.y - 1) * 4;
        }
    }

    
    if (hoveredTile.y > 1 && hoveredTile.y < 11 && editing) // Select song frame.
    {
        if (hoveredTile.x > 0 && hoveredTile.x < 6)
        {
            saveCurrentFrame();
            loadedSong.currentFrame = hoveredTile.y - 2 + frameListScroll;
            if (loadedSong.currentFrame >= loadedSong.frameSequence.size()) // Snap to end of song.
                loadedSong.currentFrame = loadedSong.frameSequence.size() - 1;
            loadCurrentFrame();
        }
    }

    if (hoveredTile.y == 0)  
    {
        if (hoveredTile.x > 60 && hoveredTile.x < 65) // Save song.
        {
            SaveSong();
        }
        else if (hoveredTile.x > 66 && hoveredTile.x < 80) // Swap between loading samples and songs.
        {
            filesSampleNotSong = !filesSampleNotSong;
            if (filesSampleNotSong)
                LoadSamples();
            else
                LoadSongFiles();
        }
    }

    if (hoveredTile.y > 1 && hoveredTile.y < 12) // Select file / Select sample.
    {
        if (hoveredTile.x > 65 && hoveredTile.x < 77) // Select file
        {
            selectedFile = hoveredTile.y - 2 + fileListScroll;
            if (selectedFile >= fileNameList.size()) // Snap to end of song.
                selectedFile = fileNameList.size() - 1;
        }
        if (hoveredTile.x == 66) // Load sample/Song
        {
            if (filesSampleNotSong)
            {
                Sample newSample;
                newSample.sampleName = fileNameList[selectedFile];
                loadedSamples.emplace_back(newSample);
            }
            else
                LoadSong(fileNameList[selectedFile]);
        }
        else if (hoveredTile.x > 77 && hoveredTile.x < 91) // Select sample
        {
            

            if (selectedSample == hoveredTile.y - 2 + sampleListScroll && hoveredTile.x == 90 && loadedSamples.size() > 0) // Delete sample.
            {
                loadedSamples.erase(loadedSamples.begin() + selectedSample);
                loadedSamples.shrink_to_fit();
                selectedSample--;
                if (selectedSample < 0)
                    selectedSample = 0;
            }
            else
                selectedSample = hoveredTile.y - 2 + sampleListScroll;

            if (selectedSample >= loadedSamples.size()) // Snap to end of song.
                selectedSample = loadedSamples.size() - 1;
        }
    }

    
    if (hoveredTile.y == 2)
    {
        if (hoveredTile.x == 6) // Frame menu scroll up.
        {
            if (frameListScroll > 0)
                frameListScroll--;
            activeUI[6][2].sprite = { 7, 3 };
        }
        else if (hoveredTile.x == 77) // File menu scroll up.
        {
            if (fileListScroll > 0)
                fileListScroll--;
            activeUI[77][2].sprite = { 7, 3 };
        }
        else if (hoveredTile.x == 91) // Sample menu scroll up.
        {
            if (sampleListScroll > 0)
                sampleListScroll--;
            activeUI[91][2].sprite = { 7, 3 };
        }
    }
    else if (hoveredTile.y == 3) // Change song BPM.
    {
        if (hoveredTile.x == 13)
        {
            loadedSong.bpm++;
            if (loadedSong.bpm > 255)
                loadedSong.bpm = 255;
            activeUI[13][3].sprite = { 8, 4 };
            activeUI[14][3].sprite = { 9, 4 };
        }
        else if (hoveredTile.x == 15)
        {
            loadedSong.bpm--;
            if (loadedSong.bpm < 0)
                loadedSong.bpm = 0;
            activeUI[15][3].sprite = { 10, 4 };
            activeUI[14][3].sprite = { 9, 4 };
        }
    }
    else if (hoveredTile.y == 5)
    {
        if (hoveredTile.x == 7) // Change song frame.
        {
            saveCurrentFrame();
            loadedSong.frameSequence[loadedSong.currentFrame]++;
            while (loadedSong.frameSequence[loadedSong.currentFrame] >= loadedSong.frames.size()) // Create a new frame when changed to one not yet used.
            {
                Frame newFrame;
                loadedSong.frames.emplace_back(newFrame);
            }
            loadCurrentFrame();
            activeUI[7][5].sprite = { 8, 4 };
            activeUI[8][5].sprite = { 9, 4 };
        }
        else if (hoveredTile.x == 9) // Change song frame.
        {
            saveCurrentFrame();
            loadedSong.frameSequence[loadedSong.currentFrame]--;
            if (loadedSong.frameSequence[loadedSong.currentFrame] < 0)
                loadedSong.frameSequence[loadedSong.currentFrame] = 0;
            loadCurrentFrame();
            activeUI[9][5].sprite = { 10, 4 };
            activeUI[8][5].sprite = { 9, 4 };
        }
        else if (hoveredTile.x == 13) // Change song TPB.
        {
            loadedSong.ticksPerBeat++;
            if (loadedSong.ticksPerBeat > 255)
                loadedSong.ticksPerBeat = 255;
            activeUI[13][5].sprite = { 8, 4 };
            activeUI[14][5].sprite = { 9, 4 };
        }
        else if (hoveredTile.x == 15) // Change song TPB.
        {
            loadedSong.ticksPerBeat--;
            if (loadedSong.ticksPerBeat < 0)
                loadedSong.ticksPerBeat = 0;
            activeUI[15][5].sprite = { 10, 4 };
            activeUI[14][5].sprite = { 9, 4 };
        }
    }
    else if (hoveredTile.y == 7) // Change song TPB.
    {
        if (hoveredTile.x == 13)
        {
            selectedOctave++;
            if (selectedOctave > 7)
                selectedOctave = 7;
            activeUI[13][7].sprite = { 8, 4 };
            activeUI[14][7].sprite = { 9, 4 };
        }
        else if (hoveredTile.x == 15)
        {
            selectedOctave--;
            if (selectedOctave < 0)
                selectedOctave = 0;
            activeUI[15][7].sprite = { 10, 4 };
            activeUI[14][7].sprite = { 9, 4 };
        }
    }
    else if (hoveredTile.y == 8) // Add/remove frame from song.
    {
        if (hoveredTile.x == 7)
        {
            // Add a duplicate of the selected frame after it.
            saveCurrentFrame();
            loadedSong.frameSequence.emplace(loadedSong.frameSequence.begin() + loadedSong.currentFrame, loadedSong.frameSequence[loadedSong.currentFrame]);
            activeUI[7][8].sprite = { 8, 4 };
            activeUI[8][8].sprite = { 9, 4 };
            loadedSong.currentFrame++;
            loadCurrentFrame();
        }
        else if (hoveredTile.x == 9)
        {
            // Delete the current frame if there is more than one in the song.
            saveCurrentFrame();
            if (loadedSong.frameSequence.size() > 1)
            {
                loadedSong.frameSequence.erase(loadedSong.frameSequence.begin() + loadedSong.currentFrame);
                activeUI[4][2 + loadedSong.currentFrame].bgCol = 0;
                activeUI[5][2 + loadedSong.currentFrame].bgCol = 0;
                loadedSong.frameSequence.shrink_to_fit();
                if (loadedSong.currentFrame >= loadedSong.frameSequence.size())
                    loadedSong.currentFrame--;
                loadCurrentFrame();
            }
        }
    }
    else if (hoveredTile.y == 9) // Change frame row count.
    {
        if (hoveredTile.x == 13)
        {
            if (loadedFrame.rows.size() < 255) // Max frame size.
            {
                FrameRow newRow;
                loadedFrame.rows.emplace_back(newRow);
                saveCurrentFrame();
                activeUI[13][9].sprite = { 8, 4 };
                activeUI[14][9].sprite = { 9, 4 };
            }
        }
        else if (hoveredTile.x == 15) // Min frame size.
        {
            if (loadedFrame.rows.size() > 1)
            {
                loadedFrame.rows.erase(loadedFrame.rows.begin() + loadedFrame.rows.size() - 1);
                loadedFrame.rows.shrink_to_fit();
            }
            saveCurrentFrame();
            activeUI[15][9].sprite = { 10, 4 };
            activeUI[14][9].sprite = { 9, 4 };
        }
    }
    else if (hoveredTile.y == 10)
    {
        if (hoveredTile.x == 6) // Frame menu scroll down.
        {
            activeUI[6][10].sprite = { 7, 4 };
            if (loadedSong.frameSequence.size() - frameListScroll > 1)
                frameListScroll++;
        }
    }
    else if (hoveredTile.y == 11)
    {
        if (hoveredTile.x == 77) // File menu scroll down.
        {
            activeUI[77][11].sprite = { 7, 4 };
            if (fileNameList.size() - fileListScroll > 1)
                fileListScroll++;
        }
        else if (hoveredTile.x == 91) // File menu scroll down.
        {
            activeUI[91][11].sprite = { 7, 4 };
            if (loadedSamples.size() - sampleListScroll > 1)
                sampleListScroll++;
        }
    }
    else if (hoveredTile.y == 16)
    {
        if (hoveredTile.x == 91) // Frame scroll up.
        {
            frameScroll--;
            if (frameScroll < 0)
                frameScroll = 0;
            activeUI[91][16].sprite = { 7, 3 };
            if (frameScroll < 0)
                frameScroll = 0;
        }
    }
    else if (hoveredTile.y == 55)
    {
        if (hoveredTile.x == 91) // Frame scroll down.
        {
            frameScroll++;
            activeUI[91][55].sprite = { 7, 4 };
            if (loadedFrame.rows.size() > 40)
            {
                if (frameScroll > loadedFrame.rows.size() - 40)
                    frameScroll = loadedFrame.rows.size() - 40;
            }
            else
                frameScroll = 0;
        }
    }


    return;
}


void releaseButton()
{
    mouseHoldTime = 0.0f;
    mouseDown = false;

    for (int y = 0; y < 4; y++) // Revert buttons to unpressed.
    {
        activeUI[13][3 + 2 * y].sprite = { 8, 3 };
        activeUI[14][3 + 2 * y].sprite = { 9, 3 };
        activeUI[15][3 + 2 * y].sprite = { 10, 3 };
        activeUI[7][5].sprite = { 8, 3 };
        activeUI[8][5].sprite = { 9, 3 };
        activeUI[9][5].sprite = { 10, 3 };
        activeUI[7][8].sprite = { 8, 3 };
        activeUI[8][8].sprite = { 9, 3 };
        activeUI[9][8].sprite = { 10, 3 };
        // Frame menu scroll arrows
        activeUI[6][2].sprite = { 6, 3 };
        activeUI[6][10].sprite = { 6, 4 };
        // Frame scroll arrows
        activeUI[91][16].sprite = { 6, 3 };
        activeUI[91][55].sprite = { 6, 4 };
        // File scroll arrows
        activeUI[77][2].sprite = { 6, 3 };
        activeUI[77][11].sprite = { 6, 4 };
        // Sample scroll arrows
        activeUI[91][2].sprite = { 6, 3 };
        activeUI[91][11].sprite = { 6, 4 };
    }
}


int main(void)
{
    RunEngine();


    return 0;
}


/*
unsigned char* ChangeTheme()
{
    uiColorTheme++;
    if (uiColorTheme > 2)
        uiColorTheme = 0;

    // Load the ui texture map.
    int sizeX, sizeY, comps;
    unsigned int uiTexture;
    glGenTextures(1, &uiTexture);
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    unsigned char* data = stbi_load("Tiles.png", &sizeX, &sizeY, &comps, 3);

    for (int i = 0; i < sizeX * sizeY; i++)
    {
        if (uiColorTheme == 1)
        {
            int oldBlue = data[(3 * i) + 2];
            data[(3 * i) + 2] = data[3 * i];
            data[3 * i] = oldBlue;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return data;
}*/