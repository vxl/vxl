// This example displays an image in a window.  The image is loaded from
// the filename given as the first command line parameter.

#include <vcl_iostream.h>

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_load.h>

#include <vgui/vgui.h> 
#include <vgui/vgui_vil2_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h> 

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line" << vcl_endl;
    return 0;
  }

  // Except for the following two statements, this example is exactly
  // the same as basic01_display_image

  // We load it as a byte image. There are more complex processing
  // techniques to load arbitrary image types, but that is an issue
  // for vil2, not vgui.
  //

  vil2_image_view< vxl_byte > b_im = vil2_load(argv[1]);

  // Load image (given in the first command line param) into an image tableau.
  vgui_vil2_image_tableau_new< vxl_byte > image(b_im);

  // Put the image tableau inside a 2D viewer tableau (for zoom, etc).
  vgui_viewer2D_tableau_new viewer(image);
 
  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);
  
  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, image->width(), image->height());
}
