/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_test.h>
#include <vgui/vgui_manipulator.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_bitab.h>

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

  // bitab
  vgui_bitab bi(&view3d, &view2d);

  // run
#if 0
  vgui_shell_tableau shell(&bi);
  return vgui::run(&shell, 512, 256, "test_bitab");
#endif // 0
  return vgui::run(&bi, 512, 256, "test_bitab");
}
