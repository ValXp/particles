#include <stdlib.h>
#include "Bitmap.h"
#include "utils.h"

Bitmap::Bitmap(JNIEnv *env, jobject bitmap) : m_env(env), m_bitmap(bitmap)
{
   m_class = env->FindClass("android/graphics/Bitmap");
   if (m_class == 0)
   {
        LOGE("Cannot locate class Bitmap\n");
        exit(EXIT_FAILURE);
   }
}


void    Bitmap::getSize(int &_width, int &_height)
{
       jint width, height;
       jmethodID gWidth = getMethodId("getWidth", "()I");
       jmethodID gHeight = getMethodId("getHeight", "()I");
       width = m_env->CallIntMethod(m_bitmap, gWidth);
       height = m_env->CallIntMethod(m_bitmap, gHeight);
       _width = width;
       _height = height;
}

Bitmap::Config  Bitmap::getConfig()
{
    jfieldID configs[_size];

    jmethodID gConfig = getMethodId("getConfig", "()");
    jclass enumType = m_env->FindClass("android/graphics/Bitmap$Config");
    configs[ALPHA_8] = m_env->GetStaticFieldID(enumType, "ALPHA_8", "Landroid/graphics/Bitmap$Config;");
    configs[ARGB_4444] = m_env->GetStaticFieldID(enumType, "ARGB_4444", "Landroid/graphics/Bitmap$Config;");
    configs[ARGB_8888] = m_env->GetStaticFieldID(enumType, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    configs[RGB_565] = m_env->GetStaticFieldID(enumType, "RGB_565", "Landroid/graphics/Bitmap$Config;");

    // TODO : get config, convert enum and return
    return _size;
}

void    Bitmap::pixelAt(int x, int y, Color &_color)
{
    jint color;
    jmethodID gPixel = getMethodId("getPixel", "(II)I");
    color = m_env->CallIntMethod(m_bitmap, gPixel, x, y);
    _color.b = color & 0xFF;
    color = color >> 4;
    _color.g = color & 0xFF;
    color = color >> 4;
    _color.r = color & 0xFF;
    color = color >> 4;
    _color.a = color & 0xFF;
}

jmethodID   Bitmap::getMethodId(const char *methodName, const char *signature)
{
   jmethodID method = m_env->GetMethodID(m_class, methodName, signature);
   if (method == 0)
   {
        LOGE("Cannot find method %s in class Bitmap\n", methodName);
        exit(EXIT_FAILURE);
   }
   return method;
}

