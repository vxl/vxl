//:
// \file
// \author Geoffrey Cross, Oxford RRG
// \date   09 Oct 99
//-----------------------------------------------------------------------------

#include <vgui/vgui_load.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_zoomer.h>
#include <vgui/vgui.h>

#include <vcl_cstdlib.h>

//////////////////////////////////////////
#include <vgui/impl/gtk/vgui_gtk_adaptor.h>

//////////////////////////////////////////
#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>


//////////////////////////////////////////

gint idle( gpointer data)
{
  //  vcl_cerr << "idle\n";
}

//////////////////////////////////////////

int main(int argc, char ** argv)
{
  vgui::init("gtk", argc,argv);
  //gtk_init(&argc, &argv);

  // at the bottom sits an image :
  vgui_image_tableau image((char*)0); // 0 means use the official vgui test image.

  // then a zoomer :
  vgui_zoomer zr(&image);
  //zr.delay_pan = false;

  // set initial image region :
  vgui_load vs(&zr);
  vs.set_image(image.width(), image.height());

  // plug into a GL adaptor :
  vgui_gtk_adaptor *ct1 = new vgui_gtk_adaptor;//("GTK Window",10,10,0,0);
  vgui_gtk_adaptor *ct2 = new vgui_gtk_adaptor;//("GTK Window",10,10,0,0);

  ///////////////////////////////////
  GtkWidget *window= gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *glarea1= (( vgui_gtk_adaptor *)ct1)->get_gtkwidget();
  GtkWidget *glarea2= (( vgui_gtk_adaptor *)ct2)->get_gtkwidget();
  GtkObject *adjustment= gtk_adjustment_new( 50, 0, 100, 1, 10, 0);
  GtkWidget *scale= gtk_hscale_new( GTK_ADJUSTMENT( adjustment));
  GtkWidget *box= gtk_vbox_new (FALSE, 0);

  gtk_box_pack_start (GTK_BOX (box), scale, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (box), glarea1, TRUE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (box), glarea2, TRUE, TRUE, 3);

  /* When window is resized viewport needs to be resized also. */
  gtk_widget_set_usize(GTK_WIDGET(glarea1), 200,200);
  gtk_widget_set_usize(GTK_WIDGET(glarea2), 200,200);

  gtk_container_add (GTK_CONTAINER (window), box);
  //  gtk_container_add(GTK_CONTAINER(window),GTK_WIDGET(glarea));

  gtk_widget_show(box);
  gtk_widget_show(scale);
  gtk_widget_show(glarea1);
  gtk_widget_show(glarea2);
  gtk_widget_show(window);

  ct1->set_tableau(&vs);
  ct2->set_tableau(&vs);

  return vgui::run();
}
