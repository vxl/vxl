// This is oxl/ogui/impl/Fl/vgui_Fl_adaptor.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   11 Sep 99
//-----------------------------------------------------------------------------

#include "vgui_Fl_adaptor.h"

#include <FL/Fl.H>
#include <FL/Fl_Menu_Button.h>
#include <vgui/vgui_gl.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_popup_params.h>

#include "vgui_Fl.h"
#include "vgui_Fl_window.h"
#include "vgui_Fl_utils.h"

static bool debug = false;


vgui_Fl_adaptor::vgui_Fl_adaptor(int X, int Y, int W, int H, const char *L)
  : Fl_Gl_Window(X, Y, W, H, L)
  , is_drawn(false)
{
  popup = new Fl_Menu_Button(X,Y,W,H);
  bind_popups(vgui_MODIFIER_NULL, vgui_RIGHT);
  end();
}

vgui_Fl_adaptor::vgui_Fl_adaptor()
  : Fl_Gl_Window(0,0,10,10)
  , is_drawn(false)
{
  popup = new Fl_Menu_Button(0,0,10,10);
  bind_popups(vgui_MODIFIER_NULL, vgui_RIGHT);
  end();
}


vgui_Fl_adaptor::~vgui_Fl_adaptor()
{
}


void vgui_Fl_adaptor::make_current()
{
  Fl_Gl_Window::make_current();
}

void vgui_Fl_adaptor::swap_buffers()
{
  Fl_Gl_Window::swap_buffers();
}


// this function provides much of the mapping from the FLTK event model
// to the vgui event model

static int ignore_next_event = false;

int vgui_Fl_adaptor::handle(int event_type)
{
  if (!valid() || !visible_r() || !is_drawn) return 0;


  // make this context current
  make_current();

  // nasty hack to get rid of extra FLTK_MOVE event that is generated
  // after a FLTK_RELEASE event
  if (ignore_next_event)
  {
    if (debug)
    {
      vcl_cerr << "vgui_Fl_adaptor::handle ignoring ";
      if (event_type == FL_DRAG)
        vcl_cerr << "FL_DRAG";
      else if (event_type == FL_MOVE)
        vcl_cerr << "FL_MOVE";
      else
      vcl_cerr << event_type;
      vcl_cerr << vcl_endl;
    }

    ignore_next_event = false;
    return 1;
  }

  if (event_type == FL_RELEASE)
    ignore_next_event = true;


  // first create the vgui_event
  if (debug)
    vcl_cerr << "vgui_Fl_adaptor " << (void*)this << "   event_type : " << event_type << vcl_endl;

  vgui_event event;


  // setup modifiers for event
  {
    int m_int = int(event.modifier); // gcc 2.95 won't cast ints to enums

    // vgui_CONTROL
    if (Fl::event_state(FL_CTRL))
    {
      m_int |= vgui_CTRL;
      //vcl_cerr << "CTRL\n";
    }

    // vgui_SHIFT
    if (Fl::event_state(FL_SHIFT))
    {
      m_int |= vgui_SHIFT;
      //vcl_cerr << "SHIFT\n";
    }

    // vgui_ALT
    if (Fl::event_state(FL_ALT))
    {
      m_int |= vgui_ALT;
      //vcl_cerr << "ALT\n";
    }

    event.modifier = vgui_modifier(m_int);
  }


  //----------------------
  // button_down

  if (event_type == FL_PUSH)
  {
    if (debug) vcl_cerr << "button_down\n";

    event.type = vgui_BUTTON_DOWN;
    int button = Fl::event_button();

    if (button == 1)
      event.button = vgui_LEFT;
    else if (button == 2)
      event.button = vgui_MIDDLE;
    else
      event.button = vgui_RIGHT;

    event.wx = Fl::event_x();
    event.wy = Fl::event_y();
  }


  //----------------------
  // button_up
  else if (event_type == FL_RELEASE)
  {
    if (debug) vcl_cerr << "button_up\n";

    event.type = vgui_BUTTON_UP;
    int button = Fl::event_button();

    if (button == 1)
      event.button = vgui_LEFT;
    else if (button == 2)
      event.button = vgui_MIDDLE;
    else
      event.button = vgui_RIGHT;

    event.wx = Fl::event_x();
    event.wy = Fl::event_y();
  }


  //----------------------
  // motion
  else if (event_type == FL_DRAG ||
           event_type == FL_MOVE)
  {
    if (debug)
      vcl_cerr << "motion\nx y "<< Fl::event_x() <<' '<< Fl::event_y() << '\n';

    event.type = vgui_MOTION;
    event.wx = Fl::event_x();
    event.wy = Fl::event_y();
  }

  //----------------------
  // keypress

  else if (event_type == FL_KEYBOARD ||
           event_type == FL_SHORTCUT)
  {
    if (debug) vcl_cerr << "key\n";

    event.type = vgui_KEY_PRESS;
    int key = Fl::event_key();

    event.wx = Fl::event_x();
    event.wy = Fl::event_y();

    switch(key)
    {
    case FL_Button: // A mouse button; use Fl_Button + n for mouse button n.
      break;
    case FL_BackSpace: // The backspace key.
      break;
    case FL_Tab: // The tab key.
      break;
    case FL_Enter: // The enter key.
      break;
    case FL_Pause: // The pause key.
      break;
    case FL_Scroll_Lock: // The scroll lock key.
      break;
    case FL_Escape: // The escape key.
      event.key = vgui_ESC;
      break;
    case FL_Home: // The home key.
      break;
    case FL_Left: // The left arrow key.
      event.key = vgui_CURSOR_LEFT;
      break;
    case FL_Up: // The up arrow key.
      event.key = vgui_CURSOR_UP;
      break;
    case FL_Right: // The right arrow key.
      event.key = vgui_CURSOR_RIGHT;
      break;
    case FL_Down: // The down arrow key.
      event.key = vgui_CURSOR_DOWN;
      break;
    case FL_Page_Up: // The page-up key.
      event.key = vgui_PAGE_UP;
      break;
    case FL_Page_Down: // The page-down key.
      event.key = vgui_PAGE_DOWN;
      break;
    case FL_End: // The end key.
      break;
    case FL_Print: // The print (or print-screen) key.
      break;
    case FL_Insert: // The insert key.
      break;
    case FL_Menu: // The menu key.
      break;
    case FL_Num_Lock: // The num lock key.
      break;
    case FL_KP: // One of the keypad numbers; use case FL_KP + n for number n.
      break;
    case FL_KP_Enter: // The enter key on the keypad.
      break;
    case FL_F: // One of the function keys; use FL_F + n for function key n.
      break;
    case FL_Shift_L: // The lefthand shift key.
      break;
    case FL_Shift_R: // The righthand shift key.
      break;
    case FL_Control_L: // The lefthand control key.
      break;
    case FL_Control_R: // The righthand control key.
      break;
    case FL_Caps_Lock: // The caps lock key.
      break;
    case FL_Meta_L: // The left meta/Windows key.
      break;
    case FL_Meta_R: // The right meta/Windows key.
      break;
    case FL_Alt_L: // The left alt key.
      break;
    case FL_Alt_R: // The right alt key.
      break;
    case FL_Delete: // The delete key.
      break;
    default:
      // if the key press was normal text
      // then use Fl::event_text rather than
      // Fl::event_key - as that gives
      // unshifted values

      if (Fl::event_state(FL_SHIFT))
        event.key = vgui_key(Fl::event_text()[0]);
      else
        event.key = vgui_key(Fl::event_key());
      break;
    }
  }

  //----------------------
  // keyrelease
  // doesnt appear to have fltk version

  //----------------------
  // enter

  else if (event_type == FL_ENTER)
  {
    if (debug) vcl_cerr << "enter\n";

    event.type = vgui_ENTER;
    event.wx = Fl::event_x();
    event.wy = Fl::event_y();

    // make this context the currently active context
    //FLTK_window* win = FLTK_app::instance()->get_current_window();
    //if (win != 0)
    //  win->set_active_context(this);


    // fltk command that grabs the focus for this widget
    // this is needed to force keyboard events to be sent here
    take_focus();
  }

  //----------------------
  // leave

  else if (event_type == FL_LEAVE)
  {
    if (debug) vcl_cerr << "leave\n";

    event.type = vgui_LEAVE;
    event.wx = Fl::event_x();
    event.wy = Fl::event_y();
  }


  //----------------------
  // other
  else
  {
    if (debug) vcl_cerr << "other\n";

    event.type = vgui_EVENT_NULL;
    event.wx = Fl::event_x();
    event.wy = Fl::event_y();
  }

  if (debug)
    vcl_cerr << "vgui_Fl_adaptor::handle " << (int)event.type << vcl_endl;


  // check for popups
  if (event.type == vgui_BUTTON_DOWN &&
      event.button == popup_button &&
      event.modifier == popup_modifier)
  {
    //vcl_cerr << "POPUP\n";

    vgui_popup_params params;
    params.x = event.wx;
    params.y = event.wy;

    last_popup = vgui_adaptor::get_total_popup(params);
    vgui_Fl_utils::set_menu(popup, last_popup); // ** CORE LEAK ? **
    popup->handle(event_type);
    vgui_Fl_utils::set_menu(popup, vgui_menu()); // hmm..

    return 1;
  }


  // translate from window to viewport coordinates :
  event.wy = get_height()-1 - event.wy;

  // tableau now processes the event
  dispatch_to_tableau(event);

  // always return 1 to indicate that the event is being handled
  // by this fltk widget. when we have multiple widgets we will
  // have to look at this again
  return 1;
}

// unsigned vgui_Fl_adaptor::get_width() const
// {
//   return this->Fl_Gl_Window::w();
// }
// unsigned vgui_Fl_adaptor::get_height() const
// {
//   return this->Fl_Gl_Window::h();
// }

void vgui_Fl_adaptor::draw()
{
  if (debug) vcl_cerr << "vgui_Fl_adaptor::draw\n";
  is_drawn = true;

  width = w();
  height = h();

  dispatch_to_tableau(vgui_DRAW);
}

void vgui_Fl_adaptor::draw_overlay()
{
#if 1
  // fsm@robots: It seems that the fltk code which is calling draw_overlay()
  // performs an invalid operation. So we discard any GL errors stored up
  // till now. That doesn't fix the overlay bug, though, it just quells the
  // warning messages from vgui_macro_report_errors
  glGetError();
#endif
  if (debug)
    vcl_cerr << "vgui_Fl_adaptor::draw_overlay\n";
  is_drawn = true;

  width = w();
  height = h();

  dispatch_to_tableau(vgui_DRAW_OVERLAY);
}

void vgui_Fl_adaptor::post_redraw()
{
  Fl_Gl_Window::redraw();
}

void vgui_Fl_adaptor::post_overlay_redraw()
{
#if 1
  if (! Fl_Gl_Window::can_do_overlay())
  {
    vcl_cerr << "cannot do overlay\n";
    return;
  }
#endif
  Fl_Gl_Window::redraw_overlay();
}

void vgui_Fl_adaptor::bind_popups(vgui_modifier mod, vgui_button button)
{
  popup_button = button;
  switch (popup_button)
  {
  case vgui_LEFT: popup->type(Fl_Menu_Button::POPUP1); break;
  case vgui_MIDDLE: popup->type(Fl_Menu_Button::POPUP2); break;
  case vgui_RIGHT: popup->type(Fl_Menu_Button::POPUP3); break;
  default: vcl_cerr << "vgui_Fl_adaptor::bind_popups  incorrect popup_button given\n"; break;
  }
  popup_modifier = mod;
}
