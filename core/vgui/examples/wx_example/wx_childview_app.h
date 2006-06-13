// This is core/vgui/examples/wx_example/wx_childview_app.h
#ifndef wx_childview_app_h_
#define wx_childview_app_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets main application class.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/31/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <wx/app.h>

class wx_childview_frame;

//-------------------------------------------------------------------------
//: The wxWidgets main application class.
//-------------------------------------------------------------------------
class wx_childview_app : public wxApp
{
 public:
  //: Constructor - default.
  wx_childview_app();

  //: Destructor.
  virtual ~wx_childview_app() {}
  
  //: Called on app initialization.
  virtual bool OnInit();

  //: Provides access to this Apps main frame.
  wx_childview_frame* get_main_frame() const { return frame_; }

 private:
  wx_childview_frame*  frame_;
};

//: Implements wx_childview_app& wxGetApp().
DECLARE_APP(wx_childview_app)

#endif // wx_childview_app_h_
