// This is core/vgui/examples/wx_example/wx_childview_frame.h
#ifndef wx_childview_frame_h_
#define wx_childview_frame_h_
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

#include <wx/frame.h>

#include <vgui/impl/wx/vgui_wx_adaptor.h>

//-------------------------------------------------------------------------
//: The wxWidgets main frame class.
//-------------------------------------------------------------------------
class wx_childview_frame : public wxFrame
{
  DECLARE_CLASS(wx_childview_frame)
  DECLARE_EVENT_TABLE()

public:
  //: Constructor - default.
  wx_childview_frame(wxWindow* parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long type);

  //wxMenu* edit_menu() const { return edit_menu_; }

  // Event handlers.
  void on_about(wxCommandEvent& event);
  void on_quit (wxCommandEvent& event);

private:
  vgui_wx_adaptor*  canvas_;
};

#endif // wx_childview_frame_h_
