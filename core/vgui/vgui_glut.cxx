/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl_cstdlib.h>

extern "C" 
void vgui_glut_exit(int x)
{
  vcl_exit(x);
}
