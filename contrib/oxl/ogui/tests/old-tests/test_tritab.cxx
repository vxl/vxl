/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_test.h>
#include <vgui/vgui_manipulator.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_tritab.h>
#include <vgui/vgui_image_tableau.h>

int main(int argc,char **argv)
{
  vgui::init(argc,argv);

  // 2d
  vgui_test::thingy2d thing2d;
  vgui_manipulator man2d(&thing2d);
  man2d.default_2D();
  vgui_load view2d(&man2d);
  view2d.set_ortho(-1,-1, +1,+1);

  // 3d
  vgui_test::thingy3d thing3d;
  vgui_manipulator man3d(&thing3d);
  man3d.default_3D();
  vgui_load view3d(&man3d);
  view3d.set_ortho(-1,-1,-1, +1,+1,+1);

  // image
  vgui_image_tableau image(argv[1] ? argv[1] : "az32_10.tif");
  vgui_manipulator manim(&image);
  manim.default_2D();
  vgui_load viewim(&manim);
  viewim.set_ortho(0, image.height(),
                   image.width(), 0);

  // tritab
  vgui_tritab tri(&view3d, &view2d, &viewim);

  // top
  const unsigned int size = 256;
#if 0
  vgui_shell_tableau shell(&tri);
  return vgui::run(&shell, size*3, size, __FILE__);
#endif // 0
  return vgui::run(&tri, size*3, size, __FILE__);
}
