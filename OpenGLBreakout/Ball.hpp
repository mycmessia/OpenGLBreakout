//
//  Ball.hpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/24.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#ifndef Ball_hpp
#define Ball_hpp

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.hpp"

class Ball : public GameObject
{
public:
    // Ball state
    GLfloat   Radius;
    GLboolean Stuck;
    GLboolean Sticky, PassThrough;
    
    Ball ();
    Ball (glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);
    
    glm::vec2 Move (GLfloat dt, GLuint window_width);
    void      Reset (glm::vec2 position, glm::vec2 velocity);
};

#endif /* Ball_hpp */
