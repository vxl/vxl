//:
// \file
// This is a more complicated example using GTK to make our GUI instead
// of VGUI (apart from the OpenGL area).
//
// It would not be possible to write this example using VGUI because VGUI
// does not have buttons.

#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui.h>

#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/impl/gtk/vgui_gtk_adaptor.h>
#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>

int idlecallbackid= -1;
int timer= 30;

gint idlecallback( gpointer context)
{
  ((vgui_deck_tableau*)context)->next();
  ((vgui_deck_tableau*)context)->post_redraw();
  return TRUE;
}

gint playimage(GtkWidget*, gpointer deckptr)
{
  idlecallbackid = gtk_timeout_add(timer, idlecallback, deckptr);
  return TRUE;
}

gint stopimage(GtkWidget*, gpointer deckptr)
{
  gtk_timeout_remove(idlecallbackid);
  idlecallbackid= -1;
  return TRUE;
}

int main(int argc, char ** argv)
{
  if (argc < 2) { vcl_cerr << "Usage: " << argv[0] << " img1 [img2 ... ]\n"; return 1; }
  vgui::select("gtk");
  vgui::init(argc, argv);

  vgui_deck_tableau_new deck;

  // Load the images into image tableaux and add them to the deck:
  for (int i=1; i<argc; ++i)
  {
    vgui_image_tableau_new image(argv[i]);
    deck->add(image);
  }

  // then a zoomer :
  vgui_viewer2D_tableau_new zr(deck);

  // plug into a GL context :
  vgui_gtk_adaptor *ct = new vgui_gtk_adaptor;
  //vgui_gtk_adaptor *ct = vgui_gtk_adaptor();
  GtkWidget *glarea= ct->get_glarea_widget();
  gtk_widget_set_usize(GTK_WIDGET(glarea), 400, 400);

  // and set up the context
  ct->set_tableau(zr);

  GtkWidget *window= gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *buttonbox= gtk_hbox_new (FALSE, 0);
  GtkWidget *holderbox= gtk_vbox_new (FALSE, 0);
  GtkWidget *playbutton= gtk_button_new_with_label ("Play");
  GtkWidget *stopbutton= gtk_button_new_with_label ("Stop");

  gtk_box_pack_start (GTK_BOX (buttonbox), stopbutton, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (buttonbox), playbutton, TRUE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (holderbox), buttonbox, TRUE, TRUE, 4);
  gtk_box_pack_start (GTK_BOX (holderbox), glarea, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window),GTK_WIDGET(holderbox));
  gtk_signal_connect (GTK_OBJECT (playbutton), "clicked", GTK_SIGNAL_FUNC (playimage), &deck);
  gtk_signal_connect (GTK_OBJECT (stopbutton), "clicked", GTK_SIGNAL_FUNC (stopimage), &deck);

  gtk_widget_show(playbutton);
  gtk_widget_show(stopbutton);
  gtk_widget_show(buttonbox);
  gtk_widget_show(holderbox);
  gtk_widget_show(glarea);
  gtk_widget_show(window);

  return vgui::run();
}
