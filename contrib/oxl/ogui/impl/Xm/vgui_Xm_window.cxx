// This is oxl/ogui/impl/Xm/vgui_Xm_window.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   05 Oct 99
//-----------------------------------------------------------------------------

#include "vgui_Xm_window.h"

#include <Xm/Frame.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/MenuShell.h>
#include <Xm/RowColumnP.h>
#include <Xm/CascadeB.h>
#include <Xm/Form.h>
#include <Xm/Text.h>

#include <X11/IntrinsicP.h>
#include <GL/glx.h>

#include <vgui/vgui.h>

#include "vgui_Xm.h"
#include "vgui_Xm_adaptor.h"
#include "vgui_Xm_utils.h"


static bool debug = false;
static int attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 1, GLX_RED_SIZE, 1, GLX_DOUBLEBUFFER, None};

int vgui_Xm_window::num_win = 0;

void vgui_Xm_window::destroy(Widget /*w*/, XtPointer /*client_data*/, XtPointer /*call*/)
{
  //vgui_Xm_window *window = (vgui_Xm_window*)client_data;
  num_win--;

  if (num_win <= 0)
    vgui::quit();
}


static void enter(Widget w, XtPointer client_data, XEvent *, Boolean *)
{
  vgui_Xm_window *window = (vgui_Xm_window*)client_data;

#ifdef __GNUC__ // ios::rdbuf(streambuf*) is nonstandard
  if (window->use_statusbar)
    vgui::out.rdbuf(window->statusbar.statusbuf);
  else
    vgui::out.rdbuf(vcl_cout.rdbuf());
#endif
}

#ifdef __GNUC__
static void leave(Widget w, XtPointer client_data, XEvent *, Boolean *)
{
  vgui::out.rdbuf(vcl_cout.rdbuf());
}
#endif


vgui_Xm_window::vgui_Xm_window(int w, int h, const char* title)
  : use_menubar(false)
  , use_statusbar(true)
{
  last_menubar = new vgui_menu;

  if (debug)
    vcl_cerr << "vgui_Xm_window::vgui_Xm_window\n";

  adaptor = new vgui_Xm_adaptor;

  char *shell_name = "popup";
  Arg shell_args[3]; int n=0;
  XtSetArg(shell_args[n], XtNtitle, title); n++;
  XtSetArg(shell_args[n], XtNwidth, w); n++;
  XtSetArg(shell_args[n], XtNheight, h); n++;

  widget = XtCreatePopupShell(shell_name, topLevelShellWidgetClass,
                              vgui_Xm::instance()->app_widget,
                              shell_args, n);
  form = XtVaCreateManagedWidget("form", xmFormWidgetClass, widget, NULL);
}


vgui_Xm_window::vgui_Xm_window(int w, int h, const vgui_menu& menu, const char* title)
  : use_menubar(true)
  , use_statusbar(true)
{
  last_menubar = new vgui_menu;

  if (debug)
    vcl_cerr << "vgui_Xm_window::vgui_Xm_window with menus\n";

  adaptor = new vgui_Xm_adaptor;

  char *shell_name = "popup";

  Arg shell_args[3]; int n=0;
  XtSetArg(shell_args[n], XtNtitle, title); n++;
  XtSetArg(shell_args[n], XtNwidth, w); n++;
  XtSetArg(shell_args[n], XtNheight, h); n++;


  widget = XtCreatePopupShell(shell_name, topLevelShellWidgetClass,
                              vgui_Xm::instance()->app_widget,
                              shell_args, XtNumber(shell_args) );
  form = XtVaCreateManagedWidget("form", xmFormWidgetClass, widget, NULL);

  set_menubar(menu);
}


void vgui_Xm_window::set_menubar(const vgui_menu &menu)
{
  use_menubar = true;
  *last_menubar = menu; // fsm@robots

  menubar = XmCreateMenuBar(form,"menubar",NULL, 0);
  XtVaSetValues(menubar,
                XmNmarginHeight, 0,
                XmNmarginWidth, 0,
                NULL);

  XtManageChild(menubar);
  vgui_Xm_utils::set_menu(menubar, *last_menubar);
}


vgui_Xm_window::~vgui_Xm_window()
{
  delete last_menubar;
}


void vgui_Xm_window::set_adaptor(vgui_adaptor* a)
{
  adaptor = static_cast<vgui_Xm_adaptor*>(a);
}

vgui_adaptor* vgui_Xm_window::get_adaptor()
{
  return adaptor;
}


void vgui_Xm_window::init()
{
  XtAddCallback(widget, XmNdestroyCallback, destroy, this);
  XtAddEventHandler(widget, EnterWindowMask, False, enter, this);
  //XtAddEventHandler(widget, LeaveWindowMask, False, leave, this);


  frame = XmCreateFrame(form, "frame", NULL, 0);
  XtManageChild(frame);

  XtVaSetValues(frame,
                XmNtopAttachment,    XmATTACH_FORM,
                XmNleftAttachment,   XmATTACH_FORM,
                XmNrightAttachment,  XmATTACH_FORM,
                XmNbottomAttachment, XmATTACH_FORM,
                0);


  if (use_menubar)
  {
    XtVaSetValues(menubar,
                  XmNtopAttachment,   XmATTACH_FORM,
                  XmNleftAttachment,  XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  XmNbottomAttachment,   XmATTACH_NONE,
                  0);

    XtVaSetValues(frame,
                  XmNtopAttachment,    XmATTACH_WIDGET,
                  XmNtopWidget,        menubar,
                  0);
  }


  if (use_statusbar)
  {
    statusbar.widget = XmCreateText(form, "statusbar", 0,0);
    XtManageChild(statusbar.widget);

    XtVaSetValues(statusbar.widget,
                  XmNcursorPositionVisible, False,
                  XmNsensitive, False,
                  XmNtopAttachment,     XmATTACH_NONE,
                  XmNleftAttachment,    XmATTACH_FORM,
                  XmNrightAttachment,   XmATTACH_FORM,
                  XmNbottomAttachment,  XmATTACH_FORM,
                  0);


    XtVaSetValues(frame,
                  XmNbottomAttachment, XmATTACH_WIDGET,
                  XmNbottomWidget,     statusbar.widget,
                  0);
  }

  XtManageChild(form);


  // specify visual directly
  Display *dpy = XtDisplay(widget);
  XVisualInfo *visinfo;
  if (!(visinfo = glXChooseVisual(dpy, DefaultScreen(dpy), attribs)))
    XtAppError(vgui_Xm::instance()->app_context, "no suitable RGB visual");

  adaptor->setup_widget(&frame, visinfo);

  XtRealizeWidget(widget);

  adaptor->setup_context(dpy, visinfo);
}


void vgui_Xm_window::show()
{
  init();

  XtPopup(widget, XtGrabNone);
  num_win++;
}

void vgui_Xm_window::hide()
{
  XtPopdown(widget);
  num_win--;
}
