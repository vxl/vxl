/*
  fsm@robots.ox.ac.uk
*/
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_spinner.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_test.h>

int main(int argc,char **argv) {
  vgui::init(argc,argv);

  vgui_test::thingy3d thing;

  vgui_spinner spin(&thing);

  vgui_load vv(&spin);
  vv.set_ortho(-1,-1,-1, +1,+1,+1);
  
  //vgui_shell_tableau shell(&vv);
  //return vgui::run(&shell, 512, 512, "test_spinner");
  return vgui::run(&vv, 512, 512, "test_spinner");
}
