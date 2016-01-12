// This is core/vgui/examples/wx_example/wx_xrc_app.h
#ifndef wx_xrc_app_h_
#define wx_xrc_app_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets main application class.
// \author Matt Leotta (mleotta)
//
// \verbatim
//  Modifications
//   08/01/2008 - File created. (mleotta)
// \endverbatim
//=========================================================================

#include <wx/app.h>

class wx_xrc_frame;

//-------------------------------------------------------------------------
//: The wxWidgets main application class.
//-------------------------------------------------------------------------
class wx_xrc_app : public wxApp
{
 public:
  //: Constructor - default.
  wx_xrc_app();

  //: Destructor.
  virtual ~wx_xrc_app() {}

  //: Called on app initialization.
  virtual bool OnInit();

  //: Provides access to this Apps main frame.
  wx_xrc_frame* get_main_frame() const { return frame_; }

 private:
  wx_xrc_frame*  frame_;
};

//: Implements wx_xrc_app& wxGetApp().
DECLARE_APP(wx_xrc_app)

#endif // wx_xrc_app_h_
