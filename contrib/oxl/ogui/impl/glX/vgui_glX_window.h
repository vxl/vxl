// This is oxl/ogui/impl/glX/vgui_glX_window.h
#ifndef vgui_glX_window_h_
#define vgui_glX_window_h_
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
#include <X11/Xlib.h>
#include <GL/glx.h>

class vgui_menu;
class vgui_glX_adaptor;

class vgui_glX_window : public vgui_window
{
 public:

  vgui_glX_window(int w, int h, const char* title);
  ~vgui_glX_window();

  void set_adaptor(vgui_adaptor* a);
  vgui_adaptor* get_adaptor();

  void show();

  void handle(XEvent*);

  Window window;
  GLXContext context;
  vgui_glX_adaptor *adaptor;
};

#endif // vgui_glX_window_h_
