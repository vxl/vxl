// This is core/vgui/impl/mfc/vgui_mfc.h
#ifndef vgui_mfc_h_
#define vgui_mfc_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief   MFC (Microsoft Foundation Classes) implementation of vgui_toolkit.
// \author  awf@robots.ox.ac.uk
// \date    July 2000
//
// \verbatim
//  Modifications
//   July 2000   Marko Bacic, Oxford RRG   -- Implemented support for MFC
//   15-AUG-2000 Marko Bacic, Oxford RRG    -- Implemented run_one_event.
//   29-AUG-2001 K.Y.McGaul - Added destructor to remove Purify mem leak.
//   23-DEC-2002 J.L. Mundy - Set window on adaptor for convenient access
// \endverbatim

#include <vcl_vector.h>
#include <vgui/vgui_toolkit.h>
class vgui_mfc_adaptor;
class vgui_mfc_window;
class vgui_mfc_utils;
class vgui_mfc_app;

//: The MFC (Microsoft Foundation Classes) implementation of vgui_toolkit.
//
// Provides functions for controlling the event loop.
// Based upon vgui_gtk.
class vgui_mfc : public vgui_toolkit
{
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
  ~vgui_mfc();
  void init(int &, char **);
  void uninit();
  vgui_mfc_app* theApp_;
  vcl_vector<vgui_window*> windows_to_delete;
};

#endif // vgui_mfc_h_
