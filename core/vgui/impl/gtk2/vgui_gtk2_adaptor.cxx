// This is core/vgui/impl/gtk2/vgui_gtk2_adaptor.cxx
//:
// \file
// \brief  See vgui_gtk2_adaptor.h for a description of this file.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Dec 99
//
// \verbatim
//  Modifications
//   04-OCT-2002 K.Y.McGaul - Use event.set_key() to set key for events,
//                            makes all key chars lower case.
//                          - Set event.ascii_char to actual key stroke
// \endverbatim

#include <cstdlib>
#include <utility>
#include <iostream>
#include "vgui_gtk2_adaptor.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkgl.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/internals/vgui_overlay_helper.h>
#include "vgui_gtk2_utils.h"
#include "vgui_gtk2_window.h"

vgui_menu vgui_gtk2_adaptor::last_popup;

extern "C" {
  static gint timeout_callback(gpointer);
}

//--------------------------------------------------------------------------------
//: Constructors
vgui_gtk2_adaptor::vgui_gtk2_adaptor(vgui_gtk2_window* win)
  : idle_request_posted_(false),
    widget(0),
    win_(win),
    ovl_helper(0),
    last_mouse_x(0),
    last_mouse_y(0)
{
  // Configure OpenGL-capable visual.

  // Try double-buffered visual
  GdkGLConfig* glconfig = gdk_gl_config_new_by_mode (GdkGLConfigMode(GDK_GL_MODE_RGB |
                                                                     GDK_GL_MODE_DEPTH |
                                                                     GDK_GL_MODE_DOUBLE));
  if (glconfig == 0)
  {
    g_print ("*** Cannot find the double-buffered visual.\n");
    g_print ("*** Trying single-buffered visual.\n");

    // Try single-buffered visual
    glconfig = gdk_gl_config_new_by_mode(GdkGLConfigMode(GDK_GL_MODE_RGB |
                                                         GDK_GL_MODE_DEPTH));
    if (glconfig == 0)
    {
      g_print ("*** No appropriate OpenGL-capable visual found.\n");
      std::abort();
    }
  }

  widget = gtk_drawing_area_new ();
  //gtk_widget_set_size_request (drawing_area, 300, 300);

  if (!widget)
  {
    std::cerr << __FILE__ << " : Could not get a GL widget!\n";
    std::abort();
  }

  // Set OpenGL-capability to the widget.
  if ( !gtk_widget_set_gl_capability(widget,
                                     glconfig,
                                     0 /*NULL*/,
                                     TRUE,
                                     GDK_GL_RGBA_TYPE) )
  {
    std::cerr << __FILE__ << " : Could not set GL capability!\n";
    std::abort();
  }

  // Since we need to access the widget from time to time (e.g. to
  // make this OpenGL context the current context), we need to keep a
  // reference to the widget.
  gtk_object_ref( GTK_OBJECT(widget) );

  gtk_widget_set_events(widget,
                        GDK_EXPOSURE_MASK |
                        GDK_POINTER_MOTION_MASK |
                        GDK_POINTER_MOTION_HINT_MASK |
                        GDK_BUTTON_PRESS_MASK |
                        GDK_BUTTON_RELEASE_MASK |
                        GDK_KEY_PRESS_MASK |
                        GDK_KEY_RELEASE_MASK |
                        GDK_ENTER_NOTIFY_MASK |
                        GDK_LEAVE_NOTIFY_MASK);

#if 0
  gtk_signal_connect(GTK_OBJECT(widget), "event", GTK_SIGNAL_FUNC(handle), this);
#else
  gtk_signal_connect(GTK_OBJECT(widget), "configure_event", GTK_SIGNAL_FUNC(handle_configure), this);
  gtk_signal_connect(GTK_OBJECT(widget), "expose_event", GTK_SIGNAL_FUNC(handle_draw), this);
  gtk_signal_connect(GTK_OBJECT(widget), "map_event", GTK_SIGNAL_FUNC(handle_draw), this);
  gtk_signal_connect(GTK_OBJECT(widget), "motion_notify_event", GTK_SIGNAL_FUNC(handle_motion_notify), this);
  gtk_signal_connect(GTK_OBJECT(widget), "button_press_event", GTK_SIGNAL_FUNC(handle_button), this);
  gtk_signal_connect(GTK_OBJECT(widget), "button_release_event", GTK_SIGNAL_FUNC(handle_button), this);
  gtk_signal_connect(GTK_OBJECT(widget), "key_press_event", GTK_SIGNAL_FUNC(handle_key), this);
  gtk_signal_connect(GTK_OBJECT(widget), "key_release_event", GTK_SIGNAL_FUNC(handle_key), this);
  gtk_signal_connect(GTK_OBJECT(widget), "enter_notify_event", GTK_SIGNAL_FUNC(handle_enter_leave), this);
  gtk_signal_connect(GTK_OBJECT(widget), "leave_notify_event", GTK_SIGNAL_FUNC(handle_enter_leave), this);
#endif
  GTK_WIDGET_SET_FLAGS(widget, GTK_CAN_FOCUS);

  redraw_requested = false;
  destroy_requested = false;
}

//: Destructor
vgui_gtk2_adaptor::~vgui_gtk2_adaptor()
{
  if (ovl_helper)
    delete ovl_helper;
  ovl_helper = 0;

  glFlush();
  gtk_object_unref( GTK_OBJECT(widget) );
  widget = 0; // to detect potential bugs
}


vgui_window* vgui_gtk2_adaptor::get_window() const
{
  return win_;
}

void vgui_gtk2_adaptor::swap_buffers()
{
  make_current();
  gdk_gl_drawable_swap_buffers (gtk_widget_get_gl_drawable(widget));
}

void vgui_gtk2_adaptor::make_current()
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
  assert( gldrawable );
  gdk_gl_drawable_make_current(gldrawable, glcontext);
}

void vgui_gtk2_adaptor::post_redraw()
{
  if (!redraw_requested)
  {
    redraw_requested = true;
    gtk_idle_add(idle_callback_for_redraw, this);
  }
}

void vgui_gtk2_adaptor::post_overlay_redraw()
{
  if (!ovl_helper)
    ovl_helper = new vgui_overlay_helper(this);
  ovl_helper->post_overlay_redraw();
}

void vgui_gtk2_adaptor::post_idle_request()
{
  if (!idle_request_posted_){
    idle_request_posted_ = true;
    g_idle_add(idle_callback_for_tableaux, this);
  }
}

//: gtk will pass this structure to the timer callback.
typedef struct
{
  vgui_gtk2_adaptor *adapt;
  int name;
} vgui_gtk2_adaptor_callback_data;

//: timeout is in milliseconds
void vgui_gtk2_adaptor::post_timer(float timeout, int name)
{
  vgui_gtk2_adaptor_callback_data *cd = new vgui_gtk2_adaptor_callback_data; // <*> acquire
  cd->adapt = this;
  cd->name = name;

  gint id = gtk_timeout_add(int(timeout),
                            timeout_callback,
                            cd);

  // add them to timer map
  internal_timer i( id, (void*)cd );
  timers_.insert( std::pair<int, internal_timer>(name, i) );
}

//: timeout is in milliseconds
void vgui_gtk2_adaptor::kill_timer(int name)
{
  std::map<int, internal_timer>::iterator it
    = timers_.find( name );
  if ( it == timers_.end() )  // if such timer does not exist
    return;

  internal_timer timer;
  timer = (*it).second;
  // remove timer
  gtk_timeout_remove(timer.real_id_);
  // remove callback ptr
  delete  (vgui_gtk2_adaptor_callback_data*)(timer.callback_ptr_);

  // remove timer from map
  timers_.erase(it);
}

void vgui_gtk2_adaptor::post_destroy()
{
  if (!destroy_requested)
  {
    destroy_requested = true;
    gtk_idle_add(idle_callback_for_destroy, this);
  }
}

void vgui_gtk2_adaptor::set_default_popup(vgui_menu)
{
#ifdef DEBUG
  std::cerr << "vgui_gtk2_adaptor::set_default_popup\n";
#endif
}

vgui_menu vgui_gtk2_adaptor::get_popup()
{
#ifdef DEBUG
  std::cerr << "vgui_gtk2_adaptor::get_popup\n";
#endif
  return vgui_menu();
}

gint vgui_gtk2_adaptor::handle_configure(
                               GtkWidget *widget,
                               GdkEvent *gev,
                               gpointer context)
{
  vgui_gtk2_adaptor* adaptor = (vgui_gtk2_adaptor*) context;

  // The following 5 lines are required to make GL context available
  // so that some GL functions (such as glGenLists()) can succeed.
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;
  gdk_gl_drawable_gl_end (gldrawable);

  adaptor->reshape();
  return TRUE;
}

gint vgui_gtk2_adaptor::handle_draw(
                               GtkWidget *widget,
                               GdkEvent *gev,
                               gpointer context)
{
  vgui_gtk2_adaptor* adaptor = (vgui_gtk2_adaptor*) context;
  adaptor->draw();
  return TRUE;
}

gint vgui_gtk2_adaptor::handle_motion_notify(
                               GtkWidget *widget,
                               GdkEvent *gev,
                               gpointer context)
{
  vgui_gtk2_adaptor* adaptor = (vgui_gtk2_adaptor*) context;
  vgui_event event;

  event.type = vgui_MOTION;
  GdkEventMotion *e = (GdkEventMotion*)gev;
  if (e->is_hint)
  {
    int x,y;
    GdkModifierType state;
    gdk_window_get_pointer(e->window, &x, &y, &state);
    vgui_gtk2_utils::set_modifiers(event, state);
    vgui_gtk2_utils::set_coordinates(event, x, y);
  }
  else
  {
    vgui_gtk2_utils::set_modifiers(event,e->state);
    vgui_gtk2_utils::set_coordinates(event,e->x, e->y);
  }
  adaptor->last_mouse_x = event.wx;
  adaptor->last_mouse_y = event.wy;

  return handle(event, widget, gev, context);
}


gint vgui_gtk2_adaptor::handle_button(
                               GtkWidget *widget,
                               GdkEvent *gev,
                               gpointer context)
{
  vgui_gtk2_adaptor* adaptor = (vgui_gtk2_adaptor*) context;
  vgui_event event;
  GdkEventType type = gev->type;

  if (type==GDK_BUTTON_PRESS)
    event.type = vgui_BUTTON_DOWN;
  else if (type==GDK_BUTTON_RELEASE)
    event.type = vgui_BUTTON_UP;
  GdkEventButton *e = (GdkEventButton*)gev;
  event.button = vgui_gtk2_utils::translate_button(e->button);
  vgui_gtk2_utils::set_modifiers(event,e->state);
  vgui_gtk2_utils::set_coordinates(event,e->x, e->y);
  adaptor->last_mouse_x = event.wx;
  adaptor->last_mouse_y = event.wy;

  if (event.type == vgui_BUTTON_DOWN &&
      event.button == adaptor->popup_button &&
      event.modifier == adaptor->popup_modifier)
  {
    GdkEventButton *bevent = (GdkEventButton *)gev;

    GtkWidget *popup_menu = gtk_menu_new ();    /* Don't need to show menus */

    vgui_popup_params params;
    params.x = event.wx;
    params.y = event.wy;

    // fsm - assign the popup menu to 'last_popup' to ensure the
    // commands stay in scope for the lifetime of the gtk popup.
    adaptor->last_popup = adaptor->get_total_popup(params);

    vgui_gtk2_utils::set_menu(popup_menu, adaptor->last_popup, false);
    gtk_menu_popup(GTK_MENU(popup_menu), 0/*NULL*/, 0/*NULL*/, 0/*NULL*/, 0/*NULL*/,
                   bevent->button, bevent->time);
    return TRUE;
  }

  return handle(event, widget, gev, context);
}

gint vgui_gtk2_adaptor::handle_key(
                               GtkWidget *widget,
                               GdkEvent *gev,
                               gpointer context)
{
  vgui_gtk2_adaptor* adaptor = (vgui_gtk2_adaptor*) context;
  vgui_event event;
  GdkEventType type = gev->type;

  if (type==GDK_KEY_PRESS)
    event.type = vgui_KEY_PRESS;
  else if (type==GDK_KEY_RELEASE)
    event.type = vgui_KEY_RELEASE;
  GdkEventKey *e = (GdkEventKey*)gev;
  event.set_key( vgui_gtk2_utils::translate_key(e));
  event.ascii_char = vgui_gtk2_utils::translate_key(e);
  vgui_gtk2_utils::set_modifiers(event,e->state);
  event.wx = adaptor->last_mouse_x;
  event.wy = adaptor->last_mouse_y;

  return handle(event, widget, gev, context);
}

gint vgui_gtk2_adaptor::handle_enter_leave(
                               GtkWidget *widget,
                               GdkEvent *gev,
                               gpointer context)
{
  vgui_event event;
  GdkEventType type = gev->type;

  if (type==GDK_ENTER_NOTIFY)
  {
    event.type = vgui_ENTER;
    gtk_widget_grab_focus(GTK_WIDGET(widget));
  }
  else if (type==GDK_LEAVE_NOTIFY)
  {
    event.type = vgui_LEAVE;
  }

  return handle(event, widget, gev, context);
}

gint vgui_gtk2_adaptor::handle(const vgui_event &event,
                               GtkWidget *widget,
                               GdkEvent *gev,
                               gpointer context)
{
  vgui_gtk2_adaptor* adaptor = (vgui_gtk2_adaptor*) context;

  bool ret_value = TRUE;
  if (vgui_gtk2_utils::is_modifier(gev))
    ret_value = FALSE;

#ifdef DEBUG
  std::cerr << "vgui_event " << event << '\n';
#endif
  // Only send events to the tableau if the widget is mapped; that is,
  // only when an OpenGL context exists.
  if ( GTK_WIDGET_MAPPED(widget) )
  {
    if (adaptor->ovl_helper)
      adaptor->ovl_helper->dispatch(event);
    else
      adaptor->dispatch_to_tableau(event);
  }
  else
    std::cerr << __FILE__ << ": error: event " << event
             << " while GL area was not mapped\n";

  return ret_value;
}


void vgui_gtk2_adaptor::reshape()
{
  width = widget->allocation.width;
  height = widget->allocation.height;

  // Only send events to the tableau if the widget is mapped; that is,
  // only when an OpenGL context exists.
  if ( GTK_WIDGET_MAPPED(widget) )
  {
    make_current();
    if (ovl_helper)
      ovl_helper->dispatch(vgui_RESHAPE);
    else
      dispatch_to_tableau(vgui_RESHAPE);
  }
}

bool vgui_gtk2_adaptor::do_idle()
{
  if ( idle_request_posted_ )
    idle_request_posted_ =  dispatch_to_tableau( vgui_event( vgui_IDLE ) );
  return idle_request_posted_;
}

//--------------------------------------------------------------------------------
//: This is overriding the gtk draw() method.
void vgui_gtk2_adaptor::draw()
{
#ifdef DEBUG
  std::cerr << "vgui_gtk2_adaptor::draw\n";
#endif
  if ( GTK_WIDGET_MAPPED(widget) )
  {
    make_current();
    glDrawBuffer(GL_BACK);
    if (ovl_helper)
      ovl_helper->dispatch(vgui_DRAW);
    else
    {
      dispatch_to_tableau(vgui_DRAW);
      swap_buffers();
    }
  }
}


gint vgui_gtk2_adaptor::idle_callback_for_tableaux(gpointer data)
{
  vgui_gtk2_adaptor *adaptor = static_cast<vgui_gtk2_adaptor*>(data);

  return adaptor->do_idle();
}


gint vgui_gtk2_adaptor::idle_callback_for_redraw(gpointer data)
{
  vgui_gtk2_adaptor *adaptor = static_cast<vgui_gtk2_adaptor*>(data);

  adaptor->draw();

  adaptor->redraw_requested = false;

  // capes - returning FALSE automagically cancels this callback
  return FALSE;
}

// Callback setup by post_destroy. First notifies tableau of the impending
// destruction. Then deletes the adaptor and its associated window.
gint vgui_gtk2_adaptor::idle_callback_for_destroy(gpointer data)
{
  vgui_gtk2_adaptor *adaptor = static_cast<vgui_gtk2_adaptor*>(data);

  adaptor->dispatch_to_tableau(vgui_DESTROY);

  vgui_window* win = adaptor->get_window();

  // If we know the parent window then delete it now.
  if (win)
    delete win;
  else
    std::cerr << __FILE__ " : parent vgui_gtk2_window is unknown, so cannot destroy!\n";

  // The adaptor destructor unrefs its tableau and disconnects/destroys
  // its glarea widget
  delete adaptor;

  // capes - returning FALSE automagically cancels this callback
  return FALSE;
}

extern "C" {
  gint timeout_callback(gpointer data)
  {
    vgui_gtk2_adaptor_callback_data* cd = static_cast<vgui_gtk2_adaptor_callback_data*> (data);
    vgui_event e(vgui_TIMER);
    e.timer_id = cd->name;
    cd->adapt->dispatch_to_tableau(e);

    // return non-zero value to reuse the timer
    return true;
  }
}
