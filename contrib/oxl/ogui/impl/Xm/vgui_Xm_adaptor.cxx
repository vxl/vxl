// This is oxl/vgui/impl/Xm/vgui_Xm_adaptor.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   11 Sep 99
//-----------------------------------------------------------------------------

#include "vgui_Xm_adaptor.h"

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glx.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>

#include <Xm/Frame.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/MenuShell.h>
#include <Xm/RowColumnP.h>
#include <Xm/CascadeB.h>

#include <X11/IntrinsicP.h>

#include <vgui/vgui_popup_params.h>
#include "vgui_Xm.h"
#include "vgui_Xm_window.h"
#include "vgui_Xm_utils.h"

#include <vgui/impl/X/vgui_X_utils.h>


static bool debug = false;

vgui_menu vgui_Xm_adaptor::last_popup;

vgui_Xm_adaptor::vgui_Xm_adaptor(int X, int Y, int W, int H, const char *L)
{
  init();
}

vgui_Xm_adaptor::vgui_Xm_adaptor()
{
  init();
}


vgui_Xm_adaptor::~vgui_Xm_adaptor()
{
}


void vgui_Xm_adaptor::make_current()
{
  GLwDrawingAreaMakeCurrent(widget, context);
}

void vgui_Xm_adaptor::swap_buffers()
{
  GLwDrawingAreaSwapBuffers(widget);
}

void vgui_Xm_adaptor::post_redraw()
{
  draw();
}

void vgui_Xm_adaptor::post_overlay_redraw()
{
  draw();
}


void vgui_Xm_adaptor::init()
{
}


void vgui_Xm_adaptor::draw()
{
  make_current();
  glDrawBuffer(GL_BACK);
  dispatch_to_tableau(vgui_DRAW);
  swap_buffers();
}

void vgui_Xm_adaptor::input_cb(Widget w, XtPointer client_data, XtPointer call)
{
  vgui_Xm_adaptor *adaptor = (vgui_Xm_adaptor*)client_data;
  XEvent *xev = ((GLwDrawingAreaCallbackStruct *) call)->event;

  adaptor->make_current();

  if (vgui_X_utils::is_modifier(xev))
    return;

  vgui_event e;

  switch (xev->type) {
  case ButtonPress:
  {
    if (debug) vcl_cerr << "ButtonPress\n";
    e.type = vgui_BUTTON_DOWN;
    e.button = vgui_X_utils::translate_button(xev->xbutton.button);
  } break;
  case ButtonRelease:
  {
    if (debug) vcl_cerr << "ButtonRelease\n";
    e.type = vgui_BUTTON_UP;
    e.button = vgui_X_utils::translate_button(xev->xbutton.button);
  } break;
  case KeyPress:
  {
    if (debug) vcl_cerr << "KeyPress\n";
    e.type = vgui_KEY_PRESS;
    e.key = vgui_X_utils::translate_key(&xev->xkey);
  } break;
  case KeyRelease:
  {
    if (debug) vcl_cerr << "KeyRelease\n";
    vgui_event e;
    e.type = vgui_KEY_RELEASE;
    e.key = vgui_X_utils::translate_key(&xev->xkey);
  } break;
  case MotionNotify:
  {
    if (debug) vcl_cerr << "MotionNotify\n";
    e.type = vgui_MOTION;
  } break;
  default:
    vcl_cerr << "vgui_Xm_adaptor::input()  Error " << xev->type << vcl_endl;
    break;
  }

  vgui_X_utils::set_modifiers(e,xev);
  vgui_X_utils::set_coordinates(e,xev);

  if (e.type == vgui_BUTTON_DOWN &&
      e.button == adaptor->popup_button &&
      e.modifier == adaptor->popup_modifier)
  {
    vgui_popup_params params;
    params.x = e.wx;
    params.y = e.wy;

    adaptor->xpopup = XmCreatePopupMenu(adaptor->widget,"menubar",NULL, 0);
    adaptor->last_popup = adaptor->get_total_popup(params); // fsm@robots
    vgui_Xm_utils::set_menu(adaptor->xpopup, adaptor->last_popup);

    XButtonEvent *be = (XButtonEvent*)xev;
    XmMenuPosition(adaptor->xpopup, be);
    XtManageChild(adaptor->xpopup);

    return;
  }

  adaptor->dispatch_to_tableau(e);
}


void vgui_Xm_adaptor::resize_cb(Widget w, XtPointer client_data, XtPointer call)
{
  vgui_Xm_adaptor *adaptor = (vgui_Xm_adaptor*)client_data;

  GLwDrawingAreaCallbackStruct *call_data;
  call_data = (GLwDrawingAreaCallbackStruct *) call;

  adaptor->make_current();

  adaptor->width = call_data->width;
  adaptor->height = call_data->height;

  adaptor->dispatch_to_tableau(vgui_RESHAPE);
}

void vgui_Xm_adaptor::expose_cb(Widget w, XtPointer client_data, XtPointer call)
{
  vgui_Xm_adaptor *adaptor = (vgui_Xm_adaptor*)client_data;

  GLwDrawingAreaCallbackStruct *call_data;
  call_data = (GLwDrawingAreaCallbackStruct *) call;

  adaptor->make_current();
  adaptor->width = call_data->width;
  adaptor->height = call_data->height;
  adaptor->draw();
}


void vgui_Xm_adaptor::setup_widget(Widget *frame, XVisualInfo *visinfo)
{
  widget = XtVaCreateManagedWidget("glxwidget", glwMDrawingAreaWidgetClass,
                                   *frame, GLwNvisualInfo, visinfo, NULL);

  XtAddCallback(widget, GLwNexposeCallback, expose_cb, this);
  XtAddCallback(widget, GLwNresizeCallback, resize_cb, this);
  XtAddCallback(widget, GLwNinputCallback, input_cb, this);
}

void vgui_Xm_adaptor::setup_context(Display *dpy, XVisualInfo *visinfo)
{
  context = glXCreateContext(dpy, visinfo, 0, GL_TRUE);
  GLwDrawingAreaMakeCurrent(widget, context);
}
