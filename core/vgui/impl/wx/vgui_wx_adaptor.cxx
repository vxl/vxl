// This is core/vgui/impl/wx/vgui_wx_adaptor.cxx
#include <iostream>
#include <map>
#include "vgui_wx_adaptor.h"
#include "vgui_wx_window.h"
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_adaptor.
//
// See vgui_wx_adaptor.h for details.
//=========================================================================

#include "vgui_wx_menu.h"
#include "vgui/vgui_macro.h"
#include "vgui/vgui_popup_params.h"

#include <wx/log.h>

#include <wx/menu.h>
#include <wx/timer.h>
#include <wx/dcclient.h>

#ifndef wxEventHandler // wxWidgets-2.5.3 doesn't define this
#  define wxEventHandler(func) (wxObjectEventFunction) wxStaticCastEvent(wxEventFunction, &func)
#endif
#ifdef __WXMSW__
#  include <wx/msw/msvcrt.h>
#endif
#include <cassert>

// not used? #include <vcl_compiler.h>
//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
//: Event type for dynamic timer events.
// TODO timer implementation is incomplete (JLM)
const wxEventType wxEVT_VGUI_TIMER = wxNewEventType();

inline bool
is_modifier(int key_code);
inline vgui_modifier
get_modifiers(const wxMouseEvent & e);
inline vgui_modifier
get_modifiers(const wxKeyEvent & e);
inline vgui_event_type
translate_mouse_event_type(const wxMouseEvent & e);
inline vgui_button
translate_mouse_button(int button);
inline vgui_key
translate_key(int key_code);
} // namespace

//-------------------------------------------------------------------------
// vgui_wx_adaptor implementation - construction & destruction.
//-------------------------------------------------------------------------
IMPLEMENT_CLASS(vgui_wx_adaptor, wxGLCanvas)
// Enables wxWidgets runtime typing mechanism - supplanted now by C++ itself

//: ***** To ensure the commands stay in scope for the lifetime of the popup.
vgui_menu vgui_wx_adaptor::last_popup_;

// interface for 3.0
vgui_wx_adaptor::vgui_wx_adaptor(wxWindow * parent,
                                 wxWindowID id,
                                 const int * attributes,
                                 const wxPoint & pos,
                                 const wxSize & size,
                                 long style,
                                 const wxString & name,
                                 const wxPalette & palette)
  : wxGLCanvas(parent, id, attributes, pos, size, style | wxFULL_REPAINT_ON_RESIZE | wxBORDER_SUNKEN, name)
  , redraw_posted_(true)
  , overlay_redraw_posted_(true)
  , idle_request_posted_(false)
  , destroy_posted_(false)
  , window_(nullptr)
{

  wxLogTrace(wxTRACE_RefCount, wxT("vgui_wx_adaptor::vgui_wx_adaptor"));
  // 3.0 requires the gl context explictly in calls such as SetCurrent(context)
  context_ = std::shared_ptr<wxGLContext>(new wxGLContext(this));
}
/* Constructor. deprecated 2.8 form
vgui_wx_adaptor::vgui_wx_adaptor(wxWindow * parent,
                                 wxWindowID id,
                                 const wxPoint & pos,
                                 const wxSize & size,
                                 long style,
                                 const wxString & name,
                                 int * attributes)
  : wxGLCanvas(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE | wxBORDER_SUNKEN, name, attributes)
  , view_(0)
  , redraw_posted_(true)
  , overlay_redraw_posted_(true)
  , idle_request_posted_(false)
  , destroy_posted_(false)
{
  wxLogTrace(wxTRACE_RefCount, wxT("vgui_wx_adaptor::vgui_wx_adaptor"));
}
*/
//: Destructor.
vgui_wx_adaptor::~vgui_wx_adaptor()
{
  wxWindow * win = this->GetParent();
  wxEvtHandler * hnd = win->GetEventHandler();
  bool hand_neq_win = hnd != win;
  if (hand_neq_win) // can't remove self as event handler
    win->PopEventHandler(true);
  wxLogTrace(wxTRACE_RefCount, wxT("vgui_wx_adaptor::~vgui_wx_adaptor"));
}

//-------------------------------------------------------------------------
// vgui_wx_adaptor implementation - virtual functions from vgui_adaptor.
//-------------------------------------------------------------------------

double
vgui_wx_adaptor::get_scale_factor() const
{
#if defined(__WXX11__) || defined(__WXMSW__)
  // For some reason, X11 wx gives a scale factor that is not related to
  // the logical/physical pixel difference between glViewport and wxGLCanvas
  return 1.0;
#else
  return GetContentScaleFactor();
#endif
}

//: Redraw the rendering area.
void
vgui_wx_adaptor::post_redraw(void)
{
  if (!redraw_posted_)
  {
    redraw_posted_ = true;
    invalidate_canvas();
  }
}

//: Redraws overlay buffer.
void
vgui_wx_adaptor::post_overlay_redraw(void)
{
  if (!overlay_redraw_posted_)
  {
    overlay_redraw_posted_ = true;
    invalidate_canvas();
  }
}

//: Flags that a child requests idle processing.
void
vgui_wx_adaptor::post_idle_request(void)
{
  if (!idle_request_posted_)
  {
    idle_request_posted_ = true;
    /*This function wakes up the (internal and platform dependent) idle system, i.e.it will force the system
      to send an idle event even if the system currently is idle and thus would not send any idle event
      until after some other event would get sent.*/
    wxWakeUpIdle();
  }
}


//: ***** What is this for???
void
vgui_wx_adaptor::post_message(const char *, const void *)
{
  assert(false);
}

//: Schedules destruction of parent vgui_window.
void
vgui_wx_adaptor::post_destroy(void)
{
  // ***** forget about any posted redraws
  redraw_posted_ = false;
  overlay_redraw_posted_ = false;

  vgui_macro_report_errors;

  if (!destroy_posted_)
  {
    destroy_posted_ = true;
    Close();
  }

  vgui_macro_report_errors;
}
/*
  In 3.0 the better approach is to use Bind/Unbind as in:
  m_timer->Bind(wxEVT_TIMER, &callback_function, &adaptor, id);
  It will be necessary to maintain a set of wxTimer(s) for each id.
  As each timer times out the corresponding vgui_event is dispatched.
  Note timer implementation is incomplete - FIXME (JLM)
 */
//: Sets timer 'id' to dispatch a vgui_TIMER event every 'timeout' ms.
void
vgui_wx_adaptor::post_timer(float timeout, int id)
{

  Connect(id, wxEVT_VGUI_TIMER, wxEventHandler(vgui_wx_adaptor::on_timer));
}

//: Stop timer 'id'.
void
vgui_wx_adaptor::kill_timer(int id)
{
  Disconnect(id, wxEVT_VGUI_TIMER);
}

//: Swap buffers if using double buffering.
void
vgui_wx_adaptor::swap_buffers()
{
  SwapBuffers();
}

//: Make this the current GL rendering context.
/*
  3.0 requires a context, e.g.,
  wxGLContext* m_context; a member of adaptor
  wxGLCanvas::SetCurrent(*m_context);
 */
void
vgui_wx_adaptor::make_current()
{
  SetCurrent(*context_);
}

//-------------------------------------------------------------------------
// vgui_wx_adaptor implementation - event handling.
//-------------------------------------------------------------------------
BEGIN_EVENT_TABLE(vgui_wx_adaptor, wxGLCanvas)
EVT_SIZE(vgui_wx_adaptor::on_size)
EVT_PAINT(vgui_wx_adaptor::on_paint)
EVT_ERASE_BACKGROUND(vgui_wx_adaptor::on_erase_background)
EVT_KEY_DOWN(vgui_wx_adaptor::on_key_down)
EVT_KEY_UP(vgui_wx_adaptor::on_key_up)
EVT_CHAR(vgui_wx_adaptor::on_char)
EVT_MOUSE_EVENTS(vgui_wx_adaptor::on_mouse_event)
EVT_IDLE(vgui_wx_adaptor::on_idle)
EVT_CLOSE(vgui_wx_adaptor::on_close)
END_EVENT_TABLE()

//: Called when canvas is resized.
/* wx doc on Update()
Calling this method immediately repaints the invalidated area of the window and all of its children recursively
(this normally only happens when the flow of control returns to the event loop).
Notice that this function doesn't invalidate any area of the window so nothing happens if nothing has been
invalidated (i.e. marked as requiring a redraw).
Use Refresh() first if you want to immediately redraw the window unconditionally.
*/
void
vgui_wx_adaptor::on_size(wxSizeEvent & event)
{
// c/o doublemax https://forums.wxwidgets.org/viewtopic.php?t=29948
// And reading glx11.cpp's SetCurrent and msw's SetCurrent
// IsShown SHOULD have worked https://trac.wxwidgets.org/ticket/4343
// Surprise! it didn't
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
  if (!GetXWindow() || !IsShown())
    return;
#elif defined(__WXMSW__)
  if (!GetHDC() || !IsShown())
    return;
#endif

  SetCurrent(*context_); // necesssary to insure the canvas is valid
  dispatch_to_tableau(vgui_RESHAPE);
  post_redraw();
  Update();
}

//: Called when a window needs to be repainted.
void
vgui_wx_adaptor::on_paint(wxPaintEvent & WXUNUSED(event))
{
  vgui_macro_report_errors;
  // must always be here
  // wxPaintDC dc(this); //previous call

  /*Do not call the derived class' paint function from any function in your code... ever.
    If you need to redraw the GL canvas, call my_canvas_window->Refresh();,
    which will send a new paint event to the window.(https://forums.wxwidgets.org/viewtopic.php?t=21513)*/
  this->Refresh();

#ifndef __WXMOTIF__
//   if (!GetContext())
//   {
//     return;
//   }
#endif

  SetCurrent(*context_);

  if (redraw_posted_ || overlay_redraw_posted_)
  {
    dispatch_to_tableau(vgui_DRAW);
    redraw_posted_ = false;
  }

  if (overlay_redraw_posted_)
  {
    dispatch_to_tableau(vgui_DRAW_OVERLAY);
    overlay_redraw_posted_ = false;
  }

  SwapBuffers();
  vgui_macro_report_errors;
}

//: Called when the background needs erasing (i.e., before repainting).
void
vgui_wx_adaptor::on_erase_background(wxEraseEvent & WXUNUSED(event))
{
  vgui_macro_report_errors;
  // do nothing (avoids flickering in wxMSW)
}

//: Helper used by on_key_up/down to reduce code duplication.
void
vgui_wx_adaptor::on_key(vgui_event & ve, wxKeyEvent & event)
{
  ve.origin = this;

  ve.ascii_char = translate_key(event.GetKeyCode());
  ve.modifier = get_modifiers(event);
  // ***** this should be handled in vgui_event::set_key(ascii,bool)
  //       ve.set_key(ve.ascii_char, ve.modifier != vgui_MODIFIER_NULL)
  // or    ve.set_key(ve.ascii_char, ve.modifier)
  if (0 < ve.ascii_char && ve.ascii_char < 27 && ve.modifier_is_down(vgui_CTRL))
  {
    ve.key = static_cast<vgui_key>(ve.ascii_char + 'a' - 1);
  }
  else
  {
    ve.set_key(ve.ascii_char);
  }

#if 0
  if (ve.type == vgui_KEY_PRESS)
  {
    std::cout << "key (wx, vgui): " << ve.key << '\t' << ve.ascii_char
             << '\t' << ve.modifier << std::endl;
  }
#endif

  // ***** what should I return here?? What about if scrolled window??
  ve.wx = event.GetX();
  ve.wy = get_height() - event.GetY();

  // ***** what exactly goes here??
  ve.timestamp = 0;

  if (!dispatch_to_tableau(ve))
  {
    event.Skip();
  }
  // invalidate_canvas();
}

//: Called when a key is pressed.
void
vgui_wx_adaptor::on_key_down(wxKeyEvent & event)
{
  // if it's a modifier event let it propagate
  if (is_modifier(event.GetKeyCode()))
  {
    event.Skip();
    return;
  }

#if 0
  std::cout << "EVT_KEY_DOWN: " << event.GetKeyCode() << std::endl;
#endif

  // save the code and let the event propagate to the on_char handler
  last_key_down_ = event.GetKeyCode();
  event.Skip();

  // vgui_event e(vgui_KEY_PRESS);
  // on_key(e, event);
}

//: Called when a key is released.
void
vgui_wx_adaptor::on_key_up(wxKeyEvent & event)
{
  if (is_modifier(event.GetKeyCode()))
  {
    event.Skip();
    return;
  }

#if 0
  std::cout << "EVT_KEY_UP  : " << event.GetKeyCode()
           << " (" << ascii_code_[event.GetKeyCode()] << ')' << std::endl;
#endif

  // get the ascii char of the last key down with same code
  event.m_keyCode = ascii_code_[event.GetKeyCode()];

  vgui_event e(vgui_KEY_RELEASE);
  on_key(e, event);
}

//: Called when a key is pressed, but carries a translated ascii code.
//
// To catch this event after a key_down has been caught, call
// event.skip() from the on_key_down handler. Note that the char event
// is always generated after the key_down event in wxWidgets.
void
vgui_wx_adaptor::on_char(wxKeyEvent & event)
{
  if (is_modifier(event.GetKeyCode()))
  {
    event.Skip();
    return;
  }

#if 0
  std::cout << "EVT_CHAR    : " << last_key_down_
           << " (" << event.GetKeyCode() << ')' << std::endl;
#endif

  // save ascii code of last on_key_down event for on_key_up to use
  ascii_code_[last_key_down_] = event.GetKeyCode();

  vgui_event e(vgui_KEY_PRESS);
  on_key(e, event);
}

//: Called for all types of mouse events (e.g., button-up, motion, etc.).
void
vgui_wx_adaptor::on_mouse_event(wxMouseEvent & event)
{
  vgui_macro_report_errors;
  vgui_event e;

  e.origin = this;
  e.modifier = get_modifiers(event);
  e.button = translate_mouse_button(event.GetButton());
  e.type = translate_mouse_event_type(event);


  // wxWidgets on Linux returns mouse position in logical pixel coordinates
  // not gl canvas screen coordinates. On Windows, the mouse position does
  // take into account dpi scale on high resolution screens, so vgui::dpi_scale_
  // is set to 1, while on Linux it is set to ::GetContentScaleFactor().
  e.wx = event.GetX();
  e.wy = get_height() - event.GetY(); // needed to make (0,0) lower left as a Cartesian system

  // ***** what exactly goes here??
  e.timestamp = 0;

  if (e.modifier == mixin::popup_modifier && e.button == mixin::popup_button)
  {
    vgui_macro_report_errors;
    vgui_popup_params params;
    params.x = e.wx;
    params.y = e.wy;

    vgui_macro_report_errors;
    // ***** why can't last_popup be local??
    last_popup_ = get_total_popup(params);

    vgui_macro_report_errors;
    // present the popup menu
    vgui_wx_menu popup;
    wxMenu * menu = popup.create_wx_menu(last_popup_);
    vgui_macro_report_errors;
    PushEventHandler(&popup);
    PopupMenu(menu);
    PopEventHandler();
    vgui_macro_report_errors;
    delete menu;

    vgui_macro_report_errors;
    invalidate_canvas();
    vgui_macro_report_errors;
    return;
  }

  // ***** Grab Mouse (ENTER/LEAVE)
  if (e.button == vgui_LEFT)
  {
    if (e.type == vgui_BUTTON_DOWN)
    {
      CaptureMouse();
    }
    else if (e.type == vgui_BUTTON_UP)
    {
      ReleaseMouse();
    }
  }

  if (!dispatch_to_tableau(e))
  {
    event.Skip();
  }
  // invalidate_canvas();
  vgui_macro_report_errors;
}

//: Called when the system becomes idle.
void
vgui_wx_adaptor::on_idle(wxIdleEvent & event)
{
  if (idle_request_posted_)
  {
    idle_request_posted_ = dispatch_to_tableau(vgui_IDLE);
    event.RequestMore(idle_request_posted_);
    // post_redraw();
    // invalidate_canvas();
  }

  event.Skip();
}

//: Called when the user tries to close a frame or dialog box.
// The event can be generated programmatically or when the user tries to
// close using the window manager (X) or system menu (Windows).
void
vgui_wx_adaptor::on_close(wxCloseEvent & event)
{
  // apparently everything done to close is taken care of by this call
  // keep callback in case something comes up
}

//: Called at fixed intervals when using a timer.
void
vgui_wx_adaptor::on_timer(wxEvent & event)
{
  vgui_event e(vgui_TIMER);
  e.origin = this;
  e.timer_id = event.GetId();
  dispatch_to_tableau(e);
}

//: Generates a wxPaintEvent for the window to be repainted.
void
vgui_wx_adaptor::invalidate_canvas(void)
{
  Refresh();
}

//-------------------------------------------------------------------------
// Private helpers - definitions.
//-------------------------------------------------------------------------
namespace
{
inline bool
is_modifier(int key_code)
{
  switch (key_code)
  {
    case WXK_SHIFT:
    case WXK_ALT:
    case WXK_CONTROL:
      // case WXK_META: // ***** which one is the meta key??
      return true;
    default:
      return false;
  }
}

// ***** check if design allows for multiple modifiers... if so, this needs to change
inline vgui_modifier
get_modifiers(const wxMouseEvent & e)
{
  int mod = 0;
  if (e.ShiftDown())
  {
    mod |= vgui_SHIFT;
  }
  if (e.AltDown())
  {
    mod |= vgui_ALT;
  }
#ifdef __WXMAC__
  // Swap META(CMD) and CTRL on Mac because this is closer to native behavior
  // and CTRL-Left-Click triggers a Right-Click which interferes with some
  // built-in vgui controls
  if (e.MetaDown())
  {
    mod |= vgui_CTRL;
  }
  if (e.ControlDown())
  {
    mod |= vgui_META;
  }
#else
  if (e.ControlDown())
  {
    mod |= vgui_CTRL;
  }
  if (e.MetaDown())
  {
    mod |= vgui_META;
  }
#endif
  return static_cast<vgui_modifier>(mod);
}

// ***** check if design allows for multiple modifiers... if so, this needs to change
inline vgui_modifier
get_modifiers(const wxKeyEvent & e)
{
  int mod = 0;
  if (e.ShiftDown())
  {
    mod |= vgui_SHIFT;
  }
  if (e.AltDown())
  {
    mod |= vgui_ALT;
  }
#ifdef __WXMAC__
  // Swap META(CMD) and CTRL on Mac because this is closer to native behavior
  // and CTRL-Left-Click triggers a Right-Click which interferes with some
  // built-in vgui controls
  if (e.MetaDown())
  {
    mod |= vgui_CTRL;
  }
  if (e.ControlDown())
  {
    mod |= vgui_META;
  }
#else
  if (e.ControlDown())
  {
    mod |= vgui_CTRL;
  }
  if (e.MetaDown())
  {
    mod |= vgui_META;
  }
#endif
  return static_cast<vgui_modifier>(mod);
}

inline vgui_event_type
translate_mouse_event_type(const wxMouseEvent & e)
{
  if (e.Moving() || e.Dragging())
  {
    return vgui_MOTION;
  }
  else if (e.ButtonDown() || e.ButtonDClick())
  {
    return vgui_BUTTON_DOWN;
  }
  else if (e.ButtonUp())
  {
    return vgui_BUTTON_UP;
  }
  else if (e.Entering())
  {
    return vgui_ENTER;
  }
  else if (e.Leaving())
  {
    return vgui_LEAVE;
  }
  else if (e.GetEventType() == wxEVT_MOUSEWHEEL)
  {
    if (e.GetWheelRotation() > 0)
    {
      return vgui_WHEEL_UP;
    }
    else
    {
      return vgui_WHEEL_DOWN;
    }
  }
  else
  {
    return vgui_OTHER;
  }
}

inline vgui_button
translate_mouse_button(int button)
{
  switch (button)
  {
    case wxMOUSE_BTN_NONE:
      return vgui_BUTTON_NULL;
    case wxMOUSE_BTN_LEFT:
      return vgui_LEFT;
    case wxMOUSE_BTN_MIDDLE:
      return vgui_MIDDLE;
    case wxMOUSE_BTN_RIGHT:
      return vgui_RIGHT;
    default:
      std::cerr << "VGUI wx Error: Unknown button identifier.\n";
      return vgui_BUTTON_NULL;
  }
}

inline vgui_key
translate_key(int key_code)
{
  switch (key_code)
  {
    case WXK_ESCAPE:
      return vgui_ESC;
    case WXK_TAB:
      return vgui_TAB;
    case WXK_RETURN:
      return vgui_RETURN;
    case '\n':
      return vgui_NEWLINE;

    case WXK_F1:
      return vgui_F1;
    case WXK_F2:
      return vgui_F2;
    case WXK_F3:
      return vgui_F3;
    case WXK_F4:
      return vgui_F4;
    case WXK_F5:
      return vgui_F5;
    case WXK_F6:
      return vgui_F6;
    case WXK_F7:
      return vgui_F7;
    case WXK_F8:
      return vgui_F8;
    case WXK_F9:
      return vgui_F9;
    case WXK_F10:
      return vgui_F10;
    case WXK_F11:
      return vgui_F11;
    case WXK_F12:
      return vgui_F12;
    case WXK_LEFT:
      return vgui_CURSOR_LEFT;
    case WXK_UP:
      return vgui_CURSOR_UP;
    case WXK_RIGHT:
      return vgui_CURSOR_RIGHT;
    case WXK_DOWN:
      return vgui_CURSOR_DOWN;
      //     case WXK_PRIOR:
      //       return vgui_PAGE_UP;
      //     case WXK_NEXT:
      //       return vgui_PAGE_DOWN;
    case WXK_PAGEUP:
      return vgui_PAGE_UP; // ***** ??
    case WXK_PAGEDOWN:
      return vgui_PAGE_DOWN; // ***** ??
    case WXK_HOME:
      return vgui_HOME;
    case WXK_END:
      return vgui_END;
    case WXK_DELETE:
      return vgui_DELETE;
    case WXK_INSERT:
      return vgui_INSERT;
    default:
      if (0 < key_code && key_code < 256) // it's an ascii code
      {
        return static_cast<vgui_key>(key_code);
      }
      else
      {
#ifdef __WXDEBUG__
        std::cerr << "VGUI wx Error: Unknown key code.\n";
#endif
        return vgui_KEY_NULL;
      }
  }
}
} // unnamed namespace
