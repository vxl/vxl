// display-image.cxx
// 
// This example displays an image in a window.

#include <vgui/vgui.h> 
#include <vgui/vgui_image_tableau.h> 
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_shell_tableau.h> 

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  // Load an image into an image tableau: 
  vgui_image_tableau_new image( argv[1] ? argv[1] : "az32_10.tif" );

  // Put the image tableau inside a 2D viewer tableau: 
  vgui_viewer2D_new viewer(image);
  
  vgui_shell_tableau_new shell(viewer);
  
  // Create a window, add the tableau and show it on screen:
  return vgui::run(shell, image->width(), image->height());
}
