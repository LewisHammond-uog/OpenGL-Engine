#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <string>

#include "glad/glad.h"

//Structure to Define a texture in OpenGL
class Texture
{
public:
    Texture(GLenum a_textureTarget, const std::string& a_fileName);

    // Should be called once to load the texture
    bool Load();

    // Must be called at least once for the specific texture unit
    void Bind(GLenum a_textureUnit) const;

private:
    std::string m_fileName;
    GLenum m_textureType;
    GLuint m_textureHandle;
};

#endif // __TEXTURE_H__
