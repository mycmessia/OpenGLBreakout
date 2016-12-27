//
//  TextRenderer.hpp
//  OpenGLBreakout
//
//  Created by 梅宇宸 on 16/12/27.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

#ifndef TextRenderer_hpp
#define TextRenderer_hpp

#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture2D.hpp"
#include "Shader.hpp"

#define FONT_FULL_DIR "/Users/meiyuchen/Projects/OpenGLBreakout/OpenGLBreakout/"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
    // Holds a list of pre-compiled Characters
    std::map<GLchar, Character> Characters;
    // Shader used for text rendering
    Shader TextShader;
    // Constructor
    TextRenderer(GLuint width, GLuint height);
    // Pre-compiles a list of characters from the given font
    void Load(std::string font, GLuint fontSize);
    // Renders a string of text using the precompiled list of characters
    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));
private:
    // Render state
    GLuint VAO, VBO;
};


#endif /* TextRenderer_hpp */
