
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "ParticleEngine.h"
#include "GlApp.hpp"


ParticleEngine  *engine = 0;
JNIEnv          *jenv = 0;
GlApp           *app = 0;



extern "C" {
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_init(JNIEnv * env, jobject obj,  jint width, jint height, jint pnumber, jfloat ptSize, jboolean motionBlur);
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_step(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_changeOrientation(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z);
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_die(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_randomize(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_pause(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_unpause(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_beat(JNIEnv * env, jobject obj, jfloat intesity, jfloat speed);
	JNIEXPORT int JNICALL Java_com_valxp_particles_ParticlesCPP_getBuild(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_setZoom(JNIEnv * env, jobject obj, jfloat zoom);
    JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_setBlur(JNIEnv * env, jobject obj, jfloat blur);
    JNIEXPORT float JNICALL Java_com_valxp_particles_ParticlesCPP_getBlur(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_setSize(JNIEnv * env, jobject obj, jfloat size);
    JNIEXPORT float JNICALL Java_com_valxp_particles_ParticlesCPP_getSize(JNIEnv * env, jobject obj);
    JNIEXPORT jstring JNICALL Java_com_valxp_particles_ParticlesCPP_getArch(JNIEnv * env, jobject obj);
};

JNIEXPORT jstring JNICALL Java_com_valxp_particles_ParticlesCPP_getArch(JNIEnv * env, jobject obj)
{
    return env->NewStringUTF(Utils::getArch());
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_setSize(JNIEnv * env, jobject obj, jfloat size)
{
    if (app)
        app->setSize(size);
}

JNIEXPORT float JNICALL Java_com_valxp_particles_ParticlesCPP_getSize(JNIEnv * env, jobject obj)
{
    if (app)
        return app->getSize();
    return 0;
}

JNIEXPORT float JNICALL Java_com_valxp_particles_ParticlesCPP_getBlur(JNIEnv * env, jobject obj)
{
    if (app)
        return app->getBlur();
    return 0;
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_setBlur(JNIEnv * env, jobject obj, jfloat blur)
{
    if (app)
        app->setBlur(blur);
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_setZoom(JNIEnv * env, jobject obj, jfloat zoom)
{
    if (app)
        app->setZoom(zoom);
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_randomize(JNIEnv * env, jobject obj)
{
	if (engine)
	{
        engine->setRandomSpeed();
	}
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_beat(JNIEnv * env, jobject obj, jfloat intensity, jfloat speed)
{
	if (engine)
		engine->startWave(intensity, speed); // 0.05, 0.5 are good values
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_pause(JNIEnv * env, jobject obj)
{
	LOGI("Pausing Particle Engine\n");
	if (engine)
		engine->pause();
}
JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_unpause(JNIEnv * env, jobject obj)
{
	LOGI("Unpausing Particle Engine\n");
	if (engine)
		engine->unpause();
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_die(JNIEnv * env, jobject obj)
{
	LOGI("Activity lost focus\n");
	if (app)
		delete app;
	app = 0;
	if (engine)
		delete engine;
	engine = 0;
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_init(JNIEnv * env, jobject obj,  jint width, jint height, jint pnumber, jfloat ptSize, jboolean motionBlur)
{
	jenv = env;
	if (app)
		delete app;
	app = 0;
	LOGI("Init ParticlesLib Build nb : %d\n", BUILD_NB);
	Utils::printABI();
	if (!engine)
		engine = new ParticleEngine(pnumber);
	if (!engine || engine->hasFailed()) {
		if (engine)
			delete engine;
		engine = 0;
		Utils::engineLoaded(env, 1);
        return ;
    }
	engine->unpause();
	app = new GlApp(static_cast<int>(ptSize), motionBlur != 0 ? true : false, static_cast<int>(width), static_cast<int>(height), jenv);
	app->setEngine(engine);
	//Utils::printMessage(env, "Starting engine !", SHORT_DURATION);
	Utils::engineLoaded(env, 0);
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_step(JNIEnv * env, jobject obj)
{
	if (!engine)
	{
		LOGE("Engine is dead !");
		return;
	}
	if (!app)
	{
		LOGE("GlApp is dead !");
		return;
	}
	app->draw();
	app->countFPS();
}

JNIEXPORT void JNICALL Java_com_valxp_particles_ParticlesCPP_changeOrientation(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z)
{
	if (engine)
		engine->changeSpeed(x, y, z);
}


JNIEXPORT int JNICALL Java_com_valxp_particles_ParticlesCPP_getBuild(JNIEnv * env, jobject obj)
{
	return BUILD_NB;
}

