/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_manipulator.h>
#include <vgui/vgui_test.h>

int main(int argc,char **argv)
{
  vgui::init(argc,argv);

  // 2D
  vgui_image_tableau image(argv[1] ? argv[1] : "az32_10.tif");

  vgui_manipulator man1(&image);
  man1.default_2D();

  vgui_load vs(&man1);
  vs.set_ortho(0,256, 256,0);

  // 3D
  vgui_test::thingy3d thing;

  vgui_manipulator man2(&thing);
  man2.default_3D();

  vgui_load vv(&man2);
  vv.set_ortho(-1,-1,-1, +1,+1,+1);

  // plug
#if 0
  vgui_shell_tableau shell1(&vs);
  vgui::adapt(&shell1, 256, 256, "first");
#endif // 0
  vgui::adapt(&vs,256, 256, "first" );

#if 0
  vgui_shell_tableau shell2(&vv);
  vgui::adapt(&shell2, 256, 256, "second");
#endif // 0
  vgui::adapt(&vv,256, 256, "second");

  // events
  return vgui::run();
}
