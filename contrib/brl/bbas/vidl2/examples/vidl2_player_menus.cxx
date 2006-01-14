// This is brl/bbas/vidl2/examples/vidl2_player_menus.cxx
#include "vidl2_player_menus.h"
#include "vidl2_player_manager.h"
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>


//----- Static menu callback functions -----

void vidl2_player_menus::open_image_list_istream_callback()
{
  vidl2_player_manager::instance()->open_image_list_istream();
}


#ifdef HAS_FFMPEG
void vidl2_player_menus::open_ffmpeg_istream_callback()
{
  vidl2_player_manager::instance()->open_ffmpeg_istream();
}
#endif


#ifdef HAS_DC1394
void vidl2_player_menus::open_dc1394_istream_callback()
{
  vidl2_player_manager::instance()->open_dc1394_istream();
}
#endif


void vidl2_player_menus::close_istream_callback()
{
  vidl2_player_manager::instance()->close_istream();
}


void vidl2_player_menus::open_image_list_ostream_callback()
{
  vidl2_player_manager::instance()->open_image_list_ostream();
}


#ifdef HAS_FFMPEG
void vidl2_player_menus::open_ffmpeg_ostream_callback()
{
  vidl2_player_manager::instance()->open_ffmpeg_ostream();
}
#endif


void vidl2_player_menus::close_ostream_callback()
{
  vidl2_player_manager::instance()->close_ostream();
}


void vidl2_player_menus::pipe_streams_callback()
{
  vidl2_player_manager::instance()->pipe_streams();
}


void vidl2_player_menus::quit_callback()
{
  vidl2_player_manager::instance()->quit();
}


void vidl2_player_menus::pause_video_callback()
{
  vidl2_player_manager::instance()->pause_video();
}


void vidl2_player_menus::next_frame_callback()
{
  vidl2_player_manager::instance()->next_frame();
}


void vidl2_player_menus::prev_frame_callback()
{
  vidl2_player_manager::instance()->prev_frame();
}


void vidl2_player_menus::go_to_frame_callback()
{
  vidl2_player_manager::instance()->go_to_frame();
}


void vidl2_player_menus::play_video_callback()
{
  vidl2_player_manager::instance()->play_video();
}


void vidl2_player_menus::stop_video_callback()
{
  vidl2_player_manager::instance()->stop_video();
}


//----- vidl2_player_menus definition -----

vgui_menu vidl2_player_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuvstream;
  vgui_menu menuview;

  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  vgui_menu menu_istream;
  menu_istream.add( "Image List",  open_image_list_istream_callback);
#ifdef HAS_FFMPEG
  menu_istream.add( "FFMPEG",      open_ffmpeg_istream_callback);
#endif
#ifdef HAS_DC1394
  menu_istream.add( "dc1394",      open_dc1394_istream_callback);
#endif

  vgui_menu menu_ostream;
  menu_ostream.add( "Image List",  open_image_list_ostream_callback);
#ifdef HAS_FFMPEG
  menu_ostream.add( "FFMPEG",      open_ffmpeg_ostream_callback);
#endif

  menuvstream.add( "Open Input",   menu_istream);
  menuvstream.add( "Close Input",  close_istream_callback);
  menuvstream.separator();
  menuvstream.add( "Open Output",  menu_ostream);
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
