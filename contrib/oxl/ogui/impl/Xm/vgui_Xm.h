// This is oxl/vgui/impl/Xm/vgui_Xm.h
#ifndef vgui_Xm_h_
#define vgui_Xm_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Undocumented class FIXME
//
// \author
//              Philip C. Pritchett, 16 Sep 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include <vgui/vgui_toolkit.h>
#include <X11/Intrinsic.h>

class vgui_Xm_adaptor;
class vgui_Xm_window;


class vgui_Xm : public vgui_toolkit
{
 public:
  // singleton method
  static vgui_Xm* instance();

  virtual vcl_string name() const;
  void init(int &, char **);


  virtual void run();
  virtual void run_one_event();

  virtual vgui_window* produce_window(int width, int height, const vgui_menu& menubar,
                                           const char* title="vgui Xm window");

  virtual vgui_window* produce_window(int width, int height,
                                           const char* title="vgui Xm popup");

  vgui_dialog_impl* produce_dialog(const char* name);

  // motif specific

  XtAppContext app_context;
  Widget app_widget;

 protected:
  vgui_Xm();
  static vgui_Xm* instance_;
};

#endif // vgui_Xm_h_
