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
//   11/18/2019 - Modifications to eliminate gl errors and memory leaks (JLM)
// \endverbatim
//=========================================================================

#include <map>
#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>
#include <memory>
#include <wx/docview.h>
#include <wx/glcanvas.h>
#include "vgui_wx_window.h"
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
//  WxWidgets explaination of wxGLCanvas coordinates:
//  "Please note that wxGLContext always uses physical pixels, even on the platforms where
//  wxWindow uses logical pixels, affected by the coordinate scaling, on high DPI displays.
//  Thus, if you want to set the OpenGL view port to the size of entire window, you must
//  multiply the result returned by wxWindow::GetClientSize() by wxWindow::GetContentScaleFactor()
//  before passing it to glViewport(). Same considerations apply to other OpenGL functions and
//  other coordinates, notably those retrieved from wxMouseEvent in the event
//
//  https://github.com/wxWidgets/wxWidgets/blob/master/docs/changes.txt
//    wxGLCanvas now uses physical pixels on high DPI displays under platforms
//    where they're different from logical ones (wxGTK3, wxOSX). Multiply logical
//    coordinates, e.g. returned by wxWindow::GetSize() by GetContentScaleFactor()
//    before using them with OpenGL functions.
//
//  On Windows, the dpi scaling is handled after rendering by the windows manager, so no extra
//  correction is needed. On Linux with wxWidgets+GTK and wxWidgets+macos version 3+ it is
//  necessary to apply the dpi scale factor wherever the viewport screen coordinate system
//  (gl canvas) is utilized. Other wx backends do not support dpi scaling.
//
//  The solution is to keep all the math in units of logical pixels, and to be sure to convert
//  to and from physical pixels when needed. There are only a few commands that need to take
//  this into considerations, they include (but are not limited to):
//
//  - glCopyPixels (uses glPixelZoom)
//  - glDrawPixels (uses glPixelZoom)
//  - glReadPixels
//  - glBitmap
//  - glLineWidth
//  - glPointSize
//  - glScissor
//  - glViewport
//  - Reading:
//    - GL_VIEWPORT
//    - GL_SCISSOR_BOX
//    - GL_MAX_VIEWPORT_DIMS
//
// For this reason, these functions (except gl*Pixels) are wrapped in functions in vgui_utils
// that will automatically apply the scale factors needed to them, so that they behave as if
// you are in logical pixels. By multiplying glPixelZoom by the scale factor, glCopyPixels
// and glDrawPixels will be in logical pixels. glReadPixels is always in physical pixels and
// is handled manually and not used very often. Functions like glBitmap and glReadPixels
// can only ever operate in physical pixels, and used vil_images to resample to behave as if
// they are logical pixels.

static int wx_adaptor_args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
class vgui_wx_adaptor
  : public wxGLCanvas
  , public vgui_adaptor
  , public vgui_adaptor_mixin
{
  DECLARE_CLASS(vgui_wx_adaptor)
  DECLARE_EVENT_TABLE()

  typedef vgui_adaptor_mixin mixin;

public:
  // interface for 3.0 replaces 2_8
  vgui_wx_adaptor(wxWindow * parent,
                  wxWindowID id = wxID_ANY,
                  const int * args = wx_adaptor_args,
                  const wxPoint & pos = wxDefaultPosition,
                  const wxSize & size = wxDefaultSize,
                  long style = 0,
                  const wxString & name = wxGLCanvasName,
                  const wxPalette & palette = wxNullPalette);

  //: Constructor - used by dynamic creation. version 2_8
  /*
   vgui_wx_adaptor(wxWindow* parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxT("vgui_wx_adaptor"),
                    int* attributes = 0);
  */
  //: Destructor.
  virtual ~vgui_wx_adaptor();

  //-----------------------------------------------------------------------
  // vgui_adaptor virtual implementations
  //-----------------------------------------------------------------------

  //: Return width of rendering area.
  virtual unsigned int
  get_width() const
  {
    return GetClientSize().GetWidth();
  }

  //: Return height of rendering area.
  virtual unsigned int
  get_height() const
  {
    return GetClientSize().GetHeight();
  }

  virtual double
  get_scale_factor() const;

  //: Redraw the rendering area.
  virtual void
  post_redraw();

  //: Redraws overlay buffer.
  virtual void
  post_overlay_redraw();

  //: Flags that a child requests idle processing.
  virtual void
  post_idle_request();

  //: ***** What is this for???
  virtual void
  post_message(const char *, const void *);

  //: Schedules destruction of parent vgui_window.
  virtual void
  post_destroy();

  //: Sets timer 'id' to dispatch a WM_TIMER event every 'timeout' ms.
  virtual void
  post_timer(float timeout, int id);

  //: Stop timer 'id'.
  virtual void
  kill_timer(int id);

  //: Bind the given modifier/button combination to the popup menu.
  virtual void
  bind_popups(vgui_modifier m, vgui_button b)
  {
    mixin::popup_modifier = m;
    mixin::popup_button = b;
  }

  //: Return the modifier/button which displayed the popup menu.
  virtual void
  get_popup_bindings(vgui_modifier & m, vgui_button & b) const
  {
    m = mixin::popup_modifier;
    b = mixin::popup_button;
  }
  //: ***** Set the window that constructed this
  void
  set_window(vgui_wx_window * w)
  {
    window_ = w;
  }
  //: ***** Return window that contains this adaptor.
  virtual vgui_window *
  get_window() const
  {
    return window_;
  }

  //: Swap buffers if using double buffering.
  virtual void
  swap_buffers();

  //: Make this the current GL rendering context.
  virtual void
  make_current();

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
private:
  //: Called when canvas is resized.
  void
  on_size(wxSizeEvent & event);

  //: Called when a window needs to be repainted.
  void
  on_paint(wxPaintEvent & WXUNUSED(event));

  //: Called when the background needs erasing (i.e., before repainting).
  void
  on_erase_background(wxEraseEvent & WXUNUSED(event));

  //: Helper used by on_key_up/down to reduce code duplication.
  void
  on_key(vgui_event & ve, wxKeyEvent & event);

  //: Called when a key is pressed.
  void
  on_key_down(wxKeyEvent & event);

  //: Called when a key is released.
  void
  on_key_up(wxKeyEvent & event);

  //: Called when a key is pressed, but carries a translated ascii code.
  //
  // To catch this event after a key_down has been caught, call
  // event.skip() from the on_key_down handler. Note that the char event
  // is always generated after the key_down event in wxWidgets.
  void
  on_char(wxKeyEvent & event);

  //: Called for all types of mouse events (e.g., button-up, motion, etc.).
  void
  on_mouse_event(wxMouseEvent & event);

  //: Called when the system becomes idle.
  void
  on_idle(wxIdleEvent & event);

  //: Called when the user tries to close a frame or dialog box.
  // The event can be generated programmatically or when the user tries to
  // close using the window manager (X) or system menu (Windows).
  void
  on_close(wxCloseEvent & event);

  //: Called at fixed intervals when using a timer.
  void
  on_timer(wxEvent & event);

  //: Generates a wxPaintEvent for the window to be repainted.
  //  == not implemented ===
  void
  invalidate_canvas();

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
private:
  std::shared_ptr<wxGLContext> context_;
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
  std::map<int, int> ascii_code_;

  vgui_wx_window * window_;
};

#endif // vgui_wx_adaptor_h_
