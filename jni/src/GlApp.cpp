#include <unistd.h>
#include "GlApp.hpp"
using namespace Utils;

#ifdef ANDROID
GlApp::GlApp(int ptSize, bool motionBlur, int width, int height, JNIEnv *jenv) : 
	m_ptSize(ptSize), m_motionBlur(motionBlur), m_width(width), m_height(height), m_blur(0.8), m_jenv(jenv)
#else
GlApp::GlApp(int ptSize, bool motionBlur, int width, int height) : 
	m_ptSize(ptSize), m_motionBlur(motionBlur), m_width(width), m_height(height), m_blur(0.8)
#endif
{
	GLfloat screenTarget[]  = {    -1.0f,  -1.0f,
		-1.0f,  1.0f,
		1.0f,   -1.0f,
		1.0f,   1.0f    };

	GLfloat texCoord[] = {0, 0,
		0, 1,
		1, 0,
		1, 1};

	m_screenTarget = new GLfloat[sizeofArray(screenTarget)];
	m_texCoord = new GLfloat[sizeofArray(texCoord)];

	memcpy(static_cast<void*>(m_screenTarget), &(screenTarget[0]), sizeof(screenTarget));
	memcpy(static_cast<void*>(m_texCoord), &(texCoord[0]), sizeof(texCoord));


	setupGraphics(width, height);

	m_stepCount = 0;
	m_start = time(0);
	m_last = time(0);
	m_engineLast = 0;
	m_gpuLast = 0;
	m_zoom = 0;
	m_lastMs = 0;
}


GlApp::~GlApp()
{
    // I don't really need to clean the current context since it will be already destroyed at this time
	delete m_particlesProgram;
	delete m_blurProgram;
	delete m_screenProgram;
}

void            GlApp::genBlurText(int w, int h)
{
	// Creating the blur texture map
	glGenTextures(1, &m_blurTexture);
	checkGlError("glGenTextures");
	glBindTexture(GL_TEXTURE_2D, m_blurTexture);
	checkGlError("glBindTexture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	checkGlError(__FILE__, __LINE__);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	checkGlError(__FILE__, __LINE__);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	checkGlError(__FILE__, __LINE__);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	checkGlError(__FILE__, __LINE__);

	void *imageData = (void*)malloc(sizeof(unsigned char) * w * h * 4);
	memset(imageData, 0, (w * h * 4));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	checkGlError("glTexImage2D");
	free(imageData);

	glGenFramebuffers(1, &m_texFrameBuffer);;
	checkGlError(__FILE__, __LINE__);
	glBindFramebuffer(GL_FRAMEBUFFER, m_texFrameBuffer);
	checkGlError(__FILE__, __LINE__);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTexture, 0);
	checkGlError(__FILE__, __LINE__);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGlError(__FILE__, __LINE__);
}

#ifndef ANDROID

#define RESOURCE_START(name) (_binary____assets_##name##_glsl_start)
#define RESOURCE_END(name) (_binary____assets_##name##_glsl_end)
#define RESOURCE_PTR(name) (RESOURCE_START(name))

#define RESOURCE_DECLARE(name)	extern "C"\
				{\
				    extern char RESOURCE_START(name)[];\
				    extern char RESOURCE_END(name)[];\
				}
#define RESOURCE_INIT(name) {\
				char *ptBegin = RESOURCE_START(name);\
				char *ptEnd = RESOURCE_END(name);\
				int size = (int)(ptEnd - ptBegin);\
				RESOURCE_PTR(name)[size - 1] = 0;\
			    }

RESOURCE_DECLARE(ParticleFS)
RESOURCE_DECLARE(ParticleVS)
RESOURCE_DECLARE(ScreenFS)
RESOURCE_DECLARE(ScreenVS)
RESOURCE_DECLARE(BlurFS)

#endif

bool            GlApp::setupGraphics(int w, int h)
{

	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);
	LOGI("setupGraphics(%d, %d)", w, h);

#ifdef ANDROID
	m_particlesProgram = createProgramFromResource("ParticleVS.glsl", "ParticleFS.glsl");
	m_blurProgram = createProgramFromResource("ScreenVS.glsl", "BlurFS.glsl");
	m_screenProgram = createProgramFromResource("ScreenVS.glsl", "ScreenFS.glsl");
#else
	RESOURCE_INIT(ParticleFS)
	RESOURCE_INIT(ParticleVS)
	RESOURCE_INIT(ScreenFS)
	RESOURCE_INIT(ScreenVS)
	RESOURCE_INIT(BlurFS)

	m_particlesProgram = createProgram( RESOURCE_PTR(ParticleVS), 
					    RESOURCE_PTR(ParticleFS));

	m_blurProgram = createProgram(	RESOURCE_PTR(ScreenVS), 
					RESOURCE_PTR(BlurFS));

	m_screenProgram = createProgram(RESOURCE_PTR(ScreenVS), 
					RESOURCE_PTR(ScreenFS));
#endif

	m_ptSizeHandle = glGetUniformLocation(m_particlesProgram->getProgram(), "ptSize");
	checkGlError("glGetUniformLocation");

	m_positionHandleBlur = glGetAttribLocation(m_blurProgram->getProgram(), "vPosition");
	checkGlError("glGetAttribLocation");

	m_texCoordScreen = glGetAttribLocation(m_screenProgram->getProgram(), "texCoord");
	checkGlError("glGetAttribLocation");

	m_texCoordBlur = glGetAttribLocation(m_blurProgram->getProgram(), "texCoord");
	checkGlError("glGetAttribLocation");

	m_shaderTexAttribBlur = glGetUniformLocation(m_blurProgram->getProgram(), "texture");
	checkGlError("glGetUniformLocation");

	m_positionHandleScreen = glGetAttribLocation(m_screenProgram->getProgram(), "vPosition");
	checkGlError("glGetAttribLocation");

	m_shaderTexAttribScreen = glGetUniformLocation(m_screenProgram->getProgram(), "texture");
	checkGlError("glGetUniformLocation");

	m_positionHandleParticles = glGetAttribLocation(m_particlesProgram->getProgram(), "vPosition");
	checkGlError("glGetAttribLocation");

	m_blurHandle = glGetUniformLocation(m_blurProgram->getProgram(), "blur");
	checkGlError("glGetUniformLocation");

	m_mvpHandle = glGetUniformLocation(m_particlesProgram->getProgram(), "mvp");
	checkGlError("glGetUniformLocation");

	if (m_motionBlur)
	{
		genBlurText(w, h);
        setBlur(m_blur); // set default value
	}

	glViewport(0, 0, w,  h);
	checkGlError(__FILE__, __LINE__);
	m_model.setidentity();
	m_projection.setProjection(0.1, 50, 45, ((float)w)/((float)h));
	return true;
}

#ifdef ANDROID

ShaderProgram   *GlApp::createProgramFromResource(const char *vertexResource, const char *fragmentResource)
{
	char *vfile = Utils::loadRessource(m_jenv, vertexResource);
	char *ffile = Utils::loadRessource(m_jenv, fragmentResource);
	ShaderProgram *program = new ShaderProgram(vfile, ffile);
	free(vfile);
	free(ffile);
	if (program->hasError()) {
		LOGE("Could not create program. Using \"%s\" and \"%s\"", vertexResource, fragmentResource);
		exit(EXIT_FAILURE);
	}
	return program;
}

#else
ShaderProgram   *GlApp::createProgram(const char *vfile, const char *ffile)
{
	ShaderProgram *program = new ShaderProgram(vfile, ffile);
	if (program->hasError()) {
		LOGE("Could not create program. Using \"%s\" and \"%s\"", vfile, ffile);
		exit(EXIT_FAILURE);
	}
	return program;
}

#endif

void            GlApp::draw()
{
	++m_frameNb;
	static float angle = 0;
	angle += .5;
	m_model.setidentity();
	m_model.setTranslation(0, 2 + cos(angle / 40), -8 + m_zoom);
	m_model.setRotationY(angle);
	m_mvp = m_projection * m_model;

	if (m_motionBlur)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_texFrameBuffer);
		checkGlError(__FILE__, __LINE__);

		m_blurProgram->enable();
        glUniform1f(m_blurHandle, m_blur);
		checkGlError(__FILE__, __LINE__);
		glActiveTexture(GL_TEXTURE0);
		checkGlError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, m_blurTexture);
		checkGlError(__FILE__, __LINE__);
		glUniform1i(m_shaderTexAttribBlur, 0);
		checkGlError(__FILE__, __LINE__);
		glVertexAttribPointer(m_positionHandleBlur, 2, GL_FLOAT, GL_FALSE, 0, m_screenTarget);
		checkGlError("glVertexAttribPointer");
		glEnableVertexAttribArray(m_positionHandleBlur);
		checkGlError("glEnableVertexAttribArray");
		glVertexAttribPointer(m_texCoordBlur, 2, GL_FLOAT, GL_FALSE, 0, m_texCoord);
		checkGlError("glVertexAttribPointer");
		glEnableVertexAttribArray(m_texCoordBlur);
		checkGlError("glEnableVertexAttribArray");
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		checkGlError("glDrawArrays");

	} else
	{
		glClearColor(0, 0, 0, 1.0f);
		checkGlError("glClearColor");
		glClear(GL_COLOR_BUFFER_BIT);
		checkGlError("glClear");
	}

	// particles
	checkGlError("glViewport");
	m_particlesProgram->enable();
	checkGlError("glUseProgram");

	glUniform1f(m_ptSizeHandle, m_ptSize);
	checkGlError(__FILE__, __LINE__);
	glUniformMatrix4fv(m_mvpHandle, 1, GL_FALSE, m_mvp.ptr());
	checkGlError("glUniformMatrix");
	glVertexAttribPointer(m_positionHandleParticles, 3, GL_FLOAT, GL_FALSE, 0, m_engine->positions());
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(m_positionHandleParticles);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_POINTS, 0, m_engine->particleNumber() -1);

	checkGlError("glDrawArrays");

	if (m_motionBlur)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		checkGlError(__FILE__, __LINE__);

		glClearColor(0, 0, 0, 1.0f);
		checkGlError("glClearColor");
		glClear(GL_COLOR_BUFFER_BIT);
		checkGlError("glClear");

		m_screenProgram->enable();
		checkGlError(__FILE__, __LINE__);
		glActiveTexture(GL_TEXTURE0);
		checkGlError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, m_blurTexture);
		checkGlError(__FILE__, __LINE__);
		glUniform1i(m_shaderTexAttribScreen, 0);
		checkGlError(__FILE__, __LINE__);
		glVertexAttribPointer(m_positionHandleScreen, 2, GL_FLOAT, GL_FALSE, 0, m_screenTarget);
		checkGlError("glVertexAttribPointer");
		glEnableVertexAttribArray(m_positionHandleScreen);
		checkGlError("glEnableVertexAttribArray");
		glVertexAttribPointer(m_texCoordScreen, 2, GL_FLOAT, GL_FALSE, 0, m_texCoord);
		checkGlError("glVertexAttribPointer");
		glEnableVertexAttribArray(m_texCoordScreen);
		checkGlError("glEnableVertexAttribArray");
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		checkGlError("glDrawArrays");
	}
}

void            GlApp::countFPS()
{
	static long gpuSlept = 0;
	clock_t now = TIME_MS();
	clock_t delta = now - m_lastMs;
	if (delta < MAX_FRAME_TIME_MS && delta >= 0)
	{
	    long frame = (MAX_FRAME_TIME_MS - delta);
	    gpuSlept += frame;
	    usleep(1000 * frame);
	}
	m_lastMs = TIME_MS();

	time_t      current = time(0);
	++m_stepCount;
	if (current - m_last > 1)
	{
		time_t elapsed = current - m_last;
		long engineFrameNb = m_engine->getFrameNb() - m_engineLast;
		long gpuFrameNb = m_frameNb - m_gpuLast;
		if (engineFrameNb != 0 && gpuFrameNb != 0)
		{
			float GPUFPS = (float)gpuFrameNb / (float)elapsed;
			float CPUFPS = (float)engineFrameNb / (float)elapsed;
			LOGI("Step : %ld GPU FPS : %f\nCPU FPS : %fCPU sleep (ms) : %ld GPU sleep : %ld\n", m_stepCount, GPUFPS, CPUFPS, m_engine->getTimeSlept(), gpuSlept);
			LOGI("Engine Paused : %s ,  Engine Emitting : %s\n", SHOW_BOOL(m_engine->paused()), SHOW_BOOL(m_engine->emitting()));
#ifdef ANDROID
			Utils::onFPSUpdate(m_jenv, CPUFPS, GPUFPS);
#endif
		}
		/*
		   if (!engine->paused() && stepCount > 2)
		   engine->setParticleNumber(engine->particleNumber() + FPS_to_particles(GPUFPS, CPUFPS));
		 */
		m_engineLast = m_engine->getFrameNb();
		m_gpuLast = m_frameNb;
		m_last = current;
		time_t cminuss = current - m_start;
		/*
		   if (cminuss >= 25 && cminuss <= 28)
		   {
		   m_engine->stop_emit();
		   }
		   if (cminuss >= 30)
		   {
		   m_engine->start_emit();
		   m_start = current;
		   }
		 */
	}
}
