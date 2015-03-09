#ifndef BITMAP_H_
# define BITMAP_H_

#include <jni.h>

class Bitmap
{
    public:
    struct Color
    {
        int r, g, b, a;
    };
    enum Config
    {
        ALPHA_8,
        ARGB_4444,
        ARGB_8888,
        RGB_565,
        _size
    };
    Bitmap(JNIEnv *env, jobject bitmap);
    void    getSize(int &width, int &height);
    void    pixelAt(int x, int y, Color &color);
    Config  getConfig();

    private:
    jmethodID   getMethodId(const char *method, const char *signature);
    jclass  m_class;
    JNIEnv    *m_env;
    jobject m_bitmap;
};


#endif

