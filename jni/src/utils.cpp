#include <math.h>
#include <stdlib.h>
#ifdef ANDROID
# include <jni.h>
# include <cpu-features.h>
#endif
#include "utils.h"


void Utils::printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}


#ifdef ANDROID

void Utils::printABI()
{
	uint64_t cpu = android_getCpuFeatures();

	LOGI("ARMv7 ? %s\n", (cpu & ANDROID_CPU_ARM_FEATURE_ARMv7 ? "YES" : "NO"));
	LOGI("Hardware FP ? %s\n", (cpu & ANDROID_CPU_ARM_FEATURE_VFPv3 ? "YES" : "NO"));
	LOGI("NEON ? %s\n", (cpu & ANDROID_CPU_ARM_FEATURE_NEON ? "YES" : "NO"));
	LOGI("SSE3 ? %s\n", (cpu & ANDROID_CPU_X86_FEATURE_SSSE3 ? "YES" : "NO"));
	LOGI("POPCNT ? %s\n", ((cpu & ANDROID_CPU_X86_FEATURE_POPCNT) ? "YES" : "NO"));
	LOGI("MOVBE ? %s\n", ((cpu & ANDROID_CPU_X86_FEATURE_MOVBE) ? "YES" : "NO"));
}

char *Utils::loadRessource(JNIEnv *env, const char *ressource)
{
	jstring jres = env->NewStringUTF(ressource);
	if (jres == 0)
	{
		LOGE("Cannot create Java String\n");
		exit(EXIT_FAILURE);
	}
	jclass jcaller = env->FindClass("com/valxp/particles/JavaCaller");
	if (jcaller == 0)
	{
		LOGE("Cannot locate class JavaCaller\n");
		exit(EXIT_FAILURE);
	}
	jmethodID loader = env->GetStaticMethodID(jcaller, "readAsset", "(Ljava/lang/String;)Ljava/lang/String;");
	if (loader == 0)
	{
		LOGE("Cannot locate static method readAsset\n");
		exit(EXIT_FAILURE);
	}
	jobject res = env->CallStaticObjectMethod(jcaller, loader, jres);
	if (res == 0)
	{
		LOGE("Cannot call static metho readAssert\n");
		exit(EXIT_FAILURE);
	}
	const char *ret = env->GetStringUTFChars((jstring) res, NULL);
	if (ret == 0)
	{
		LOGE("Cannot convert UTF string\n");
		exit(EXIT_FAILURE);
	}
	char *cp = strdup(ret);
	env->ReleaseStringUTFChars((jstring)res, ret);
	return cp;
}

Bitmap *Utils::loadImage(JNIEnv *env)
{
	jclass jcaller = env->FindClass("com/valxp/particles/JavaCaller");
	if (jcaller == 0)
	{
		LOGE("Cannot locate class JavaCaller\n");
		exit(EXIT_FAILURE);
	}
	jmethodID loader = env->GetStaticMethodID(jcaller, "getImage", "()Landroid/graphics/Bitmap;");
	if (loader == 0)
	{
		LOGE("Cannot locate static method getImage\n");
		exit(EXIT_FAILURE);
	}
	jobject res = env->CallStaticObjectMethod(jcaller, loader);
	if (res == 0)
	{
		LOGE("Cannot call static methoid getImage\n");
		exit(EXIT_FAILURE);
	}

	return new Bitmap(env, res);
}


void    Utils::printMessage(JNIEnv *env, const char *message, int duration)
{
	jstring jmsg = env->NewStringUTF(message);
	if (jmsg == 0)
	{
		LOGE("Cannot create Java String\n");
		exit(EXIT_FAILURE);
	}
	jclass jcaller = env->FindClass("com/valxp/particles/JavaCaller");
	if (jcaller == 0)
	{
		LOGE("Cannot locate class JavaCaller\n");
		exit(EXIT_FAILURE);
	}
	jmethodID loader = env->GetStaticMethodID(jcaller, "printMessage", "(Ljava/lang/String;I)V");
	if (loader == 0)
	{
		LOGE("Cannot locate static method printMessage\n");
		exit(EXIT_FAILURE);
	}
	env->CallStaticVoidMethod(jcaller, loader, jmsg, duration);
}


void    Utils::engineLoaded(JNIEnv *env, int status)
{
	jclass jcaller = env->FindClass("com/valxp/particles/JavaCaller");
	if (jcaller == 0)
	{
		LOGE("Cannot locate class JavaCaller\n");
		exit(EXIT_FAILURE);
	}
	jmethodID loader = env->GetStaticMethodID(jcaller, "onEngineLoaded", "(I)V");
	if (loader == 0)
	{
		LOGE("Cannot locate static method printMessage\n");
		exit(EXIT_FAILURE);
	}
	env->CallStaticVoidMethod(jcaller, loader, status);
}

void Utils::onFPSUpdate(JNIEnv *env, float cpu, float gpu) 
{
	jclass jcaller = env->FindClass("com/valxp/particles/JavaCaller");
	if (jcaller == 0)
	{
		LOGE("Cannot locate class JavaCaller\n");
		exit(EXIT_FAILURE);
	}
	jmethodID loader = env->GetStaticMethodID(jcaller, "onFPSUpdate", "(FF)V");
	if (loader == 0)
	{
		LOGE("Cannot locate static method printMessage\n");
		exit(EXIT_FAILURE);
	}
	env->CallStaticVoidMethod(jcaller, loader, cpu, gpu);
}


#else // ANDROID

void LOGI(const char *fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
}

void LOGE(const char *fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
}


#endif // ! ANDROID

void Utils::genRandomDiskPoint(float disk_radius, float &x, float &y) // Z will be zero
{
	float mrand = random() / (float)RAND_MAX;

	float phi = 2 * M_PI * rand();
	float rad = sqrt(mrand) * disk_radius;

	x = sin(phi) * rad;
	y = cos(phi) * rad;
}

float Utils::myRand(float min, float max)
{
	float mrand = random() / (float)RAND_MAX;
	return ((max - min) * mrand) + min;
}



