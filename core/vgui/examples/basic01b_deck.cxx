// This example displays two images in a deck.
// Images are loaded from the two filenames
// on the command line.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_deck_tableau.h>

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

  //Put the image tableaux into a deck
  vgui_deck_tableau_sptr deck = vgui_deck_tableau_new();
  deck->add(image_tab1);
  deck->add(image_tab2);

  vgui_viewer2D_tableau_new viewer(deck);

  // Put the deck into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  int width = std::max(image_tab1->width(), image_tab2->width());
  int height = std::max(image_tab1->height(), image_tab2->height());

  //Add 50 to account for window borders
  return vgui::run(shell, width+50, height+50);
}
