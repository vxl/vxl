#include "brct_menus.h"

#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_menu.h>
#include "brct_windows_frame.h"

//Static munu callback functions

void brct_menus::quit_callback()
{
  brct_windows_frame::instance()->quit();
}

void brct_menus::init_kalman_callback()
{
  brct_windows_frame::instance()->init_kalman();
}

//bmvv_menus definition
vgui_menu brct_menus::get_menu()
{
  vgui_menu menu_bar;
  vgui_menu file_menu;
  vgui_menu action_menu;

  file_menu.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  action_menu.add("init", init_kalman_callback, (vgui_key)'i', vgui_CTRL);
  action_menu.add("go", kalman_go_callback, (vgui_key)'g', vgui_CTRL);

  menu_bar.add("file", file_menu);
  menu_bar.add("action", action_menu);
  return menu_bar;
}

void brct_menus::kalman_go_callback()
{
  vcl_cerr<<"goes into kalman_go_callback\n";
  vcl_vector<vgl_point_3d<double> > curve;
  curve.push_back(vgl_point_3d<double> (25, 25, 25));
  curve.push_back(vgl_point_3d<double> (50, 50, 50));
  brct_windows_frame::instance()->add_curve3d(curve);
}
