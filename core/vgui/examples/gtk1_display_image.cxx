// This is an example using GTK instead of VGUI to make the user-interface.
//
// In this case we could do the same thing more simply using VGUI, but if
// we wanted to make it more complicated (eg. sliders, buttons) this would
// not be possible in VGUI.

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/impl/gtk/vgui_gtk_adaptor.h>
#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>

int main(int argc, char ** argv)
{
  // IMPORTANT NOTE, pass in "gtk" to select the gtk toolkit.
  vgui::select("gtk");
  vgui::init(argc, argv);

  // Set up the tableaux as before:
  vgui_image_tableau_new image(argc>1 ? argv[1] : "az32_10.tif");
  vgui_viewer2D_tableau_new viewer(image);

  // Plug into a GL context :
  //vgui_gtk_adaptor *ct = new vgui_gtk_adaptor("Gtk Window", 512,512,0,0);
  vgui_gtk_adaptor *ct = new vgui_gtk_adaptor();
  GtkWidget *glarea= (( vgui_gtk_adaptor *)ct)->get_glarea_widget();
  gtk_widget_set_usize(glarea, 512, 512);
  gtk_widget_show(glarea);

  ct->set_tableau(viewer);
  // Use GTK to create the window etc:
  GtkWidget *window= gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget *holderbox= gtk_vbox_new (FALSE, 0);

  gtk_box_pack_start (GTK_BOX (holderbox), glarea, TRUE, TRUE, 2);
  gtk_container_add(GTK_CONTAINER(window),GTK_WIDGET(holderbox));

  gtk_widget_show(holderbox); gtk_widget_show(window);
  return vgui::run();
}
