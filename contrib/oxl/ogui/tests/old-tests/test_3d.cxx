#include <vgui/vgui.h>
#include <vgui/vgui_easy3D.h>
#include <vgui/vgui_viewer3D.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);


  vgui_easy3D list3D;

  list3D.set_point_radius(15);

  // origin
  list3D.set_foreground(1,1,1);
  list3D.add_point(0,0,0);

  // xaxis
  list3D.set_foreground(1,1,0);
  list3D.add_line(1,0,0, 4,0,0);
  list3D.add_point(5,0,0);

  // yaxis
  list3D.set_foreground(0,1,0);
  list3D.add_line(0,1,0, 0,4,0);
  list3D.add_point(0,5,0);

  // zaxis
  list3D.set_foreground(0,0,1);
  list3D.add_line(0,0,1, 0,0,4);
  list3D.add_point(0,0,5);


  vgui_viewer3D viewer(&list3D);

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, 512, 512);
#endif // 0
  return vgui::run(&viewer,512,512);
}


