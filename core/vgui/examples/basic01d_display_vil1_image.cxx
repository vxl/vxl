// This example displays an image in a window.  The image is loaded from
// the filename given as the first command line parameter.

#include <vcl_iostream.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  vil1_image img = vil1_load(argv[1]);

  // Load image (given in the first command line param) into an image tableau.
  vgui_image_tableau_new image(img);

  // Put the image tableau inside a 2D viewer tableau (for zoom, etc).
  vgui_viewer2D_tableau_new viewer(image);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, image->width(), image->height());
}
