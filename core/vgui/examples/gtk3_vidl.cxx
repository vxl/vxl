// Example using GTK to make a user-interface and vidl_vil1 to load a movie.

#include <vul/vul_sprintf.h>
#include <vgui/vgui.h>

#include <vgui/vgui_loader_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/impl/gtk/vgui_gtk_adaptor.h>
#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>

#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vidl_vil1/vidl_vil1_movie.h>

int idlecallbackid= -1;
int timer= 30;
vidl_vil1_movie_sptr my_movie;
vidl_vil1_movie::frame_iterator pframe(my_movie);

gint idlecallback( gpointer context)
{
  if (pframe == my_movie->last())
    pframe = my_movie->first();
  ++pframe;

  vil1_image im = pframe->get_image();
  ((vgui_image_tableau*)context)->set_image(im);
  ((vgui_image_tableau*)context)->post_redraw();
  return TRUE;
}

gint playimage(GtkWidget*, gpointer ptr)
{
  idlecallbackid = gtk_timeout_add(timer, idlecallback, ptr);
  return TRUE;
}

gint stopimage(GtkWidget*, gpointer)
{
  gtk_timeout_remove(idlecallbackid);
  idlecallbackid= -1;
  return TRUE;
}

gint quit(GtkWidget*, gpointer)
{
  vgui::quit();
  return FALSE;
}

int main(int argc, char ** argv)
{
  vgui::select("gtk");
  vgui::init(argc, argv);

  vcl_list<vcl_string> img_filenames;
  if (argc <= 1)
    for (int i=1; i<10; ++i)
    {
      vul_sprintf name("bt.%03d.quarter.tif", i);
      img_filenames.push_back(name);
    }
  else
    for (int i=1; i<argc; ++i)
      img_filenames.push_back(argv[i]);

  vcl_cout << argv[0] << ": playing " << img_filenames.size() << " frames\n";

  my_movie = vidl_vil1_io::load_movie(img_filenames);
  pframe = my_movie->first();

  vil1_image im = pframe->get_image();
  vgui_image_tableau_new img_tab(im);

  // set initial image region :
  vgui_loader_tableau_new vs(img_tab);
  vs->set_ortho(0,im.height(),im.width(),0);

  // plug into a GL context :
  vgui_gtk_adaptor *ct = new vgui_gtk_adaptor;
  GtkWidget *glarea= ct->get_glarea_widget();
  gtk_widget_set_usize(GTK_WIDGET(glarea), im.width(), im.height());

  // and set up the context
  ct->set_tableau(vs);

  GtkWidget *window= gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *buttonbox= gtk_hbox_new (FALSE, 0);
  GtkWidget *holderbox= gtk_vbox_new (FALSE, 0);
  GtkWidget *playbutton= gtk_button_new_with_label ("Play");
  GtkWidget *stopbutton= gtk_button_new_with_label ("Stop");
  GtkWidget *quitbutton= gtk_button_new_with_label ("Quit");

  gtk_box_pack_start (GTK_BOX (buttonbox), stopbutton, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (buttonbox), playbutton, TRUE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (buttonbox), quitbutton, TRUE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (holderbox), buttonbox, TRUE, TRUE, 4);
  gtk_box_pack_start (GTK_BOX (holderbox), glarea, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window),GTK_WIDGET(holderbox));
  gtk_signal_connect (GTK_OBJECT (playbutton), "clicked", GTK_SIGNAL_FUNC (playimage), &(*img_tab));
  gtk_signal_connect (GTK_OBJECT (stopbutton), "clicked", GTK_SIGNAL_FUNC (stopimage), &(*img_tab));
  gtk_signal_connect (GTK_OBJECT (quitbutton), "clicked", GTK_SIGNAL_FUNC (quit), &(*img_tab));

  gtk_widget_show(playbutton);
  gtk_widget_show(stopbutton);
  gtk_widget_show(quitbutton);
  gtk_widget_show(buttonbox);
  gtk_widget_show(holderbox);
  gtk_widget_show(glarea);
  gtk_widget_show(window);

  vgui::run();
}
