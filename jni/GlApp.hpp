#ifndef GLAPP_H_
# define GL_APP_H_

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "Matrix4x4.hpp"
#include "utils.h"
#include "ParticleEngine.h"
#include "ShaderProgram.hpp"

#define LOW_FPS   30
#define HIGH_FPS  50

class GlApp
{
    public:

    GlApp(int ptSize, bool motionBlur, int width, int height, JNIEnv *jenv);

    ~GlApp();

    // engine setup
    void            setEngine(ParticleEngine *engine) { m_engine = engine; }

    // OpenGL drawing
    void            draw();

    // Time management
    void            countFPS();

    private:

    template<typename T, size_t N>
    inline size_t   sizeofArray(T(&)[ N ])
    {
        return N;
    }
    //  Creates a shader program from an android resource
    ShaderProgram   *createProgramFromResource(const char *vertexResource, const char *fragmentResource);

    // Creates the frameBuffer texture for motion blur
    void            genBlurText(int w, int h);

    // Init shaders, and matrices
    bool            setupGraphics(int w, int h);

    // screen coordinates for blur texture
    GLfloat   *m_screenTarget;

    // texrure coordinates for blur texture
    GLfloat   *m_texCoord;


    // Shaders
    ShaderProgram   *m_particlesProgram;
    ShaderProgram   *m_blurProgram;
    ShaderProgram   *m_screenProgram;

    // Postion handles
    GLuint          m_positionHandleParticles;
    GLuint          m_positionHandleBlur;
    GLuint          m_positionHandleScreen;

    // Texture handles
    GLuint          m_shaderTexAttribBlur;
    GLuint          m_shaderTexAttribScreen;

    // Texture coordinates handles
    GLuint          m_texCoordScreen;
    GLuint          m_texCoordBlur;

    // Color handle
    // Not used anymore
    //GLuint        m_gvColorHandle;

    // MVP matrix handle
    GLuint          m_mvpHandle;

    // Point size handle
    GLuint          m_ptSizeHandle;

    // GL textures
    GLuint          m_blurTexture;
    GLuint          m_texFrameBuffer;

    // matrices
    Mat4            m_projection;
    Mat4            m_model;
    Mat4            m_mvp;

    // point size
    float           m_ptSize;
    bool            m_motionBlur;

    // screen size
    int             m_width;
    int             m_height;

    // time management information
    long            m_frameNb;
    long            m_stepCount;
    time_t          m_start;
    time_t          m_last;
    long            m_engineLast;
    long            m_gpuLast;

    // Particle engine
    ParticleEngine *m_engine;

    // jni environment
    JNIEnv          *m_jenv;
};


#endif // GL_APP


