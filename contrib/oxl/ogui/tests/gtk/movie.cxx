//:
// \file
// \author Geoffrey Cross, Oxford RRG
// \date   09 Oct 99
//-----------------------------------------------------------------------------

#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_zoomer.h>

#include <vgui/impl/gtk/vgui_gtk_adaptor.h>

#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>


//////////////////////////////////////////

int idlecallbackid= -1;
int timer= 30;

gint nextimage( GtkWidget *, gpointer deckptr )
{
  ((vgui_deck_tableau*)deckptr)->next();
  ((vgui_deck_tableau*)deckptr)->post_redraw();
  return TRUE;
}

gint previmage( GtkWidget *,
                gpointer   deckptr )
{
  ((vgui_deck_tableau*)deckptr)->prev();
  ((vgui_deck_tableau*)deckptr)->post_redraw();
  return TRUE;
}

gint idlecallback( gpointer context)
{
  nextimage( 0, context);
  return TRUE;
}

void slider( GtkAdjustment *adj,
             gpointer deckptr)
{
  timer= int(adj->value)+5;

  if ( idlecallbackid!= -1) {
      gtk_timeout_remove( idlecallbackid);
      idlecallbackid= gtk_timeout_add( timer, idlecallback, deckptr);
    }
}

gint playimage( GtkWidget *widget,
                gpointer   deckptr )
{
  idlecallbackid= gtk_timeout_add( timer, idlecallback, deckptr);
  return TRUE;
}

gint stopimage( GtkWidget *widget,
                gpointer   deckptr )
{
  gtk_timeout_remove( idlecallbackid);
  idlecallbackid= -1;
  return TRUE;
}


//////////////////////////////////////////

int main(int argc, char ** argv)
{
  if (argc < 11) return 1;

  vgui::init("gtk");
  gtk_init(&argc, &argv);
  vgui_deck_tableau deck;

  // Load the images into image tableaux and add them to the deck:
  for (int i=10; i<10; ++i) {
    vgui_image_tableau *image = new vgui_image_tableau(argv[i+1]);
    deck.add(image);
  }

  // then a zoomer :
  vgui_zoomer zr(&deck);

  // set initial image region :
  vgui_load vs(&zr);
  vs.set_image(400, 400);

#if 0
  vgui_shell_tableau shell(&vs);
#else
  vgui_tableau &shell(vs);
#endif // 0

  // plug into a GL context :
  vgui_gtk_adaptor *ct = new vgui_gtk_adaptor();
  GtkWidget *glarea= ct->get_gtkwidget();
  gtk_widget_set_usize(GTK_WIDGET(glarea), 400, 400);

  // and set up the context
  ct->set_tableau(&shell);

  ///////////////////////////////////
  GtkWidget *window= gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *buttonbox= gtk_hbox_new (FALSE, 0);
  GtkWidget *holderbox= gtk_vbox_new (FALSE, 0);

  GtkObject *adjustment= gtk_adjustment_new( timer, 0, 300, 1, 10, 0);
  GtkWidget *scale= gtk_hscale_new( GTK_ADJUSTMENT( adjustment));

  GtkWidget *prevbutton= gtk_button_new_with_label ("Prev");
  GtkWidget *nextbutton= gtk_button_new_with_label ("Next");
  GtkWidget *playbutton= gtk_button_new_with_label ("Play");
  GtkWidget *stopbutton= gtk_button_new_with_label ("Stop");

  ///////////////////////////////////
  gtk_box_pack_start (GTK_BOX (buttonbox), prevbutton, TRUE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (buttonbox), nextbutton, TRUE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (buttonbox), stopbutton, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (buttonbox), playbutton, TRUE, TRUE, 3);

  gtk_box_pack_start (GTK_BOX (holderbox), buttonbox, TRUE, TRUE, 4);
  gtk_box_pack_start (GTK_BOX (holderbox), scale, TRUE, TRUE, 4);
  gtk_box_pack_start (GTK_BOX (holderbox), glarea, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window),GTK_WIDGET(holderbox));

  ///////////////////////////////////
  gtk_scale_set_digits (GTK_SCALE (scale), 0);

  ///////////////////////////////////
  gtk_signal_connect (GTK_OBJECT (nextbutton), "clicked",
                      GTK_SIGNAL_FUNC (nextimage), &deck);

  gtk_signal_connect (GTK_OBJECT (prevbutton), "clicked",
                      GTK_SIGNAL_FUNC (previmage), &deck);

  gtk_signal_connect (GTK_OBJECT (playbutton), "clicked",
                      GTK_SIGNAL_FUNC (playimage), &deck);

  gtk_signal_connect (GTK_OBJECT (stopbutton), "clicked",
                      GTK_SIGNAL_FUNC (stopimage), &deck);

  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      GTK_SIGNAL_FUNC (slider), &deck);

  gtk_widget_show(nextbutton);
  gtk_widget_show(prevbutton);
  gtk_widget_show(playbutton);
  gtk_widget_show(stopbutton);
  gtk_widget_show(scale);
  gtk_widget_show(buttonbox);
  gtk_widget_show(holderbox);
  gtk_widget_show(glarea);
  gtk_widget_show(window);

  return vgui::run();
}
