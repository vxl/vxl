#ifndef vgui_impl_glut_h_
#define vgui_impl_glut_h_
// .NAME vgui_glut
// .LIBRARY vgui-glut
// .INCLUDE vgui/impl/glut/vgui_glut.h
// .FILE vgui_glut.cxx
// @author fsm@robots.ox.ac.uk

#include <vgui/vgui_toolkit.h>

struct vgui_glut : vgui_toolkit
{
  vgui_glut();
  ~vgui_glut();
  
  void init(int &, char **);
  vcl_string name() const;
  
  vgui_window* produce_window(int width, int height, 
				   vgui_menu const & menubar,
				   char const* title);
  
  vgui_window* produce_window(int width, int height, 
				   char const* title);
  
  void run();
  void run_one_event();
  void run_till_idle();
  void quit();
  
private:
  static int count;
};

#endif // vgui_impl_glut_h_
