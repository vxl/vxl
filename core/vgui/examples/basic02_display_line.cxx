// This example displays a line in an OpenGL area in the window:

#include <vgui/vgui.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  // Use an easy2D tableau which allows us to draw 2D objects:
  vgui_easy2D_tableau_new easy2D("unnamed");

  // Add a line:
  easy2D->set_foreground(0,0,1);
  easy2D->set_line_width(2);
  easy2D->add_line(100,100,200,400);

  vgui_viewer2D_tableau_new viewer(easy2D);
  vgui_shell_tableau_new shell(viewer);
  return vgui::run(shell, 512, 512);
}
