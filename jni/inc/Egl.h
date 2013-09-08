#ifndef EGL_H_
#define EGL_H_

#define EGL_USE_GLES2

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#ifdef EGL_USE_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#endif 

#include <stdlib.h>
#include <string.h>

#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE !TRUE
#endif


void		eglInit(int width, int height);
void		eglSwap();
float		eglWidth();
float		eglHeight();
#ifdef EGL_USE_X11	
void		initX11();
#else
void		initEglOnly();
#endif
/*
void		fatal(const char *msg, EGLint result = EGL_SUCCESS);
void		checkEgl(const char *msg);
*/

#endif // EGL_H_


