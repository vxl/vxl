// This is core/vgui/impl/gtk/vgui_gtk_adaptor.h
#ifndef vgui_gtk_adaptor_h_
#define vgui_gtk_adaptor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   19 Dec 99
// \brief  The GTK implementation of vgui_adaptor.

#ifdef __SUNPRO_CC
// <string> breaks if NULL is defined to "(void*)0".
# include <vcl_string.h>
#endif

#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>

#include <gtk/gtk.h>

struct vgui_overlay_helper;
class vgui_gtk_window;

//: The GTK implementation of vgui_adaptor.
class vgui_gtk_adaptor : public vgui_adaptor, public vgui_adaptor_mixin
{
 public:
  typedef vgui_adaptor_mixin mixin;

  vgui_gtk_adaptor(vgui_gtk_window* win = 0);
  ~vgui_gtk_adaptor();

  // vgui_adaptor methods
  void swap_buffers();
  void make_current();
  void post_redraw();
  void post_overlay_redraw();
  void post_timer(float,int);
  void post_destroy();  // schedules destruction of parent vgui_window

  unsigned get_width() const {return mixin::width;}
  unsigned get_height() const {return mixin::height;}
  void bind_popups(vgui_modifier m, vgui_button b)
  { mixin::popup_modifier = m; mixin::popup_button = b; }
  void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
  { m = mixin::popup_modifier; b = mixin::popup_button; }

  void set_default_popup(vgui_menu);
  vgui_menu get_popup();

  void draw();
  void reshape();

  // Returns NULL if the empty constructor was used
  vgui_window* get_window() const;

  // gtk stuff
  GtkWidget *get_glarea_widget() { return widget; }

 private:
  // main GDK-to-vgui event dispatcher
  static gint handle(GtkWidget *, GdkEvent *, gpointer);

  // idle callbacks which service pending redraw/destroy posts
  static gint idle_callback_for_redraw(gpointer data);
  static gint idle_callback_for_destroy(gpointer data);

  // Flags to prevent queuing of multiple redraw/destroy callbacks
  bool redraw_requested;
  bool destroy_requested;

  // pointer to the gtkglarea widget
  GtkWidget *widget;

  // pointer to the window which contains this adaptor
  vgui_gtk_window* win_;

  // pointer to overlay emulation data
  vgui_overlay_helper *ovl_helper;

  // This is a place to store any menu passed in,
  // so that it doesn't go out of scope while the popup is on screen.
  static vgui_menu last_popup;

  // last position where mouse was seen.
  int last_mouse_x, last_mouse_y;
};

#endif // vgui_gtk_adaptor_h_
