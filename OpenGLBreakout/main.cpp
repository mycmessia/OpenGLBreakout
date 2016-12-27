//
//  main.cpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/23.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#define GLEW_STATIC

#include "GameController.hpp"
#include "ResourceManager.hpp"

// GLFW function declerations
void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

GameController Breakout (SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[])
{
    glfwInit ();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);   // This is vital on MacOSX
    
    GLFWwindow* window = glfwCreateWindow (SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent (window);
    
    glewExperimental = GL_TRUE;
    glewInit ();
    glGetError (); // Call it once to catch glewInit() bug, all other errors are now from our application.
    
    glfwSetKeyCallback (window, key_callback);
    
    // OpenGL configuration
    int width, height;
    
    // This step is a must for retina maybe
    glfwGetFramebufferSize (window, &width, &height);
    glViewport (0, 0, width, height);
//    glEnable (GL_CULL_FACE); No culling to draw use TRIANGLE_STRIP
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize game
    Breakout.Init (width, height);
    
    // DeltaTime variables
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
    
    // Start Game within Menu State
    Breakout.mState = GAME_MENU;
    
    while (!glfwWindowShouldClose (window))
    {
        // Calculate delta time
        GLfloat currentFrame = glfwGetTime ();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents ();
        
        //deltaTime = 0.001f;
        // Manage user input
        Breakout.ProcessInput (deltaTime);
        
        // Update Game state
        Breakout.Update (deltaTime);
        
        // Render
        glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT);
        Breakout.Render ();
        
        glfwSwapBuffers (window);
    }
    
    // Delete all resources as loaded using the resource manager
    ResourceManager::Clear ();
    
    glfwTerminate ();
    return 0;
}

void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose (window, GL_TRUE);
    }
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            Breakout.mKeys[key] = GL_TRUE;
        }
        else if (action == GLFW_RELEASE)
        {
            Breakout.mKeys[key] = GL_FALSE;
            Breakout.mKeysProcessed[key] = GL_FALSE;
        }
    }
}