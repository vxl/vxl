/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_manipulator.h>
#include <vgui/vgui_polytab.h>
#include <vgui/vgui_test.h>

int main(int argc,char **argv)
{
  vgui::init(argc,argv);

  // 2d
  vgui_test::thingy2d thing2d;
  vgui_manipulator man2d(&thing2d);
  man2d.default_2D();
  vgui_load view2d(&man2d); view2d.set_ortho(-1,-1, +1,+1);

  // 3d
  vgui_test::thingy3d thing3d;
  vgui_manipulator man3d(&thing3d);
  man3d.default_3D();
  vgui_load view3d(&man3d); view3d.set_ortho(-1,-1,-1, +1,+1,+1);

  // image
  vgui_image_tableau image(argc > 1 ? argv[1] : "az32_10.tif");
  vgui_manipulator manim(&image);
  manim.default_2D();
  vgui_load viewim(&manim);
  viewim.set_ortho(0,image.height(),
                   image.width(),0);

  // polytab
  vgui_polytab tab;
  //               x    y    w    h
  tab.add(&view3d, 0.0, 0.0, 0.5, 0.5);
  tab.add(&view2d, 0.5, 0.5, 0.5, 0.5);
  tab.add(&viewim, 0.3, 0.3, 0.3, 0.3);

  // context
  unsigned size=256;
#if 0
  vgui_shell_tableau shell(&tab);
  return vgui::run(&shell, 2*size, 2*size,__FILE__);
#endif // 0
  return vgui::run(&tab, 2*size, 2*size,__FILE__);
}
