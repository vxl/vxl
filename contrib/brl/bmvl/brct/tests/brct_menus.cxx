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
  action_menu.add("show prediction", show_predicted_curves, (vgui_key)'p', vgui_CTRL);
  menu_bar.add("file", file_menu);
  menu_bar.add("action", action_menu);
  return menu_bar;
}

void brct_menus::kalman_go_callback()
{
  vcl_cerr<<"goes into kalman_go_callback\n";
  brct_windows_frame::instance()->go();
}

void brct_menus::show_predicted_curves()
{
  vcl_cerr<<"goes into predicted_curves\n";
  brct_windows_frame::instance()->show_predicted_curve();
}
