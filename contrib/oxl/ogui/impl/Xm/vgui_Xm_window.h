// This is oxl/ogui/impl/Xm/vgui_Xm_window.h
#ifndef vgui_Xm_window_h_
#define vgui_Xm_window_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author
//              Philip C. Pritchett, 05 Oct 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include <vgui/vgui_window.h>
#include <X11/Intrinsic.h>
#include <GL/glx.h>
// It seems that the location of GLwMDrawA.h has moved in recent
// versions of Mesa. If we need to support both, we should put in a
// mechanism to figure out where it is.
//#include <X11/GLw/GLwMDrawA.h>
#include <GL/GLwMDrawA.h>
#include "vgui_Xm_statusbar.h"
#include <vgui/vgui_menu.h>

class vgui_Xm_adaptor;

class vgui_Xm_window : public vgui_window
{
 public:

  vgui_Xm_window(int w, int h, const vgui_menu& menu, const char* title);
  vgui_Xm_window(int w, int h, const char* title);
  ~vgui_Xm_window();

  void init();

  void set_menubar(const vgui_menu &);
  void set_statusbar(bool v) {use_statusbar = v;}

  void set_adaptor(vgui_adaptor* a);
  vgui_adaptor* get_adaptor();

  bool use_menubar;
  bool use_statusbar;


  void show();
  void hide();

  vgui_Xm_adaptor *adaptor;
  vgui_Xm_statusbar statusbar;

  Widget widget;
  Widget form;
  Widget frame;
  Widget menubar;

  static void destroy(Widget w, XtPointer client_data, XtPointer call);
  static int num_win;

 private:
  vgui_menu* last_menubar;
};

#endif // vgui_Xm_window_h_
