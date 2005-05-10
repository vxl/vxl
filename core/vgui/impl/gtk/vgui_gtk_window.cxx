// This is core/vgui/impl/gtk/vgui_gtk_window.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   18 Dec 99
// \brief  See vgui_gtk_window.h for a description of this file.

#include "vgui_gtk_window.h"

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>

#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>

#include "vgui_gtk_adaptor.h"
#include "vgui_gtk_utils.h"
#include "vgui_gtk_statusbar.h"

#include <vcl_compiler.h>

static bool debug = false;

extern "C" {

// capes@robots. Catch window-manager closing window.
// post_destroy on adaptor and block emission of "destroy" signal so that this window
// is not prematurely destroyed.
static gint delete_event_callback(GtkWidget* w, GdkEvent* e, gpointer data)
{
  vgui_gtk_adaptor* adaptor = static_cast<vgui_gtk_adaptor*>(data);

  adaptor->post_destroy();

  // Don't emit the "destroy" signal. The adaptor will take care of calling
  // gtk_widget_destroy on this window after it has disconnected and destroyed itself.
  return true;
}

} // extern "C"

//--------------------------------------------------------------------------------
//: Constructor
vgui_gtk_window::vgui_gtk_window(int w, int h, const char* title)
  : use_menubar(false)
  , use_statusbar(true)
  , statusbar(new vgui_gtk_statusbar)
  , last_menubar(new vgui_menu)
{
  if (debug) vcl_cerr << "vgui_gtk_window::vgui_gtk_window\n";

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_window_set_default_size(GTK_WINDOW(window),w,h);

  adaptor = new vgui_gtk_adaptor(this);

#ifndef VCL_SGI_CC // SGI's iostream does not allow re-initialising
  vgui::out.rdbuf(static_cast<vgui_gtk_statusbar*>(statusbar)->statusbuf);
#endif

  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
                     GTK_SIGNAL_FUNC(delete_event_callback),
                     static_cast<vgui_gtk_adaptor*>(adaptor));
}


//--------------------------------------------------------------------------------
//: Constructor
vgui_gtk_window::vgui_gtk_window(int w, int h, const vgui_menu& menu, const char* title)
  : use_menubar(true)
  , use_statusbar(true)
  , statusbar(new vgui_gtk_statusbar)
  , last_menubar(new vgui_menu)
{
  if (debug) vcl_cerr << "vgui_gtk_window::vgui_gtk_window\n";

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_window_set_default_size(GTK_WINDOW(window),w,h);

  adaptor = new vgui_gtk_adaptor(this);

  set_menubar(menu);

#ifndef VCL_SGI_CC // SGI's iostream does not allow re-initialising
  vgui::out.rdbuf(static_cast<vgui_gtk_statusbar*>(statusbar)->statusbuf);
#endif

  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
                     GTK_SIGNAL_FUNC(delete_event_callback),
                     static_cast<vgui_gtk_adaptor*>(adaptor));
}


//--------------------------------------------------------------------------------
//: Destructor
vgui_gtk_window::~vgui_gtk_window()
{
  gtk_widget_destroy(window);
  delete last_menubar;
  delete statusbar;
}


//--------------------------------------------------------------------------------
//: Useful initialisation functions
void vgui_gtk_window::init()
{
  box = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER (window), box);

  if (use_menubar) {
    gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, TRUE, 0);
    gtk_widget_show(menubar);
  }

  // place glarea inside a frame
  GtkWidget *frame = gtk_frame_new(0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
  gtk_container_set_border_width(GTK_CONTAINER(frame), 2);

  gtk_box_pack_start(GTK_BOX(box), frame, TRUE, TRUE, 0);
  gtk_widget_show(frame);


  // This re-parents the glarea widget, so the adaptor should yield
  // ownership.
  GtkWidget *glarea = (static_cast<vgui_gtk_adaptor*>(adaptor))->get_glarea_widget();
  gtk_container_add(GTK_CONTAINER(frame), glarea);
  gtk_widget_show(glarea);

  if (use_statusbar) {
    static_cast<vgui_gtk_statusbar*>(statusbar)->widget = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX(box), static_cast<vgui_gtk_statusbar*>(statusbar)->widget, FALSE, TRUE, 0);
    gtk_widget_show(static_cast<vgui_gtk_statusbar*>(statusbar)->widget);
  }

  gtk_widget_show(box);
}


//: Puts the given menubar onto the window.
void vgui_gtk_window::set_menubar(const vgui_menu &menu)
{
  if (debug) vcl_cerr << "vgui_gtk_window::set_menubar\n";

  use_menubar = true;

  // fsm - assign menu to 'last_menubar' to ensure the commands
  // stay in scope for the lifetime of the menubar :
  *last_menubar = menu;

  menubar = gtk_menu_bar_new();
  if (vgui_gtk_utils::accel_group == NULL)
  {
    vgui_gtk_utils::accel_group = gtk_accel_group_get_default();
    gtk_accel_group_attach(vgui_gtk_utils::accel_group,GTK_OBJECT(window));
  }
  vgui_gtk_utils::set_menu(menubar, *last_menubar, true);
}


void vgui_gtk_window::show()
{
  init();

  if (debug) vcl_cerr << "vgui_gtk_window::show\n";
  gtk_widget_show(window);
}

void vgui_gtk_window::hide()
{
  if (debug) vcl_cerr << "vgui_gtk_window::hide\n";
}

void vgui_gtk_window::set_title(vcl_string const &title)
{
  gtk_window_set_title(GTK_WINDOW(window), title.c_str());
}
