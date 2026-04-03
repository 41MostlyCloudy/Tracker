
// SUPERSOUND








#include "GlobalVariables.h"
#include "Shaders.h"
#define STB_IMAGE_IMPLEMENTATION // Image loading library made by Sean Barrett. Loads PNGs Modified to remove most parts having to do with other file formats.
#include "stb_image.h"
#include "Draw.cpp"
#include "FileHandling.cpp"
#include "PatternHandling.cpp"
#include "SongHandling.cpp"
#include "PromptWindowPrompts.cpp"



// The main window.
GLFWwindow* mainWindow;


// Function Prototypes
void processInput(GLFWwindow* window);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods);

void character_callback(GLFWwindow* window, unsigned int codepoint);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void RunEngine();

void pressButton(GLFWwindow* window);

void pressAndHoldButton(GLFWwindow* window);

void rightClickButton(GLFWwindow* window);

void releaseButton();

void releaseRightButton();





void RunEngine()
{
    // Set up OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    

    // Create a windowed mode window and its OpenGL context
    //screen.screenSize.y = 912.0f;
    //screen.screenSize.x = 1472.0f;
    //mainWindow = glfwCreateWindow(screen.screenSize.x, screen.screenSize.y, "Dual Tracker", NULL, NULL);

    screen.screenSize.y = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
    screen.screenSize.x = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;

    screen.windowRatio = (736.0f / 456.0f) * (screen.screenSize.y / screen.screenSize.x);
    

    // Create the window
    mainWindow = glfwCreateWindow(screen.screenSize.x, screen.screenSize.y, "Dual Tracker", glfwGetPrimaryMonitor(), nullptr);
    glfwMakeContextCurrent(mainWindow);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, screen.screenSize.x, screen.screenSize.y);


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

    ////////////////////////////////////////////////////////////////////////////// Create and compile the vertex shader
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
    glDeleteShader(uiVertexShader);
    glDeleteShader(fragmentShader);
    glLinkProgram(uiShaderProgram);
    // Use the screen shader.
    glUseProgram(uiShaderProgram);

    // Load the ui texture map.
    int sizeX, sizeY, comps;
    glGenTextures(1, &gui.uiTexture);
    glBindTexture(GL_TEXTURE_2D, gui.uiTexture);
    unsigned char* data = stbi_load("Tiles.png", &sizeX, &sizeY, &comps, 3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512,  512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    

    ///////////////////////////////////////////////////////////////////////////////////// (Initialize the sample display)
    float sampleDisplayVertices[] = {
        -1.0, -1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, -1.0, 1.0, 0.0,
        -1.0, 1.0f, 0.0, 1.0,
    };

    for (int i = 0; i < 6; i++)
    {
        sampleDisplayVertices[4 * i] *= 528.0f / 1472.0f;
        sampleDisplayVertices[4 * i + 1] *= 192.0f / 912.0f;
    }

    

    unsigned int sampleVBO;
    unsigned int sampleVAO;

    glGenBuffers(1, &sampleVBO);
    glGenVertexArrays(1, &sampleVAO);

    // 1. bind Vertex Array Object
    glBindVertexArray(sampleVAO);
    // Bind the buffer to the VBO
    glBindBuffer(GL_ARRAY_BUFFER, sampleVBO);

    // Use the buffer to fill the VBO with the vertices
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), sampleDisplayVertices, GL_STATIC_DRAW);
    // Assign how vertex attributes should be interpreted.

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);



    // Create and compile the vertex shaders
    unsigned int sampleVertexShader;
    sampleVertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Compile the shader
    glShaderSource(sampleVertexShader, 1, &sampleVertexShaderSource, NULL);
    glCompileShader(sampleVertexShader);
    // Create and compile the fragment shaders
    unsigned int sampleFragmentShader;
    sampleFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sampleFragmentShader, 1, &sampleFragmentShaderSource, NULL);
    glCompileShader(sampleFragmentShader);
    // Create the shader programs
    unsigned int sampleShaderProgram;
    sampleShaderProgram = glCreateProgram();

    glAttachShader(sampleShaderProgram, sampleVertexShader);
    glAttachShader(sampleShaderProgram, sampleFragmentShader);
    glDeleteShader(sampleVertexShader);
    glDeleteShader(sampleFragmentShader);
    glLinkProgram(sampleShaderProgram);
    // Use the screen shader.
    glUseProgram(sampleShaderProgram);


    // Create Screen Texture
    unsigned int sampleTex;
    glGenTextures(1, &sampleTex);
    glBindTexture(GL_TEXTURE_2D, sampleTex);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);




    /////////////////////////////////////////////////////////////////////////////////////



    // Initialize channels.
    channels.resize(loadedSong.numberOfChannels);
    // Initialize decoders.
    ResizeDecoders(loadedSong.numberOfChannels);

    

    // Create the initial song.
    //LoadSong("NewSong.wav");
    Pattern firstFrame;
    firstFrame.channels.resize(loadedSong.numberOfChannels);
    loadedSong.patterns.emplace_back(firstFrame);
    loadedSong.patternSequence.emplace_back(0);


    loadedPattern = resizeUnrolledPatternRows(loadedPattern, 32);

    

    saveCurrentPattern();

    loadCurrentPattern();

    
    
    //unrollFrame(firstFrame);
    //LoadSong("Bob.song");
    
    

    // Set buttons to unpressed state.
    releaseButton();
    releaseRightButton();

    

    // Set up the audio engine.
    SetUpAudioEngine();

    // Load GUI Theme.
    LoadSettings();
    LoadGUIThemes();
    ChangeTheme(gui.uiColorTheme);

    fileNavigator.NavigateToSamplesFile();


    
    

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Start the delta timer
        std::chrono::high_resolution_clock time;
        auto start = time.now();


        


        //if (editor.playingSong) // Play the song.
            //stepSongOutCallback(delta);
            //stepSong();

        if (editor.toRecordSong)
        {
            std::lock_guard<std::mutex> lock(mtx);
            editor.toRecordSong = false;
            RecordSong();
            for (int wind = 0; wind < windowController.windows.size(); wind++) // Stop dragging windows.
            {
                if (windowController.windows[wind].name == "Exporting...")
                {
                    windowController.windows.erase(windowController.windows.begin() + wind);
                    windowController.windows.shrink_to_fit();
                    break;
                }
            }

            continue;
        }
        

        gui.drawScreen = false;

        
       
        // Process player input
        processInput(mainWindow);


        if (screen.mouseDown)
        {
            screen.mouseHoldTime += delta;
            if (screen.mouseHoldTime > 500.0f) // Repeatedly press button after holding for a while.
            {
                if (int((screen.mouseHoldTime - delta) * 0.01) % 2 != int((screen.mouseHoldTime) * 0.01) % 2)
                    pressAndHoldButton(mainWindow);
            }
        }
        
        // Poll for and process events
        glfwPollEvents();

        // Draw stuff
        DrawEverything();
        

        // Set the program not to draw the interface by default.
        gui.drawUIThisFrame = false;
        gui.drawFrameThisFrame = false;
        
        

        
        // Draw the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (gui.drawScreen)
        {
            glUseProgram(uiShaderProgram);
            glBindTexture(GL_TEXTURE_2D, gui.uiTexture);
            glBindVertexArray(sVAO);

            GLint ratioInUIShader = glGetUniformLocation(uiShaderProgram, "windowRatio");
            glUniform1f(ratioInUIShader, screen.windowRatio);

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
                        uiPos[i * 4 + 2] = float(gui.activeUI[indexX][indexY].sprite.x);
                        uiPos[i * 4 + 3] = float(gui.activeUI[indexX][indexY].sprite.y);

                        uiText[i] = float(gui.activeUI[indexX][indexY].textCol);
                        uiBg[i] = float(gui.activeUI[indexX][indexY].bgCol);
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
                glUniform1f(ratioInUIShader, screen.windowRatio);

                GLint colorsInUIShader = glGetUniformLocation(uiShaderProgram, "uiColors");
                glUniform3fv(colorsInUIShader, 18, gui.uiColors);

                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, spriteCount); // Draw the sprites.

                
            }





            glUseProgram(uiShaderProgram);
            glBindTexture(GL_TEXTURE_2D, gui.uiTexture);
            glBindVertexArray(sVAO);


            
            


            

            //GLint ratioInUIShader = glGetUniformLocation(uiShaderProgram, "windowRatio");
            //glUniform1f(ratioInUIShader, screen.windowRatio);



            for (int i = windowController.windows.size() - 1; i > -1; i--) // Draw windows.
            {
                for (int y = windowController.windows[i].position.y; y <= windowController.windows[i].size.y + windowController.windows[i].position.y; y++)
                {
                    float uiPos[200];

                    float uiText[100];
                    float uiBg[100];

                    float spriteCount = windowController.windows[i].size.x;

                    int xPos = windowController.windows[i].position.x;

                    for (int x = 0; x <= windowController.windows[i].size.x; x++)
                    {
                        uiPos[x * 4] = x + xPos;
                        uiPos[x * 4 + 1] = y;
                        uiPos[x * 4 + 2] = float(gui.activeUI[x + xPos][y].sprite.x);
                        uiPos[x * 4 + 3] = float(gui.activeUI[x + xPos][y].sprite.y);

                        uiText[x] = float(gui.activeUI[x + xPos][y].textCol);
                        uiBg[x] = float(gui.activeUI[x + xPos][y].bgCol);
                    }

                    

                    GLint posInShader2 = glGetUniformLocation(uiShaderProgram, "spriteOffsets");
                    glUniform4fv(posInShader2, spriteCount, uiPos);

                    GLint posInShader3 = glGetUniformLocation(uiShaderProgram, "textCol");
                    glUniform1fv(posInShader3, spriteCount, uiText);

                    GLint posInShader4 = glGetUniformLocation(uiShaderProgram, "bgCol");
                    glUniform1fv(posInShader4, spriteCount, uiBg);


                    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, spriteCount); // Draw the sprites.


                }

                
                if (sampleDisplay.visible && windowController.windows[i].name == "Instrument Editor")
                {
                    glUseProgram(sampleShaderProgram);

                    glBindTexture(GL_TEXTURE_2D, sampleTex);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 528, 192, 0, GL_RGB, GL_UNSIGNED_BYTE, sampleDisplay.pixelData);
                    glBindVertexArray(sampleVAO);

                    GLint ratioInUIShader3 = glGetUniformLocation(sampleShaderProgram, "windowRatio");
                    glUniform1f(ratioInUIShader3, screen.windowRatio);


                    GLint posInShader3 = glGetUniformLocation(sampleShaderProgram, "offset");
                    glUniform2f(posInShader3, sampleDisplay.position.x, sampleDisplay.position.y + 0.5f);

                    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1); // Draw the sprites.


                    glUseProgram(uiShaderProgram);
                    glBindTexture(GL_TEXTURE_2D, gui.uiTexture);
                    glBindVertexArray(sVAO);
                }

                /*
                if (i == 0) // Draw scrollbars.
                {
                    if (windowController.windows[i].name == "Load File" || windowController.windows[i].name == "Save Song" || windowController.windows[i].name == "Export as .WAV")
                    {
                        glUseProgram(scrollBarShaderProgram);

                        GLint ratioInUIShader2 = glGetUniformLocation(scrollBarShaderProgram, "windowRatio");
                        glUniform1f(ratioInUIShader2, screen.windowRatio);



                        float uiPos[4];

                        uiPos[0] = gui.scrollBars[4].topLeft.x;
                        uiPos[1] = gui.scrollBars[4].topLeft.y + gui.scrollBars[4].position;

                        uiPos[2] = 30.0f;
                        uiPos[3] = 2.0f;

                        GLint posInShader2 = glGetUniformLocation(scrollBarShaderProgram, "spriteOffset");
                        glUniform4f(posInShader2, uiPos[0], uiPos[1], uiPos[2], uiPos[3]);

                        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1); // Draw the sprites. 
                    }

                    glUseProgram(uiShaderProgram);
                    glBindTexture(GL_TEXTURE_2D, gui.uiTexture);
                    glBindVertexArray(sVAO);
                }*/
            }
        }

        
        // Swap front and back buffers
        glfwSwapBuffers(mainWindow);

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


    glfwGetCursorPos(window, &mouseX, &mouseY);

    screen.mouseCoords.x = (mouseX / screen.screenSize.x) * 736;
    screen.mouseCoords.y = (mouseY / screen.screenSize.y) * 456;

    if (screen.windowRatio < 1.0f)
    {
        screen.mouseCoords.x -= 368;
        screen.mouseCoords.x /= screen.windowRatio;
        screen.mouseCoords.x += 368;
    }
    else
    {
        screen.mouseCoords.y -= 228;
        screen.mouseCoords.y *= screen.windowRatio;
        screen.mouseCoords.y += 228;
    }

    if (screen.mouseCoords.x < 0)
        screen.mouseCoords.x = 0;
    else if (screen.mouseCoords.x > 735)
        screen.mouseCoords.x = 735;
    if (screen.mouseCoords.y < 0)
        screen.mouseCoords.y = 0;
    else if (screen.mouseCoords.y > 456)
        screen.mouseCoords.y = 455;

    gui.hoveredTile = { screen.mouseCoords.x / 8, screen.mouseCoords.y / 8 };

    // Clamp the room selection to the map.
    //if (gui.hoveredTile.x < 0) gui.hoveredTile.x = 0;
    //else if (gui.hoveredTile.x > 91) gui.hoveredTile.x = 91;
    //if (gui.hoveredTile.y < 0) gui.hoveredTile.y = 0;
    //else if (gui.hoveredTile.y > 56) gui.hoveredTile.y = 56;

    gui.floatHoveredTile = gui.hoveredTile;

    gui.hoveredTile.x = int(gui.hoveredTile.x);
    gui.hoveredTile.y = int(gui.hoveredTile.y);
    
    

    
    if (screen.mouseDown) // Select note (End selection)
    {
        for (int wind = 0; wind < windowController.windows.size(); wind++) // Drag windows.
        {
            if (windowController.windows[wind].dragWindow)
            {
                windowController.windows[wind].position.x = gui.hoveredTile.x - windowController.windows[wind].dragPoint.x;
                windowController.windows[wind].position.y = gui.hoveredTile.y - windowController.windows[wind].dragPoint.y;

                if (windowController.windows[wind].position.x < 0) windowController.windows[wind].position.x = 0;
                if (windowController.windows[wind].position.x > 91 - windowController.windows[wind].size.x) windowController.windows[wind].position.x = 91 - windowController.windows[wind].size.x;
                if (windowController.windows[wind].position.y < 0) windowController.windows[wind].position.y = 0;
                if (windowController.windows[wind].position.y > 56 - windowController.windows[wind].size.y) windowController.windows[wind].position.y = 56 - windowController.windows[wind].size.y;

                gui.drawFrameThisFrame = true;
                gui.drawUIThisFrame = true;
            }
        }




        if (!editor.playingSong)
        {
            if (!gui.draggingScrollBar && gui.hoveredTile.y > 15 && gui.hoveredTile.y < 56 && gui.hoveredTile.x > 3 && gui.hoveredTile.x < 91 && !gui.clickingOnFloatingWind)
            {
                int mouseTileX = gui.hoveredTile.x - 5 + gui.patternScroll.x;
                int mouseTileY = gui.hoveredTile.y - 16 + gui.patternScroll.y;

                if (mouseTileX <= editor.noteSelectionStart.x)
                    editor.noteSelectionStart.x = mouseTileX;
                else if (mouseTileX >= editor.noteSelectionStart.x)
                    editor.noteSelectionEnd.x = mouseTileX;

                if (mouseTileY <= editor.noteSelectionStart.y)
                    editor.noteSelectionStart.y = mouseTileY;
                else if (mouseTileY >= editor.noteSelectionStart.y)
                    editor.noteSelectionEnd.y = mouseTileY;

                loadedSong.currentNote = mouseTileY;

                gui.drawFrameThisFrame = true;
            }

        
            if (loadedSong.currentNote >= loadedPattern.rows.size()) // Snap the current selected note to the end of the frame.
                loadedSong.currentNote = loadedPattern.rows.size() - 1;
            if (editor.noteSelectionStart.y >= loadedPattern.rows.size())
                editor.noteSelectionStart.y = loadedPattern.rows.size() - 1;
            if (editor.noteSelectionEnd.y >= loadedPattern.rows.size())
                editor.noteSelectionEnd.y = loadedPattern.rows.size() - 1;
        }
    }
    else
    {
        /*
        for (int bar = 0; bar < gui.scrollBars.size(); bar++) // Stop dragging scroll bars.
        {
            gui.scrollBars[bar].drag = false;
        }*/
    }



    // Drag mouse on floating windows.
    if (screen.mouseDown)
    {
        if (gui.patternListScrollBar.drag) // Scroll pattern list.
        {
            gui.patternListScrollBar.position = (gui.floatHoveredTile.y - 3.5f) / 7.0f;

            if (gui.patternListScrollBar.position < 0.0f) gui.patternListScrollBar.position = 0.0f;
            if (gui.patternListScrollBar.position > 1.0f) gui.patternListScrollBar.position = 1.0f;

            gui.patternListScroll = gui.patternListScrollBar.position * (loadedSong.patternSequence.size() - 1.0f);
            gui.drawUIThisFrame = true;
        }
        if (gui.instrumentListScrollBar.drag) // Scroll pattern list.
        {
            gui.instrumentListScrollBar.position = (gui.floatHoveredTile.y - 3.5f) / 7.0f;
            if (gui.instrumentListScrollBar.position < 0.0f) gui.instrumentListScrollBar.position = 0.0f;
            if (gui.instrumentListScrollBar.position > 1.0f) gui.instrumentListScrollBar.position = 1.0f;
            gui.instrumentListScroll = gui.instrumentListScrollBar.position * (256.0f - 10.0f);
            gui.drawUIThisFrame = true;
        }
        if (gui.patternVerticalScrollBar.drag) // Scroll pattern y.
        {
            if (!editor.playingSong)
            {
                gui.patternVerticalScrollBar.position = (gui.floatHoveredTile.y - 17.5f) / 38.0f;

                if (gui.patternVerticalScrollBar.position < 0.0f) gui.patternVerticalScrollBar.position = 0.0f;
                if (gui.patternVerticalScrollBar.position > 1.0f) gui.patternVerticalScrollBar.position = 1.0f;

                if (loadedPattern.rows.size() > 39)
                    gui.patternScroll.y = gui.patternVerticalScrollBar.position * (loadedPattern.rows.size() - 39);
                else
                    gui.patternScroll.y = 0;
                gui.drawFrameThisFrame = true;
            }
        }
        if (gui.patternHorizontalScrollBar.drag) // Scroll pattern x.
        {
            gui.patternHorizontalScrollBar.position = (gui.floatHoveredTile.x - 6.5f) / 84.0f;

            if (gui.patternHorizontalScrollBar.position < 0.0f) gui.patternHorizontalScrollBar.position = 0.0f;
            if (gui.patternHorizontalScrollBar.position > 1.0f) gui.patternHorizontalScrollBar.position = 1.0f;

            int channelsSize = 0;
            for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
                channelsSize += 8 + channels[ch].effectCountPerRow * 4;

            gui.patternScroll.x = gui.patternHorizontalScrollBar.position * (channelsSize);
            gui.drawFrameThisFrame = true;
        }
        if (gui.mixScrollBar.drag) // Scroll mix.
        {
            gui.mixScrollBar.position = (gui.floatHoveredTile.x - 44.5f) / 23.0f;

            if (gui.mixScrollBar.position < 0.0f) gui.mixScrollBar.position = 0.0f;
            if (gui.mixScrollBar.position > 1.0f) gui.mixScrollBar.position = 1.0f;


            gui.mixScroll = int(gui.mixScrollBar.position * float((channels.size() - 8) * 3));
            if (gui.mixScroll < 0) gui.mixScroll = 0;
            gui.drawFrameThisFrame = true;
        }


        // Change channel mix sliders.
        if (gui.uiDisplayMenuOption == 3) // Mix GUI.
        {
            if (!gui.draggingScrollBar && gui.hoveredTile.x > 42 && gui.hoveredTile.x < 67 && int(gui.hoveredTile.x - 44 + gui.mixScroll + 2) % 3 == 1)
            {
                if (gui.hoveredTile.y > 4 && gui.hoveredTile.y < 11)
                {
                    int ch = int(float(gui.hoveredTile.x - 44 + gui.mixScroll + 2) / 3.0f);

                    if (ch < channels.size())
                    {
                        channels[ch].mixVolume = 1.0f - ((gui.floatHoveredTile.y - 5.5f) / 5.0f);
                        gui.drawUIThisFrame = true;
                    }

                }
            }
        }



        for (int wind = 0; wind < windowController.windows.size(); wind++)
        {
            Vector2i posTL = windowController.windows[wind].position;
            Vector2i posBR = posTL;
            posBR.x += windowController.windows[wind].size.x + 1;
            posBR.y += windowController.windows[wind].size.y;
            if (gui.hoveredTile.y >= posTL.y && gui.hoveredTile.y < posBR.y)
            {
                if (gui.hoveredTile.x >= posTL.x && gui.hoveredTile.x < posBR.x)
                {
                    Vector2 windowPos = gui.floatHoveredTile;
                    windowPos.x -= posTL.x;
                    windowPos.y -= posTL.y;
                    HoldClickFloatingWindow(&windowController.windows[wind], wind, windowPos, window);
                    gui.clickingOnFloatingWind = true;
                    return;
                }
            }
        }
    }
    if (screen.rightMouseDown)
    {
        for (int wind = 0; wind < windowController.windows.size(); wind++)
        {
            Vector2i posTL = windowController.windows[wind].position;
            Vector2i posBR = posTL;
            posBR.x += windowController.windows[wind].size.x + 1;
            posBR.y += windowController.windows[wind].size.y;
            if (gui.hoveredTile.y >= posTL.y && gui.hoveredTile.y < posBR.y)
            {
                if (gui.hoveredTile.x >= posTL.x && gui.hoveredTile.x < posBR.x)
                {
                    Vector2 windowPos = gui.hoveredTile;
                    windowPos.x -= posTL.x;
                    windowPos.y -= posTL.y;
                    RightHoldFloatingWindow(&windowController.windows[wind], wind, windowPos, window);
                    gui.clickingOnFloatingWind = true;
                    return;
                }
            }
        }
    }

    if (gui.clickingOnFloatingWind)
        return;


    
    
    
    
    glfwSetKeyCallback(window, key_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetCharCallback(window, character_callback);

    glfwSetScrollCallback(window, scroll_callback);

    

    return;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Draw the ui.
    gui.drawUIThisFrame = true;
    gui.drawFrameThisFrame = true;


    if (key == GLFW_KEY_LEFT_SHIFT || (key == GLFW_KEY_RIGHT_SHIFT))
    {
        if (action == GLFW_PRESS)
            screen.shiftDown = true;
        else if (action == GLFW_RELEASE)
            screen.shiftDown = false;
    }

    if (key == GLFW_KEY_LEFT_CONTROL || (key == GLFW_KEY_RIGHT_CONTROL))
    {
        if (action == GLFW_PRESS)
            screen.ctrDown = true;
        else if (action == GLFW_RELEASE)
            screen.ctrDown = false;
    }

    


    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_BACKSPACE)
        {
            if (editor.selectedButton == 0) // Delete text (Song name)
            {
                if (gui.uiDisplayMenuOption != 2) // Effect GUI covers name.
                {
                    int selectedChar = gui.selectedTile.x - 45;
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
                            gui.selectedTile.x--;
                    }
                    loadedSong.unsavedChanges = true;
                }
            }
            else if (editor.selectedButton == 6) // Delete text (Artist name)
            {
                if (gui.uiDisplayMenuOption != 2) // Effect GUI covers name.
                {
                    int selectedChar = gui.selectedTile.x - 48;
                    if (loadedSong.artistName.length() > 0 && selectedChar > -1)
                    {
                        if (selectedChar == loadedSong.artistName.length())
                        {
                            loadedSong.artistName = loadedSong.artistName.substr(0, selectedChar);
                        }
                        else
                        {
                            loadedSong.artistName = loadedSong.artistName.substr(0, selectedChar) + loadedSong.artistName.substr(selectedChar + 1, loadedSong.artistName.length());
                        }

                        if (selectedChar > 0)
                            gui.selectedTile.x--;
                    }
                    loadedSong.unsavedChanges = true;
                }
            }
            else if (editor.selectedButton == 8) // Delete text (Sample name)
            {
                int selectedChar = gui.selectedTile.x - 72;
                if (loadedInstruments[editor.selectedInstrument].name.length() > 0 && selectedChar > -1)
                {
                    if (selectedChar == loadedInstruments[editor.selectedInstrument].name.length())
                    {
                        loadedInstruments[editor.selectedInstrument].name = loadedInstruments[editor.selectedInstrument].name.substr(0, selectedChar);
                    }
                    else
                    {
                        loadedInstruments[editor.selectedInstrument].name = loadedInstruments[editor.selectedInstrument].name.substr(0, selectedChar) + loadedInstruments[editor.selectedInstrument].name.substr(selectedChar + 1, loadedInstruments[editor.selectedInstrument].name.length());
                    }

                    if (selectedChar > 0)
                        gui.selectedTile.x--;
                }
                loadedSong.unsavedChanges = true;
            }
            else // Delete notes
            {
                deleteNotes();
            }
        }
        
        if (key == GLFW_KEY_UP || key == GLFW_KEY_KP_8) // Move up
        {
            if (!editor.playingSong)
            {
                if (gui.patternScroll.y > 0)
                    gui.patternScroll.y--;
                if (loadedSong.currentNote > 0)
                    loadedSong.currentNote--;
                editor.noteSelectionStart.y = loadedSong.currentNote;
                editor.noteSelectionEnd.y = loadedSong.currentNote;
                gui.activeUI[91][16].sprite = { 7, 3 };
            }

            
        }
        if (key == GLFW_KEY_DOWN || key == GLFW_KEY_KP_5) // Move down
        {
            if (!editor.playingSong)
            {
                if (gui.patternScroll.y < loadedPattern.rows.size() - 40 && loadedSong.currentNote > 8)
                    gui.patternScroll.y++;
                if (loadedSong.currentNote < loadedPattern.rows.size() - 1)
                    loadedSong.currentNote++;
                editor.noteSelectionStart.y = loadedSong.currentNote;
                editor.noteSelectionEnd.y = loadedSong.currentNote;
                gui.activeUI[91][55].sprite = { 7, 4 };
            }
        }
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_KP_4) // Move left
        {
            if (editor.noteSelectionStart.x > 0)
            {
                int selectedPart = int(editor.noteSelectionStart.x) % 12;

                if (selectedPart == 0)
                {
                    editor.noteSelectionStart.x -= 2;
                }
                else
                {
                    editor.noteSelectionStart.x -= 1;
                }
                editor.noteSelectionEnd.x = editor.noteSelectionStart.x + 1;
            }
        }
        if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_KP_6) // Move right
        {
            if (editor.noteSelectionStart.x < loadedSong.numberOfChannels * 12)
            {
                int selectedPart = int(editor.noteSelectionStart.x) % 11;

                if (selectedPart == 9)
                {
                    editor.noteSelectionStart.x += 2;
                }
                else
                {
                    editor.noteSelectionStart.x += 1;
                }
                editor.noteSelectionEnd.x = editor.noteSelectionStart.x + 1;
            }
        }

        if (key == GLFW_KEY_SPACE && editor.selectedButton == -1) // Start/stop song
        {
            StartOrStopSong();
        }

        if (key == GLFW_KEY_ENTER) // Return to start of song
        {
            loadedSong.currentNote = 0;
            gui.patternScroll.y = 0.0f;

            if (editor.playingSong) // Restart the frame if playing.
            {
                StartOrStopSong();
                StartOrStopSong();
            }
        }

        if (key == GLFW_KEY_TAB) // Create stop note
        {
            Vector2 selection = findFrameTileByPosition(editor.noteSelectionStart.x);
            int selectedChannel = selection.x;
            int selectedPart = selection.y;

            if (selectedPart < 3) // Playing keys
            {
                loadedPattern.rows[loadedSong.currentNote].note[selectedChannel] = 255;

                if (loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] < 0)
                    loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] = editor.selectedInstrument;
            }
            loadedSong.unsavedChanges = true;
        }

        if (key == GLFW_KEY_LEFT_SHIFT && !editor.playingSong) // Copy selected notes.
        {
            copyNotes();
        }

        if (key == GLFW_KEY_RIGHT_SHIFT && !editor.playingSong) // Paste selected notes.
        {
            pasteNotes();
        }
    }

    if (key == GLFW_KEY_ESCAPE)
    {
        if (loadedSong.unsavedChanges)
            windowController.InitializeWindow("Save and Exit", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 20, 16 });
        else
            glfwSetWindowShouldClose(window, true);
    }


    if (action == GLFW_PRESS || action == GLFW_RELEASE)
        screen.keyDown = false;

    return;
}


void mouse_button_callback(GLFWwindow* window, int key, int action, int mods)
{
    
    if (action == GLFW_PRESS)
    {
        // Mouse click actions
        if (key == GLFW_MOUSE_BUTTON_LEFT)
        {
            // Note selection
            screen.mouseDown = true;

            pressButton(window);
        }
        if (key == GLFW_MOUSE_BUTTON_RIGHT)
        {
            screen.rightMouseDown = true;
            rightClickButton(window);
        }
    }

    if (action == GLFW_REPEAT)
    {
        // Mouse click actions
        if (key == GLFW_MOUSE_BUTTON_LEFT)
        {
            // Note selection
            screen.mouseDown = true;

            pressAndHoldButton(window);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_MOUSE_BUTTON_LEFT && !editor.playingSong && !gui.clickingOnFloatingWind)
        {
            if (!editor.playingSong)
            {
                if (gui.hoveredTile.y > 15 && gui.hoveredTile.y < 56 && gui.hoveredTile.x > 3 && gui.hoveredTile.x < 91) // Select note
                {
                    editor.noteSelectionStart = { gui.hoveredTile.x - 4 + gui.patternScroll.x, gui.hoveredTile.y - 16 + gui.patternScroll.y };
                    editor.noteSelectionEnd = { gui.hoveredTile.x - 4 + gui.patternScroll.x, gui.hoveredTile.y - 16 + gui.patternScroll.y };
                    loadedSong.currentNote = gui.hoveredTile.y - 16 + gui.patternScroll.y;


                    int selectedPart = editor.noteSelectionStart.x;
                    int selectedChannel = 0;
                    bool inChannel = false;
                    while (!inChannel)
                    {
                        if (selectedPart >= 7 + channels[selectedChannel].effectCountPerRow * 4)
                        {
                            selectedPart -= 7 + channels[selectedChannel].effectCountPerRow * 4;
                            selectedChannel++;
                            if (selectedChannel > channels.size() - 1)
                            {
                                break;
                            }
                        }
                        else
                        {
                            inChannel = true;
                        }
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////

    if (action == GLFW_RELEASE)
    {
        // Mouse click actions
        if (key == GLFW_MOUSE_BUTTON_LEFT)
        {
            releaseButton();
        }
        else if (key == GLFW_MOUSE_BUTTON_RIGHT)
        {
            releaseRightButton();
        }
    }
    
    return;
}


void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    // Set to draw the interface.
    gui.drawUIThisFrame = true;
    gui.drawFrameThisFrame = true;


    char input = codepoint;

    bool writing = false;

    // Text writing
    if ((input > 38 && input < 42) || (input > 42 && input < 60) || (input > 64 && input < 91) || (input > 96 && input < 123) || input == 32 || input == 34)
    {
        if (screen.shiftDown) // Character inputs with the shift key.
        {
            if (input == 49)
                input = 33;
            else if (input == 47)
                input = 63;
        }

        

        if (editor.selectedButton == 0) // Song name
        {
            if (gui.uiDisplayMenuOption != 2) // Effect GUI covers name.
            {
                writing = true;
                if (loadedSong.songName.length() < 21)
                {
                    int selectedChar = gui.selectedTile.x - 45;

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

                    gui.selectedTile.x++;
                }
                loadedSong.unsavedChanges = true;
            }
        }
        else if (editor.selectedButton == 6) // Artist name
        {
            if (gui.uiDisplayMenuOption != 2) // Effect GUI covers name.
            {
                writing = true;
                if (loadedSong.artistName.length() < 18)
                {
                    int selectedChar = gui.selectedTile.x - 45;

                    if (loadedSong.artistName.length() == 0)
                        loadedSong.artistName = input;
                    else if (selectedChar >= loadedSong.artistName.length() - 1)
                    {
                        loadedSong.artistName = loadedSong.artistName + input;
                    }
                    else
                    {
                        loadedSong.artistName = loadedSong.artistName.substr(0, selectedChar + 1) + input + loadedSong.artistName.substr(selectedChar + 1, loadedSong.artistName.length());
                    }

                    gui.selectedTile.x++;
                }
                loadedSong.unsavedChanges = true;
            }
        }
        if (editor.selectedButton == 8) // Sample name
        {
            writing = true;
            if (loadedInstruments[editor.selectedInstrument].name.length() < 20)
            {
                int selectedChar = gui.selectedTile.x - 72;

                if (loadedInstruments[editor.selectedInstrument].name.length() == 0)
                    loadedInstruments[editor.selectedInstrument].name = input;
                else if (selectedChar >= loadedInstruments[editor.selectedInstrument].name.length() - 1)
                {
                    loadedInstruments[editor.selectedInstrument].name = loadedInstruments[editor.selectedInstrument].name + input;
                }
                else
                {
                    loadedInstruments[editor.selectedInstrument].name = loadedInstruments[editor.selectedInstrument].name.substr(0, selectedChar + 1) + input + loadedInstruments[editor.selectedInstrument].name.substr(selectedChar + 1, loadedInstruments[editor.selectedInstrument].name.length());
                }

                gui.selectedTile.x++;
            }
            loadedSong.unsavedChanges = true;
        }
    }
    if (input > 47 && input < 58) // Edit settings.
    {
        int selectedChar = gui.selectedTile.x - 15;
        int newVal = input - 48;
        if (editor.selectedButton == 1) // BPM
        {
            int leftSide = int(loadedSong.startingBPM / float(pow(10, 3 - selectedChar))) * pow(10, 3 - selectedChar);
            int rightSide = int(loadedSong.startingBPM) % int(pow(10, 2 - selectedChar));
            loadedSong.startingBPM = leftSide + newVal * pow(10, 2 - selectedChar) + rightSide;
            if (loadedSong.startingBPM < 1)
                loadedSong.startingBPM = 1;
            if (loadedSong.startingBPM > 255)
                loadedSong.startingBPM = 255;
            loadedSong.bpm = loadedSong.startingBPM;
            loadedSong.unsavedChanges = true;
        }
        else if (editor.selectedButton == 2) // EDO
        {
            int leftSide = int(loadedSong.edo / float(pow(10, 2 - selectedChar))) * pow(10, 2 - selectedChar);
            int rightSide = int(loadedSong.edo) % int(pow(10, 1 - selectedChar));
            loadedSong.edo = leftSide + newVal * pow(10, 1 - selectedChar) + rightSide;
            if (loadedSong.edo > 46)
                loadedSong.edo = 46;
            if (loadedSong.edo < 2)
                loadedSong.edo = 2;
            loadedSong.edo = loadedSong.edo;
            loadedSong.unsavedChanges = true;
            gui.drawFrameThisFrame = true;
        }
        else if (editor.selectedButton == 3) // OCT
        {
            editor.selectedOctave = newVal;
            if (editor.selectedOctave > 7)
                editor.selectedOctave = 7;
        }
        else if (editor.selectedButton == 4) // ROW
        {
            int leftSide = int(loadedPattern.rows.size() / float(pow(10, 3 - selectedChar))) * pow(10, 3 - selectedChar);
            int rightSide = int(loadedPattern.rows.size()) % int(pow(10, 2 - selectedChar));
            int newSize = leftSide + newVal * pow(10, 2 - selectedChar) + rightSide;
            if (newSize < 1)
                newSize = 1;
            if (newSize > 255)
                newSize = 255;

            if (newSize > loadedPattern.rows.size())
            {
                for (int row = 0; row < loadedPattern.rows.size() - newSize; row++)
                {
                    PatternRow newRow;
                    newRow.note.resize(loadedSong.numberOfChannels);
                    newRow.instrument.resize(loadedSong.numberOfChannels);
                    newRow.volume.resize(loadedSong.numberOfChannels);
                    newRow.effects.resize(loadedSong.numberOfChannels);
                    for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
                    {
                        newRow.note[ch] = -1;
                        newRow.instrument[ch] = -1;
                        newRow.volume[ch] = -1;

                        for (int ef = 0; ef < loadedPattern.rows[0].effects[ch].cEffect.size(); ef++)
                        {
                            newRow.effects[ch].cEffect.emplace_back(-1);
                            newRow.effects[ch].cEffectValue.emplace_back(-1);
                        }
                    }
                    loadedPattern.rows.emplace_back(newRow);
                }
                saveCurrentPattern();
                loadedSong.unsavedChanges = true;
            }
            else if (newSize < loadedPattern.rows.size())
            {
                loadedPattern.rows.resize(newSize);
                saveCurrentPattern();
                loadedSong.unsavedChanges = true;
            }
        }
        else if (editor.selectedButton == 5) // BEAT
        {
            int leftSide = int(loadedPattern.beatsPerMeasure / float(pow(10, 2 - selectedChar))) * pow(10, 2 - selectedChar);
            int rightSide = int(loadedPattern.beatsPerMeasure) % int(pow(10, 1 - selectedChar));
            loadedPattern.beatsPerMeasure = leftSide + newVal * pow(10, 1 - selectedChar) + rightSide;
            loadedSong.unsavedChanges = true;
            gui.drawFrameThisFrame = true;
        }
        else if (editor.selectedButton == 28) // Resample Size
        {

            
            writing = true;
            int selectedChar = gui.selectedWindowTile.x - 2;
            //sampleDisplay.resampleString[selectedChar] = input;
            
            

            float selectionSize = (sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos) * sampleDisplay.resampleMultiplier;

            sampleDisplay.resampleString = std::to_string(int(selectionSize));
            // Pad out the string.
            int strLen = sampleDisplay.resampleString.length();

            if (18 - strLen > 0)
                sampleDisplay.resampleString.insert(0, 18 - strLen, '0');
            //for (int i = 0; i < 18 - strLen; i++)
                //sampleDisplay.resampleString = '0' + sampleDisplay.resampleString;
            
            //std::cout << " " << selectedChar;

            sampleDisplay.resampleString[selectedChar] = input;
            //std::cout << "sdfs";
            

            selectionSize = std::atoi(&sampleDisplay.resampleString[0]);
            if (sampleDisplay.sampleSelectionEnd == sampleDisplay.sampleStartPos)
                selectionSize = 0;
            else
                selectionSize /= float(sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);

            sampleDisplay.resampleMultiplier = selectionSize;

            //sampleDisplay.resampleString = sampleDisplay.resampleString.substr(0, selectedChar) + input + sampleDisplay.resampleString.substr(selectedChar + 1, notesText[int(gui.selectedWindowTile.y - 1)].length());

        }
    }

    if (editor.focusOnNotes) // Create notes
    {
        Vector2 notePos = findFrameTileByPosition(editor.noteSelectionStart.x);
        int selectedPart = notePos.y;
        int selectedChannel = notePos.x;
        
        if (selectedPart < 0)
            return;

        if (selectedPart < 3 && selectedPart != -2) // Playing keys
        {
            int noteNum;
            if (loadedSong.edo != 12) // Special note finding for microtonal scales.
            {
                noteNum = editor.findEDONotePlayed(input, loadedSong.edo);
            }
            else
                noteNum = editor.findNotePlayed(input);
            
            if (noteNum != -1)
            {
                if (!screen.keyDown) // Play the note sound.
                {
                    channels[selectedChannel].volume = 64.0f;

                    StartSample(selectedChannel, editor.selectedInstrument, noteNum, 0);
                    for (int wave = 0; wave < 4; wave++)
                        channels[selectedChannel].waveforms[wave].note = noteNum;
                }
                loadedPattern.rows[loadedSong.currentNote].note[selectedChannel] = noteNum;
                loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] = editor.selectedInstrument;

                if (editor.playingSong)
                {
                    saveCurrentPattern();
                }

                loadedSong.unsavedChanges = true;
            }
        }
        else if (selectedPart == 3 || selectedPart == 4) // Instrument
        {
            if (loadedPattern.rows[loadedSong.currentNote].note[selectedChannel] < 0)
                return;

            int val = 0;

            if (input > 47 && input < 58)
                val = input - 48;
            else if (input > 96 && input < 103)
                val = input - 97 + 10;
            else
                return;

            if (loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] < 0)
                loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] = 0;

            if (selectedPart == 3)
            {
                int d1 = loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] % 16;
                loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] = val * 16 + d1;
            }
            else if (selectedPart == 4)
            {
                int d2 = loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] - loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] % 16;
                loadedPattern.rows[loadedSong.currentNote].instrument[selectedChannel] = d2 + val;
            }
            loadedSong.unsavedChanges = true;
        }
        else if (selectedPart == 5 || selectedPart == 6) // Volume
        {
            int inputNum;
            if (input > 47 && input < 58)
                inputNum = input - 48;
            else if (input > 96 && input < 103)
                inputNum = input - 97 + 10;
            else
                return;

            if (loadedPattern.rows[loadedSong.currentNote].volume[selectedChannel] < 0)
                loadedPattern.rows[loadedSong.currentNote].volume[selectedChannel] = 0;

            int effectValue = loadedPattern.rows[loadedSong.currentNote].volume[selectedChannel];


            float val1 = effectValue % 16;
            float val10 = effectValue - val1;

            if (selectedPart == 5)
            {
                loadedPattern.rows[loadedSong.currentNote].volume[selectedChannel] = inputNum * 16 + val1;
            }
            else
            {
                loadedPattern.rows[loadedSong.currentNote].volume[selectedChannel] = val10 + inputNum;
            }
            loadedSong.unsavedChanges = true;
            return;
        }
        else // Effect value
        {
            if (selectedPart >= 7) // Effect
            {
                int effectNum = (selectedPart - 7) / 4;
                if (effectNum >= loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffect.size())
                    return;
                int effectSelect = (selectedPart - 7) % 4;
                int effectType = loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffect[effectNum];
                int effectValue = loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffectValue[effectNum];


                if (effectSelect == 0)
                {
                    int inputLetter = input - 97;
                    if (inputLetter < 0 || inputLetter > 26)
                        return;

                    if (inputLetter == 2) //Clear effects effect does not have a value.
                    {
                        loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffectValue[effectNum] = 0;
                    }

                    float val = float(int(effectType / 100.0f)) * 100.0f;

                    if (loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffect[effectNum] == -1)
                        loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffectValue[effectNum] = 0;
                    loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffect[effectNum] = val + inputLetter;
                    loadedSong.unsavedChanges = true;
                    saveCurrentPattern();
                    return;
                }
                else if (effectSelect == 1) // Increase/decrease
                {
                    if (effectType > -1) // Return if there is no effect on the line.
                    {
                        if (effectType == 2) // Clear effects effect does not increase/decrease.
                            return;
                        if (input == 100 || input == 105)
                        {
                            float val = effectType % 100;

                            if (input == 100)
                                loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffect[effectNum] = val + 100;
                            else
                                loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffect[effectNum] = val + 200;
                            loadedSong.unsavedChanges = true;
                        }
                        saveCurrentPattern();
                    }
                    return;
                }
                else
                {
                    if (effectType == 2) // Clear effects effect does not have a value.
                        return;

                    int inputNum;
                    if (input > 47 && input < 58)
                        inputNum = input - 48;
                    else if (input > 96 && input < 103)
                        inputNum = input - 97 + 10;
                    else
                        return;

                    float val1 = effectValue % 16;
                    float val10 = effectValue - val1;

                    if (effectSelect == 2)
                    {
                        loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffectValue[effectNum] = inputNum * 16 + val1;
                    }
                    else
                    {
                        loadedPattern.rows[loadedSong.currentNote].effects[selectedChannel].cEffectValue[effectNum] = val10 + inputNum;
                    }
                    loadedSong.unsavedChanges = true;
                    saveCurrentPattern();
                    return;
                }
            }
        }
    }

    screen.keyDown = true;

    return;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

    if (!editor.playingSong)
    {
        gui.patternScroll.y -= yoffset * 1.0f;

        gui.patternScroll.y = int(gui.patternScroll.y);

        if (gui.patternScroll.y > loadedPattern.rows.size() - 40) gui.patternScroll.y = loadedPattern.rows.size() - 40;
        if (gui.patternScroll.y < 0) gui.patternScroll.y = 0;
    }

    gui.patternScroll.x -= xoffset * 1.0f;

    if (gui.patternScroll.x < 0)
        gui.patternScroll.x = 0;
    else
    {
        int channelsSize = 0;
        for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
            channelsSize += 8 + channels[ch].effectCountPerRow * 4;

        if (gui.patternScroll.x > channelsSize)
        {
            gui.patternScroll.x = channelsSize;
        }
    }

    gui.patternScroll.x = int(gui.patternScroll.x);

    

    gui.drawFrameThisFrame = true;

    return;
}



void pressButton(GLFWwindow* window)
{



    int hoveredXScrolled = gui.hoveredTile.x + gui.patternScroll.x;


    // Set to draw the interface.
    if (gui.hoveredTile.y < 16)
    {
        gui.drawUIThisFrame = true;
        editor.focusOnNotes = false;
    }
    else
    {
        gui.drawFrameThisFrame = true;
        editor.focusOnNotes = true;
    }


    // Floating windows.
    gui.clickingOnFloatingWind = false;
    for (int wind = 0; wind < windowController.windows.size(); wind++)
    {
        Vector2i posTL = windowController.windows[wind].position;
        Vector2i posBR = posTL;
        posBR.x += windowController.windows[wind].size.x + 1;
        posBR.y += windowController.windows[wind].size.y;
        if (gui.hoveredTile.y >= posTL.y && gui.hoveredTile.y < posBR.y)
        {
            if (gui.hoveredTile.x >= posTL.x && gui.hoveredTile.x < posBR.x)
            {
                Vector2 windowPos = gui.hoveredTile;
                windowPos.x -= posTL.x;
                windowPos.y -= posTL.y;
                gui.clickingOnFloatingWind = true;

                if (wind > 0) // Move the dragged window to front.
                {
                    FloatingWindow moveWind = windowController.windows[wind];
                    windowController.windows.erase(windowController.windows.begin() + wind);
                    windowController.windows.emplace(windowController.windows.begin(), moveWind);
                    windowController.windows.shrink_to_fit();
                    wind = 0;
                }


                ClickFloatingWindow(&windowController.windows[wind], wind, windowPos, window);

                return;
            }
        }
    }


    // Check for dragging scroll bars.
    if (gui.hoveredTile.x == 6 && (gui.hoveredTile.y > 2 && gui.hoveredTile.y < 11)) // Scroll pattern list.
    {
        gui.patternListScrollBar.drag = true;
        gui.draggingScrollBar = true;
    }
    else if (gui.hoveredTile.x == 91 && (gui.hoveredTile.y > 2 && gui.hoveredTile.y < 11)) // Scroll pattern list.
    {
        gui.instrumentListScrollBar.drag = true;
        gui.draggingScrollBar = true;
    }
    else if (gui.hoveredTile.x == 91 && (gui.hoveredTile.y > 16 && gui.hoveredTile.y < 56)) // Scroll pattern y.
    {
        gui.patternVerticalScrollBar.drag = true;
        gui.draggingScrollBar = true;
    }
    else if (gui.hoveredTile.y == 56 && (gui.hoveredTile.x > 5 && gui.hoveredTile.x < 90)) // Scroll pattern x.
    {
        gui.patternHorizontalScrollBar.drag = true;
        gui.draggingScrollBar = true;
    }
    else if (gui.hoveredTile.y == 11 && (gui.hoveredTile.x > 43 && gui.hoveredTile.x < 67)) // Scroll mix GUI.
    {
        gui.mixScrollBar.drag = true;
        gui.draggingScrollBar = true;
    }
    


    editor.selectedButton = -1;
    gui.selectedTile = gui.hoveredTile; // Select the tile that the mouse is currently on.

    if (gui.hoveredTile.x > 14 && gui.hoveredTile.x < 18) // Settings
    {
        if (gui.hoveredTile.y == 2) // BPM
            editor.selectedButton = 1;
        else if (gui.hoveredTile.y == 4) // EDO
            editor.selectedButton = 2;
        else if (gui.hoveredTile.y == 6) // OCT
            editor.selectedButton = 3;
        else if (gui.hoveredTile.y == 8) // ROW
            editor.selectedButton = 4;
        else if (gui.hoveredTile.y == 10) // BEAT
            editor.selectedButton = 5;
    }


    


    



    if (gui.hoveredTile.y == 0)
    {
        if (gui.hoveredTile.x == 86 || gui.hoveredTile.x == 87) // Minimize program
        {
            glfwIconifyWindow(window);
        }
        else if (gui.hoveredTile.x == 88 || gui.hoveredTile.x == 89) // Window program
        {
            screen.windowed = !screen.windowed;
            if (screen.windowed)
            {
                glfwSetWindowMonitor(window, NULL, (screen.screenSize.x - 1472.0f) * 0.5f, (screen.screenSize.y - 912.0f) * 0.5f, 1472.0f, 912.0f, 0);

                glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
                

                screen.screenSize.y = 912.0f;
                screen.screenSize.x = 1472.0f;

                screen.windowRatio = (736.0f / 456.0f) * (screen.screenSize.y / screen.screenSize.x);

                // Create the window
                glViewport(0, 0, screen.screenSize.x, screen.screenSize.y);
            }
            else
            {
                screen.screenSize.y = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
                screen.screenSize.x = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;

                screen.windowRatio = (736.0f / 456.0f) * (screen.screenSize.y / screen.screenSize.x);

                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screen.screenSize.x, screen.screenSize.y, glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate);

                // Create the window
                glViewport(0, 0, screen.screenSize.x, screen.screenSize.y);
            }
        }
        else if (gui.hoveredTile.x == 90 || gui.hoveredTile.x == 91) // Close program
        {
            if (loadedSong.unsavedChanges)
                windowController.InitializeWindow("Save and Exit", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 20, 16 });
            else
                glfwSetWindowShouldClose(window, true);
        }
    }





    if (gui.hoveredTile.x > 18 && gui.hoveredTile.x < 35)
    {
        if (gui.hoveredTile.y == 8) // Open File window.
        {
            windowController.InitializeWindow("File", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 16, 13 });
        }
        else if (gui.hoveredTile.y == 9) // Open Configure window.
        {
            windowController.InitializeWindow("Settings", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 16, 34 });
        }
        else if (gui.hoveredTile.y == 10) // Read Help Page
        {
            windowController.InitializeWindow("Help", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 40, 40 });
        }
        else if (gui.hoveredTile.y == 11) // Open sample editor.
        {
            for (int wave = 0; wave < 4; wave++)
            {
                if (loadedInstruments[editor.selectedInstrument].waveforms[wave].pcmFrames.size() < 1)
                {
                    loadedInstruments[editor.selectedInstrument].waveforms[wave].pcmFrames.assign(480 * 2, 0.0f);
                    loadedInstruments[editor.selectedInstrument].waveforms[wave].loopStart = 0.0f;
                    loadedInstruments[editor.selectedInstrument].waveforms[wave].loopEnd = 480 * 2;
                }
            }

            windowController.InitializeWindow("Instrument Editor", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 34, 40 });

            sampleDisplay.visible = true;
            sampleDisplay.drawing = false; // Stop sample drawing.
            sampleDisplay.zoomed = false; // Reset zoom.
            sampleDisplay.selectedOperator = 0; // Select the first sample operator.
            DrawSampleDisplay();
        }
    }



    if (gui.hoveredTile.y > 11 && gui.hoveredTile.y < 16) // Start/stop song.
    {
        if (gui.hoveredTile.x < 4) // Start/pause song.
        {
            if (gui.hoveredTile.x < 2)
            {
                if (!editor.playingSong)
                    StartOrStopSong();
            }
            else
            {
                if (editor.playingSong)
                    StartOrStopSong();
            }
        }



        int selectedX = hoveredXScrolled - 4.0f;
        int selectedChannel = 0;
        bool inChannel = false;


        int selectedChannelSize = 0;
        


        while (!inChannel)
        {
            if (channels[selectedChannel].compressed)
                selectedChannelSize = 3;
            else
            {
                selectedChannelSize = 7 + channels[selectedChannel].effectCountPerRow * 4;
            }


            if (channels[selectedChannel].compressed)
            {
                if (selectedX == 3) // decompress channel.
                {
                    if (gui.hoveredTile.y == 12) // Move channel
                    {
                        if (editor.channelBeingMoved == -1)
                            editor.channelBeingMoved = selectedChannel;
                        editor.channelMoveTo = selectedChannel;
                    }
                    else if (gui.hoveredTile.y < 15)
                    {
                        if (editor.channelFocus)
                        {
                            for (int ch = 0; ch < channels.size(); ch++)
                                channels[ch].compressed = true;
                        }
                        channels[selectedChannel].compressed = false;
                    }
                    gui.drawFrameThisFrame = true;
                    return;
                }
                if (selectedX >= 3)
                {
                    selectedX -= 3;
                    selectedChannel++;



                    if (selectedChannel >= channels.size())
                    {
                        inChannel = true;
                    }
                }
                else
                {
                    inChannel = true;
                }
            }
            else
            {
                if (selectedX == 1 + selectedChannelSize) // Compress channel.
                {
                    if (gui.hoveredTile.y == 12)
                    {
                        if (editor.channelBeingMoved == -1)
                            editor.channelBeingMoved = selectedChannel;
                        editor.channelMoveTo = selectedChannel;
                    }
                    else if (gui.hoveredTile.y < 15)
                    {
                        channels[selectedChannel].compressed = true;
                    }
                    gui.drawFrameThisFrame = true;
                    return;
                }
                if (selectedX >= 1 + selectedChannelSize)
                {
                    selectedX -= 1 + selectedChannelSize;
                    selectedChannel++;



                    if (selectedChannel >= channels.size())
                    {
                        inChannel = true;
                    }
                }
                else
                {
                    inChannel = true;
                }
            }
        }
        if (selectedChannel < loadedSong.numberOfChannels && gui.hoveredTile.y != 12 && gui.hoveredTile.y < 15)
        {
            if (hoveredXScrolled > 4)
            {
                if (selectedX < selectedChannelSize - 3)
                {
                    channels[selectedChannel].muted = !channels[selectedChannel].muted; // Mute/unmute
                }
                else if (selectedX == selectedChannelSize - 3) // Add extra effect column.
                {
                    if (channels[selectedChannel].effectCountPerRow < 8) // Max effects
                    {
                        channels[selectedChannel].effectCountPerRow++;
                        loadedPattern = resizeSongPatternEffectColumns(loadedPattern);
                        loadedSong.unsavedChanges = true;
                    }
                }
                else if (selectedX == selectedChannelSize - 1) // Delete extra effect column.
                {
                    if (channels[selectedChannel].effectCountPerRow > 1) // Min effects
                    {
                        channels[selectedChannel].effectCountPerRow--;
                        loadedPattern = resizeSongPatternEffectColumns(loadedPattern);
                        loadedSong.unsavedChanges = true;
                    }
                }
                gui.drawFrameThisFrame = true; // The interface has changed, and must be redrawn.
            }
        }
        else
        {
            int hoverPosAfterChannels = hoveredXScrolled;
            for (int ch = 0; ch < channels.size(); ch++)
            {
                if (channels[ch].compressed)
                    hoverPosAfterChannels -= 3;
                else
                {
                    hoverPosAfterChannels -= 8 + channels[ch].effectCountPerRow * 4;
                }
            }
            if (hoverPosAfterChannels == 5 || hoverPosAfterChannels == 6) // Add channel
            {
                if (loadedSong.numberOfChannels < 64)
                {
                    ResizeDecoders(loadedSong.numberOfChannels + 1);
                    loadedSong.unsavedChanges = true;
                }
            }
            else if (hoverPosAfterChannels == 8 || hoverPosAfterChannels == 9) // Remove channel
            {
                if (loadedSong.numberOfChannels > 1)
                {
                    ResizeDecoders(loadedSong.numberOfChannels - 1);
                    loadedSong.unsavedChanges = true;
                }
            }
        }
    }


    if (gui.hoveredTile.y == 2 && gui.hoveredTile.x > 44 && gui.hoveredTile.x < 66) // Edit Song Name
    {
        editor.selectedButton = 0;
        if (gui.selectedTile.x - 45 >= loadedSong.songName.length()) // Snap to end of text.
            gui.selectedTile.x = 44 + loadedSong.songName.length();
    }
    else if (gui.hoveredTile.y == 3 && gui.hoveredTile.x > 47 && gui.hoveredTile.x < 66) // Edit Artist Name
    {
        editor.selectedButton = 6;
        if (gui.selectedTile.x - 48 >= loadedSong.artistName.length()) // Snap to end of text.
            gui.selectedTile.x = 47 + loadedSong.artistName.length();
    }

    
    

    
    if (gui.hoveredTile.y > 1 && gui.hoveredTile.y < 12) // Select song frame.
    {
        if (gui.hoveredTile.x > 0 && gui.hoveredTile.x < 6)
        {
            int moveToFrame = gui.hoveredTile.y - 2 + gui.patternListScroll;
            if (moveToFrame >= loadedSong.patternSequence.size()) // Snap to end of song.
                moveToFrame = loadedSong.patternSequence.size() - 1;

            if (moveToFrame != loadedSong.currentPattern)
            {
                if (editor.playingSong) // Restart the frame if playing.
                {
                    StartOrStopSong();
                    StartOrStopSong();
                }

                saveCurrentPattern();
                loadedSong.currentPattern = moveToFrame;
                loadCurrentPattern();
            }
        }
    }


    if (gui.hoveredTile.y > 1 && gui.hoveredTile.y < 6) // Select GUI display.
    {
        if (gui.hoveredTile.x > 35 && gui.hoveredTile.x < 42)
        {
            gui.uiDisplayMenuOption = gui.hoveredTile.y - 2;
        }
    }



    if (gui.hoveredTile.y > 1 && gui.hoveredTile.y < 12) // Select sample.
    {
        if (gui.hoveredTile.x > 68 && gui.hoveredTile.x < 91) // Select sample
        {
            
            if (editor.selectedInstrument == gui.hoveredTile.y - 2 + gui.instrumentListScroll)
            {
                if (gui.hoveredTile.x == 90)
                {
                    if (!loadedInstruments[editor.selectedInstrument].enabled) // Create sample.
                    {
                        //loadedSamples[editor.selectedSample] = new Sample;
                        loadedInstruments[editor.selectedInstrument].enabled = true;
                        
                        for (int wave = 0; wave < 4; wave++)
                        {
                            loadedInstruments[editor.selectedInstrument].waveforms[wave].pcmFrames.clear();
                            loadedInstruments[editor.selectedInstrument].waveforms[wave].pcmFrames.assign(480 * 2, 0.0f);
                            loadedInstruments[editor.selectedInstrument].waveforms[wave].loopStart = 0.0f;
                            loadedInstruments[editor.selectedInstrument].waveforms[wave].loopEnd = 480 * 2;
                        }
                        sampleDisplay.drawing = false; // Stop sample drawing.
                        sampleDisplay.zoomed = false; // Reset zoom.
                        sampleDisplay.selectedOperator = 0; // Select the first sample operator.
                    }
                    else // Delete sample.
                    {
                        //delete loadedSamples[editor.selectedSample];
                        //loadedSamples[editor.selectedSample] = nullptr;
                        loadedInstruments[editor.selectedInstrument].enabled = false;
                    }
                }
                else if (gui.hoveredTile.x > 71) // Edit selected sample name.
                {
                    if (loadedInstruments[editor.selectedInstrument].enabled)
                    {
                        if (gui.hoveredTile.x - 72 > loadedInstruments[editor.selectedInstrument].name.size())
                        {
                            gui.hoveredTile.x = loadedInstruments[editor.selectedInstrument].name.size() + 71;
                            gui.selectedTile.x = gui.hoveredTile.x;
                        }
                        editor.selectedButton = 8;
                    }
                }
            }
            else
            {
                editor.selectedInstrument = gui.hoveredTile.y - 2 + gui.instrumentListScroll;
                sampleDisplay.drawing = false; // Stop sample drawing.
                sampleDisplay.zoomed = false; // Reset zoom.
                sampleDisplay.selectedOperator = 0; // Select the first sample operator.
            }

            if (editor.selectedInstrument < 0)
                editor.selectedInstrument = 0;

            if (sampleDisplay.visible)
            {
                DrawSampleDisplay();
            }
        }
    }

    
    if (gui.hoveredTile.y == 2)
    {
        if (gui.hoveredTile.x == 6) // Frame menu scroll up.
        {
            if (gui.patternListScroll > 0)
                gui.patternListScroll--;
            gui.activeUI[6][2].sprite = { 7, 3 };
        }
        else if (gui.hoveredTile.x == 91) // Sample menu scroll up.
        {
            if (gui.instrumentListScroll > 0)
                gui.instrumentListScroll--;
            gui.activeUI[91][2].sprite = { 7, 3 };
        }
    }
    else if (gui.hoveredTile.y == 3) // Change song BPM.
    {
        if (gui.hoveredTile.x == 11)
        {
            loadedSong.startingBPM++;
            if (loadedSong.startingBPM > 255)
                loadedSong.startingBPM = 255;
            loadedSong.bpm = loadedSong.startingBPM;
            gui.activeUI[11][3].sprite = { 8, 4 };
            gui.activeUI[12][3].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 13)
        {
            loadedSong.startingBPM--;
            if (loadedSong.startingBPM < 0)
                loadedSong.startingBPM = 0;
            loadedSong.bpm = loadedSong.startingBPM;
            gui.activeUI[13][3].sprite = { 10, 4 };
            gui.activeUI[12][3].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
    }
    else if (gui.hoveredTile.y == 5)
    {
        if (gui.hoveredTile.x == 7) // Change song frame.
        {
            saveCurrentPattern();
            loadedSong.patternSequence[loadedSong.currentPattern]++;
            while (loadedSong.patternSequence[loadedSong.currentPattern] >= loadedSong.patterns.size()) // Create a new frame when changed to one not yet used.
            {
                Pattern newFrame;
                newFrame.channels.resize(loadedSong.numberOfChannels);
                newFrame.beatsPerMeasure = loadedPattern.beatsPerMeasure;
                loadedSong.patterns.emplace_back(newFrame);
            }
            loadCurrentPattern();
            gui.activeUI[7][5].sprite = { 8, 4 };
            gui.activeUI[8][5].sprite = { 9, 4 };
            saveCurrentPattern();
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 9) // Change song frame.
        {
            saveCurrentPattern();
            loadedSong.patternSequence[loadedSong.currentPattern]--;
            if (loadedSong.patternSequence[loadedSong.currentPattern] < 0)
                loadedSong.patternSequence[loadedSong.currentPattern] = 0;
            loadCurrentPattern();
            gui.activeUI[9][5].sprite = { 10, 4 };
            gui.activeUI[8][5].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 11) // Change song EDO.
        {
            loadedSong.edo++;
            if (loadedSong.edo > 46)
                loadedSong.edo = 46;
            loadedSong.edo = loadedSong.edo;
            gui.activeUI[11][5].sprite = { 8, 4 };
            gui.activeUI[12][5].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
            gui.drawFrameThisFrame = true;
        }
        else if (gui.hoveredTile.x == 13) // Change song EDO.
        {
            loadedSong.edo--;
            if (loadedSong.edo < 2)
                loadedSong.edo = 2;
            loadedSong.edo = loadedSong.edo;
            gui.activeUI[13][5].sprite = { 10, 4 };
            gui.activeUI[12][5].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
            gui.drawFrameThisFrame = true;
        }
    }
    else if (gui.hoveredTile.y == 7) // Change song octave.
    {
        if (gui.hoveredTile.x == 11)
        {
            editor.selectedOctave++;
            if (editor.selectedOctave > 7)
                editor.selectedOctave = 7;
            gui.activeUI[11][7].sprite = { 8, 4 };
            gui.activeUI[12][7].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 13)
        {
            editor.selectedOctave--;
            if (editor.selectedOctave < 0)
                editor.selectedOctave = 0;
            gui.activeUI[13][7].sprite = { 10, 4 };
            gui.activeUI[12][7].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
    }
    else if (gui.hoveredTile.y == 8) // Add/remove frame from song.
    {
        if (gui.hoveredTile.x == 7)
        {
            // Add a duplicate of the selected frame after it.
            saveCurrentPattern();
            loadedSong.patternSequence.emplace(loadedSong.patternSequence.begin() + loadedSong.currentPattern, loadedSong.patternSequence[loadedSong.currentPattern]);
            gui.activeUI[7][8].sprite = { 8, 4 };
            gui.activeUI[8][8].sprite = { 9, 4 };
            loadedSong.currentPattern++;
            loadCurrentPattern();
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 9)
        {
            // Delete the current frame if there is more than one in the song.
            saveCurrentPattern();
            if (loadedSong.patternSequence.size() > 1)
            {
                loadedSong.patternSequence.erase(loadedSong.patternSequence.begin() + loadedSong.currentPattern);
                gui.activeUI[4][2 + loadedSong.currentPattern].bgCol = 0;
                gui.activeUI[5][2 + loadedSong.currentPattern].bgCol = 0;
                loadedSong.patternSequence.shrink_to_fit();
                if (loadedSong.currentPattern >= loadedSong.patternSequence.size())
                    loadedSong.currentPattern--;
                loadCurrentPattern();
                loadedSong.unsavedChanges = true;
            }
        }
    }
    else if (gui.hoveredTile.y == 9)
    {
        if (gui.hoveredTile.x == 11) // Change frame row count.
        {
            if (loadedPattern.rows.size() < 255) // Max frame size.
            {
                PatternRow newRow;
                newRow.note.resize(loadedSong.numberOfChannels);
                newRow.instrument.resize(loadedSong.numberOfChannels);
                newRow.volume.resize(loadedSong.numberOfChannels);
                newRow.effects.resize(loadedSong.numberOfChannels);
                for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
                {
                    newRow.note[ch] = -1;
                    newRow.instrument[ch] = -1;
                    newRow.volume[ch] = -1;

                    for (int ef = 0; ef < loadedPattern.rows[0].effects[ch].cEffect.size(); ef++)
                    {
                        newRow.effects[ch].cEffect.emplace_back(-1);
                        newRow.effects[ch].cEffectValue.emplace_back(-1);
                    }
                }
                loadedPattern.rows.emplace_back(newRow);
                saveCurrentPattern();
                gui.activeUI[11][9].sprite = { 8, 4 };
                gui.activeUI[12][9].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
        else if (gui.hoveredTile.x == 13) // Min frame size.
        {
            if (loadedPattern.rows.size() > 1)
            {
                loadedPattern.rows.erase(loadedPattern.rows.begin() + loadedPattern.rows.size() - 1);
                loadedPattern.rows.shrink_to_fit();
                saveCurrentPattern();
                gui.activeUI[13][9].sprite = { 10, 4 };
                gui.activeUI[12][9].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
    }
    else if (gui.hoveredTile.y == 11)
    {
        if (gui.hoveredTile.x == 6) // Frame menu scroll down.
        {
            gui.activeUI[6][10].sprite = { 7, 4 };
            if (loadedSong.patternSequence.size() - gui.patternListScroll > 1)
                gui.patternListScroll++;
        }
        else if (gui.hoveredTile.x == 91) // File menu scroll down.
        {
            gui.activeUI[91][11].sprite = { 7, 4 };
            if (gui.instrumentListScroll < 256 - 10)
                gui.instrumentListScroll++;
        }
        // Change frame beats per measure.
        if (gui.hoveredTile.x == 11)
        {
            if (loadedPattern.beatsPerMeasure < 99) // Max measure size.
            {
                loadedPattern.beatsPerMeasure++;
                saveCurrentPattern();
                gui.activeUI[11][11].sprite = { 8, 4 };
                gui.activeUI[12][11].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
        else if (gui.hoveredTile.x == 13) // Min measure size.
        {
            if (loadedPattern.beatsPerMeasure > 0)
            {
                loadedPattern.beatsPerMeasure--;
                saveCurrentPattern();
                gui.activeUI[13][11].sprite = { 10, 4 };
                gui.activeUI[12][11].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
        gui.drawFrameThisFrame = true;
    }
    else if (gui.hoveredTile.y == 16)
    {
        if (gui.hoveredTile.x == 91) // Frame scroll up.
        {
            if (!editor.playingSong)
            {
                gui.patternScroll.y--;
                if (gui.patternScroll.y < 0)
                    gui.patternScroll.y = 0;
                gui.activeUI[91][16].sprite = { 7, 3 };
            }
        }
    }
    else if (gui.hoveredTile.y == 55)
    {
        if (gui.hoveredTile.x == 91) // Frame scroll down.
        {
            if (!editor.playingSong)
            {
                gui.patternScroll.y++;
                gui.activeUI[91][55].sprite = { 7, 4 };
                if (gui.patternScroll.y > loadedPattern.rows.size() - 40) gui.patternScroll.y = loadedPattern.rows.size() - 40;
                if (gui.patternScroll.y < 0) gui.patternScroll.y = 0;
            }
        }
    }
    else if (gui.hoveredTile.y == 56) // Frame scroll horizontally.
    {
        if (gui.hoveredTile.x == 5) // Frame scroll left.
        {
            gui.patternScroll.x--;
            if (gui.patternScroll.x < 0)
                gui.patternScroll.x = 0;
            gui.activeUI[5][56].sprite = { 29, 4 };
        }
        else if (gui.hoveredTile.x == 90) // Frame scroll right.
        {
            gui.patternScroll.x++;
            int channelsSize = 0;
            for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
                channelsSize += 8 + channels[ch].effectCountPerRow * 3;

            if (gui.patternScroll.x > channelsSize)
            {
                gui.patternScroll.x = channelsSize;
            }
            gui.activeUI[90][56].sprite = { 29, 3 };
        }
    }


    


    return;
}



void pressAndHoldButton(GLFWwindow* window)
{

    if (gui.clickingOnFloatingWind)
        return;


    int hoveredXScrolled = gui.hoveredTile.x + gui.patternScroll.x;


    // Set to draw the interface.
    if (gui.hoveredTile.y < 16)
    {
        gui.drawUIThisFrame = true;
        editor.focusOnNotes = false;
    }
    else
    {
        gui.drawFrameThisFrame = true;
        editor.focusOnNotes = true;
    }


    



    editor.selectedButton = -1;
    gui.selectedTile = gui.hoveredTile; // Select the tile that the mouse is currently on.

    if (gui.hoveredTile.x > 14 && gui.hoveredTile.x < 18) // Settings
    {
        if (gui.hoveredTile.y == 2) // BPM
            editor.selectedButton = 1;
        else if (gui.hoveredTile.y == 4) // TPB
            editor.selectedButton = 2;
        else if (gui.hoveredTile.y == 6) // OCT
            editor.selectedButton = 3;
        else if (gui.hoveredTile.y == 8) // ROW
            editor.selectedButton = 4;
        else if (gui.hoveredTile.y == 10) // BEAT
            editor.selectedButton = 5;
    }


    




    if (gui.hoveredTile.y > 11 && gui.hoveredTile.y < 16) // Select channel mute buttons.
    {
        int selectedX = hoveredXScrolled - 4.0f;
        int selectedChannel = 0;
        bool inChannel = false;



        while (!inChannel)
        {
            if (channels[selectedChannel].compressed)
            {
                if (selectedX == 3)
                {
                    if (gui.hoveredTile.y == 12) // Move channel
                    {
                        if (editor.channelBeingMoved == -1)
                            editor.channelBeingMoved = selectedChannel;
                        editor.channelMoveTo = selectedChannel;
                    }
                    
                    gui.drawFrameThisFrame = true;
                    return;
                }
                if (selectedX >= 3)
                {
                    selectedX -= 3;
                    selectedChannel++;



                    if (selectedChannel >= channels.size())
                    {
                        inChannel = true;
                    }
                }
                else
                {
                    inChannel = true;
                }
            }
            else
            {
                if (selectedX == 8 + channels[selectedChannel].effectCountPerRow * 4)
                {
                    if (gui.hoveredTile.y == 12)
                    {
                        if (editor.channelBeingMoved == -1)
                            editor.channelBeingMoved = selectedChannel;
                        editor.channelMoveTo = selectedChannel;
                    }
                    return;
                }
                if (selectedX >= 8 + channels[selectedChannel].effectCountPerRow * 4)
                {
                    selectedX -= 8 + channels[selectedChannel].effectCountPerRow * 4;
                    selectedChannel++;



                    if (selectedChannel >= channels.size())
                    {
                        inChannel = true;
                    }
                }
                else
                {
                    inChannel = true;
                }
            }
        }
        
    }


    





    



    


    if (gui.hoveredTile.y == 2)
    {
        if (gui.hoveredTile.x == 6) // Frame menu scroll up.
        {
            if (gui.patternListScroll > 0)
                gui.patternListScroll--;
            gui.activeUI[6][2].sprite = { 7, 3 };
        }
        else if (gui.hoveredTile.x == 91) // Sample menu scroll up.
        {
            if (gui.instrumentListScroll > 0)
                gui.instrumentListScroll--;
            gui.activeUI[91][2].sprite = { 7, 3 };
        }
    }
    else if (gui.hoveredTile.y == 3) // Change song BPM.
    {
        if (gui.hoveredTile.x == 11)
        {
            loadedSong.startingBPM++;
            if (loadedSong.startingBPM > 255)
                loadedSong.startingBPM = 255;
            loadedSong.bpm = loadedSong.startingBPM;
            gui.activeUI[11][3].sprite = { 8, 4 };
            gui.activeUI[12][3].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 13)
        {
            loadedSong.startingBPM--;
            if (loadedSong.startingBPM < 0)
                loadedSong.startingBPM = 0;
            loadedSong.bpm = loadedSong.startingBPM;
            gui.activeUI[13][3].sprite = { 10, 4 };
            gui.activeUI[12][3].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
    }
    else if (gui.hoveredTile.y == 5)
    {
        if (gui.hoveredTile.x == 7) // Change song frame.
        {
            saveCurrentPattern();
            loadedSong.patternSequence[loadedSong.currentPattern]++;
            while (loadedSong.patternSequence[loadedSong.currentPattern] >= loadedSong.patterns.size()) // Create a new frame when changed to one not yet used.
            {
                Pattern newFrame;
                newFrame.channels.resize(loadedSong.numberOfChannels);
                newFrame.beatsPerMeasure = loadedPattern.beatsPerMeasure;
                loadedSong.patterns.emplace_back(newFrame);
            }
            loadCurrentPattern();
            gui.activeUI[7][5].sprite = { 8, 4 };
            gui.activeUI[8][5].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 9) // Change song frame.
        {
            saveCurrentPattern();
            loadedSong.patternSequence[loadedSong.currentPattern]--;
            if (loadedSong.patternSequence[loadedSong.currentPattern] < 0)
                loadedSong.patternSequence[loadedSong.currentPattern] = 0;
            loadCurrentPattern();
            gui.activeUI[9][5].sprite = { 10, 4 };
            gui.activeUI[8][5].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 11) // Change song EDO.
        {
            loadedSong.edo++;
            if (loadedSong.edo > 46)
                loadedSong.edo = 46;
            loadedSong.edo = loadedSong.edo;
            gui.activeUI[11][5].sprite = { 8, 4 };
            gui.activeUI[12][5].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
            gui.drawFrameThisFrame = true;
        }
        else if (gui.hoveredTile.x == 13) // Change song EDO.
        {
            loadedSong.edo--;
            if (loadedSong.edo < 2)
                loadedSong.edo = 2;
            loadedSong.edo = loadedSong.edo;
            gui.activeUI[13][5].sprite = { 10, 4 };
            gui.activeUI[12][5].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
            gui.drawFrameThisFrame = true;
        }
    }
    else if (gui.hoveredTile.y == 7) // Change song Octave.
    {
        if (gui.hoveredTile.x == 11)
        {
            editor.selectedOctave++;
            if (editor.selectedOctave > 7)
                editor.selectedOctave = 7;
            gui.activeUI[11][7].sprite = { 8, 4 };
            gui.activeUI[12][7].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
        else if (gui.hoveredTile.x == 13)
        {
            editor.selectedOctave--;
            if (editor.selectedOctave < 0)
                editor.selectedOctave = 0;
            gui.activeUI[13][7].sprite = { 10, 4 };
            gui.activeUI[12][7].sprite = { 9, 4 };
            loadedSong.unsavedChanges = true;
        }
    }
    else if (gui.hoveredTile.y == 9)
    {
        if (gui.hoveredTile.x == 11) // Change frame row count.
        {
            if (loadedPattern.rows.size() < 255) // Max frame size.
            {
                PatternRow newRow;
                newRow.note.resize(loadedSong.numberOfChannels);
                newRow.instrument.resize(loadedSong.numberOfChannels);
                newRow.volume.resize(loadedSong.numberOfChannels);
                newRow.effects.resize(loadedSong.numberOfChannels);
                for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
                {
                    newRow.note[ch] = -1;
                    newRow.instrument[ch] = -1;
                    newRow.volume[ch] = -1;

                    for (int ef = 0; ef < loadedPattern.rows[0].effects[ch].cEffect.size(); ef++)
                    {
                        newRow.effects[ch].cEffect.emplace_back(-1);
                        newRow.effects[ch].cEffectValue.emplace_back(-1);
                    }
                }
                loadedPattern.rows.emplace_back(newRow);
                saveCurrentPattern();
                gui.activeUI[11][9].sprite = { 8, 4 };
                gui.activeUI[12][9].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
        else if (gui.hoveredTile.x == 13) // Min frame size.
        {
            if (loadedPattern.rows.size() > 1)
            {
                loadedPattern.rows.erase(loadedPattern.rows.begin() + loadedPattern.rows.size() - 1);
                loadedPattern.rows.shrink_to_fit();
                saveCurrentPattern();
                gui.activeUI[13][9].sprite = { 10, 4 };
                gui.activeUI[12][9].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
    }
    else if (gui.hoveredTile.y == 11)
    {
        if (gui.hoveredTile.x == 6) // Frame menu scroll down.
        {
            gui.activeUI[6][10].sprite = { 7, 4 };
            if (loadedSong.patternSequence.size() - gui.patternListScroll > 1)
                gui.patternListScroll++;
        }
        else if (gui.hoveredTile.x == 91) // File menu scroll down.
        {
            gui.activeUI[91][11].sprite = { 7, 4 };
            if (256 - gui.instrumentListScroll > 1)
                gui.instrumentListScroll++;
        }
        // Change frame beats per measure.
        if (gui.hoveredTile.x == 11)
        {
            if (loadedPattern.beatsPerMeasure < 99) // Max measure size.
            {
                loadedPattern.beatsPerMeasure++;
                saveCurrentPattern();
                gui.activeUI[11][11].sprite = { 8, 4 };
                gui.activeUI[12][11].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
        else if (gui.hoveredTile.x == 13) // Min measure size.
        {
            if (loadedPattern.beatsPerMeasure > 0)
            {
                loadedPattern.beatsPerMeasure--;
                saveCurrentPattern();
                gui.activeUI[13][11].sprite = { 10, 4 };
                gui.activeUI[12][11].sprite = { 9, 4 };
                loadedSong.unsavedChanges = true;
            }
        }
        gui.drawFrameThisFrame = true;
    }
    else if (gui.hoveredTile.y == 16)
    {
        if (gui.hoveredTile.x == 91) // Frame scroll up.
        {
            if (!editor.playingSong)
            {
                gui.patternScroll.y--;
                if (gui.patternScroll.y < 0)
                    gui.patternScroll.y = 0;
                gui.activeUI[91][16].sprite = { 7, 3 };
            }
        }
    }
    else if (gui.hoveredTile.y == 55)
    {
        if (gui.hoveredTile.x == 91) // Frame scroll down.
        {
            if (!editor.playingSong)
            {
                gui.patternScroll.y++;
                gui.activeUI[91][55].sprite = { 7, 4 };
                if (gui.patternScroll.y > loadedPattern.rows.size() - 40) gui.patternScroll.y = loadedPattern.rows.size() - 40;
                if (gui.patternScroll.y < 0) gui.patternScroll.y = 0;
            }
        }
    }
    else if (gui.hoveredTile.y == 56) // Frame scroll horizontally.
    {
        if (gui.hoveredTile.x == 5) // Frame scroll left.
        {
            gui.patternScroll.x--;
            if (gui.patternScroll.x < 0)
                gui.patternScroll.x = 0;
            gui.activeUI[5][56].sprite = { 29, 4 };
        }
        else if (gui.hoveredTile.x == 90) // Frame scroll right.
        {
            gui.patternScroll.x++;
            int channelsSize = 0;
            for (int ch = 0; ch < loadedSong.numberOfChannels; ch++)
                channelsSize += 8 + channels[ch].effectCountPerRow * 3;

            if (gui.patternScroll.x > channelsSize)
            {
                gui.patternScroll.x = channelsSize;
            }
            gui.activeUI[90][56].sprite = { 29, 3 };
        }
    }





    return;
}



void rightClickButton(GLFWwindow* window)
{
    int hoveredXScrolled = gui.hoveredTile.x + gui.patternScroll.x;


    // Set to draw the interface.
    if (gui.hoveredTile.y < 16)
    {
        gui.drawUIThisFrame = true;
        editor.focusOnNotes = false;
    }
    else
    {
        gui.drawFrameThisFrame = true;
        editor.focusOnNotes = true;
    }



    // Floating windows.
    for (int wind = 0; wind < windowController.windows.size(); wind++)
    {
        Vector2i posTL = windowController.windows[wind].position;
        Vector2i posBR = posTL;
        posBR.x += windowController.windows[wind].size.x + 1;
        posBR.y += windowController.windows[wind].size.y;
        if (gui.hoveredTile.y >= posTL.y && gui.hoveredTile.y < posBR.y)
        {
            if (gui.hoveredTile.x >= posTL.x && gui.hoveredTile.x < posBR.x)
            {
                Vector2 windowPos = gui.hoveredTile;
                windowPos.x -= posTL.x;
                windowPos.y -= posTL.y;
                gui.clickingOnFloatingWind = true;
                RightClickFloatingWindow(&windowController.windows[wind], wind, windowPos, window);
                return;
            }
        }
    }

    if (gui.clickingOnFloatingWind)
        return;



    


    


    editor.selectedButton = -1;
    gui.selectedTile = gui.hoveredTile; // Select the tile that the mouse is currently on.

    


    // Open the Selection window.
    if (gui.hoveredTile.y > 15 && !editor.playingSong)
    {
        windowController.InitializeWindow("Selection", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 16, 16 });
    }
    else if (gui.hoveredTile.y > 11 && gui.hoveredTile.y < 15) // Select channel.
    {
        int selectedX = hoveredXScrolled - 4.0f;
        int selectedChannel = 0;
        bool inChannel = false;



        while (!inChannel)
        {
            if (channels[selectedChannel].compressed)
            {
                if (selectedX >= 3)
                {
                    selectedX -= 3;
                    selectedChannel++;


                    if (selectedChannel >= channels.size())
                    {
                        inChannel = true;
                    }
                }
                else
                {
                    inChannel = true;
                }
            }
            else
            {
                if (selectedX >= 8 + channels[selectedChannel].effectCountPerRow * 4)
                {
                    selectedX -= 8 + channels[selectedChannel].effectCountPerRow * 4;
                    selectedChannel++;


                    if (selectedChannel >= channels.size())
                    {
                        inChannel = true;
                    }
                }
                else
                {
                    inChannel = true;
                }
            }
        }
        if (selectedChannel < loadedSong.numberOfChannels && gui.hoveredTile.y != 12 && gui.hoveredTile.y < 15)
        {
            if (hoveredXScrolled > 4)
            {
                if (selectedX < 8 + channels[selectedChannel].effectCountPerRow * 4)
                {
                    editor.selectedChannel = selectedChannel;
                    windowController.InitializeWindow("Channel", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 16, 34 }); // Open the channel menu.
                }
                gui.drawFrameThisFrame = true; // The interface has changed, and must be redrawn.
            }
        }
    }


    return;
}



void releaseButton()
{

    // Stop dragging scroll bars.
    gui.patternListScrollBar.drag = false;
    gui.instrumentListScrollBar.drag = false;
    gui.patternVerticalScrollBar.drag = false;
    gui.patternHorizontalScrollBar.drag = false;
    gui.mixScrollBar.drag = false;
    gui.draggingScrollBar = false;

    // Set to draw the interface.
    if (gui.hoveredTile.y < 16)
        gui.drawUIThisFrame = true;
    else
        gui.drawFrameThisFrame = true;


    screen.mouseHoldTime = 0.0f;
    screen.mouseDown = false;
    screen.rightMouseDown = false;

    if (editor.channelBeingMoved != -1 && editor.channelBeingMoved != editor.channelMoveTo) // Drag and drop channel.
    {
        
        int ch1 = editor.channelBeingMoved;
        int ch2 = editor.channelMoveTo;

        saveCurrentPattern();

        Channel moveChannel = channels[editor.channelBeingMoved];
        channels.erase(channels.begin() + editor.channelBeingMoved);
        channels.emplace(channels.begin() + editor.channelMoveTo, moveChannel);

        loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channels[ch1].notes.swap(loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channels[ch2].notes);
        loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channels[ch1].volumes.swap(loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channels[ch2].volumes);
        loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channels[ch1].effects.swap(loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channels[ch2].effects);
        

        loadCurrentPattern();


        gui.drawFrameThisFrame = true;
    }
    editor.channelBeingMoved = -1;
    editor.channelMoveTo = -1;



    sampleDisplay.dragLoopStart = false;
    sampleDisplay.dragLoopEnd = false;
    if (sampleDisplay.sampleSelectionEnd < sampleDisplay.sampleStartPos)
        std::swap(sampleDisplay.sampleSelectionEnd, sampleDisplay.sampleStartPos);

    /*
    for (int y = 0; y < 5; y++) // Revert buttons to unpressed.
    {
        gui.activeUI[11][3 + 2 * y].sprite = { 8, 3 };
        gui.activeUI[12][3 + 2 * y].sprite = { 9, 3 };
        gui.activeUI[13][3 + 2 * y].sprite = { 10, 3 };
    }
    gui.activeUI[7][5].sprite = { 8, 3 };
    gui.activeUI[8][5].sprite = { 9, 3 };
    gui.activeUI[9][5].sprite = { 10, 3 };
    gui.activeUI[7][8].sprite = { 8, 3 };
    gui.activeUI[8][8].sprite = { 9, 3 };
    gui.activeUI[9][8].sprite = { 10, 3 };
    // Frame menu scroll arrows
    gui.activeUI[6][2].sprite = { 6, 3 };
    gui.activeUI[6][10].sprite = { 6, 4 };
    // Frame scroll arrows
    gui.activeUI[91][16].sprite = { 6, 3 };
    gui.activeUI[91][55].sprite = { 6, 4 };
    gui.activeUI[5][56].sprite = { 30, 4 };
    gui.activeUI[90][56].sprite = { 30, 3 };
    // File scroll arrows
    gui.activeUI[77][2].sprite = { 6, 3 };
    gui.activeUI[77][11].sprite = { 6, 4 };
    // Sample scroll arrows
    gui.activeUI[91][2].sprite = { 6, 3 };
    gui.activeUI[91][11].sprite = { 6, 4 };
    */

    for (int wind = 0; wind < windowController.windows.size(); wind++) // Stop dragging windows.
    {
        windowController.windows[wind].dragWindow = false;
    }

    gui.clickingOnFloatingWind = false;

    return;
}


void releaseRightButton()
{
    // Set to draw the interface.
    if (gui.hoveredTile.y < 16)
        gui.drawUIThisFrame = true;
    else
        gui.drawFrameThisFrame = true;


    screen.mouseHoldTime = 0.0f;
    screen.mouseDown = false;
    screen.rightMouseDown = false;


    return;
}



int main(void)
{

    RunEngine();


    return 0;
}