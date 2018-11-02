/*
  fsm
*/
#include "menu_hack.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// without X11, I don't know how to do this.
// fsm

void vgui_glut_menu_hack::bind(int button, int mods, int menu_id) { }

bool vgui_glut_menu_hack::mouse(int button, int state, int x,int y) { return false; }

void vgui_glut_menu_hack::menustatus(int status,int x,int y) { }

void (*vgui_glut_menu_hack::last_minute_change_callback)(int menu_id) = nullptr;
