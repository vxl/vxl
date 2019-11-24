// This is core/vidl/examples/vidl_player_menus.cxx
#include "vidl_player_menus.h"
#include "vidl_player_manager.h"
#include "vgui/vgui.h"
#include "vgui/vgui_key.h"
#include "vgui/vgui_menu.h"


//----- Static menu callback functions -----

void vidl_player_menus::open_istream_callback()
{
  vidl_player_manager::instance()->open_istream();
}


void vidl_player_menus::close_istream_callback()
{
  vidl_player_manager::instance()->close_istream();
}


void vidl_player_menus::open_ostream_callback()
{
  vidl_player_manager::instance()->open_ostream();
}


void vidl_player_menus::close_ostream_callback()
{
  vidl_player_manager::instance()->close_ostream();
}


void vidl_player_menus::pipe_streams_callback()
{
  vidl_player_manager::instance()->pipe_streams();
}


void vidl_player_menus::quit_callback()
{
  vidl_player_manager::instance()->quit();
}


void vidl_player_menus::pause_video_callback()
{
  vidl_player_manager::instance()->pause_video();
}


void vidl_player_menus::next_frame_callback()
{
  vidl_player_manager::instance()->next_frame();
}


void vidl_player_menus::prev_frame_callback()
{
  vidl_player_manager::instance()->prev_frame();
}


void vidl_player_menus::go_to_frame_callback()
{
  vidl_player_manager::instance()->go_to_frame();
}


void vidl_player_menus::play_video_callback()
{
  vidl_player_manager::instance()->play_video();
}


void vidl_player_menus::stop_video_callback()
{
  vidl_player_manager::instance()->stop_video();
}


//----- vidl_player_menus definition -----

vgui_menu vidl_player_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuvstream;
  vgui_menu menuview;

  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  menuvstream.add( "Open Input",   open_istream_callback);
  menuvstream.add( "Close Input",  close_istream_callback);
  menuvstream.separator();
  menuvstream.add( "Open Output",  open_ostream_callback);
  menuvstream.add( "Close Output", close_ostream_callback);
  menuvstream.separator();
  menuvstream.add( "Input to Output", pipe_streams_callback);

  //view menu entries
  menuview.add( "Play", play_video_callback,(vgui_key)'p', vgui_CTRL);
  menuview.add( "Pause", pause_video_callback,(vgui_key)'p');
  menuview.add( "Next", next_frame_callback,(vgui_key)'f');
  menuview.add( "Prev", prev_frame_callback,(vgui_key)'b');
  menuview.add( "Go to Frame", go_to_frame_callback,(vgui_key)'g', vgui_CTRL);
  menuview.add( "Stop", stop_video_callback,(vgui_key)'s', vgui_CTRL);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "Video Stream", menuvstream);
  menubar.add( "View", menuview);
  return menubar;
}
