/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_zoomer.h>
#include <vgui/vgui_proj.h>
#include <vgui/vgui_tritab.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_test.h>

int main(int argc,char **argv)
{
  vgui::init(argc,argv);

  vgui_test::thingy2d th;

  vgui_zoomer zoom(&th);

  // adapt the tableau 'tab' into three other tableaux :
  vgui_proj proj(&zoom);

  // set modelview matrices :
  vgui_load xyw(proj.get_xyw()); xyw.set_ortho(-1,-1, +1,+1);
  vgui_load ywx(proj.get_ywx()); ywx.set_ortho(-1,-1, +1,+1);
  vgui_load wxy(proj.get_wxy()); wxy.set_ortho(-1,-1, +1,+1);

  // contexts
  unsigned size = 200;

  // 3 tableaux in one window
  vgui_tritab tri(&xyw, &ywx, &wxy);

#if 0
  vgui_shell_tableau shell(&tri);
  return vgui::run(&shell, 3*size, size, __FILE__);
#endif // 0
  return vgui::run(&tri, 3*size, size, __FILE__);
}
