//
//  GameController.cpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/23.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#include "Texture2D.hpp"
#include "Shader.hpp"
#include "GameController.hpp"
#include "ResourceManager.hpp"
#include "SpriteRenderer.hpp"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE (100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY (500.0f);

GameObject* Player;

GameController::GameController (GLuint width, GLuint height)
: mState (GAME_ACTIVE), mKeys (), mWidth (width), mHeight (height)
{
    
}

GameController::~GameController ()
{
    if (Renderer)
    {
        delete Renderer;
        Renderer = nullptr;
    }
}

void GameController::Init ()
{
    // Load shaders
    ResourceManager::LoadShader (SHADER_FULL_DIR"sprite.vs", SHADER_FULL_DIR"sprite.frag", nullptr, "sprite");
    
    // Configure shaders
    // The first four arguments specify in order the left, right, bottom and top part of the projection frustum.
    // This projection matrix transforms all x coordinates between 0 and 800 to -1 and 1 and all y coordinates between 0 and 600 to -1 and 1.
    // ---------------------------------
    // |(0, 0)              (mWidth, 0)|
    // |                               |
    // |                               | ---\  (-1, -1)  (1, -1)
    // |                               | ---/  (-1, 1)   (1, 1)
    // |                               |       Normalized Device Coordinates
    // |                               |
    // |(0, mHeight)  (mWidth, mHeight)|
    // ---------------------------------
    glm::mat4 projection = glm::ortho (0.0f, static_cast<GLfloat> (this->mWidth),
                                       static_cast<GLfloat> (this->mHeight), 0.0f, -1.0f, 1.0f);
    
    ResourceManager::GetShader ("sprite").Use ().SetInteger ("image", 0);
    ResourceManager::GetShader ("sprite").SetMatrix4 ("projection", projection);
    
    // Set render-specific controls
    Renderer = new SpriteRenderer (ResourceManager::GetShader("sprite"));
    
    // Load textures
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"monsterface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"paddle.png", true, "paddle");
    
    // Load levels
    GameLevel one; one.Load (LEVEL_FULL_DIR"one.lvl", this->mWidth, this->mHeight * 0.5);
    GameLevel two; two.Load (LEVEL_FULL_DIR"two.lvl", this->mWidth, this->mHeight * 0.5);
    GameLevel three; three.Load (LEVEL_FULL_DIR"three.lvl", this->mWidth, this->mHeight * 0.5);
    GameLevel four; four.Load (LEVEL_FULL_DIR"four.lvl", this->mWidth, this->mHeight * 0.5);
    this->Levels.push_back (one);
    this->Levels.push_back (two);
    this->Levels.push_back (three);
    this->Levels.push_back (four);
    this->Level = 1;
    
    glm::vec2 playerPos = glm::vec2 (this->mWidth / 2 - PLAYER_SIZE.x / 2,
                                     this->mHeight - PLAYER_SIZE.y);
    Player = new GameObject (playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

void GameController::Update (GLfloat dt)
{
    
}


void GameController::ProcessInput (GLfloat dt)
{
    if (this->mState == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // Move playerboard
        if (this->mKeys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0)
                Player->Position.x -= velocity;
        }
        if (this->mKeys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->mWidth - Player->Size.x)
                Player->Position.x += velocity;
        }
    }
}

void GameController::Render ()
{
    if(this->mState == GAME_ACTIVE)
    {
        // Draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
                             glm::vec2(0, 0), glm::vec2(this->mWidth, this->mHeight), 0.0f);
        
        // Draw level
        this->Levels[this->Level].Draw (*Renderer);
        
        Player->Draw (*Renderer);

    }
}