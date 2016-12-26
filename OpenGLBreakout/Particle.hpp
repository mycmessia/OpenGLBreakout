//
//  Particle.hpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/26.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#ifndef Particle_hpp
#define Particle_hpp

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture2D.hpp"
#include "GameObject.hpp"


// Represents a single particle and its state
struct Particle
{
    Particle () : Position (0.0f), Velocity (0.0f), Color (1.0f), Life (0.0f) { }
    
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life;
};


// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
public:
    // Constructor
    ParticleGenerator (Shader shader, Texture2D texture, GLuint amount);
    // Update all particles
    void Update (GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2 (0.0f, 0.0f));
    // Render all particles
    void Draw ();
private:
    // State
    std::vector<Particle> particles;
    GLuint amount;
    // Render state
    Shader shader;
    Texture2D texture;
    GLuint VAO;
    // Initializes buffer and vertex attributes
    void init ();
    // Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    GLuint firstUnusedParticle ();
    // Respawns particle
    void respawnParticle (Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2 (0.0f, 0.0f));
};

#endif /* Particle_hpp */
