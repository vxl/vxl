// This is core/vgui/vgui.h
#ifndef vgui_h_
#define vgui_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   30 Sep 99
// \brief  Namespace with a singleton vgui_toolkit instance.
//
// \verbatim
//  Modifications
//   30-SEP-1999  P.Pritchett - Initial version.
//   26 Oct 1999  fsm. various changes to facilitate the port of old impl code.
//   30-AUG-2000  Marko Bacic, Oxford RRG -- Added flags to support MFC accel.
//   26-APR-2002  K.Y.McGaul - Added some doxygen style comments.
// \endverbatim

#include "dll.h"
#include <vcl_string.h>
#include <vcl_iosfwd.h> // for the status bar vcl_ostream.

class vgui_window;
class vgui_adaptor;
class vgui_event;
class vgui_menu;
class vgui_dialog_impl;
class vgui_tableau;
class vgui_toolkit;
struct vgui_tableau_sptr;

//: Namespace with a singleton vgui_toolkit instance.
//
// vgui is a namespace with a singleton vgui_toolkit instance
// which acts as an abstract factory. The static methods on
// vgui simply dispatch the call to the selected toolkit.
//
// Order of things :
// 1. Registration. Toolkits available to the application are registered
//    in a global list. Registration is done by the constructor of vgui_toolkit,
//    so creating a vgui_toolkit amounts to registration. Sometimes this can be
//    done at library initialization time but sometimes it can't, e.g. for
//    static builds. In that case, a {\em tag function} must be called
//    explicitly. See vgui_tag.* and vgui_register_all.cxx.
//
// 2. Choice of toolkit to use. Several toolkits may be available, and so
//    a choice must be made as to which to use. The choice of toolkit is
//    specified with the select() methods.
//
// 3. Initialization of toolkit. There is only one method for doing this, namely
//      vgui::init(argc, argv);
//    which needs a plausible command line. If no toolkit has been selected
//    the init() function will try to choose one for you based on the
//    given command line.

class vgui
{
  //: Selected toolkit instance.
  static vgui_DLLDATA vgui_toolkit* instance_;

  //: True once init() has been called.
  static vgui_DLLDATA bool init_called;
 public:
  //: Needed for statusbar.
  static vgui_DLLDATA vcl_ostream out;

  //: Method for determining if a given toolkit is available.
  static bool exists(char const *toolkit);

  //: Method for selecting a specific toolkit.
  //  This will abort() if given a toolkit which is not available.
  static void select(char const *toolkit);

  //: Select a toolkit from command line arguments and environment variables.
  //
  //  First, the command line is scanned for --factory=xxx options.
  //
  //  If no such option is given, the environment variable 'vgui' is inspected.
  //
  //  If no such environment variable is set, no toolkit is selected and the
  //  function returns false. Else the return value is true.
  static bool select(int &argc, char **argv);

  //: Initialize the selected toolkit passing it the given command line.
  static void init(int &argc, char **argv);

  //: Uninitialize any previously initialized toolkit.
  //
  // This will be called before application exit; the user normally
  // need not call this.
  //
  static void uninit();

  // Factory methods
  //----------------

  //: Produce window with menubar.
  static vgui_window* produce_window(int width, int height,
                                     vgui_menu const & menubar,
                                     vcl_string const &title ="");
  //: Produce window without menubar.
  static vgui_window* produce_window(int width, int height,
                                     vcl_string const &title ="");
  //: Produce dialog box.
  static vgui_dialog_impl* produce_dialog(vcl_string const &name);

  // Convenience methods
  //--------------------

  //: Display this tableau and run till dead (no menubar).
  static int run(vgui_tableau_sptr const&, int w, int h,
                 vcl_string const &title ="");

  //: Display this tableau and run till dead (with menubar)
  static int run(vgui_tableau_sptr const&, int w, int h,
                 vgui_menu const &menubar, vcl_string const &title ="");

  //: Create the vgui_window but don't run it (no menubar).
  static vgui_window *adapt(vgui_tableau_sptr const&, int w, int h,
                            vcl_string const &title ="");

  //: Create the vgui_window but don't run it (with menubar).
  static vgui_window *adapt(vgui_tableau_sptr const&, int w, int h,
                            vgui_menu const &, vcl_string const &title ="");

  // Functions for event-loop management
  //------------------------------------

  //: Run until quit is called.
  static int  run();

  //: Run the next event in the event queue.
  static void run_one_event();

  //: Run all events in the event queue.
  static void run_till_idle();

  //: Remove all events from the event queue.
  static void flush();

  //: Add event to the event queue.
  static void add_event(vgui_event const &);

  //: Quit application.
  static void quit();
};


// Trigger the registration of all the available toolkit
// implementations. This variable has to appear in a translation unit
// (.cxx file) that has a function used by the main
// program. Otherwise, for static libraries, the linker will not link
// in the corresponding object file, which means the variable is never
// initialized, which means that no toolkits are registered. Since
// anyone using vgui must call vgui::init at some point, this
// translation unit will be included in the final executable, and
// therefore the trigger variable below will be initialized, causing
// the toolkits to be registered.
int vgui_register_all_implementations();
static int trigger = vgui_register_all_implementations();

// This function is here just to avoid "unused variable" compiler warnings
static int dummy_use_of_trigger() { return trigger? 0: dummy_use_of_trigger(); }

#endif // vgui_h_
