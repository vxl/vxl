#include "brct_menus.h"

#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
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
  vgui_menu debug_menu;

  file_menu.add("save status", save_status, (vgui_key)'s', vgui_CTRL);
  file_menu.add("load status", load_status, (vgui_key)'o', vgui_CTRL);
  file_menu.add("write vrml", write_vrml_file, (vgui_key)'w', vgui_CTRL);
  file_menu.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  action_menu.add("create line", create_line, (vgui_key)'n', vgui_CTRL);
  action_menu.add("init", init_kalman_callback, (vgui_key)'i', vgui_CTRL);
  action_menu.add("go", kalman_go_callback, (vgui_key)'g', vgui_CTRL);

  debug_menu.add("show prediction", show_predicted_curves, (vgui_key)'p', vgui_CTRL);
  debug_menu.add("show next observes", show_next_observes, (vgui_key)'o', vgui_CTRL);
  debug_menu.add("show back projection", show_back_projection, (vgui_key)'b', vgui_CTRL);
  debug_menu.add("remove debug info", remove_debug_info, (vgui_key)'r', vgui_CTRL);
  debug_menu.add("load image", load_image, (vgui_key)'l', vgui_CTRL);
  debug_menu.add("show epipole", show_epipole, (vgui_key)'e', vgui_CTRL);

  menu_bar.add("file", file_menu);
  menu_bar.add("action", action_menu);
  menu_bar.add("debug", debug_menu);
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

void brct_menus::show_next_observes()
{
  brct_windows_frame::instance()->show_next_observes();
}

void brct_menus::remove_debug_info()
{
  brct_windows_frame::instance()->remove_debug_info();
}

void brct_menus::show_back_projection()
{
  brct_windows_frame::instance()->show_back_projection();
}

void brct_menus::load_image()
{
  brct_windows_frame::instance()->load_image();
}

void brct_menus::show_epipole()
{
  brct_windows_frame::instance()->show_epipole();
}

void brct_menus::create_line()
{
  brct_windows_frame::instance()->creat_line();
}

void brct_menus::load_status()
{
  brct_windows_frame::instance()->load_status();
}

void brct_menus::save_status()
{
  brct_windows_frame::instance()->save_status();
}

void brct_menus::write_vrml_file()
{
  brct_windows_frame::instance()->write_vrml_file();
}
