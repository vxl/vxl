// This example displays an image in a window.  The image is loaded from
// the filename given as the first command line parameter.

#include <vcl_iostream.h>

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

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
    return 1;
  }

  // Except for the following two statements, this example is exactly
  // the same as basic01_display_image

  // We load it as a byte image. There are more complex processing
  // techniques to load arbitrary image types, but that is an issue
  // for vil, not vgui.
  //

  vil_image_resource_sptr im = vil_load_image_resource(argv[1]);

  if( !im )
  {
    vcl_cerr << "Could not load " << argv[1] << "\n";
    return 1;
  }
    

  // Load image (given in the first command line param) into an image tableau.
  vgui_image_tableau_new image(im);

  // Put the image tableau inside a 2D viewer tableau (for zoom, etc).
  vgui_viewer2D_tableau_new viewer(image);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, image->width(), image->height());
}
