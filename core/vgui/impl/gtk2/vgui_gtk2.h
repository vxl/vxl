// This is core/vgui/impl/gtk2/vgui_gtk2.h
#ifndef vgui_gtk2_h_
#define vgui_gtk2_h_
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   16 Sep 99
// \brief  The GTK+ (GIMP Toolkit) implementation of vgui_toolkit.
//
//  Contains classes: vgui_gtk2
//  For info on GTK see: http://www.gtk.org/
//
// \verbatim
//  Modifications
//   K.Y.McGaul    10-NOV-1999 Initial version. Based on deprecated versions
//                             impl/vgui_impl::factory and impl/gtk/gtk_factory.
//   K.Y.McGaul    18-NOV-1999 Added menubar.
//   K.Y.McGaul    10-DEC-1999 Removed class vgui_gtk2_VGUI and made vgui_gtk2
//                             a derived class of vgui.
//                             Moved menubar code to vgui_gtk2_window.
// \endverbatim


#include <vgui/vgui_toolkit.h>
class vgui_gtk2_adaptor;
class vgui_gtk2_window;


//: The GTK+-2.0 implementation of vgui_toolkit.
//  Provides functions for controlling the event loop.
//  For info on GTK see: http://www.gtk.org/
class vgui_gtk2 : public vgui_toolkit
{
public:
  //: Singleton method instance.
  static vgui_gtk2 *
  instance();

protected:
  //: Returns the name of the GUI toolkit ("gtk2").
  virtual std::string
  name() const;

  //: Run the event loop.
  virtual void
  run();

  //: Run the next event.
  virtual void
  run_one_event();

  //: Run until there are no more events left in the event queue.
  virtual void
  run_till_idle();

  //: Clear all events from the event queue.
  virtual void
  flush();

  //: Quit the application.
  virtual void
  quit();

  //: Create a window with a menubar.
  virtual vgui_window *
  produce_window(int width, int height, const vgui_menu & menubar, const char * title = "vgui gtk window");

  //: Create a window.
  virtual vgui_window *
  produce_window(int width, int height, const char * title = "vgui gtk popup");

  //: Create a dialog box.
  virtual vgui_dialog_impl *
  produce_dialog(const char * name);

protected:
  //: Default constructor.
  vgui_gtk2() {}

  //: Initialise the implementation of vgui.
  void
  init(int &, char **);
};

#endif // vgui_gtk2_h_
