#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_listmanager2D.h>

int main(int argc, char **argv)
{
  if (argc < 2) return 1;

  vgui::init(argc, argv);

  vgui_image_tableau image_tab(argv[1]);
  vgui_easy2D image(&image_tab);

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

  vgui_listmanager2D list;
  list.add(&image);
  list.add(&points);
  list.add(&lines);

  vgui_viewer2D viewer(&list);

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, 512, 512, "test_listmanager2d");
#endif // 0
  return vgui::run(&viewer, 512, 512, "test_listmanager2d");
}
