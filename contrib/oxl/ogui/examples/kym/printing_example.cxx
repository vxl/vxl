// 2d-example.cc
//

//#include <vbl/ArgParse.h> 
#include <vgui/vgui.h> 
#include <vgui/vgui_image_tableau.h> 
#include <vgui/vgui_easy2D.h> 
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc, char **argv)
{ 
  vgui::init(argc, argv);

  // Load an image into an image.tableau 
  vgui_image_tableau_new image( argc>1 ? argv[1] : "az32_10.tif" );
  //vgui_image_tableau image(argv[1]);

  // Put the image.tableau into a easy2D tableau 
  vgui_easy2D_new easy2D(image);

  // Add a point, line, and infinite line 
  easy2D->set_foreground(0,1,0); 
  easy2D->set_point_radius(5); 
  easy2D->add_point(10, 20);

  easy2D->set_foreground(0,0,1); 
  easy2D->set_line_width(2); 
  easy2D->add_line(100,100,200,400);

  easy2D->add_circle(100, 100, 50);

  // Put the easy2D tableau into a viewer2D tableau: 
  vgui_viewer2D_new viewer(easy2D);
  vgui_shell_tableau_new shell(viewer);

  easy2D->print_psfile("/tmp/kymfile.ps", 1, true);
  vcl_cout << "Attempting to print to /tmp/kymfile.ps" << vcl_endl;

  // Create a window, add the tableau and show it on screen:
  return vgui::run(shell, image->width(), image->height());
}
