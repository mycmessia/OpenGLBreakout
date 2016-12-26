//
//  GameController.cpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/23.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#include <iostream>

#include "Texture2D.hpp"
#include "Shader.hpp"
#include "GameController.hpp"
#include "ResourceManager.hpp"
#include "SpriteRenderer.hpp"
#include "Ball.hpp"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE (100, 20);

// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY (500.0f);

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY (100.0f, -350.0f);

// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

GameController::GameController (GLuint width, GLuint height)
: mState (GAME_ACTIVE), mKeys (), mWidth (width), mHeight (height)
{
    
}

GameController::~GameController ()
{
    if (mRenderer)
    {
        delete mRenderer;
        mRenderer = nullptr;
    }
    
    if (mPlayer)
    {
        delete mPlayer;
        mPlayer = nullptr;
    }
    
    if (mBall)
    {
        delete mBall;
        mBall = nullptr;
    }
    
    if (mParticles)
    {
        delete mParticles;
        mParticles = nullptr;
    }
}

void GameController::Init ()
{
    // Load shaders
    ResourceManager::LoadShader (SHADER_FULL_DIR"sprite.vs", SHADER_FULL_DIR"sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader (SHADER_FULL_DIR"particle.vs", SHADER_FULL_DIR"particle.frag", nullptr, "particle");
    
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
    
    ResourceManager::GetShader ("particle").Use ().SetInteger ("sprite", 0);
    ResourceManager::GetShader ("particle").SetMatrix4 ("projection", projection);
    
    // Set render-specific controls
    mRenderer = new SpriteRenderer (ResourceManager::GetShader("sprite"));
    
    // Load textures
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"monsterface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"paddle.png", true, "paddle");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"particle.png", GL_TRUE, "particle");
    
    // Load levels
    GameLevel one; one.Load (LEVEL_FULL_DIR"one.lvl", this->mWidth, this->mHeight * 0.5);
    GameLevel two; two.Load (LEVEL_FULL_DIR"two.lvl", this->mWidth, this->mHeight * 0.5);
    GameLevel three; three.Load (LEVEL_FULL_DIR"three.lvl", this->mWidth, this->mHeight * 0.5);
    GameLevel four; four.Load (LEVEL_FULL_DIR"four.lvl", this->mWidth, this->mHeight * 0.5);
    this->mLevels.push_back (one);
    this->mLevels.push_back (two);
    this->mLevels.push_back (three);
    this->mLevels.push_back (four);
    this->mLevel = 1;
    
    glm::vec2 playerPos = glm::vec2 (this->mWidth / 2 - PLAYER_SIZE.x / 2,
                                     this->mHeight - PLAYER_SIZE.y);
    mPlayer = new GameObject (playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    
    glm::vec2 ballPos = playerPos + glm::vec2 (PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    mBall = new Ball (ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                     ResourceManager::GetTexture ("face"));
    
    mParticles = new ParticleGenerator (
        ResourceManager::GetShader ("particle"),
        ResourceManager::GetTexture ("particle"),
        100
    );
}

void GameController::Update (GLfloat dt)
{
    mBall->Move (dt, this->mWidth);
    
    this->DetectCollision ();
    
    if (mBall->Position.y >= this->mHeight) // Did ball reach bottom edge?
    {
        this->ResetLevel ();
        this->ResetPlayer ();
    }
    
    // Update particles
    mParticles->Update (dt, *mBall, 2, glm::vec2 (mBall->Radius / 2));
}


void GameController::ProcessInput (GLfloat dt)
{
    if (this->mState == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        
        // Move playerboard
        if (this->mKeys[GLFW_KEY_A])
        {
            if (mPlayer->Position.x >= 0)
            {
                mPlayer->Position.x -= velocity;
                
                if (mBall->Stuck)
                    mBall->Position.x -= velocity;
            }
        }
        
        if (this->mKeys[GLFW_KEY_D])
        {
            if (mPlayer->Position.x <= this->mWidth - mPlayer->Size.x)
            {
                mPlayer->Position.x += velocity;
                
                if (mBall->Stuck)
                    mBall->Position.x += velocity;
            }
        }
        
        if (this->mKeys[GLFW_KEY_SPACE])
            mBall->Stuck = false;
    }
}

void GameController::Render ()
{
    if(this->mState == GAME_ACTIVE)
    {
        // Draw background
        mRenderer->DrawSprite (ResourceManager::GetTexture ("background"),
                               glm::vec2 (0, 0), glm::vec2 (this->mWidth, this->mHeight), 0.0f);
//
        // Draw level
        this->mLevels[this->mLevel].Draw (*mRenderer);
        
        mPlayer->Draw (*mRenderer);
        
        // Draw particles
        mParticles->Draw ();

        mBall->Draw (*mRenderer);
    }
}

void GameController::BroadPhaseCollisionDetect ()
{
    for (GameObject &box : this->mLevels[this->mLevel].Bricks)
    {
        if (!box.Destroyed)
        {
            CollisionPairs cp (*mBall, box);
            
            cpVector.push_back (cp);
        }
    }
}

Direction GameController::VectorDirection (glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2 (0.0f, 1.0f),     // up
        glm::vec2 (1.0f, 0.0f),     // right
        glm::vec2 (0.0f, -1.0f),	// down
        glm::vec2 (-1.0f, 0.0f)     // left
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++)
    {
        GLfloat dot_product = glm::dot (glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

Collision GameController::NarrowPhaseCollisionDetect (Ball& one, GameObject& two)
{
    // Get center point circle first
    glm::vec2 center (one.Position + one.Radius);
    
    // Calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents (two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center (two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    
    // Get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp (difference, -aabb_half_extents, aabb_half_extents);
    
    // Add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    
    // Retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    
    if (glm::length (difference) <= one.Radius)
        return std::make_tuple (GL_TRUE, VectorDirection (difference), difference, closest);
    else
        return std::make_tuple (GL_FALSE, UP, glm::vec2 (0, 0), glm::vec2 (0, 0));
}

void GameController::DetectCollision ()
{
    BroadPhaseCollisionDetect ();
    
    for (unsigned i = 0; i < cpVector.size (); i++)
    {
        Collision collision = NarrowPhaseCollisionDetect (cpVector[i].ball, cpVector[i].brick);
        if (std::get<0>(collision)) // If collision is true
        {
            // Destroy brick if not solid
            if (!cpVector[i].brick.IsSolid)
            {
                cpVector[i].brick.Destroyed = GL_TRUE;
            }
            
            // Collision resolution
            Direction dir = std::get<1>(collision);
            glm::vec2 diff_vector = std::get<2>(collision);
            if (dir == LEFT || dir == RIGHT) // Horizontal collision
            {
                mBall->Velocity.x = -mBall->Velocity.x; // Reverse horizontal velocity
                // Relocate
                GLfloat penetration = mBall->Radius - std::abs (diff_vector.x);
                if (dir == LEFT)
                    mBall->Position.x += penetration; // Move ball to right
                else
                    mBall->Position.x -= penetration; // Move ball to left;
            }
            else // Vertical collision
            {
                mBall->Velocity.y = -mBall->Velocity.y; // Reverse vertical velocity
                // Relocate
                GLfloat penetration = mBall->Radius - std::abs (diff_vector.y);
                if (dir == UP)
                    mBall->Position.y -= penetration; // Move ball back up
                else
                    mBall->Position.y += penetration; // Move ball back down
            }
            
            break;
        }
    }
    
    Collision result = NarrowPhaseCollisionDetect (*mBall, *mPlayer);
    if (!mBall->Stuck &&
        std::get<0>(result) &&
        std::get<3>(result).y < mHeight - PLAYER_SIZE.y / 2)    // not the bottom part of the paddle touch the player
    {
        // Check where it hit the board, and change velocity based on where it hit the board
        GLfloat centerBoard = mPlayer->Position.x + mPlayer->Size.x / 2;
        GLfloat distance = (mBall->Position.x + mBall->Radius) - centerBoard;
        GLfloat percentage = distance / (mPlayer->Size.x / 2);
        
        // Then move accordingly
        GLfloat strength = 2.0f;
        glm::vec2 oldVelocity = mBall->Velocity;
        mBall->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        
        //ball->Velocity.y = -ball->Velocity.y;
        mBall->Velocity.y = -1 * std::abs (mBall->Velocity.y);
        mBall->Velocity = glm::normalize (mBall->Velocity) * glm::length (oldVelocity);
    }
    
    cpVector.clear ();
}

void GameController::ResetLevel ()
{
    if (this->mLevel == 1)
        this->mLevels[0].Load (LEVEL_FULL_DIR"one.lvl", this->mWidth, this->mHeight * 0.5f);
    else if (this->mLevel == 2)
        this->mLevels[1].Load (LEVEL_FULL_DIR"two.lvl", this->mWidth, this->mHeight * 0.5f);
    else if (this->mLevel == 3)
        this->mLevels[2].Load (LEVEL_FULL_DIR"three.lvl", this->mWidth, this->mHeight * 0.5f);
    else if (this->mLevel == 4)
        this->mLevels[3].Load (LEVEL_FULL_DIR"four.lvl", this->mWidth, this->mHeight * 0.5f);
}

void GameController::ResetPlayer ()
{
    // Reset player/ball stats
    mPlayer->Size = PLAYER_SIZE;
    mPlayer->Position = glm::vec2 (this->mWidth / 2 - PLAYER_SIZE.x / 2, this->mHeight - PLAYER_SIZE.y);
    mBall->Reset (mPlayer->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}