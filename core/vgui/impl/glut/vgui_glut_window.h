// This is core/vgui/impl/glut/vgui_glut_window.h
#ifndef vgui_glut_window_h_
#define vgui_glut_window_h_
//:
// \file
// \author fsm
// \brief  The GLUT (OpenGL Utility Toolkit) implementation of vgui_window.

struct vgui_glut_adaptor;
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>

//: The GLUT (OpenGL Utility Toolkit) implementation of vgui_window.
struct vgui_glut_window : public vgui_window
{
  vgui_glut_window(const char * title, unsigned w, unsigned h, int posx = -1, int posy = -1);
  ~vgui_glut_window();

  vgui_menu menubar;
  void
  set_menubar(const vgui_menu & m)
  {
    menubar = m;
  }

  vgui_adaptor * glutwin;
  vgui_adaptor *
  get_adaptor()
  {
    return glutwin;
  }

  //
  void
  show();
  void
  hide();
  void
  iconify();
  void
  reshape(unsigned, unsigned);
  void
  reposition(int, int);
  void
  set_title(const std::string &);

private:
  bool pending_reshape;
  bool pending_reposition;
  int data0;
  int data1;

  friend struct vgui_glut_adaptor;
  void
  hello_from_adaptor();
};

#endif // vgui_glut_window_h_
