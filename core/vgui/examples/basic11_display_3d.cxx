// This example displays a point with 3 lines radiating from it in different
// non-coplanar directions (along the X, Y and Z axes).

#include <vgui/vgui.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  // Create an easy3D tableau:
  vgui_easy3D_tableau_new list3D;

  list3D->set_point_radius(15);
  // Add a point at the origin
  list3D->set_foreground(1,1,1);
  list3D->add_point(0,0,0);

  // Add a line in the xaxis:
  list3D->set_foreground(1,1,0);
  list3D->add_line(1,0,0, 4,0,0);
  list3D->add_point(5,0,0);

  // Add a line in the yaxis:
  list3D->set_foreground(0,1,0);
  list3D->add_line(0,1,0, 0,4,0);
  list3D->add_point(0,5,0);

  // Add a line in the zaxis:
  list3D->set_foreground(0,0,1);
  list3D->add_line(0,0,1, 0,0,4);
  list3D->add_point(0,0,5);

  // Add a small triangle
  list3D->set_foreground(1,0,0);
  list3D->add_triangle( 2,0,0, 0,2,0, 0,0,2 );

  // Add the easy3D tableau to a viewer3D tableau (this
  // gives us functionality to zoom and move the 3D object):
  vgui_viewer3D_tableau_new viewer(list3D);
  vgui_shell_tableau_new shell(viewer);

  return vgui::run(shell, 512, 512, "3d-example");
}
