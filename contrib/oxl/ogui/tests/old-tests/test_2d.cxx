// easy switch between viewer2D and zoomer :
#define FSM 0

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D.h>
#if FSM
# include <vgui/vgui_zoomer.h>
# include <vgui/vgui_load.h>
#else
# include <vgui/vgui_viewer2D.h>
#endif


int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  vgui_image_tableau image( argv[1] ? argv[1] : "az32_10.tif" );

  vgui_easy2D easy2D(&image);

  easy2D.set_point_radius(5);
  easy2D.set_foreground(0,1,0); easy2D.add_point(10, 20);
  easy2D.set_foreground(1,0,1); easy2D.add_point(10, 30);
  easy2D.set_foreground(0,0,1); easy2D.add_point(30, 50);

  easy2D.set_point_radius(10);
  easy2D.set_foreground(0,1,0); easy2D.add_point(50, 10);

  easy2D.set_line_width(2);
  easy2D.set_foreground(0,1,0); easy2D.add_infinite_line(1,1,-100);
  easy2D.set_foreground(0,0,1); easy2D.add_line(100,100,200,400);

#if FSM
  vgui_zoomer zoom(&easy2D);
  vgui_load viewer(&zoom); viewer.set_ortho(0,512, 512,0);
#else
  vgui_viewer2D viewer(&easy2D);
#endif

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, 512, 512, "test_2d");
#endif // 0
  return vgui::run(&viewer, 512, 512, "test_2d");
}
