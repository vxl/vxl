// This is oxl/vgui/impl/gtk/vgui_gtk.h
#ifndef vgui_gtk_h_
#define vgui_gtk_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief vgui_gtk is a the GTK+ implementation of vgui
//
// vgui_gtk is a the GTK+ implementation of vgui.
// Provides functions for controlling the event loop.
//
// \author
//              Philip C. Pritchett, 16 Sep 99
//              Robotics Research Group, University of Oxford
//
// \verbatim
//  Modifications
//   K.Y.McGaul    10-NOV-1999    Initial version. Based on deprecated versions
//                                 impl/vgui_impl::factory and impl/gtk/gtk_factory.
//   K.Y.McGaul    18-NOV-1999    Added menubar.
//   K.Y.McGaul    10-DEC-1999    Removed class vgui_gtk_VGUI and made vgui_gtk
//                                 a derived class of vgui.
//                                 Moved menubar code to vgui_gtk_window.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_toolkit.h>
class vgui_gtk_adaptor;
class vgui_gtk_window;


class vgui_gtk : public vgui_toolkit
{
 public:
  // singleton method
  static vgui_gtk* instance();

 protected:
  virtual vcl_string name() const;

  virtual void run();
  virtual void run_one_event();
  virtual void run_till_idle();
  virtual void flush();
  virtual void quit();

  virtual vgui_window* produce_window(int width, int height, const vgui_menu& menubar,
                                           const char* title="vgui gtk window");

  virtual vgui_window* produce_window(int width, int height,
                                           const char* title="vgui gtk popup");

  virtual vgui_dialog_impl* produce_dialog(const char* name);

 protected:
  vgui_gtk();
  void init(int &, char **);
  static vgui_gtk* instance_;
};

#endif // vgui_gtk_h_
