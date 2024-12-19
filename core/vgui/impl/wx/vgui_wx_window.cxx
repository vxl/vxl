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
#include "vgui.h"
#include "vgui/vgui.h"
#include <wx/frame.h>
#include <wx/statusbr.h>

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#ifdef __WXMSW__
#  include <wx/msw/msvcrt.h>
#endif
//-------------------------------------------------------------------------
// vgui_wx_window implementation - construction & destruction.
//-------------------------------------------------------------------------
// On high resolution screens with dots per inch larger than 96 it is necessarty
// to scale the size of the window accoringly. That is if one wants a 100x100 window on
// the screen and the dpi_scale factor is 2 then a 200x 200 window should be created.
// the dpi scale is returned by wxWidgets by calling GetContentScaleFactor()
//
//: Constructor - create a new window.
vgui_wx_window::vgui_wx_window(int width, int height, const char * title)
  : wxFrame((wxFrame *)nullptr, wxID_ANY, wxString(title, wxConvUTF8)) //, wxDefaultPosition, wxSize(width, height)))
  , menu_(0)
{
  this->SetWindowStyle(this->GetWindowStyle() | wxFULL_REPAINT_ON_RESIZE);
  wxSize sz(width + 3, height + 3);
  this->SetClientSize(sz);
  init_window();
}

//: Constructor - create a new window with a menubar.
vgui_wx_window::vgui_wx_window(int width, int height, const vgui_menu & menubar, const char * title)
  : wxFrame((wxFrame *)nullptr, wxID_ANY, wxString(title, wxConvUTF8)) //, wxDefaultPosition, wxSize(width, height))
  , menu_(0)
{
  wxSize sz(width, height);
  this->SetClientSize(sz);
  this->SetWindowStyle(this->GetWindowStyle() | wxFULL_REPAINT_ON_RESIZE);
  set_menubar(menubar);
  init_window();
}

//: Destructor.
vgui_wx_window::~vgui_wx_window(void)
{
  // delete statusbar_;
  adaptor_->Destroy();
}

//: Catch all constructor.
void
vgui_wx_window::init_window(void)
{
  adaptor_ = new vgui_wx_adaptor(this);
  adaptor_->set_window(nullptr); // stop scroll errors
  statusbar_ = std::shared_ptr<vgui_wx_statusbar>(new vgui_wx_statusbar);
  statusbar_->set_widget(CreateStatusBar());

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
void
vgui_wx_window::set_menubar(const vgui_menu & menubar)
{
  if (menu_)
  {
    this->PopEventHandler(true);
    menu_ = 0;
    // ***** delete the wxMenu, or does the frame do it?
    //       if so, I need to store a handle to it...
    // *****
  }

  menu_ = new vgui_wx_menu;
  this->SetMenuBar(menu_->create_wx_menubar(menubar));
  this->PushEventHandler(menu_);
}

//: If true, activate the statusbar (if it exists).
void
vgui_wx_window::set_statusbar(bool activate)
{
  activate ? this->GetStatusBar()->Show() : this->GetStatusBar()->Hide();
}

//: Set the default adaptor (if it exists) to the given vgui_adaptor.
// These refer to the default/current adaptor, if that makes sense. It is
// not a requirement that it should make sense.
void
vgui_wx_window::set_adaptor(vgui_adaptor * adaptor)
{
  // ***** how do I replace the client canvas? like this?
  // delete adaptor_;
  this->RemoveChild(adaptor_);
  adaptor_ = new vgui_wx_adaptor(this);

  // ***** do I need to show it now?
}

//: Get the default adaptor (if it exists).
vgui_adaptor *
vgui_wx_window::get_adaptor(void)
{
  return adaptor_;
}

//: Get the status bar (if it exists).
vgui_statusbar *
vgui_wx_window::get_statusbar(void)
{
  // only called to write to statusbar so returning ptr is ok
  return statusbar_.get();
}

//: If true, activate horizontal scrollbar (if it exists).
void
vgui_wx_window::enable_hscrollbar(bool show)
{
  if (show)
  {
    this->SetScrollbar(wxHORIZONTAL, 0, 1, 100);
  }
  else
  {
    // ***** how do I hide it?
    assert(false);
  }
}

//: If true, activate vertical scrollbar (if it exists).
void
vgui_wx_window::enable_vscrollbar(bool show)
{
  if (show)
  {
    this->SetScrollbar(wxVERTICAL, 0, 1, 100);
  }
  else
  {
    // ***** how do I hide it?
    assert(false);
  }
}

//: Change window shape to new given width and height.
void
vgui_wx_window::reshape(unsigned width, unsigned height)
{
  // ***** should this resize the window, or the client area?
  // frame_->SetSize(width, height);
  this->SetClientSize(width, height);
}

//: Move the window to the new given x,y position.
void
vgui_wx_window::reposition(int x, int y)
{
  this->SetSize(x, y, -1, -1);
}

//: Use the given text as the window title (if the window has a title).
void
vgui_wx_window::set_title(const std::string & title)
{
  this->SetTitle(wxString(title.c_str(), wxConvUTF8));
}

//: Set the position of the horizontal scrollbar, returns old position.
int
vgui_wx_window::set_hscrollbar(int pos)
{
  int temp = this->GetScrollPos(wxHORIZONTAL);
  this->SetScrollPos(wxHORIZONTAL, pos);
  return temp;
}

//: Set the position of the vertical scrollbar, returns old position.
int
vgui_wx_window::set_vscrollbar(int pos)
{
  int temp = this->GetScrollPos(wxVERTICAL);
  this->SetScrollPos(wxVERTICAL, pos);
  return temp;
}
void
vgui_wx_window::add_close_event()
{
  wxCloseEvent e;
  this->AddPendingEvent(e);
}
