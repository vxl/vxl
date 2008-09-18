// This is brl/bbas/bwm/wxWidgets/bwm_wx_app.cxx
//=========================================================================
#include "bwm_wx_app.h"
//:
// \file
// \brief  wxWidgets main application class.
//
// See bwm_wx_app.h for details.
//=========================================================================

#include "bwm_wx_frame.h"

#include <vgui/wx/vgui_wx.h>
#include <wx/xrc/xmlres.h>
#include <vgui/vgui.h>

extern void InitXmlResource(); // defined in generated file

//: Give wxWidgets the means to create a bwm_wx_app object.
IMPLEMENT_APP(bwm_wx_app)

bwm_wx_app::bwm_wx_app(void)
  : frame_(0)
{
}

//: Initialize the application.
bool bwm_wx_app::OnInit(void)
{
  // select the vgui_wx toolkit
  vgui::select("wx");

  wxImage::AddHandler(new wxXPMHandler);
  wxImage::AddHandler(new wxGIFHandler);

  vgui_wx::InitVguiHandlers();
  wxXmlResource::Get()->InitAllHandlers();
  InitXmlResource();

  // The toolbar
  if (!wxXmlResource::Get()->Load(wxT("crosscut.xrc")))
    return false;

  frame_ = new bwm_wx_frame(0,
                                  wxID_ANY,
                                  wxT("CrossCut"),
                                  wxPoint(0, 0),
                                  wxSize(1000, 800),
                                  wxDEFAULT_FRAME_STYLE);
  frame_->Centre(wxBOTH);
  frame_->Show(true);

  // start the event loop
  return true;
}
