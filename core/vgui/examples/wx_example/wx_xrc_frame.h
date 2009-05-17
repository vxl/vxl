// This is core/vgui/examples/wx_example/wx_xrc_frame.h
#ifndef wx_xrc_frame_h_
#define wx_xrc_frame_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets main frame class.
// \author Matt Leotta (mleotta)
//
// \verbatim
//  Modifications
//   08/01/2008 - File created. (mleotta)
// \endverbatim
//=========================================================================

#include <wx/frame.h>

#include <vgui/impl/wx/vgui_wx_adaptor.h>
#include <vgui/impl/wx/vgui_wx_statusbar.h>
#include <vgui/vgui_easy2D_tableau.h>

class wxSliderPanel;
class vgui_wx_observer;
class vgui_soview2D_point;

//-------------------------------------------------------------------------
//: The wxWidgets main frame class.
//-------------------------------------------------------------------------
class wx_xrc_frame : public wxFrame
{
  DECLARE_CLASS(wx_xrc_frame)
  DECLARE_EVENT_TABLE()

 public:
  //: Constructor - default.
  wx_xrc_frame(wxWindow* parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               long type);

  //: Destructor
  ~wx_xrc_frame();

  //wxMenu* edit_menu() const { return edit_menu_; }

  // Event handlers.
  void on_about(wxCommandEvent& event);
  void on_quit (wxCommandEvent& event);

  void move_point(double x, double y);

 private:
  vgui_wx_adaptor*  canvas_;
  wxSliderPanel*  sliders_;
  vgui_wx_observer* observer_;

  //: Statusbar that vgui writes to.
  vgui_wx_statusbar* statusbar_;
  vgui_easy2D_tableau_sptr tab_;
  vgui_soview2D_point* point_;
};

#endif // wx_xrc_frame_h_
