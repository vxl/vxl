/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_glut_window.h"
#include "vgui_glut_adaptor.h"
#include <vgui/vgui_glut.h>

vgui_glut_window::vgui_glut_window(char const *title,
                                   unsigned w, unsigned h,
                                   int posx VCL_DEFAULT_VALUE(-1), int posy VCL_DEFAULT_VALUE(-1))
  : vgui_window()
  , glutwin(0)
  , pending_reshape(false)
  , pending_reposition(false)
{
  // create window :
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize (w,h);
  glutInitWindowPosition(posx, posy);
  int window = glutCreateWindow( title ? title : __FILE__ );
  glutwin = new vgui_glut_adaptor(this, window);
  //glutHideWindow();
}

vgui_glut_window::~vgui_glut_window()
{
  delete glutwin;
  glutwin = 0;
}

vgui_adaptor* vgui_glut_window::get_adaptor()
{
  return glutwin;
}

void vgui_glut_window::show()
{
  int old = glutGetWindow();
  glutSetWindow( glutwin->get_id() );
  glutShowWindow();
  glutSetWindow( old );
}

void vgui_glut_window::hide()
{
  int old = glutGetWindow();
  glutSetWindow( glutwin->get_id() );
  glutHideWindow();
  glutSetWindow( old );
}

void vgui_glut_window::iconify()
{
  int old = glutGetWindow();
  glutSetWindow( glutwin->get_id() );
  glutIconifyWindow();
  glutSetWindow( old );
}

void vgui_glut_window::reshape(unsigned w, unsigned h)
{
  int old = glutGetWindow();
  glutSetWindow( glutwin->get_id() );
  glutReshapeWindow(w,h);
  glutSetWindow( old );
}

void vgui_glut_window::reposition(int x,int y)
{
  int old = glutGetWindow();
  glutSetWindow( glutwin->get_id() );
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
