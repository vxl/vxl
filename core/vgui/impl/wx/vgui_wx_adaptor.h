// This is core/vgui/impl/wx/vgui_wx_adaptor.h
#ifndef vgui_wx_adaptor_h_
#define vgui_wx_adaptor_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_adaptor.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/22/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <map>
#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>

#include <wx/docview.h>
#include <wx/glcanvas.h>
class wxMenu;

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------------------------------------
//: wxWidgets implementation of vgui_adaptor.
//
// The adaptor allows you to draw an OpenGL area in your wxWidgets
// application (i.e., embedding the adaptor in wxWidgets) or in a vgui
// application (i.e., extending the adaptor to use wxWidgets as your
// vgui_toolkit).
//-------------------------------------------------------------------------
class vgui_wx_adaptor
  : public wxGLCanvas
  , public vgui_adaptor
  , public vgui_adaptor_mixin
{
  DECLARE_CLASS(vgui_wx_adaptor)
  DECLARE_EVENT_TABLE()

  typedef vgui_adaptor_mixin mixin;

 public:
  //: Constructor - used by dynamic creation.
  vgui_wx_adaptor(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxString& name = wxT("vgui_wx_adaptor"),
                  int* attributes = 0);

  //: Destructor.
  virtual ~vgui_wx_adaptor();

  void set_view(wxView* view) { view_ = view; }

  //-----------------------------------------------------------------------
  // vgui_adaptor virtual implementations
  //-----------------------------------------------------------------------

  //: Return width of rendering area.
  virtual unsigned int get_width() const { return GetClientSize().GetWidth(); }

  //: Return height of rendering area.
  virtual unsigned int get_height() const { return GetClientSize().GetHeight(); }

  //: Redraw the rendering area.
  virtual void post_redraw();

  //: Redraws overlay buffer.
  virtual void post_overlay_redraw();

  //: Flags that a child requests idle processing.
  virtual void post_idle_request();

  //: ***** What is this for???
  virtual void post_message(char const *, void const *);

  //: Schedules destruction of parent vgui_window.
  virtual void post_destroy();

  //: Sets timer 'id' to dispatch a WM_TIMER event every 'timeout' ms.
  virtual void post_timer(float timeout, int id);

  //: Stop timer 'id'.
  virtual void kill_timer(int id);

  //: Bind the given modifier/button combination to the popup menu.
  virtual void bind_popups(vgui_modifier m, vgui_button b)
  {
    mixin::popup_modifier = m;
    mixin::popup_button   = b;
  }

  //: Return the modifier/button which displayed the popup menu.
  virtual void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
  {
    m = mixin::popup_modifier;
    b = mixin::popup_button;
  }

  //: ***** Return window that contains this adaptor.
  virtual vgui_window* get_window() const { return 0; }

  //: Swap buffers if using double buffering.
  virtual void swap_buffers();

  //: Make this the current GL rendering context.
  virtual void make_current();

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
 private:
  //: Called when canvas is resized.
  void on_size(wxSizeEvent& event);

  //: Called when a window needs to be repainted.
  void on_paint(wxPaintEvent& WXUNUSED(event));

  //: Called when the background needs erasing (i.e., before repainting).
  void on_erase_background(wxEraseEvent& WXUNUSED(event));

  //: Helper used by on_key_up/down to reduce code duplication.
  void on_key(vgui_event& ve, wxKeyEvent& event);

  //: Called when a key is pressed.
  void on_key_down(wxKeyEvent& event);

  //: Called when a key is released.
  void on_key_up(wxKeyEvent& event);

  //: Called when a key is pressed, but carries a translated ascii code.
  //
  // To catch this event after a key_down has been caught, call
  // event.skip() from the on_key_down handler. Note that the char event
  // is always generated after the key_down event in wxWidgets.
  void on_char(wxKeyEvent& event);

  //: Called for all types of mouse events (e.g., button-up, motion, etc.).
  void on_mouse_event(wxMouseEvent& event);

  //: Called when the system becomes idle.
  void on_idle(wxIdleEvent& event);

  //: Called when the user tries to close a frame or dialog box.
  // The event can be generated programmatically or when the user tries to
  // close using the window manager (X) or system menu (Windows).
  void on_close(wxCloseEvent& event);

  //: Called at fixed intervals when using a timer.
  void on_timer(wxEvent& event);

  //: Generates a wxPaintEvent for the window to be repainted.
  void invalidate_canvas();

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
 private:
  wxView* view_;

  static vgui_menu last_popup_;

  //: True while a redraw event has been requested but not implemented.
  bool redraw_posted_;

  //: True while an overlay redraw event has been requested but not implemented.
  bool overlay_redraw_posted_;

  //: True while an idle time has been requested but not implemented.
  bool idle_request_posted_;

  //: True when destruction of parent vgui_window has been scheduled.
  bool destroy_posted_;

  int last_key_down_;
  std::map<int,int> ascii_code_;
};

#endif // vgui_wx_adaptor_h_
