//
//  GameLevel.hpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/24.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#ifndef GameLevel_hpp
#define GameLevel_hpp

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.hpp"
#include "SpriteRenderer.hpp"
#include "ResourceManager.hpp"

#define LEVEL_FULL_DIR "/Users/meiyuchen/Projects/OpenGLBreakout/OpenGLBreakout/"

/// GameLevel holds all Tiles as part of a Breakout level and
/// hosts functionality to Load/render levels from the harddisk.
class GameLevel
{
public:
    // Level state
    std::vector<GameObject> Bricks;
    // Constructor
    GameLevel ();
    // Loads level from file
    void      Load (const GLchar *file, GLuint levelWidth, GLuint levelHeight);
    // Render level
    void      Draw (SpriteRenderer &renderer);
    // Check if the level is completed (all non-solid tiles are destroyed)
    GLboolean IsCompleted ();
private:
    // Initialize level from tile data
    void      init (std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};


#endif /* GameLevel_hpp */
