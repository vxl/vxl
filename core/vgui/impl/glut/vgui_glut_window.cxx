// \author fsm
#include "vgui_glut_window.h"
#include "vgui_glut_adaptor.h"
#include <vgui/vgui_glut.h>

vgui_glut_window::vgui_glut_window(char const *title,
                                   unsigned w, unsigned h,
                                   int posx, int posy)
  : vgui_window()
  , glutwin(0)
  , pending_reshape(false)
  , pending_reposition(false)
{
  // We have to be careful to remember what the current
  // window is before creating a new one because we might
  // be inside the event handler of another window and
  // when we return we will still expect that GL context
  // to be the active one.
  int old = glutGetWindow();

  // create new window:
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize (w,h);
  glutInitWindowPosition(posx, posy);
  int window = glutCreateWindow( title ? title : __FILE__ );
  glutwin = new vgui_glut_adaptor(this, window);
  //glutHideWindow();

  // if another window was already current when the new one
  // was created, switch back to the old one now:
  if (old != 0)
    glutSetWindow(old);
}

vgui_glut_window::~vgui_glut_window()
{
  delete glutwin;
  glutwin = 0;
}

void vgui_glut_window::show()
{
  int old = glutGetWindow();
  glutSetWindow( static_cast<vgui_glut_adaptor*>(glutwin)->get_id() );
  glutShowWindow();
  glutSetWindow( old );
}

void vgui_glut_window::hide()
{
  int old = glutGetWindow();
  glutSetWindow( static_cast<vgui_glut_adaptor*>(glutwin)->get_id() );
  glutHideWindow();
  glutSetWindow( old );
}

void vgui_glut_window::iconify()
{
  int old = glutGetWindow();
  glutSetWindow( static_cast<vgui_glut_adaptor*>(glutwin)->get_id() );
  glutIconifyWindow();
  glutSetWindow( old );
}

void vgui_glut_window::reshape(unsigned w, unsigned h)
{
  int old = glutGetWindow();
  glutSetWindow( static_cast<vgui_glut_adaptor*>(glutwin)->get_id() );
  glutReshapeWindow(w,h);
  glutSetWindow( old );
}

void vgui_glut_window::reposition(int x,int y)
{
  int old = glutGetWindow();
  glutSetWindow( static_cast<vgui_glut_adaptor*>(glutwin)->get_id() );
  glutPositionWindow(x,y);
  glutSetWindow( old );
}

void vgui_glut_window::set_title(vcl_string const &s)
{
  glutSetWindowTitle(s.c_str());
  glutSetIconTitle(s.c_str());
}

void vgui_glut_window::hello_from_adaptor()
{
}
