// 2d-example.cxx
//

#include <vgui/vgui.h> 
#include <vgui/vgui_image_tableau.h> 
#include <vgui/vgui_easy2D.h> 
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc, char **argv)
{ 
  vgui::init(argc, argv);

  // Load an image into an image.tableau 
  vgui_image_tableau_new image(argv[1] ? argv[1] : "az32_10.tif");

  // Put the image.tableau into a easy2D tableau 
  vgui_easy2D_new easy2D(image);

  // Add a point, line, and infinite line 
  easy2D->set_foreground(0,1,0); 
  easy2D->set_point_radius(5); 
  easy2D->add_point(10, 20);

  easy2D->set_foreground(0,0,1); 
  easy2D->set_line_width(2); 
  easy2D->add_line(100,100,200,400);

  easy2D->set_foreground(0,1,0); 
  easy2D->set_line_width(2); 
  easy2D->add_infinite_line(1,1,-100);

  // Put the easy2D tableau into a viewer2D tableau: 
  vgui_viewer2D_new viewer(easy2D);
  vgui_shell_tableau_new shell(viewer);


  // Create a window, add the tableau and show it on screen:
  return vgui::run(shell, image->width(), image->height());
}
