//
//  Ball.cpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/24.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#include "Ball.hpp"

#include "Texture2D.hpp"
#include "SpriteRenderer.hpp"

Ball::Ball ()
: GameObject (), Radius (12.5f), Stuck (true), Sticky (false), PassThrough (false)
{
    
}

Ball::Ball (glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
:  GameObject (pos, glm::vec2 (radius * 2, radius * 2), sprite, glm::vec3 (1.0f), velocity), Radius (radius), Stuck (true), Sticky (false), PassThrough (false)
{
    
}

glm::vec2 Ball::Move (GLfloat dt, GLuint window_width)
{
    // If not stuck to player board
    if (!this->Stuck)
    {
        // Move the ball
        this->Position += this->Velocity * dt;
        
        // Then check if outside window bounds and if so, reverse velocity and restore at correct position
        if (this->Position.x <= 0.0f)
        {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = 0.0f;
        }
        else if (this->Position.x + this->Size.x >= window_width)
        {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = window_width - this->Size.x;
        }
        
        if (this->Position.y <= 0.0f)
        {
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = 0.0f;
        }
    }
    
    return this->Position;
}

// Resets the ball to initial Stuck Position (if ball is outside window bounds)
void Ball::Reset (glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
}