#include "ShaderProgram.hpp"
#include "utils.h"
#include <stdlib.h>

ShaderProgram::ShaderProgram(const char *vertex, const char *fragment)
{
    m_program = createProgram(vertex, fragment);
}

ShaderProgram::~ShaderProgram()
{
    //glUseProgram(0);
    //glDeleteProgram(m_program);
    m_program = 0;
}

GLuint ShaderProgram::loadShader(GLenum shaderType, const char* src) {
    GLuint shader = glCreateShader(shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint ShaderProgram::createProgram(const char *vertexSrc, const char *fragmentSrc)
{
    if (!vertexSrc || !fragmentSrc)
        return 0;
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!vertexShader || !pixelShader)
        return 0;
    GLuint program = glCreateProgram();

    if (program)
    {
        glAttachShader(program, vertexShader);
        Utils::checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        Utils::checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(m_program);
            program = 0;
        }
    }
    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);
    return program;
}

