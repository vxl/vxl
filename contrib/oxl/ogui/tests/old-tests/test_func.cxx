//:
// \file
// \author Philip C. Pritchett, Oxford RRG
// \date   25 Nov 99
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui.h>
#include <vgui/vgui_function_tableau.h>

bool draw(const vgui_event&)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0.5, 0.5, 0.5, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0,0.0,0.0);
  glRectf(-.5,-.5,.5,.5);
  glColor3f(0.0,1.0,0.0);
  glRectf(-.4,-.4,.4,.4);
  glColor3f(0.0,0.0,1.0);
  glRectf(-.3,-.3,.3,.3);
  glFlush();

  return true;
}


bool motion(const vgui_event&)
{
  vcl_cerr << "motion\n";
  return true;
}

bool mouse_down(const vgui_event&)
{
  vcl_cerr << "mouse_down\n";
  return true;
}

bool mouse_up(const vgui_event&)
{
  vcl_cerr << "mouse_up\n";
  return true;
}

bool key_press(const vgui_event&)
{
  vcl_cerr << "key_press\n";
  return true;
}

bool help(const vgui_event&)
{
  vcl_cerr << "help\n";
  return true;
}

int main(int argc, char ** argv)
{
  vgui::init(argc, argv);

  vgui_function_tableau func;
  func.draw(draw);
  func.motion(motion);
  func.mouse_down(mouse_down);
  func.mouse_up(mouse_up);
  func.key_press(key_press);
  func.help(help);

#if 0
  vgui_shell_tableau shell(&func);
  return vgui::run(&shell, 512, 512);
#endif // 0
  return vgui::run(&func,512,512);
}
