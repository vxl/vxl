// This is core/vgui/examples/wx_example/wx_childview_frame.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets main frame class.
//
// See wx_childview_frame.h for details.
//=========================================================================

#include "wx_childview_frame.h"

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/msgdlg.h>

#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
namespace
{
  vgui_tableau_sptr create_tableau(void)
  {
    vgui_tableau_sptr tableau;

    // Put the image.tableau into a easy2D tableau
    vgui_easy2D_tableau_new easy2D;

    // Add a point, line, and infinite line
    easy2D->set_foreground(0,1,0);
    easy2D->set_point_radius(5);
    easy2D->add_point(10, 20);

    easy2D->set_foreground(0,0,1);
    easy2D->set_line_width(2);
    easy2D->add_line(100,100,200,400);

    easy2D->set_foreground(0,1,0);
    easy2D->set_line_width(2);
    easy2D->add_infinite_line(1,1,-100);

    vgui_viewer2D_tableau_new viewer(easy2D);

    vgui_shell_tableau_new shell(viewer);
    shell->set_enable_key_bindings(true);
    tableau = shell;

    return tableau;
  }
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
IMPLEMENT_CLASS(wx_childview_frame, wxFrame)

wx_childview_frame::wx_childview_frame(wxWindow* parent,
                                       wxWindowID id,
                                       const wxString& title,
                                       const wxPoint& pos,
                                       const wxSize& size,
                                       long type)
  : wxFrame(parent, id, title, pos, size, type)
{
  //SetIcon(wxIcon("app.ico", wxBITMAP_TYPE_ICO));

  wxMenu* file_menu = new wxMenu;
  file_menu->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Quit this program."));

  wxMenu* help_menu = new wxMenu;
  help_menu->Append(wxID_ABOUT, wxT("&About...\tF1"), wxT("Show about dialog."));

  wxMenuBar* menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, wxT("&File"));
  menu_bar->Append(help_menu, wxT("&Help"));
  SetMenuBar(menu_bar);

  CreateStatusBar(1);
  SetStatusText(wxT("Welcome to wxWidgets!"));

  canvas_ = new vgui_wx_adaptor(this);
  canvas_->set_tableau(create_tableau());
}

//-------------------------------------------------------------------------
// Event handling.
//-------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wx_childview_frame, wxFrame)
  EVT_MENU(wxID_ABOUT, wx_childview_frame::on_about)
  EVT_MENU(wxID_EXIT,  wx_childview_frame::on_quit )
END_EVENT_TABLE()

void wx_childview_frame::on_about(wxCommandEvent& event)
{
  wxString msg;
  msg.Printf(wxT("Hello and welcome to %s"), wxVERSION_STRING);
  wxMessageBox(msg, wxT("About Minimal"),
               wxOK | wxICON_INFORMATION, this);
}

void wx_childview_frame::on_quit(wxCommandEvent& event)
{
  Close();
}
