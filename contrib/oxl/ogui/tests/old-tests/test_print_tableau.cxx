/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_test.h>
#include <vgui/vgui_zoomer.h>
#include <vgui/vgui_spinner.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_function_tableau.h>
#include <vgui/vgui_print_tableau.h>
#include <vgui/vgui_gl.h>

bool display(vgui_event const &)
{
  glColor3f(1,1,1);
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0, 0, 0);
  glVertex3f( 1, 0, 0);
  glVertex3f( 1, 1, 0);
  glVertex3f( 1, 1, 1);
  glVertex3f( 1, 0, 1);
  glVertex3f( 1,-1, 1);
  glVertex3f( 0,-1, 1);
  glVertex3f( 0,-1, 0);
  glEnd();
  return true;
}

int main(int argc,char **argv)
{
  vgui::init(argc,argv);

  // 2d
  vgui_test::thingy2d thing2d;
  vgui_zoomer man2d(&thing2d);
  vgui_load view2d(&man2d);
  view2d.set_ortho(-1,-1, +1,+1);

  // 3d
  vgui_test::thingy3d thing3d;
  vgui_spinner man3d(&thing3d);
  vgui_load view3d(&man3d); 
  view3d.set_ortho(-1,-1,-1, +1,+1,+1);

  // function
  vgui_function_tableau function; function.draw( display );
  vgui_spinner fun3d(&function);
  vgui_load func3d(&fun3d); 
  func3d.set_ortho(-1,-1,-1, +1,+1,+1);

  // deck
  vgui_deck_tableau deck(&view2d, &view3d, &func3d);
  vgui_print_tableau print(&deck);

  // top
#if 0
  vgui_shell_tableau shell(&print);
  return vgui::run(&shell, 256, 256, __FILE__);
#endif // 0
  return vgui::run(&print, 256, 256, __FILE__);
}
