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
    glm::mat4 projection = glm::ortho (0.0f, static_cast<GLfloat> (this->mWidth),
                                       static_cast<GLfloat> (this->mHeight), 0.0f, -1.0f, 1.0f);
    
    ResourceManager::GetShader ("sprite").Use ().SetInteger ("image", 0);
    ResourceManager::GetShader ("sprite").SetMatrix4 ("projection", projection);
    
    // Set render-specific controls
    Renderer = new SpriteRenderer (ResourceManager::GetShader("sprite"));
    
    // Load textures
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"monsterface.png", GL_TRUE, "face");
}

void GameController::Update (GLfloat dt)
{
    
}


void GameController::ProcessInput (GLfloat dt)
{
    
}

void GameController::Render ()
{
    Renderer->DrawSprite (ResourceManager::GetTexture("face"),
                          glm::vec2(200, 200), glm::vec2(100, 100), 45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}