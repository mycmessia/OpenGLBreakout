//
//  PowerUp.hpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/26.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#ifndef PowerUp_hpp
#define PowerUp_hpp

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.hpp"

const glm::vec2 SIZE (60, 20);
const glm::vec2 VELOCITY (0.0f, 150.0f);

const glm::vec3 ORANGE_RED (1.0f, 0.27f, 0.0f);
const glm::vec3 LAWN_GREEN (0.5f, 0.96f, 0.0f);
const glm::vec3 YELLOW (1.0f, 1.0f, 1.0f);
const glm::vec3 DODGER_BLUE (0.11f, 0.56f, 1.0f);
const glm::vec3 PURPLE (0.51f, 0.0f, 0.51f);
const glm::vec3 PINK (1.0f, 20 / 255, 147 / 255);
const glm::vec3 CHOCOLATE (210 / 255, 105 / 255, 30 / 255);
const glm::vec3 GRAY (0.5f, 0.5f, 0.5f);

class PowerUp : public GameObject
{
public:
    // PowerUp State
    std::string Type;
    GLfloat Duration;
    GLboolean Activated;
    
    // Constructor
    PowerUp (std::string type, glm::vec3 color, GLfloat duration,
             glm::vec2 position, Texture2D texture)
    : GameObject (position, SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated()
    {
        
    }
};

#endif /* PowerUp_hpp */
