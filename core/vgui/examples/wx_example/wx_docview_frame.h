// This is core/vgui/examples/wx_example/wx_docview_frame.h
#ifndef wx_docview_frame_h_
#define wx_docview_frame_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets main frame class.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/19/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <wx/docview.h>

//-------------------------------------------------------------------------
//: The wxWidgets main frame class.
//-------------------------------------------------------------------------
class wx_docview_frame : public wxDocParentFrame
{
  DECLARE_CLASS(wx_docview_frame)
  DECLARE_EVENT_TABLE()

 public:
  //: Constructor - default.
  wx_docview_frame(wxDocManager* manager,
                   wxFrame* parent,
                   wxWindowID id,
                   const wxString& title,
                   const wxPoint& pos,
                   const wxSize& size,
                   long type);

  // Event handlers.
  void OnAbout(wxCommandEvent& event);
};

#endif // wx_docview_frame_h_
