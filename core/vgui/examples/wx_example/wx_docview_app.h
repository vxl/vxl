// This is core/vgui/examples/wx_example/wx_docview_app.h
#ifndef wx_docview_app_h_
#define wx_docview_app_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets main application class.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/19/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <wx/app.h>
#include <wx/docview.h>
class wx_docview_frame;

//-------------------------------------------------------------------------
//: The wxWidgets main application class.
//-------------------------------------------------------------------------
class wx_docview_app : public wxApp
{
 public:
  //: Constructor - default.
  wx_docview_app();

  //: Destructor.
  virtual ~wx_docview_app() {}
  
  //: Called on app initialization.
  virtual bool OnInit();

  //: Called on app exit.
  virtual int OnExit();

  //: Provides access to this Apps main frame.
  wx_docview_frame* get_main_frame() const { return frame_; }

 private:
  wxDocManager*      doc_manager_;
  wx_docview_frame*  frame_;
};

//: Implements wx_docview_app& wxGetApp().
DECLARE_APP(wx_docview_app)

#endif // wx_docview_app_h_
