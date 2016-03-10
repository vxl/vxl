// This is core/vgui/impl/wx/vgui_wx.h
#ifndef vgui_wx_h_
#define vgui_wx_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_toolkit.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/19/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <vgui/vgui_toolkit.h>
class vgui_wx_window;

//-------------------------------------------------------------------------
//: wxWidgets implementation of vgui_toolkit.
//
// Provides functions for controlling the event loop and creating new
// windows and dialogs.
//-------------------------------------------------------------------------
class vgui_wx : public vgui_toolkit
{
public:
  //: Singleton method instance.
  static vgui_wx* instance();

private:
  //: Returns the name of the GUI toolkit ("wx").
  virtual std::string name() const;

  //: Constructor - default.
  vgui_wx();

  //: Destructor.
  virtual ~vgui_wx();

  //: Initialize the wxWidgets GUI framework.
  virtual void init(int&, char**);

  virtual void uninit();

  //: Run the event loop.
  virtual void run();

  //: Run the next event.
  virtual void run_one_event();

  //: Run until event queue is empty.
  virtual void run_till_idle();

  //: Clear all events from the queue.
  virtual void flush();

  //: Add an event to the queue.
  virtual void add_event(const vgui_event&);

  //: Quit the application.
  virtual void quit();

  //: Create a new window with a menubar.
  virtual vgui_window* produce_window(int width, int height,
                                      const vgui_menu& menubar,
                                      const char* title="vgui wx window");

  //: Create a new window.
  virtual vgui_window* produce_window(int width, int height,
                                      const char* title="vgui wx popup");

  //: Create a new dialog window.
  virtual vgui_dialog_impl* produce_dialog(const char* name);

  //: Handle to windows created that need deleting.
  std::vector<vgui_window*> windows_to_delete_;

  //: True if we are embedding vgui_adaptor into wxWidgets app.
  bool adaptor_embedded_;
};

#endif // vgui_wx_h_
