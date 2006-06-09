// This is core/vgui/impl/wx/vgui_wx_adaptor.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_adaptor.
//
// See vgui_wx_adaptor.h for details.
//=========================================================================

#include "vgui_wx_adaptor.h"
#include "vgui_wx_menu.h"
#include <vgui/vgui_macro.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_popup_params.h>

#include <wx/menu.h>
#include <wx/timer.h>
#include <wx/dcclient.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_map.h>

//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
  //: Event type for dynamic timer events.
  const wxEventType wxEVT_VGUI_TIMER = wxNewEventType();

  inline bool is_modifier(int key_code);
  inline vgui_modifier get_modifiers(const wxMouseEvent& e);
  inline vgui_modifier get_modifiers(const wxKeyEvent& e);
  inline vgui_event_type translate_mouse_event_type(const wxMouseEvent& e);
  inline vgui_button translate_mouse_button(int button);
  inline vgui_key translate_key(int key_code);
}

//-------------------------------------------------------------------------
// vgui_wx_adaptor implementation - construction & destruction.
//-------------------------------------------------------------------------
//IMPLEMENT_DYNAMIC_CLASS(vgui_wx_adaptor, wxGLCanvas)

//: ***** To ensure the commands stay in scope for the lifetime of the popup.
vgui_menu vgui_wx_adaptor::last_popup_;

//: Constructor.
vgui_wx_adaptor::vgui_wx_adaptor(wxWindow* parent,
                                 wxWindowID id,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name,
                                 int* attributes)
  : wxGLCanvas(parent, id, pos, size,
               style|wxFULL_REPAINT_ON_RESIZE|wxBORDER_SUNKEN, name, attributes)
  , view_(0)
  , redraw_posted_(true)
  , overlay_redraw_posted_(true)
  , idle_request_posted_(false)
  , destroy_posted_(false)
{
}

//: Destructor.
vgui_wx_adaptor::~vgui_wx_adaptor()
{
}

//-------------------------------------------------------------------------
// vgui_wx_adaptor implementation - virtual functions from vgui_adaptor.
//-------------------------------------------------------------------------
//: Redraw the rendering area.
void vgui_wx_adaptor::post_redraw(void)
{
  if (!redraw_posted_)
  {
    redraw_posted_ = true;
    invalidate_canvas();
  }
}

//: Redraws overlay buffer.
void vgui_wx_adaptor::post_overlay_redraw(void)
{
  if (!overlay_redraw_posted_)
  {
    overlay_redraw_posted_ = true;
    invalidate_canvas();
  }
}

//: Flags that a child requests idle processing.
void vgui_wx_adaptor::post_idle_request(void)
{
  if (!idle_request_posted_)
  {
    idle_request_posted_ = true;
    wxWakeUpIdle();
  }
}


//: ***** What is this for???
void vgui_wx_adaptor::post_message(char const *, void const *)
{
  assert(false);
}

//: Schedules destruction of parent vgui_window.
void vgui_wx_adaptor::post_destroy(void)
{
  vgui_macro_report_errors;
  if (!destroy_posted_)
  {
    destroy_posted_ = true;
    Close();
  }
  //if (view_)
  //{
  //  view_->GetDocument()->DeleteAllViews();
  //}
  //else
  //{
  //  // ***** or should I call Destroy() ???
  //  GetParent()->Close();
  //}
  //destroy_posted_ = true;

  // ***** forget about any posted redraws
  redraw_posted_ = false;
  overlay_redraw_posted_ = false;
  vgui_macro_report_errors;
}

//: Sets timer 'id' to dispatch a vgui_TIMER event every 'timeout' ms.
void vgui_wx_adaptor::post_timer(float timeout, int id)
{
  Connect(id, wxEVT_VGUI_TIMER, wxEventHandler(vgui_wx_adaptor::on_timer));
}

//: Stop timer 'id'.
void vgui_wx_adaptor::kill_timer(int id)
{
  Disconnect(id, wxEVT_VGUI_TIMER);
}

//: Swap buffers if using double buffering.
void vgui_wx_adaptor::swap_buffers()
{
  SwapBuffers();
}

//: Make this the current GL rendering context.
void vgui_wx_adaptor::make_current()
{
  SetCurrent();
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
void vgui_wx_adaptor::on_size(wxSizeEvent& event)
{
  wxGLCanvas::OnSize(event);

  dispatch_to_tableau(vgui_RESHAPE);
  post_redraw();
  invalidate_canvas();
}

//: Called when a window needs to be repainted.
void vgui_wx_adaptor::on_paint(wxPaintEvent& WXUNUSED(event))
{
  vgui_macro_report_errors;
  // must always be here
  wxPaintDC dc(this);

  SetCurrent();

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
void vgui_wx_adaptor::on_erase_background(wxEraseEvent& WXUNUSED(event))
{
  vgui_macro_report_errors;
  // do nothing (avoids flickering in wxMSW)
}

//: Helper used by on_key_up/down to reduce code duplication.
void vgui_wx_adaptor::on_key(vgui_event& ve, wxKeyEvent& event)
{
  ve.origin   = this;

  ve.ascii_char = translate_key(event.GetKeyCode());
  ve.modifier = get_modifiers(event);
  // ***** this should be handled in vgui_event::set_key(ascii,bool)
  //       ve.set_key(ve.ascii_char, ve.modifier != vgui_MODIFIER_NULL)
  // or    ve.set_key(ve.ascii_char, ve.modifier)
  if (0 < ve.ascii_char && ve.ascii_char < 27 && ve.modifier_is_down(vgui_CTRL))
  {
    ve.key = static_cast<vgui_key>(ve.ascii_char + 'a' - 1);
  }
  else { ve.set_key(ve.ascii_char); }

#if 0
  if (ve.type == vgui_KEY_PRESS)
  {
    vcl_cout << "key (wx, vgui): " << ve.key << '\t' << ve.ascii_char
             << '\t' << ve.modifier << vcl_endl;
  }
#endif

  // ***** what should I return here?? What about if scrolled window??
  ve.wx = event.GetX();
  ve.wy = get_height() - event.GetY();

  // ***** what exactly goes here??
  ve.timestamp = 0;

  if (!dispatch_to_tableau(ve)) { event.Skip(); }
  //invalidate_canvas();
}

//: Called when a key is pressed.
void vgui_wx_adaptor::on_key_down(wxKeyEvent& event)
{
  // if it's a modifier event let it propagate
  if (is_modifier(event.GetKeyCode()))
  {
    event.Skip();
    return;
  }

#if 0
  vcl_cout << "EVT_KEY_DOWN: " << event.GetKeyCode() << vcl_endl;
#endif

  // save the code and let the event propagate to the on_char handler
  last_key_down_ = event.GetKeyCode();
  event.Skip();

  //vgui_event e(vgui_KEY_PRESS);
  //on_key(e, event);
}

//: Called when a key is released.
void vgui_wx_adaptor::on_key_up(wxKeyEvent& event)
{
  if (is_modifier(event.GetKeyCode()))
  {
    event.Skip();
    return;
  }

#if 0
  vcl_cout << "EVT_KEY_UP  : " << event.GetKeyCode()
           << " (" << ascii_code_[event.GetKeyCode()] << ')' << vcl_endl;
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
void vgui_wx_adaptor::on_char(wxKeyEvent& event)
{
  if (is_modifier(event.GetKeyCode()))
  {
    event.Skip();
    return;
  }

#if 0
  vcl_cout << "EVT_CHAR    : " << last_key_down_
           << " (" << event.GetKeyCode() << ')' << vcl_endl;
#endif

  // save ascii code of last on_key_down event for on_key_up to use
  ascii_code_[last_key_down_] = event.GetKeyCode();

  vgui_event e(vgui_KEY_PRESS);
  on_key(e, event);
}

//: Called for all types of mouse events (e.g., button-up, motion, etc.).
void vgui_wx_adaptor::on_mouse_event(wxMouseEvent& event)
{
  vgui_macro_report_errors;
  vgui_event e;

  e.origin   = this;
  e.modifier = get_modifiers(event);
  e.button   = translate_mouse_button(event.GetButton());
  e.type     = translate_mouse_event_type(event);

  // ***** what should I return here?? What about if scrolled window??
  e.wx = event.GetX();
  e.wy = get_height() - event.GetY();

  // ***** what exactly goes here??
  e.timestamp = 0;

  if ( e.modifier == mixin::popup_modifier
    && e.button   == mixin::popup_button )
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
    wxMenu* menu = popup.create_wx_menu(last_popup_);
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

  if (!dispatch_to_tableau(e)) { event.Skip(); }
  //invalidate_canvas();
  vgui_macro_report_errors;
}

//: Called when the system becomes idle.
void vgui_wx_adaptor::on_idle(wxIdleEvent& event)
{
  if (idle_request_posted_)
  {
    idle_request_posted_ = dispatch_to_tableau(vgui_IDLE);
    event.RequestMore(idle_request_posted_);
    //post_redraw();
    //invalidate_canvas();
  }

  event.Skip();
}

//: Called when the user tries to close a frame or dialog box.
//
// The event can be generated programatically or when the user tries to
// close using the window manager (X) or system menu (Windows).
void vgui_wx_adaptor::on_close(wxCloseEvent& event)
{
  // ***** can't find error with inline_tableaus
  vgui_macro_report_errors;
  dispatch_to_tableau(vgui_DESTROY);
  vgui_macro_report_errors;
  GetParent()->Destroy();
  vgui_macro_report_errors;
}

//: Called at fixed intervals when using a timer.
void vgui_wx_adaptor::on_timer(wxEvent& event)
{
  vgui_event e(vgui_TIMER);
  e.origin = this;
  e.timer_id = event.GetId();
  dispatch_to_tableau(e);
}

//: Generates a wxPaintEvent for the window to be repainted.
void vgui_wx_adaptor::invalidate_canvas(void)
{
  Refresh();

#if 0
  if (view_)
  {
    view_->GetDocument()->UpdateAllViews();
  }
  else { Refresh(); }
#endif
}

//-------------------------------------------------------------------------
// Private helpers - definitions.
//-------------------------------------------------------------------------
namespace
{
  inline bool is_modifier(int key_code)
  {
    switch (key_code)
    {
    case WXK_SHIFT:
    case WXK_ALT:
    case WXK_CONTROL:
    //case WXK_META: // ***** which one is the meta key??
      return true;
    default:
      return false;
    }
  }

  // ***** check if design allows for multiple modifiers... if so, this needs to change
  inline vgui_modifier get_modifiers(const wxMouseEvent& e)
  {
#if 1
    int mod = 0;
    if (e.ControlDown()) { mod |= vgui_CTRL;  }
    if (e.ShiftDown())   { mod |= vgui_SHIFT; }
    if (e.MetaDown())    { mod |= vgui_META;  }
    if (e.AltDown())     { mod |= vgui_ALT;   }
    return static_cast<vgui_modifier>(mod);
#else
    return static_cast<vgui_modifier>( e.ControlDown() ? vgui_CTRL  : 0
                                     | e.ShiftDown()   ? vgui_SHIFT : 0
                                     | e.MetaDown()    ? vgui_META  : 0
                                     | e.AltDown()     ? vgui_ALT   : 0 );
#endif
  }

  // ***** check if design allows for multiple modifiers... if so, this needs to change
  inline vgui_modifier get_modifiers(const wxKeyEvent& e)
  {
#if 1
    int mod = 0;
    if (e.ControlDown()) { mod |= vgui_CTRL;  }
    if (e.ShiftDown())   { mod |= vgui_SHIFT; }
    if (e.MetaDown())    { mod |= vgui_META;  }
    if (e.AltDown())     { mod |= vgui_ALT;   }
    return static_cast<vgui_modifier>(mod);
#else
    return static_cast<vgui_modifier>( e.ControlDown() ? vgui_CTRL  : 0
                                     | e.ShiftDown()   ? vgui_SHIFT : 0
                                     | e.MetaDown()    ? vgui_META  : 0
                                     | e.AltDown()     ? vgui_ALT   : 0 );
#endif
  }

  inline vgui_event_type translate_mouse_event_type(const wxMouseEvent& e)
  {
    if (e.Moving() || e.Dragging()) { return vgui_MOTION; }
    else if (e.ButtonDown() || e.ButtonDClick())
    {
      return vgui_BUTTON_DOWN;
    }
    else if (e.ButtonUp()) { return vgui_BUTTON_UP; }
    else if (e.Entering()) { return vgui_ENTER;     }
    else if (e.Leaving ()) { return vgui_LEAVE;     }
    else if (e.GetEventType() == wxEVT_MOUSEWHEEL)
    {
      if (e.GetWheelRotation() > 0) { return vgui_WHEEL_UP; }
      else { return vgui_WHEEL_DOWN; }
    }
    else { return vgui_OTHER; }
  }

  inline vgui_button translate_mouse_button(int button)
  {
    switch (button)
    {
    case wxMOUSE_BTN_NONE   : return vgui_BUTTON_NULL;
    case wxMOUSE_BTN_LEFT   : return vgui_LEFT;
    case wxMOUSE_BTN_MIDDLE : return vgui_MIDDLE;
    case wxMOUSE_BTN_RIGHT  : return vgui_RIGHT;
    default:
      vcl_cerr << "VGUI wx Error: Unknown button identifier." << vcl_endl;
      return vgui_BUTTON_NULL;
    }
  }

  inline vgui_key translate_key(int key_code)
  {
    switch (key_code)
    {
    case WXK_ESCAPE    : return vgui_ESC;
    case WXK_TAB       : return vgui_TAB;
    case WXK_RETURN    : return vgui_RETURN;
    case '\n'          : return vgui_NEWLINE;

    case WXK_F1        : return vgui_F1;
    case WXK_F2        : return vgui_F2;
    case WXK_F3        : return vgui_F3;
    case WXK_F4        : return vgui_F4;
    case WXK_F5        : return vgui_F5;
    case WXK_F6        : return vgui_F6;
    case WXK_F7        : return vgui_F7;
    case WXK_F8        : return vgui_F8;
    case WXK_F9        : return vgui_F9;
    case WXK_F10       : return vgui_F10;
    case WXK_F11       : return vgui_F11;
    case WXK_F12       : return vgui_F12;
    case WXK_LEFT      : return vgui_CURSOR_LEFT;
    case WXK_UP        : return vgui_CURSOR_UP;
    case WXK_RIGHT     : return vgui_CURSOR_RIGHT;
    case WXK_DOWN      : return vgui_CURSOR_DOWN;
    case WXK_PRIOR     : return vgui_PAGE_UP;
    case WXK_NEXT      : return vgui_PAGE_DOWN;
    //case WXK_PAGEUP    : return vgui_PAGE_UP;   // ***** ??
    //case WXK_PAGEDOWN  : return vgui_PAGE_DOWN; // ***** ??
    case WXK_HOME      : return vgui_HOME;
    case WXK_END       : return vgui_END;
    case WXK_DELETE    : return vgui_DELETE;
    case WXK_INSERT    : return vgui_INSERT;
    default:
      if (0 < key_code && key_code < 256) // it's an ascii code
      {
        return static_cast<vgui_key>(key_code);
      }
      else
      {
#ifdef DEBUG
        vcl_cerr << "VGUI wx Error: Unknown key code." << vcl_endl;
#endif
        return vgui_KEY_NULL;
      }
    }
  }
} // unnamed namespace
