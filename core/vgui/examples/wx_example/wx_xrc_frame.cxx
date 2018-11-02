// This is core/vgui/examples/wx_example/wx_xrc_frame.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets main frame class.
//
// See wx_xrc_frame.h for details.
//=========================================================================

#include <iostream>
#include <cstdlib>
#include "wx_xrc_frame.h"
#include <wx/xrc/xmlres.h>

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/msgdlg.h>
#include <wx/statusbr.h>

#include <vgui/vgui.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_observer.h>
#include <vgui/vgui_message.h>
#include <vgui/vgui_soview2D.h>

#include <vgui/wx/wxSliderPanel.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
namespace
{
  vgui_easy2D_tableau_sptr create_tableau()
  {
    // Put the image.tableau into an easy2D tableau
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

    return easy2D;
  }
}

class vgui_wx_observer: public vgui_observer
{
 public:
  virtual void update(const vgui_message& m)
  {
    const wxSliderPanel* p = static_cast<const wxSliderPanel*>(m.from);
    frame->move_point(p->data()[0], p->data()[1]);

    int i = *static_cast<const int*>(m.data);
    vgui::out << "updated index "<<i << std::endl;
  }
  wx_xrc_frame* frame;
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
IMPLEMENT_CLASS(wx_xrc_frame, wxFrame)

wx_xrc_frame::wx_xrc_frame(wxWindow* parent,
                           wxWindowID id,
                           const wxString& title,
                           const wxPoint& pos,
                           const wxSize& size,
                           long type)
  //: wxFrame(parent, id, title, pos, size, type)
{
  observer_ = new vgui_wx_observer();
  observer_->frame = this;

  //SetIcon(wxICON(appicon));
  wxXmlResource::Get()->LoadFrame(this,parent,wxT("wx_xrc_frame"));

  wxWindow* w = this->FindWindow(wxT("vgui_adaptor"));
  if (!w) {
    std::cout << "could not find vgui_adaptor" << std::endl;
    std::exit(-1);
  }
  canvas_ = dynamic_cast<vgui_wx_adaptor*>(w);
  if (!canvas_) {
    std::cout << "In valid widget type for vgui adaptor" << std::endl;
    std::exit(-1);
  }
  this->SetSize(wxSize(600,400));

  w = this->FindWindow(wxT("slider_panel"));
  if (!w) {
    std::cout << "could not find slider panel" << std::endl;
    std::exit(-1);
  }
  sliders_ = dynamic_cast<wxSliderPanel*>(w);
  if (!sliders_) {
    std::cout << "In valid widget type for slider panel" << std::endl;
    std::exit(-1);
  }
  std::vector<double> min_vals, max_vals, init_vals;
  min_vals.push_back(1.0);
  min_vals.push_back(5.0);
  max_vals.push_back(300.0);
  max_vals.push_back(200.0);
  init_vals.push_back(20.0);
  init_vals.push_back(10.0);
  sliders_->CreateSliders(init_vals,min_vals,max_vals);
  sliders_->attach(static_cast<vgui_observer*>(observer_));

  w = this->FindWindow(wxT("statusbar"));
  wxStatusBar* b = dynamic_cast<wxStatusBar*>(w);
  if (b) {
    statusbar_ = new vgui_wx_statusbar;
    statusbar_->set_widget(b);
    vgui::out.rdbuf(statusbar_->statusbuf());
  }

  tab_ = create_tableau();
  tab_->set_foreground(1,0,0);
  point_ = tab_->add_point(sliders_->data()[0],sliders_->data()[1]);

  vgui_tableau_sptr tableau;
  vgui_viewer2D_tableau_new viewer(tab_);
  vgui_shell_tableau_new shell(viewer);
  shell->set_enable_key_bindings(true);
  canvas_->set_tableau(shell);
}

wx_xrc_frame::~wx_xrc_frame()
{
  delete statusbar_;
  delete observer_;
}
//-------------------------------------------------------------------------
// Event handling.
//-------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wx_xrc_frame, wxFrame)
  EVT_MENU(XRCID("menu_about"), wx_xrc_frame::on_about)
  EVT_MENU(XRCID("menu_quit"),  wx_xrc_frame::on_quit)
END_EVENT_TABLE()


void wx_xrc_frame::on_about(wxCommandEvent& event)
{
  wxString msg;
  msg.Printf(wxT("Hello and welcome to %s"), wxVERSION_STRING);
  wxMessageBox(msg, wxT("About Minimal"),
               wxOK | wxICON_INFORMATION, this);
}

void wx_xrc_frame::on_quit(wxCommandEvent& event)
{
  Close();
}

void wx_xrc_frame::move_point(double x, double y)
{
  point_->x = x;
  point_->y = y;
  tab_->post_redraw();
}
