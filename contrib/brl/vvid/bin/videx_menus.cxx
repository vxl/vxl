#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_menu.h>
#include <vvid/vvid_live_video_manager.h>
#include <vvid/bin/videx_menus.h>

//Static munu callback functions

void videx_menus::quit_callback()
{
  vvid_live_video_manager::instance()->quit();
}


void videx_menus::set_camera_params_callback()
{
  vvid_live_video_manager::instance()->set_camera_params();
}

void videx_menus::set_detection_params_callback()
{
  vvid_live_video_manager::instance()->set_detection_params();
}

void videx_menus::no_op_callback()
{
  vvid_live_video_manager::instance()->no_op();
}

void videx_menus::start_live_video_callback()
{
  vvid_live_video_manager::instance()->start_live_video();
}

void videx_menus::stop_live_video_callback()
{
  vvid_live_video_manager::instance()->stop_live_video();
}

void videx_menus::toggle_histogram_callback()
{
  vvid_live_video_manager::instance()->toggle_histogram();
}

void videx_menus::capture_sequence_callback()
{
  vvid_live_video_manager::instance()->capture_sequence();
}

void videx_menus::init_capture_callback()
{
  vvid_live_video_manager::instance()->init_capture();
}

void videx_menus::stop_capture_callback()
{
  vvid_live_video_manager::instance()->stop_capture();
}


//videx_menus definition
vgui_menu videx_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Capture(old)", capture_sequence_callback);
  menufile.add( "Init Capture", init_capture_callback, (vgui_key)'i');
  menufile.add( "Stop Capture", stop_capture_callback, (vgui_key)'e');

  //view menu entries
  menuview.add( "Start Live Video", start_live_video_callback, (vgui_key)'s');
  menuview.add( "Stop Live Video", stop_live_video_callback, (vgui_key)'s', vgui_CTRL);
  menuview.add( "Toggle Histogram", toggle_histogram_callback, (vgui_key)'h');

  //edit menu entries
  menuedit.add("Camera Settings", set_camera_params_callback, (vgui_key)'p', vgui_CTRL);
  menuedit.add("Edge Detection Settings", set_detection_params_callback);
  menuedit.add("No Live Operation", no_op_callback);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}

