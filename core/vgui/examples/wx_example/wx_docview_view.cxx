// This is core/vgui/examples/wx_example/wx_docview_view.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets view class.
//
// See wx_docview_view.h for details.
//=========================================================================

#include "wx_docview_view.h"

#include "wx_docview_app.h"
#include "wx_docview_doc.h"
#include "wx_docview_frame.h"

#include <wx/frame.h>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wx_docview_view, wxView)

wx_docview_view::wx_docview_view(void)
  : frame_(0)
  , canvas_(0)
{
}

wx_docview_view::~wx_docview_view(void)
{
  if (canvas_) { delete canvas_; }
}

//-------------------------------------------------------------------------
// Event handling.
//-------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wx_docview_view, wxView)
END_EVENT_TABLE()

bool wx_docview_view::OnCreate(wxDocument* doc, long flags)
{
  frame_ = wxGetApp().get_main_frame();
  SetFrame(frame_);
  Activate(true);

  int width, height;
  frame_->GetClientSize(&width, &height);

  canvas_ = new vgui_wx_adaptor(frame_,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxSize(width,height));

  canvas_->set_tableau(dynamic_cast<wx_docview_doc*>(doc)->tableau());
  canvas_->set_view(this);

  return true;
}

void wx_docview_view::OnDraw(wxDC* dc)
{
}

void wx_docview_view::OnUpdate(wxView* WXUNUSED(sender),
                               wxObject* WXUNUSED(hint))
{
  if (frame_ && canvas_) { canvas_->Refresh(); }
}

bool wx_docview_view::OnClose(bool WXUNUSED(delete_window))
{
  if (!GetDocument()->Close()) { return false; }
  if (frame_)
  {
    frame_->SetTitle(wxTheApp->GetAppName());
  }
  SetFrame(0);
  Activate(false);

  return true;
}
