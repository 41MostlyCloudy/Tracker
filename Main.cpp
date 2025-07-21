
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



// Function Prototypes
void processInput(GLFWwindow* window);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods);

void character_callback(GLFWwindow* window, unsigned int codepoint);

void RunEngine();

void pressButton();

void releaseButton();





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
    loadedFrame.rows.resize(64);

    // Set buttons to unpressed state.
    releaseButton();
    

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Start the delta timer
        std::chrono::high_resolution_clock time;
        auto start = time.now();

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


    // Hover actions
    /*
    for (int x = 61; x < 65; x++)
        activeUI[x][0].bgCol = 0;
    if (hoveredTile.y == 0)
    {
        if (hoveredTile.x > 60 && hoveredTile.x < 66)
        {
            for (int x = 61; x < 65; x++)
                activeUI[x][0].bgCol = 2;
        }
    }*/


    //activeUI[int(hoveredTile.x)][int(hoveredTile.y)].sprite = { 0, 0 };

    glfwSetKeyCallback(window, key_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetCharCallback(window, character_callback);

    //glfwSetScrollCallback(window, scroll_callback);

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
        }
        else
        {

        }
    }

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);

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

    if ((input > 39 && input < 42) || (input > 47 && input < 58) || (input > 64 && input < 91) || (input > 96 && input < 123) || input == 32)
    {
        if (selectedButton == 0 && loadedSong.songName.length() < 20)
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

    return;
}


void pressButton()
{
    selectedButton = -1;

    selectedTile = hoveredTile; // Select the tile that the mouse is currently on




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

    
    if (hoveredTile.y > 1 && hoveredTile.y < 11) // Select song frame.
    {
        if (hoveredTile.x > 0 && hoveredTile.x < 6)
        {
            loadedSong.currentFrame = hoveredTile.y - 2 + frameListScroll;
            if (loadedSong.currentFrame >= loadedSong.frameSequence.size()) // Snap to end of song.
                loadedSong.currentFrame = loadedSong.frameSequence.size() - 1;
        }
    }

    
    if (hoveredTile.y == 2)
    {
        if (hoveredTile.x == 6) // Frame menu scroll up.
        {
            activeUI[6][10].sprite = { 7, 4 };
            if (frameListScroll > 0)
                frameListScroll--;
            activeUI[6][2].sprite = { 7, 3 };
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
            loadedSong.frameSequence[loadedSong.currentFrame]++;
            while (loadedSong.frameSequence[loadedSong.currentFrame] >= loadedSong.frames.size()) // Create a new frame when changed to one not yet used.
            {
                Frame newFrame;
                loadedSong.frames.emplace_back(newFrame);
            }
            activeUI[7][5].sprite = { 8, 4 };
            activeUI[8][5].sprite = { 9, 4 };
        }
        else if (hoveredTile.x == 9) // Change song frame.
        {
            loadedSong.frameSequence[loadedSong.currentFrame]--;
            if (loadedSong.frameSequence[loadedSong.currentFrame] < 0)
                loadedSong.frameSequence[loadedSong.currentFrame] = 0;
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
            loadedSong.frameSequence.emplace(loadedSong.frameSequence.begin() + loadedSong.currentFrame, loadedSong.frameSequence[loadedSong.currentFrame]);
            activeUI[7][8].sprite = { 8, 4 };
            activeUI[8][8].sprite = { 9, 4 };
            loadedSong.currentFrame++;
        }
        else if (hoveredTile.x == 9)
        {
            // Delete the current frame if there is more than one in the song.
            if (loadedSong.frameSequence.size() > 1)
            {
                loadedSong.frameSequence.erase(loadedSong.frameSequence.begin() + loadedSong.currentFrame);
                activeUI[4][2 + loadedSong.currentFrame].bgCol = 0;
                activeUI[5][2 + loadedSong.currentFrame].bgCol = 0;
                loadedSong.frameSequence.shrink_to_fit();
                if (loadedSong.currentFrame >= loadedSong.frameSequence.size())
                    loadedSong.currentFrame--;
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
            activeUI[15][9].sprite = { 10, 4 };
            activeUI[14][9].sprite = { 9, 4 };
        }
    }
    else if (hoveredTile.y == 10)
    {
        if (hoveredTile.x == 6) // Frame menu scroll up.
        {
            activeUI[6][10].sprite = { 7, 4 };
            if (loadedSong.frameSequence.size() - frameListScroll > 1)
                frameListScroll++;
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
    }
}


int main(void)
{
    RunEngine();


    return 0;
}