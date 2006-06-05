//:
// \file
// \brief  This example draw a graph with graph tableau
// \author Kongbin Kang
// \date   2005-12-12
//

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_graph_tableau.h>
#include <vgui/vgui_shell_tableau.h>
//global pointers to the tableaux
static bgui_graph_tableau_sptr graph = 0;

//the meunu callback functions
static void create_graph()
{
  vcl_vector<double> pos(256);
  vcl_vector<double> vals(256);

  for(int i = 0; i<256; i++)
    {
      pos[i]=i;
      vals[i] = i*(255-i);
    }
  graph->update(pos, vals);
  vcl_string s = "Sample Graph";
  vgui_dialog* ip_dialog = graph->popup_graph(s);
  if (!ip_dialog->ask())
  {
    delete ip_dialog;
    return;
  }
}


// Create the edit menu
vgui_menu create_menus()
{
  vgui_menu edit;
  edit.add("Draw graph",create_graph,(vgui_key)'H',vgui_CTRL);
  edit.add("Quit", vgui::quit);
  vgui_menu bar;
  bar.add("Edit",edit);
  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  graph = bgui_graph_tableau_new(512,512);
  // Create and run the window


#ifdef HAS_GLUT
  vcl_cout << "HAS GLUT\n";
#endif
  vgui_text_tableau_sptr tt = vgui_text_tableau_new();
  tt->add(100,100,"hello world");
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(tt);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  return vgui::run(s, 512, 512, create_menus());
}
