#include <vgui/vgui.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_viewer2D.h>

int main(int argc, char **argv)
{
  if (argc < 2) return 1;

  vgui::init(argc, argv);

  vgui_image_tableau image(argv[1] ? argv[1] : "az32_10.tif");

  vgui_easy2D points("points");;
  points.set_point_radius(5);
  points.set_foreground(0,1,0);
  points.add_point(10, 20);

  points.set_foreground(1,0,1);
  points.add_point(10, 30);

  points.set_foreground(0,0,1);
  points.set_point_radius(5);
  points.add_point(30, 50);

  points.set_foreground(0,1,0);
  points.set_point_radius(10);
  points.add_point(50, 10);


  vgui_easy2D lines("lines");
  lines.set_foreground(0,1,0);
  lines.set_line_width(2);
  lines.add_infinite_line(1,1,-100);

  lines.set_foreground(0,0,1);
  lines.set_line_width(2);
  lines.add_line(100,100,200,400);

  vgui_composite comp(&image, &points, &lines);

  vgui_viewer2D viewer(&comp);

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, 512, 512, "test_composite2d");
#endif // 0
  return vgui::run(&viewer, 512, 512, "test_composite2d");
}
