// This is core/vidl1/examples/vidl1_player_menus.cxx
#include "vidl1_player_menus.h"
#include "vidl1_player_manager.h"
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>

#ifdef HAS_MPEG2
#include <vidl1/vidl1_mpegcodec.h>
#include <vidl1/vidl1_io.h>

//define mpeg callback here
//this dialog box queries the user for info
//necessary to initialize the codec. normally, this
//would be done by reading the header, but that is
//not implemented here.
static void
vidl1_player_load_mpegcodec_callback(vidl1_codec * vc)
{
  vgui_dialog dialog( "MPEG player setup");

  bool grey_scale = false;
  bool demux_video = true;
  vcl_string pid = "0x00";
  int numframes = -1;

  dialog.checkbox("gray scale",grey_scale);
  dialog.checkbox("demux",demux_video);
  dialog.field("pid",pid);
  dialog.field("total frames. if not known, leave it.",numframes);

  if ( !dialog.ask())
  {
    vcl_cout << "vidl1_player_load_mpegcodec_callback. did not initialize codec.\n";
  }

  vidl1_mpegcodec * mpegcodec = vc->castto_vidl1_mpegcodec();
  if (!mpegcodec) return;

  mpegcodec->set_grey_scale(grey_scale);
  if (demux_video) mpegcodec->set_demux_video();
  mpegcodec->set_pid(pid.c_str());
  mpegcodec->set_number_frames(numframes);
  mpegcodec->init();
}

#endif // HAS_MPEG2


//----- Static menu callback functions -----

void vidl1_player_menus::load_video_callback()
{
#ifdef HAS_MPEG2
  //need to define callbacks
  vidl1_io::load_mpegcodec_callback = &vidl1_player_load_mpegcodec_callback;
#endif
  vidl1_player_manager::instance()->load_video_file();
}

void vidl1_player_menus::quit_callback()
{
  vcl_exit(1);
}


void vidl1_player_menus::pause_video_callback()
{
  vidl1_player_manager::instance()->pause_video();
}

void vidl1_player_menus::next_frame_callback()
{
  vidl1_player_manager::instance()->next_frame();
}

void vidl1_player_menus::prev_frame_callback()
{
  vidl1_player_manager::instance()->prev_frame();
}

void vidl1_player_menus::go_to_frame_callback()
{
  vidl1_player_manager::instance()->go_to_frame();
}

void vidl1_player_menus::play_video_callback()
{
  vidl1_player_manager::instance()->play_video();
}

void vidl1_player_menus::stop_video_callback()
{
  vidl1_player_manager::instance()->stop_video();
}


//----- vidl1_player_menus definition -----

vgui_menu vidl1_player_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;

  //file menu entries
  menufile.add( "Load", load_video_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  //view menu entries
  menuview.add( "Play", play_video_callback,(vgui_key)'p', vgui_CTRL);
  menuview.add( "Pause", pause_video_callback,(vgui_key)'p');
  menuview.add( "Next", next_frame_callback,(vgui_key)'f');
  menuview.add( "Prev", prev_frame_callback,(vgui_key)'b');
  menuview.add( "Go to Frame", go_to_frame_callback,(vgui_key)'g', vgui_CTRL);
  menuview.add( "Stop", stop_video_callback,(vgui_key)'s', vgui_CTRL);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  return menubar;
}
