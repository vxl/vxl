// This is core/vgui/examples/wx_example/wx_docview_frame.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets main frame class.
//
// See wx_docview_frame.h for details.
//=========================================================================

#include "wx_docview_frame.h"

#include <wx/menu.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
IMPLEMENT_CLASS(wx_docview_frame, wxDocParentFrame)

wx_docview_frame::wx_docview_frame(wxDocManager* manager,
                                   wxFrame* parent,
                                   wxWindowID id,
                                   const wxString& title,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long type)
  : wxDocParentFrame(manager, parent, id, title, pos, size, type)
{
  //SetIcon(wxIcon("flag_pr.ico", wxBITMAP_TYPE_ICO));

  wxMenu* file_menu = new wxMenu;
  file_menu->Append(wxID_NEW,   wxT("&New\tCtrl-N"),   wxT("Create new document."));
  file_menu->Append(wxID_CLOSE, wxT("&Close\tCtrl-W"), wxT("Close active document."));
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT,  wxT("E&xit\tAlt-X"),   wxT("Quit this program."));

  wxMenu* help_menu = new wxMenu;
  help_menu->Append(wxID_ABOUT, wxT("&About...\tF1"), wxT("Show about dialog."));

  wxMenuBar* menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, wxT("&File"));
  menu_bar->Append(help_menu, wxT("&Help"));
  SetMenuBar(menu_bar);

  CreateStatusBar(1);
  SetStatusText(wxT("Welcome to wxWidgets!"));
}

//-------------------------------------------------------------------------
// Event handling.
//-------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wx_docview_frame, wxDocParentFrame)
  EVT_MENU(wxID_ABOUT, wx_docview_frame::OnAbout)
END_EVENT_TABLE()

void wx_docview_frame::OnAbout(wxCommandEvent& event)
{
  wxString msg;
  msg.Printf(wxT("Hello and welcome to %s"), wxVERSION_STRING);
  wxMessageBox(msg, wxT("About Minimal"),
               wxOK | wxICON_INFORMATION, this);
}
