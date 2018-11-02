// This example displays two images in a grid tableau.
// The images are loaded from the filenames given on
// the command line
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 2)
  {
    std::cerr << "Please give two image filenames on the command line\n";
    return 0;
  }
  // Load two images(given in the first command line param)
  // and construct separate image tableaux
  vgui_image_tableau_new image_tab1(argv[1]);
  vgui_image_tableau_new image_tab2(argv[2]);

  //Put the image tableaux into viewers
  vgui_viewer2D_tableau_new viewer1(image_tab1);
  vgui_viewer2D_tableau_new viewer2(image_tab2);

  //Put the viewers into a grid
  vgui_grid_tableau_sptr grid = new vgui_grid_tableau(2,1);
  grid->add_at(viewer1, 0,0);
  grid->add_at(viewer2, 1,0);
  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(grid);

  // Create a window, add the tableau and show it on screen.
  int width = image_tab1->width() + image_tab2->width();
  int height = image_tab1->height() + image_tab2->height();
  return vgui::run(shell, width, height);
}
