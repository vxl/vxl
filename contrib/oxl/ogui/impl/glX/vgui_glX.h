// This is oxl/ogui/impl/glX/vgui_glX.h
#ifndef vgui_glX_h_
#define vgui_glX_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author
//              Philip C. Pritchett, 16 Sep 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vgui/vgui_toolkit.h>
#include <X11/Xlib.h>
class vgui_glX_adaptor;
class vgui_glX_window;


class vgui_glX : public vgui_toolkit
{
 public:
  // singleton method
  static vgui_glX* instance();

  virtual vcl_string name() const;

  virtual void run();
  virtual void run_one_event();
  virtual void flush();
  virtual void add_event(const vgui_event&);

  virtual vgui_window* produce_window(int width, int height, const vgui_menu& menubar,
                                      const char* title="vgui X window");

  virtual vgui_window* produce_window(int width, int height,
                                      const char* title="vgui X popup");

  virtual vgui_dialog_impl* produce_dialog(const char* name);


  void init(int &argc, char **argv);

  Display *display;

  vcl_vector<vgui_glX_window*> windows;

 protected:
  vgui_glX();
  static vgui_glX* instance_;
};

#endif // vgui_glX_h_
