#ifndef SHADERPROGRAM_HPP_
# define SHADERPROGRAM_HPP_

#include <GLES2/gl2.h>
#include "utils.h"

class ShaderProgram
{
    public:
    ShaderProgram(const char *vertex, const char *fragment);
    ~ShaderProgram();
    inline bool     hasError() const { return m_program == 0 ? true : false; }
    GLuint  link();
    inline GLuint   getProgram() const      { return m_program; }
    inline void enable() const
    {
        if (m_program == 0)
            LOGE("Warning : shader is not correctly loaded\n");
        glUseProgram(m_program);
    }
    private:
    GLuint  loadShader(GLenum shaderType, const char *src);
    GLuint  createProgram(const char *vertexSrc, const char *fragmentSrc);
    GLuint  m_program;

};

#endif // SHADERPROGRAM_HPP_

