#include <vgui/vgui.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_manipulator.h>

int main(int argc, char **argv)
{
  if (argc < 2) return 1;

  vgui::init(argc, argv);

  vgui_image_tableau image(argv[1]);

  vgui_easy2D easy2D(&image);

  easy2D.set_point_radius(5);
  easy2D.set_foreground(0,1,0);
  easy2D.add_point(10, 20);

  easy2D.set_foreground(1,0,1);
  easy2D.add_point(10, 30);

  easy2D.set_foreground(0,0,1);
  easy2D.set_point_radius(5);
  easy2D.add_point(30, 50);

  easy2D.set_foreground(0,1,0);
  easy2D.set_point_radius(10);
  easy2D.add_point(50, 10);

  easy2D.set_foreground(0,1,0);
  easy2D.set_line_width(2);
  easy2D.add_infinite_line(1,1,-100);

  easy2D.set_foreground(0,0,1);
  easy2D.set_line_width(2);
  easy2D.add_line(100,100,200,400);

  vgui_manipulator manip(&easy2D);
  manip.default_2D();

  vgui_load vs(&manip);
  vs.set_ortho(0,256, 256,0);

#if 0
  vgui_shell_tableau shell(&vs);
  return vgui::run(&shell, 256, 256, __FILE__);
#endif // 0
  return vgui::run(&vs, 256, 256, __FILE__);
}
