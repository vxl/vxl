// This is core/vgui/examples/wx_example/wx_childview_app.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets main application class.
//
// See wx_childview_app.h for details.
//=========================================================================

#include "wx_childview_app.h"
#include "wx_childview_frame.h"

#include <vgui/vgui.h>

//: Give wxWidgets the means to create a wx_childview_app object.
IMPLEMENT_APP(wx_childview_app)

wx_childview_app::wx_childview_app(void)
  : frame_(0)
{
}

//: Initialize the application.
bool wx_childview_app::OnInit(void)
{
  // select the vgui_wx toolkit
  vgui::select("wx");

  frame_ = new wx_childview_frame(0,
                                  wxID_ANY,
                                  wxT("Childview Example"),
                                  wxPoint(0, 0),
                                  wxSize(500, 400),
                                  wxDEFAULT_FRAME_STYLE);
  frame_->Centre(wxBOTH);
  frame_->Show(true);

  // start the event loop
  return true;
}
