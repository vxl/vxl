#ifndef vgui_glut_menu_hack_h_
#define vgui_glut_menu_hack_h_
// .LIBRARY vgui-glut
/*
  fsm
*/

struct vgui_glut_menu_hack
{
  // button is one of GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
  // mods is one of GLUT_ACTIVE_SHIFT, GLUT_ACTIVE_CTRL, GLUT_ACTIVE_ALT
  // unbind by using menu_id=0
  static void bind(int button, int mods, int menu_id);

  //
  static bool mouse(int button, int state, int x,int y);
  static void menustatus(int status,int x,int y);
  static void (*last_minute_change_callback)(int menu_id);

  struct per_window_record;

private:
  static bool active;
  static int  glut_button; // the relevant button.
  static int find_index(int button, int mods);// returns -1 on failure.
};

#endif // vgui_glut_menu_hack_h_
