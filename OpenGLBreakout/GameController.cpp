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
const GLfloat BALL_RADIUS = 15.0f;

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
    
    if (mEffects)
    {
        delete mEffects;
        mEffects = nullptr;
    }
}

void GameController::Init (GLuint frameBufferWidth, GLuint frameBufferHeight)
{
    mFrameBufferWidth = frameBufferWidth;
    mFrameBufferHeight = frameBufferHeight;
    
    // Load shaders
    ResourceManager::LoadShader (SHADER_FULL_DIR"sprite.vs", SHADER_FULL_DIR"sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader (SHADER_FULL_DIR"particle.vs", SHADER_FULL_DIR"particle.frag", nullptr, "particle");
    ResourceManager::LoadShader (SHADER_FULL_DIR"post_processing.vs", SHADER_FULL_DIR"post_processing.frag", nullptr, "postprocessing");
    
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
    
    ResourceManager::GetShader ("postprocessing").SetMatrix4 ("projection", projection);
    
    // Set render-specific controls
    mRenderer = new SpriteRenderer (ResourceManager::GetShader("sprite"));
    
    // Load textures
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"sheep.png", GL_TRUE, "face");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"paddle.png", true, "paddle");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"particle.png", GL_TRUE, "particle");
    
    // Power Ups
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"powerup_speed.png", GL_TRUE, "powerup_speed");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"powerup_sticky.png", GL_TRUE, "powerup_sticky");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"powerup_increase.png", GL_TRUE, "powerup_increase");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"powerup_confuse.png", GL_TRUE, "powerup_confuse");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"powerup_chaos.png", GL_TRUE, "powerup_chaos");
    ResourceManager::LoadTexture (TEXTURE_FULL_DIR"powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
    
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
    
    mEffects = new PostProcessor (
        ResourceManager::GetShader("postprocessing"), this->mFrameBufferWidth, this->mFrameBufferHeight);
    
    ShakeTime = 0.0f;
    
//    mEffects->Chaos = true;
//    mEffects->Confuse = true;
}

void GameController::Update (GLfloat dt)
{
    mBall->Move (dt, this->mWidth);
    
    this->DetectCollision ();
    
    // Update particles
    mParticles->Update (dt, *mBall, 2, glm::vec2 (mBall->Radius / 2));
    
    // Update PowerUps
    this->UpdatePowerUps (dt);
    
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
        {
            mEffects->Shake = false;
        }
    }
    
    if (mBall->Position.y >= this->mHeight) // Did ball reach bottom edge?
    {
        this->ResetLevel ();
        this->ResetPlayer ();
    }
}


void GameController::ProcessInput (GLfloat dt)
{
    if (this->mState == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        
        // Move playerboard
        if (this->mKeys[GLFW_KEY_LEFT])
        {
            if (mPlayer->Position.x >= 0)
            {
                mPlayer->Position.x -= velocity;
                
                if (mBall->Stuck)
                    mBall->Position.x -= velocity;
            }
        }
        
        if (this->mKeys[GLFW_KEY_RIGHT])
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
        mEffects->BeginRender();
        
        // Draw background
        mRenderer->DrawSprite (ResourceManager::GetTexture ("background"),
                               glm::vec2 (0, 0), glm::vec2 (this->mWidth, this->mHeight), 0.0f);
//
        // Draw level
        mLevels[mLevel].Draw (*mRenderer);
        
        mPlayer->Draw (*mRenderer);
        
        // Draw particles
        mParticles->Draw ();

        mBall->Draw (*mRenderer);
        
        for (PowerUp &powerUp : this->PowerUps)
            if (!powerUp.Destroyed)
                powerUp.Draw (*mRenderer);
        
        mEffects->EndRender ();
        
        mEffects->Render (glfwGetTime());
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

GLboolean GameController::NarrowPhaseCollisionDetect (GameObject &one, GameObject &two)
{
    // Collision x-axis?
    GLboolean collisionX = one.Position.x + one.Size.x >= two.Position.x &&
    two.Position.x + two.Size.x >= one.Position.x;
    // Collision y-axis?
    GLboolean collisionY = one.Position.y + one.Size.y >= two.Position.y &&
    two.Position.y + two.Size.y >= one.Position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
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
                this->SpawnPowerUps (cpVector[i].brick);
            }
            else
            {   // if block is solid, enable shake effect
                ShakeTime = 0.05f;
                mEffects->Shake = true;
            }
            // Collision resolution
            Direction dir = std::get<1>(collision);
            glm::vec2 diff_vector = std::get<2>(collision);
            if (!(cpVector[i].ball.PassThrough && !cpVector[i].brick.IsSolid))
            {
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
            }
            
            break;
        }
    }
    
    cpVector.clear ();
    
    Collision result = NarrowPhaseCollisionDetect (*mBall, *mPlayer);
    if (!mBall->Stuck &&
        std::get<0>(result) &&
        std::get<3>(result).y < mHeight - PLAYER_SIZE.y / 2)    // not the bottom part of the paddle touch the player
    {
        mBall->Stuck = mBall->Sticky;
        
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
    
    // Also check collisions on PowerUps and if so, activate them
    for (PowerUp &powerUp : this->PowerUps)
    {
        if (!powerUp.Destroyed)
        {
            // First check if powerup passed bottom edge, if so: keep as inactive and destroy
            if (powerUp.Position.y >= this->mHeight)
                powerUp.Destroyed = GL_TRUE;
            
            if (NarrowPhaseCollisionDetect (*mPlayer, powerUp))
            {	// Collided with player, now activate powerup
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = GL_TRUE;
                powerUp.Activated = GL_TRUE;
            }
        }
    }
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

GLboolean IsOtherPowerUpActive (std::vector<PowerUp> &powerUps, std::string type)
{
    for (const PowerUp &powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    
    return GL_FALSE;
}

void GameController::UpdatePowerUps (GLfloat dt)
{
    for (PowerUp &powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;
            
            if (powerUp.Duration <= 0.0f)
            {
                // Remove powerup from list (will later be removed)
                powerUp.Activated = GL_FALSE;
                // Deactivate effects
                if (powerUp.Type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {	// Only reset if no other PowerUp of type sticky is active
                        mBall->Sticky = GL_FALSE;
                        mPlayer->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {	// Only reset if no other PowerUp of type pass-through is active
                        mBall->PassThrough = GL_FALSE;
                        mBall->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                    {	// Only reset if no other PowerUp of type confuse is active
                        mEffects->Confuse = GL_FALSE;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {	// Only reset if no other PowerUp of type chaos is active
                        mEffects->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    // Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
    // Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
                                        [](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
                                        ), this->PowerUps.end());
}


GLboolean ShouldSpawn (GLuint chance)
{
    GLuint random = rand () % chance;
    return random == 0;
}

void GameController::SpawnPowerUps (GameObject &block)
{
    if (ShouldSpawn(75)) // 1 in 75 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
    if (ShouldSpawn(15)) // Negative powerups should spawn more often
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
    if (ShouldSpawn(15))
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void GameController::ActivatePowerUp(PowerUp &powerUp)
{
    // Initiate a powerup based type of powerup
    if (powerUp.Type == "speed")
    {
        mBall->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        mBall->Sticky = GL_TRUE;
        mPlayer->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        mBall->PassThrough = GL_TRUE;
        mBall->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        mPlayer->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!mEffects->Chaos)
            mEffects->Confuse = GL_TRUE; // Only activate if chaos wasn't already active
    }
    else if (powerUp.Type == "chaos")
    {
        if (!mEffects->Confuse)
            mEffects->Chaos = GL_TRUE;
    }
}