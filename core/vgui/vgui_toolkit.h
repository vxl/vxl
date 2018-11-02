// This is core/vgui/vgui_toolkit.h
#ifndef vgui_toolkit_h_
#define vgui_toolkit_h_
//:
// \file
// \author fsm
// \brief  Base class for toolkit implementations.
//
//  Contains class vgui_toolkit

#include <string>
#include <vector>
#include "dll.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vgui_window;
class vgui_adaptor;
class vgui_event;
class vgui_menu;
class vgui_dialog_impl;
class vgui_dialog_extensions_impl;
class vgui_tableau;
struct vgui_tableau_sptr;

//: Base class for toolkit implementations.
//  The toolkit is responsible for opening the initial
//  window, and running the main event loop.
//  An example derived class is vgui_mfc .
class vgui_toolkit
{
 public:
  vgui_toolkit();
  virtual ~vgui_toolkit();

  virtual void init(int &, char **);
  virtual void uninit();
  virtual std::string name() const =0;

  virtual vgui_window* produce_window(int width, int height,
                                      vgui_menu const &menubar,
                                      char const *title);

  virtual vgui_window* produce_window(int width, int height,
                                      char const *title);

  virtual vgui_dialog_impl* produce_dialog(char const *name);

  virtual vgui_dialog_extensions_impl* produce_dialog_extension(char const *name);

  virtual void run() = 0;
  virtual void run_one_event();
  virtual void run_till_idle();
  virtual void flush();
  virtual void add_event(vgui_event const &);
  virtual void quit();

  static vgui_toolkit *lookup(char const *);
  static std::vector<vgui_toolkit*> *registry();
};

#endif // vgui_toolkit_h_
