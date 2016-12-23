//
//  GameController.hpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/23.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#ifndef GameController_hpp
#define GameController_hpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class SpriteRenderer;

enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class GameController
{
public:
    GameState mState;
    GLboolean mKeys[1024];
    GLuint mWidth, mHeight;
    
    SpriteRenderer* Renderer;
    
    GameController (GLuint width, GLuint height);
    ~GameController ();
    
    void Init ();
    
    void ProcessInput (GLfloat dt);
    void Update (GLfloat dt);
    void Render ();
};

#endif /* GameController_hpp */
