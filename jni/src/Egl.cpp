#include "Egl.h"
#include <math.h>
#include <fcntl.h>
#ifndef M_PI
# define M_PI 3.14159
#endif

#ifndef EGL_USE_X11
# include <gc_vdk.h>
#endif

GLfloat		m_xmin, m_xmax, m_ymin, m_ymax;
int			m_width, m_height;
EGLDisplay	m_egldisplay;
EGLConfig	m_eglconfig;
EGLSurface	m_eglsurface;
EGLContext	m_eglcontext;
#ifdef EGL_USE_X11
Display 	*m_display;
#endif

float		eglWidth()
{
	return m_width;
}

float		eglHeight()
{
	return m_height;
}

void	eglInit(int width, int height)
{
	m_width = width;
	m_height = height;
	#ifdef EGL_USE_X11
	initX11();
	#else
	initEglOnly();
	#endif
	EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	
	m_eglcontext = eglCreateContext( m_egldisplay, m_eglconfig, EGL_NO_CONTEXT, ContextAttribList);
	eglMakeCurrent(m_egldisplay, m_eglsurface, m_eglsurface, m_eglcontext);
	
	EGLint h, w;	
	eglQuerySurface(m_egldisplay, m_eglsurface, EGL_WIDTH, &w);
	eglQuerySurface(m_egldisplay, m_eglsurface, EGL_HEIGHT, &h);
	m_ymax = 0.1f * tan(45.0f * M_PI/360.0f);
	m_ymin = -m_ymax;
	m_xmin = m_ymin * ((GLfloat)w/(GLfloat)h);
	m_xmax = m_ymax * ((GLfloat)w/(GLfloat)h);
}


EGLDisplay	getDisplay()
{
	return m_egldisplay;
}
EGLConfig	getConfig()
{
	return m_eglconfig;
}
EGLSurface	getSurface()
{
	return m_eglsurface;
}
EGLContext	getContext()
{
	return m_eglcontext;
}
#ifdef EGL_USE_X11
void	initX11()
{
	const EGLint	m_configAttribs[] = {
					EGL_RED_SIZE,		5,
					EGL_GREEN_SIZE,		6,
					EGL_BLUE_SIZE,		5,
					EGL_ALPHA_SIZE,		0,
					EGL_SAMPLES,		0,
					EGL_NONE
					};
	EGLint	numconfigs;
	m_display = XOpenDisplay(NULL);
	int screen = DefaultScreen(m_display);

	Window window, rootwindow;
	rootwindow = RootWindow(m_display, screen);
	
	m_egldisplay = eglGetDisplay(m_display);
	
	eglInitialize(m_egldisplay, NULL, NULL);

	eglBindAPI(EGL_OPENGL_ES_API);

	eglChooseConfig(m_egldisplay, m_configAttribs, &m_eglconfig, 1, &numconfigs);
	

	window = XCreateSimpleWindow(m_display, rootwindow, 0, 0, m_width, m_height, 0, 0, WhitePixel(m_display, screen));

	XMapWindow(m_display, window);

	m_eglsurface = eglCreateWindowSurface(m_egldisplay, m_eglconfig, window, NULL);

}
#else
void	initEglOnly()
{
  const EGLint config_attr[] =
    {
      EGL_RED_SIZE,	5,
      EGL_GREEN_SIZE, 	6,
      EGL_BLUE_SIZE,	5,
      EGL_ALPHA_SIZE, 	EGL_DONT_CARE,
      EGL_NONE
    };
 
  EGLint configs, major, minor;

  EGLNativeDisplayType native_disp = fbGetDisplayByIndex(0);                              /* i.MX6 specific */
  fbGetDisplayGeometry(native_disp, &m_width, &m_height);                                 /* i.MX6 specific */
  EGLNativeWindowType native_win  = fbCreateWindow(native_disp, 0, 0, m_width, m_height); /* i.MX6 specific */
  
  eglBindAPI(EGL_OPENGL_ES_API);

  m_egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(m_egldisplay, &major, &minor);
  
  eglGetConfigs(m_egldisplay, NULL, 0, &configs);
  
  eglChooseConfig(m_egldisplay, config_attr, &m_eglconfig, 1, &configs);
  m_eglsurface = eglCreateWindowSurface(m_egldisplay, m_eglconfig, native_win, NULL);
}
#endif

void eglSwap()
{
	eglSwapBuffers(m_egldisplay, m_eglsurface);
}


