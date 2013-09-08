#ifndef UTILS_H_
# define UTILS_H_

#define SHOW_BOOL(b) ((b) ? "true" : "false")
//#define CHECK_GL_ERRORS

#define MIN(x, y) ((x) < (y) ? (x) : (y))

# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#ifdef ANDROID
# include <android/log.h>
# include <jni.h>
# include "Bitmap.h"
# define  LOG_TAG    "libParticles"
# define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
# define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#include <stdio.h>
#include <stdarg.h>

void LOGI(const char *fmt, ...);

void LOGE(const char *fmt, ...);
#endif
#define  TIME_MS() (clock() / (CLOCKS_PER_SEC / 1000))
# define LONG_DURATION  0x1
# define SHORT_DURATION 0x0

namespace Utils
{


	void printGLString(const char *name, GLenum s);
	inline void checkGlError(const char* op, int line = -1)
	{
		// increase speed by removing error checking, -O3 will remove call
#ifdef CHECK_GL_ERRORS
		for (GLint error = glGetError(); error; error
				= glGetError()) {
			if (line != -1)
				LOGI("after file %s at line %d glError (0x%x)\n", op, line, error);
			else
				LOGI("after %s glError (0x%x)\n", op, error);
		}
#endif
	}
#ifdef ANDROID
	char    *loadRessource(JNIEnv *env, const char *ressource);
	void    printMessage(JNIEnv *env, const char *message, int duration);
	void    engineLoaded(JNIEnv *env, int status);
	Bitmap  *loadImage(JNIEnv *env);
	void    printABI();
	void 	onFPSUpdate(JNIEnv *env, float cpu, float gpu);
#endif
	void    genRandomDiskPoint(float disk_radius, float &x, float &y); // Z will be zero
	float   myRand(float min, float max);
	// add "addVal" to "*toSum" atomically
#define ATOMIC_ADD(toSum, addVal) { __sync_fetch_and_add(toSum, addVal); }
#define ATOMIC_INC(toSum) ATOMIC_ADD(toSum, 1)
	/*
	   inline void     atomicAdd(unsigned long *toSum, unsigned long addVal)
	   {
	   __sync_fetch_and_add(toSum, addVal);
	   }

	   inline void     atomicInc(unsigned long *toSum)
	   {
	   atomicAdd(toSum, 1);
	   }
	 */
};

#define  OFFSETOF(elem, st) ((size_t)&(((st*)0)->elem))

#ifndef M_PI
# define M_PI 3.14159
#endif

#endif // UTILS_H_

