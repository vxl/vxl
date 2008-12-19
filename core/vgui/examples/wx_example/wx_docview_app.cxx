// This is core/vgui/examples/wx_example/wx_docview_app.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets main application class.
//
// See wx_docview_app.h for details.
//=========================================================================

#include "wx_docview_app.h"
#include "wx_docview_frame.h"
#include "wx_docview_doc.h"
#include "wx_docview_view.h"

#include <vgui/vgui.h>

//: Give wxWidgets the means to create a wx_docview_app object.
IMPLEMENT_APP(wx_docview_app)

wx_docview_app::wx_docview_app(void)
  : doc_manager_(0)
  , frame_(0)
{
}

//: Initialize the application.
bool wx_docview_app::OnInit(void)
{
  // select the vgui_wx toolkit
  vgui::select("wx");

  doc_manager_ = new wxDocManager;
  new wxDocTemplate(doc_manager_,
                    wxT("wxWidgets Example"),
                    wxT(""),
                    wxT(""),
                    wxT(""),
                    wxT("wx_docview_doc"),
                    wxT("wx_docview_view"),
                    CLASSINFO(wx_docview_doc),
                    CLASSINFO(wx_docview_view));

  doc_manager_->SetMaxDocsOpen(1);

  frame_ = new wx_docview_frame(doc_manager_,
                                0,
                                wxID_ANY,
                                wxT("Hello World"),
                                wxPoint(0, 0),
                                wxSize(500, 400),
                                wxDEFAULT_FRAME_STYLE);
  frame_->Centre(wxBOTH);
  frame_->Show(true);
  SetTopWindow(frame_);

  // start with a new document
  doc_manager_->CreateDocument(wxEmptyString, wxDOC_NEW);

  // start the event loop
  return true;
}

//: Initialize the application.
int wx_docview_app::OnExit(void)
{
  delete doc_manager_;
  return 0;
}
