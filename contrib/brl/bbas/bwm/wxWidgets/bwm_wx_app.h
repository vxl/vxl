// This is brl/bbas/bwm/wxWidgets/bwm_wx_app.h
#ifndef bwm_wx_app_h_
#define bwm_wx_app_h_
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

class bwm_wx_frame;
extern void InitXmlResource(); // defined in generated file
//-------------------------------------------------------------------------
//: The wxWidgets main application class.
//-------------------------------------------------------------------------
class bwm_wx_app : public wxApp
{
 public:
  //: Constructor - default.
  bwm_wx_app();

  //: Destructor.
  virtual ~bwm_wx_app() {}

  //: Called on app initialization.
  virtual bool OnInit();

  //: Provides access to this Apps main frame.
  bwm_wx_frame* get_main_frame() const { return frame_; }

 private:
  bwm_wx_frame*  frame_;
};

//: Implements bwm_wx_app& wxGetApp().
DECLARE_APP(bwm_wx_app)

#endif // bwm_wx_app_h_
