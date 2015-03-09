#ifndef GLAPP_H_
# define GL_APP_H_

#ifdef ANDROID
# include <jni.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <string.h>
#include "Matrix4x4.hpp"
#include "utils.h"
#include "ParticleEngine.h"
#include "ShaderProgram.hpp"

#define LOW_FPS   30
#define HIGH_FPS  50

class GlApp
{
    public:

#ifdef ANDROID
    GlApp(int ptSize, bool motionBlur, int width, int height, JNIEnv *jenv);
#else
    GlApp(int ptSize, bool motionBlur, int width, int height);
#endif

    ~GlApp();

    // engine setup
    void            setEngine(ParticleEngine *engine) { m_engine = engine; }

    // OpenGL drawing
    void            draw();

    // Time management
    void            countFPS();

    inline void            setZoom(float zoom)
    {
        m_zoom = zoom;
    }

    inline float            getZoom() const
    {
        return m_zoom;
    }

    inline void         setBlur(float blur)
    {
        m_blur = blur;
    }

    inline float        getBlur() const
    {
        return m_blur;
    }

    inline void         setSize(float size)
    {
        m_ptSize = size;
    }

    inline float        getSize() const
    {
        return m_ptSize;
    }

    private:

    long FPS_to_particles(float gpu, float cpu, long particles);

    template<typename T, size_t N>
    inline size_t   sizeofArray(T(&)[ N ])
    {
        return N;
    }
#ifdef ANDROID
    //  Creates a shader program from an android resource
    ShaderProgram   *createProgramFromResource(const char *vertexResource, const char *fragmentResource);
#else
    ShaderProgram   *createProgram(const char *vertex, const char *fragment);
#endif

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

    // Blur intensity handle
    GLuint          m_blurHandle;

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

    // zoom value
    float           m_zoom;

    // blur intensity
    float           m_blur;

    // Particle engine
    ParticleEngine *m_engine;

#ifdef ANDROID
    // jni environment
    JNIEnv          *m_jenv;
#endif
};


#endif // GL_APP


