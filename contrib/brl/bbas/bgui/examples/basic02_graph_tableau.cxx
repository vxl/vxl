//:
// \file
// \brief  This example draw a graph with graph tableau
// \author Kongbin Kang
// \date   2005-12-12
//

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_graph_tableau.h>
#include <vgui/vgui_shell_tableau.h>
//global pointer to the rubberband tableau
static bgui_graph_tableau_sptr graph = 0;
static vgui_easy2D_tableau_sptr easy = 0;

//the meunu callback functions
static void create_graph()
{
  vcl_vector<double> data(256);

  for(int i = 0; i<256; i++)
    data[i] = i*(255-i);

  graph->update(data);

}


// Create the edit menu
vgui_menu create_menus()
{
  vgui_menu edit;
  edit.add("Draw histgram",create_graph,(vgui_key)'H',vgui_CTRL);
  vgui_menu bar;
  bar.add("Edit",edit);
  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);

  // Make the tableau hierarchy.
  //easy = vgui_easy2D_tableau_new(image);
  graph = bgui_graph_tableau_new();
  vgui_viewer2D_tableau_new viewer(graph);
  vgui_shell_tableau_new shell(viewer);

  // Create and run the window
  return vgui::run(shell, 512, 512, create_menus());
}
