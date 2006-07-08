// This is core/vgui/impl/wx/vgui_wx_window.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_window.
//
// See vgui_wx_window.h for details.
//=========================================================================

#include "vgui_wx_window.h"
#include "vgui_wx_adaptor.h"
#include "vgui_wx_menu.h"
#include "vgui_wx_statusbar.h"

#include <vgui/vgui.h>

#include <wx/frame.h>
#include <wx/statusbr.h>

#include <vcl_cassert.h>

//-------------------------------------------------------------------------
// vgui_wx_window implementation - construction & destruction.
//-------------------------------------------------------------------------
//: Constructor - create a new window.
vgui_wx_window::vgui_wx_window(int width, int height, const char* title)
  : frame_(new wxFrame(0, wxID_ANY, title))//, wxDefaultPosition, wxSize(width, height)))
  , menu_(0)
{
  init_window();
  frame_->SetClientSize(width+3, height+3);
}

//: Constructor - create a new window with a menubar.
vgui_wx_window::vgui_wx_window(int width,
                               int height,
                               const vgui_menu& menubar,
                               const char* title)
  : frame_(new wxFrame(0, wxID_ANY, title, wxDefaultPosition, wxSize(width, height)))
  , menu_(0)
{
  init_window();
  set_menubar(menubar);
}

//: Destructor.
vgui_wx_window::~vgui_wx_window(void)
{
  delete statusbar_;

  if (menu_)
  {
    frame_->PopEventHandler(true);
    // ***** delete the wxMenu, or does the frame do it?
    //       if so, I need to store a handle to it...
    // *****
  }
}

//: Catch all constructor.
void vgui_wx_window::init_window(void)
{
  adaptor_ = new vgui_wx_adaptor(frame_);

  statusbar_ = new vgui_wx_statusbar;
  statusbar_->set_widget(frame_->CreateStatusBar());

  // ***** if we support multiple windows, then i think we need to redirect
  //       vgui::out to the statusbar each time the window gets the focus
  //       (if the statusbar is enabled) and redirect it to standard output
  //       every time it loses the focus...
  // *****
  vgui::out.rdbuf(statusbar_->statusbuf());
}

//-------------------------------------------------------------------------
// vgui_wx_window implementation.
//-------------------------------------------------------------------------
void vgui_wx_window::set_menubar(const vgui_menu& menubar)
{
  if (menu_)
  {
    frame_->PopEventHandler(true);
    menu_ = 0;
    // ***** delete the wxMenu, or does the frame do it?
    //       if so, I need to store a handle to it...
    // *****
  }

  menu_ = new vgui_wx_menu;
  frame_->SetMenuBar(menu_->create_wx_menubar(menubar));
  frame_->PushEventHandler(menu_);
}

//: If true, activate the statusbar (if it exists).
void vgui_wx_window::set_statusbar(bool activate)
{
  activate ? frame_->GetStatusBar()->Show() : frame_->GetStatusBar()->Hide();
}

//: Set the default adaptor (if it exists) to the given vgui_adaptor.
// These refer to the default/current adaptor, if that makes sense. It is
// not a requirement that it should make sense.
void vgui_wx_window::set_adaptor(vgui_adaptor* adaptor)
{
  // ***** how do I replace the client canvas? like this?
  //delete adaptor_;
  frame_->RemoveChild(adaptor_);
  adaptor_ = new vgui_wx_adaptor(frame_);

  // ***** do I need to show it now?
}

//: Get the default adaptor (if it exists).
vgui_adaptor* vgui_wx_window::get_adaptor(void)
{
  return adaptor_;
}

//: Get the status bar (if it exists).
vgui_statusbar* vgui_wx_window::get_statusbar(void)
{
  return statusbar_;
}

//: If true, activate horizontal scrollbar (if it exists).
void vgui_wx_window::enable_hscrollbar(bool show)
{
  if (show)
  {
    frame_->SetScrollbar(wxHORIZONTAL, 0, 1, 100);
  }
  else
  {
    // ***** how do I hide it?
    assert(false);
  }
}

//: If true, activate vertical scrollbar (if it exists).
void vgui_wx_window::enable_vscrollbar(bool show)
{
  if (show)
  {
    frame_->SetScrollbar(wxVERTICAL, 0, 1, 100);
  }
  else
  {
    // ***** how do I hide it?
    assert(false);
  }
}

//: Change window shape to new given width and height.
void vgui_wx_window::reshape(unsigned width, unsigned height)
{
  // ***** should this resize the window, or the client area?
  //frame_->SetSize(width, height);
  frame_->SetClientSize(width, height);
}

//: Move the window to the new given x,y position.
void vgui_wx_window::reposition(int x, int y)
{
  frame_->SetSize(x, y, -1, -1);
}

//: Use the given text as the window title (if the window has a title).
void vgui_wx_window::set_title(vcl_string const& title)
{
  frame_->SetTitle(title.c_str());
}

//: Set the position of the horizontal scrollbar, returns old position.
int vgui_wx_window::set_hscrollbar(int pos)
{
  int temp = frame_->GetScrollPos(wxHORIZONTAL);
  frame_->SetScrollPos(wxHORIZONTAL, pos);
  return temp;
}

//: Set the position of the vertical scrollbar, returns old position.
int vgui_wx_window::set_vscrollbar(int pos)
{
  int temp = frame_->GetScrollPos(wxVERTICAL);
  frame_->SetScrollPos(wxVERTICAL, pos);
  return temp;
}
