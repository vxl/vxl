// This is oxl/vgui/impl/glX/vgui_glX_window.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 05 Oct 99
//
//-----------------------------------------------------------------------------

#include "vgui_glX_window.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <vgui/vgui_glx.h>
#include <vgui/impl/X/vgui_X_utils.h>
#include "vgui_glX.h"
#include "vgui_glX_adaptor.h"

static bool debug = false;


#ifdef __SUNPRO_CC
namespace { extern "C"
#else
static
#endif
Bool WaitForNotify(Display *, XEvent *e, char *arg) {
  return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}
#ifdef __SUNPRO_CC
}
#endif


static int attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 1, GLX_RED_SIZE, 1, GLX_DOUBLEBUFFER, None};


vgui_glX_window::vgui_glX_window(int width, int height, const char* /*title*/)
{
  Display *display = vgui_glX::instance()->display;
  vgui_glX::instance()->windows.push_back(this);

  adaptor = new vgui_glX_adaptor;
  adaptor->window = this;

  XVisualInfo *visual = glXChooseVisual(display,DefaultScreen(display),attribs);
  vcl_cerr << "visual =" << static_cast<void*>(visual) << vcl_endl;
  assert(visual);

  // create context
  context = glXCreateContext(display, visual, 0, GL_FALSE);
  vcl_cerr << "context=" << static_cast<void*>(context) << vcl_endl;
  assert(context != NULL);

  // create a color map
  Colormap cmap = XCreateColormap(display,
                                  RootWindow(display, visual->screen),
                                  visual->visual,
                                  AllocNone);

  // create a window
  XSetWindowAttributes swa;
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;
  window = XCreateWindow(display,
                         RootWindow(display, visual->screen),
                         0, 0,
                         width, height,
                         0, visual->depth,
                         InputOutput, visual->visual,
                         CWBorderPixel|CWColormap|CWEventMask,
                         &swa);
};

void vgui_glX_window::show() {

  Display *display = vgui_glX::instance()->display;

  // map window to display
  XMapWindow(display, window);
  XEvent event;
  // wait for acknowledgement of map
  XIfEvent(display, &event, WaitForNotify, (char*)window);

  // connect the context to the window
  glXMakeCurrent(display, window, context);

  XSelectInput(display,window,
               KeyPressMask |
               KeyReleaseMask |
               ButtonPressMask |
               ButtonReleaseMask |
               EnterWindowMask |
               LeaveWindowMask |
               PointerMotionMask |
               //PointerMotionHintMask |
               //Button1MotionMask |
               //Button2MotionMask |
               //Button3MotionMask       |
               //Button4MotionMask       |
               //Button5MotionMask |
               //ButtonMotionMask |
               //KeymapStateMask  |
               ExposureMask  |
               //VisibilityChangeMask |
               //ResizeRedirectMask |
               //SubstructureNotifyMask |
               //SubstructureRedirectMask |
               //FocusChangeMask  |
               //PropertyChangeMask |
               //ColormapChangeMask |
               //OwnerGrabButtonMask |
               StructureNotifyMask
               );


  // flush the cache
  adaptor->draw();
  XFlush(display);
}


vgui_glX_window::~vgui_glX_window() {
}


void vgui_glX_window::set_adaptor(vgui_adaptor* a) {
  adaptor = static_cast<vgui_glX_adaptor*>(a);
}

vgui_adaptor* vgui_glX_window::get_adaptor() {
  return adaptor;
}


void vgui_glX_window::handle(XEvent* xev)
{
  if (vgui_X_utils::is_modifier(xev))
    return;

  vgui_event e;

  switch (xev->type) {
  case Expose:
    if (debug) vcl_cerr << "Expose\n";
    adaptor->draw();
    return;

  case KeyPress:
    if (debug) vcl_cerr << "KeyPress\n";
    e.type = vgui_KEY_PRESS;
    e.key = vgui_X_utils::translate_key(&xev->xkey);
    break;

  case KeyRelease:
    if (debug) vcl_cerr << "KeyRelease\n";
    e.type = vgui_KEY_RELEASE;
    e.key = vgui_X_utils::translate_key(&xev->xkey);
    break;

  case ButtonPress:
    if (debug) vcl_cerr << "ButtonPress\n";
    e.type = vgui_BUTTON_DOWN;
    e.button = vgui_X_utils::translate_button(xev->xbutton.button);
    break;

  case ButtonRelease:
    if (debug) vcl_cerr << "ButtonRelease\n";
    e.type = vgui_BUTTON_UP;
    e.button = vgui_X_utils::translate_button(xev->xbutton.button);
    break;

  case MotionNotify:
    if (debug) vcl_cerr << "MotionNotify\n";
    e.type = vgui_MOTION;
    break;

  case EnterNotify:
    if (debug) vcl_cerr << "EnterNotify\n";
    e.type = vgui_ENTER;
    break;

  case LeaveNotify:
    if (debug) vcl_cerr << "LeaveNotify\n";
    e.type = vgui_LEAVE;
    break;

  case ConfigureNotify: {
    if (debug) vcl_cerr << "ConfigureNotify\n";
    int width  = xev->xconfigure.width;
    int height = xev->xconfigure.height;
    if (debug) vcl_cerr << "width : " << width << vcl_endl;
    if (debug) vcl_cerr << "height : " << height << vcl_endl;
    glXWaitX();
    glViewport(0,0,width,height);
    glFlush();
    glXWaitGL();
    adaptor->draw();
    e.type = vgui_RESHAPE;
    break;
                        }

  default:
    if (debug) vcl_cerr << "Event : " << xev->type << vcl_endl;
    e.type = vgui_OTHER;
    break;
  }

  vgui_X_utils::set_modifiers(e, xev);
  vgui_X_utils::set_coordinates(e,xev);

  adaptor->dispatch_to_tableau(e);
}
