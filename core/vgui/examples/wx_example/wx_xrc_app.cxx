// This is core/vgui/examples/wx_example/wx_xrc_app.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets main application class.
//
// See wx_xrc_app.h for details.
//=========================================================================

#include "wx_xrc_app.h"
#include "wx_xrc_frame.h"

#include <vgui/wx/vgui_wx.h>
#include <wx/xrc/xmlres.h>
#include <vgui/vgui.h>


extern void InitXmlResource(); // defined in generated file


//: Give wxWidgets the means to create a wx_xrc_app object.
IMPLEMENT_APP(wx_xrc_app)

wx_xrc_app::wx_xrc_app(void)
  : frame_(0)
{
}

//: Initialize the application.
bool wx_xrc_app::OnInit(void)
{
  // select the vgui_wx toolkit
  vgui::select("wx");

  vgui_wx::InitVguiHandlers();
  wxXmlResource::Get()->InitAllHandlers();
  InitXmlResource();

  frame_ = new wx_xrc_frame(0,
                            wxID_ANY,
                            wxT("XML Resource Example"),
                            wxPoint(0, 0),
                            wxSize(500, 400),
                            wxDEFAULT_FRAME_STYLE);
  frame_->Show(true);

  // start the event loop
  return true;
}
