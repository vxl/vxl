// This is oxl/vgui/impl/Fl/vgui_Fl.h
#ifndef vgui_Fl_h_
#define vgui_Fl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   16 Sep 99
// \brief  The FLTK (Fast Light Toolkit) implementation of vgui.
//
//  Contains classes:  vgui_Fl
//  For info on FLTK, see: http://fltk.easysw.com/

#include <vgui/vgui_toolkit.h>
class vgui_Fl_adaptor;
class vgui_Fl_window;

//: The FLTK (Fast Light Toolkit) implementation of vgui.
class vgui_Fl : public vgui_toolkit
{
 public:
  // singleton method
  static vgui_Fl* instance();

  virtual vcl_string name() const;

  virtual void run();
  virtual void run_one_event();
  virtual void flush();
  virtual void add_event(const vgui_event&);

  virtual vgui_window* produce_window(int width, int height, const vgui_menu& menubar,
                                           const char* title="vgui Fl window");

  virtual vgui_window* produce_window(int width, int height,
                                           const char* title="vgui Fl popup");

  virtual vgui_dialog_impl* produce_dialog(const char* name);

 protected:
  vgui_Fl();
  static vgui_Fl* instance_;
};

#endif // vgui_Fl_h_
