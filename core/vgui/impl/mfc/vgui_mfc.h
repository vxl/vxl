#ifndef vgui_mfc_h_
#define vgui_mfc_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_mfc - the mfc++ implementation of vgui
// .LIBRARY vgui-mfc
// .HEADER vxl Package
// .INCLUDE vgui/impl/mfc/vgui_mfc.h
// .FILE vgui_mfc.cxx
//
// .SECTION Description:
//
// vgui_mfc is the mfc++ implementation of vgui.
// Provides functions for controlling the event loop.
// Based upon vgui_gtk.
//
// .SECTION Author:
//              awf@robots.ox.ac.uk, July 2000
//
// .SECTION Modifications:
//   Marko Bacic,Oxford RRG  July 2000 -- Implemented support for MFC 
//   15-AUG-200 Marko Bacic, Oxford RRG -- Implemented run_one_event.
//
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vgui/vgui_toolkit.h>
class vgui_mfc_adaptor;
class vgui_mfc_window;
class vgui_mfc_utils;


class vgui_mfc : public vgui_toolkit {
public:
  // singleton method
  static vgui_mfc* instance();

  virtual vcl_string name() const;

  virtual void run();
  virtual void run_one_event();
  virtual void run_till_idle();
  virtual void flush();
  virtual void quit();
  virtual void add_event(const vgui_event&);

  virtual vgui_window* produce_window(int width, int height, const vgui_menu& menubar,
					   const char* title="vgui mfc window");
  
  virtual vgui_window* produce_window(int width, int height,
					   const char* title="vgui mfc popup");

  virtual vgui_dialog_impl* produce_dialog(const char* name);
  
  vgui_mfc_utils *utils;
protected:
  vgui_mfc();
  void init(int &, char **);
  static vgui_mfc* instance_;

};

#endif // vgui_mfc_h_
