/*------------------------------------------------------------------------
 * A demonstration of OpenGL in a  ARGB window
 *    => support for composited window transparency
 *
 * (c) 2011 by Wolfgang 'datenwolf' Draxinger
 *     See me at comp.graphics.api.opengl and StackOverflow.com

 * License agreement: This source code is provided "as is". You
 * can use this source code however you want for your own personal
 * use. If you give this source code to anybody else then you must
 * leave this message in it.
 *
 * This program is based on the simplest possible
 * Linux OpenGL program by FTB (see info below)

  The simplest possible Linux OpenGL program? Maybe...
  (c) 2002 by FTB. See me in comp.graphics.api.opengl
  --
  <\___/>
  / O O \
  \_____/  FTB.
------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


#include <sys/types.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>

#include "Core/prCoreDefs.h"
#include "prSCApp.h"
#include "prSCLineSim.h"
#include "prDataExporter.h"

#define USE_CHOOSE_FBCONFIG

static void fatalError(const char *why) {
  fprintf(stderr, "%s", why);
  exit(0x666);
}

static int Xscreen;
static Atom del_atom;
static Colormap cmap;
//static Display *Xdisplay;
static XVisualInfo *visual;
static XRenderPictFormat *pict_format;
static GLXFBConfig *fbconfigs;//, fbconfig;
static int numfbconfigs;
//static GLXContext render_context;
static Window Xroot, window_handle;
//static GLXWindow glX_window_handle;
static int width, height;

static int VisData[] = {
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_DOUBLEBUFFER, True,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 16,
    None
};

struct prLinuxWindow {
  Display* Xdisplay;
  GLXFBConfig fbconfig;
  GLXWindow glX_window_handle;
};

static prLinuxWindow window;

static Bool WaitForMapNotify(Display *d, XEvent *e, char *arg)
{
	return d && e && arg && (e->type == MapNotify) && (e->xmap.window == *(Window*)arg);
}

static void describe_fbconfig(GLXFBConfig fbconfig) {
  int doublebuffer;
  int red_bits, green_bits, blue_bits, alpha_bits, depth_bits;

  glXGetFBConfigAttrib(window.Xdisplay, fbconfig, GLX_DOUBLEBUFFER, &doublebuffer);
  glXGetFBConfigAttrib(window.Xdisplay, fbconfig, GLX_RED_SIZE, &red_bits);
  glXGetFBConfigAttrib(window.Xdisplay, fbconfig, GLX_GREEN_SIZE, &green_bits);
  glXGetFBConfigAttrib(window.Xdisplay, fbconfig, GLX_BLUE_SIZE, &blue_bits);
  glXGetFBConfigAttrib(window.Xdisplay, fbconfig, GLX_ALPHA_SIZE, &alpha_bits);
  glXGetFBConfigAttrib(window.Xdisplay, fbconfig, GLX_DEPTH_SIZE, &depth_bits);

  fprintf(stderr, "FBConfig selected:\n"
      "Doublebuffer: %s\n"
      "Red Bits: %d, Green Bits: %d, Blue Bits: %d, Alpha Bits: %d, Depth Bits: %d\n",
      doublebuffer == True ? "Yes" : "No",
          red_bits, green_bits, blue_bits, alpha_bits, depth_bits);
}

static void createTheWindow()
{
   XInitThreads();

	XEvent event;
	int x,y, attr_mask;
	XSizeHints hints;
	XWMHints *startup_state;
	XTextProperty textprop;
  XSetWindowAttributes attr = {0};
	static const char *title = "FTB's little OpenGL example - ARGB extension by WXD";

	window.Xdisplay = XOpenDisplay(NULL);
	if (!window.Xdisplay) {
		fatalError("Couldn't connect to X server\n");
	}
	Xscreen = DefaultScreen(window.Xdisplay);
	Xroot = RootWindow(window.Xdisplay, Xscreen);

	fbconfigs = glXChooseFBConfig(window.Xdisplay, Xscreen, VisData, &numfbconfigs);
	window.fbconfig = 0;
	for(int i = 0; i<numfbconfigs; i++) {
		visual = (XVisualInfo*) glXGetVisualFromFBConfig(window.Xdisplay, fbconfigs[i]);
		if(!visual)
			continue;

		pict_format = XRenderFindVisualFormat(window.Xdisplay, visual->visual);
		if(!pict_format)
			continue;

		window.fbconfig = fbconfigs[i];
		break;
		/*printf("Config: %d\n", pict_format->direct.alphaMask);
		if(pict_format->direct.alphaMask > 0) {
			//break;
		}*/
	}

	if(!window.fbconfig) {
		fatalError("No matching FB config found");
	}

	describe_fbconfig(window.fbconfig);

	/* Create a colormap - only needed on some X clients, eg. IRIX */
	cmap = XCreateColormap(window.Xdisplay, Xroot, visual->visual, AllocNone);

	attr.colormap = cmap;
	attr.background_pixmap = None;
	attr.border_pixmap = None;
	attr.border_pixel = 0;
	attr.event_mask =
		StructureNotifyMask |
		EnterWindowMask |
		LeaveWindowMask |
		ExposureMask |
		ButtonPressMask |
		ButtonReleaseMask |
		OwnerGrabButtonMask |
		KeyPressMask |
		KeyReleaseMask;

  attr_mask = CWColormap|CWBorderPixel|CWEventMask;

  width = 3*800;
  height = 3*600;
  x=0, y=0;

  window_handle = XCreateWindow(window.Xdisplay, Xroot, x, y, width, height,
                                0, visual->depth, InputOutput, visual->visual,
                                attr_mask, &attr);

  if( !window_handle ) {
    fatalError("Couldn't create the window\n");
  }

  window.glX_window_handle = window_handle;

  textprop.value = (unsigned char*)title;
  textprop.encoding = XA_STRING;
  textprop.format = 8;
  textprop.nitems = strlen(title);

  hints.x = x;
  hints.y = y;
  hints.width = width;
  hints.height = height;
  hints.flags = USPosition|USSize;

  startup_state = XAllocWMHints();
  startup_state->initial_state = NormalState;
  startup_state->flags = StateHint;

  XSetWMProperties(window.Xdisplay, window_handle, &textprop, &textprop,
                   NULL, 0, &hints, startup_state, NULL);
  XFree(startup_state);

  XMapWindow(window.Xdisplay, window_handle);
  XIfEvent(window.Xdisplay, &event, WaitForMapNotify, (char*)&window_handle);

  if ((del_atom = XInternAtom(window.Xdisplay, "WM_DELETE_WINDOW", 0)) != None) {
    XSetWMProtocols(window.Xdisplay, window_handle, &del_atom, 1);
  }
}

prSCApp* app = NULL;
prSCLineSim* sim = NULL;

static int updateTheMessageQueue() {
  XEvent event;
  XConfigureEvent *xc;

  while (true) {
    while (!XPending(window.Xdisplay))
      usleep(1000);
    XNextEvent(window.Xdisplay, &event);
    switch (event.type) {
      case ClientMessage:
        if ((unsigned long)event.xclient.data.l[0] == del_atom) {
          return 0;
        }
        break;
      case ConfigureNotify:
        xc = &(event.xconfigure);
        width = xc->width;
        height = xc->height;
        break;

        /* keyboard events */
      case KeyPress:
        if ( event.xkey.keycode == 0x09 )
          return 0;
        switch (event.xkey.keycode) {
        case 0x14: if (app) app->SetDepthScale(-1.0f); break;
        case 0x15: if (app) app->SetDepthScale(1.0f); break;
        case 0x19: if (app) app->SetDolly(1.0f); break;
        case 0x27: if (app) app->SetDolly(-1.0f); break;
        case 0x26: if (app) app->SetOffset(-1.0f); break;
        case 0x28: if (app) app->SetOffset(1.0f); break;
        case 0x71: if (app) app->RotateLeftRight(-1.0f); break;
        case 0x72: if (app) app->RotateLeftRight(1.0f); break;
        case 0x6f: if (app) app->RotateUpDown(1.0f); break;
        case 0x74: if (app) app->RotateUpDown(-1.0f); break;
        }
        break;
      case KeyRelease:
        switch (event.xkey.keycode) {
        case 0x14: if (app) app->SetDepthScale(0.0f); break;
        case 0x15: if (app) app->SetDepthScale(0.0f); break;
        case 0x19: if (app) app->SetDolly(0.0f); break;
        case 0x27: if (app) app->SetDolly(0.0f); break;
        case 0x26: if (app) app->SetOffset(0.0f); break;
        case 0x28: if (app) app->SetOffset(0.0f); break;
        case 0x71: if (app) app->RotateLeftRight(0.0f); break;
        case 0x72: if (app) app->RotateLeftRight(0.0f); break;
        case 0x6f: if (app) app->RotateUpDown(0.0f); break;
        case 0x74: if (app) app->RotateUpDown(0.0f); break;
        }
        break;
      case 4:
    	if (app) app->Switch();
        break;
    }
  }
  return 1;
}

int main(int argc, char *argv[]) {
	if (true) {
		//ExportGeoData(_T("/home/khmel/Downloads/ac/sc/test1"), 100, 2);
		//return -1;
	}
	createTheWindow();
	//createTheRenderContext();

	//exit(1);

  app = new prSCApp();
  app->Init(&window);
  //sim = new prSCLineSim();
  //sim->Init(&window);

  updateTheMessageQueue();		//redrawTheWindow();

  delete app;
  delete sim;

  return 0;
}
