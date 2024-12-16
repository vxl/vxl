// This is core/vgui/impl/wx/vgui_wx_app.h
#ifndef vgui_wx_app_h_
#define vgui_wx_app_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of wx_app
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/19/2006 - File created. (miguelfv)
//   11/16/2019 create separate class file JLM
// \endverbatim
//=========================================================================
#include <wx/app.h>

class vgui_wx_app_window;

//-------------------------------------------------------------------------
//: wxWidgets implementation of vgui_toolkit.
//
// Provides functions for controlling the event loop and creating new
// windows and dialogs.
//-------------------------------------------------------------------------
class vgui_wx_app : public wxApp
{
public:
  vgui_wx_app();

  ~vgui_wx_app();

  static vgui_wx_app *
  instance();

  static void
  delete_instance();

  bool
  OnInit()
  {
    return true;
  }

  virtual int
  OnExit()
  {
    return wxApp::OnExit();
  }

  virtual bool
  OnExceptionInMainLoop()
  {
    return false;
  }

private:
  static vgui_wx_app * instance_;
};

#endif // vgui_wx_app_h_
