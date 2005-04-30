#include "vidfpl_vil_menus.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <segv/segv_vil_segmentation_manager.h>
#include <vvid/vvid_vil_file_manager.h>

#ifdef HAS_MPEG2
# include <vidl/vidl_mpegcodec.h>
# include <vidl/vidl_io.h>

//define mpeg callback here
//this dialog box queries the user for info
//necessary to initialize the codec. normally, this
//would be done by reading the header, but that is
//not implemented here.
static void
vidfpl_vil_load_mpegcodec_callback(vidl_codec * vc)
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
    vcl_cout << "vidfpl_vil_load_mpegcodec_callback. did not initialize codec.\n";
  }

  vidl_mpegcodec * mpegcodec = vc->castto_vidl_mpegcodec();
  if (!mpegcodec) return;

  mpegcodec->set_grey_scale(grey_scale);
  if (demux_video) mpegcodec->set_demux_video();
  mpegcodec->set_pid(pid.c_str());
  mpegcodec->set_number_frames(numframes);
  mpegcodec->init();
}

#endif // HAS_MPEG2

//Static munu callback functions
void vidfpl_vil_menus::load_video_callback()
{
#ifdef HAS_MPEG2
   //need to define callbacks
   vidl_io::load_mpegcodec_callback = &vidfpl_vil_load_mpegcodec_callback;
#endif
   vvid_vil_file_manager::instance()->load_video_file();
}

void vidfpl_vil_menus::pause_video_callback()
{
  vvid_vil_file_manager::instance()->pause_video();
}

void vidfpl_vil_menus::next_frame_callback()
{
  vvid_vil_file_manager::instance()->next_frame();
}

void vidfpl_vil_menus::prev_frame_callback()
{
  vvid_vil_file_manager::instance()->prev_frame();
}


void vidfpl_vil_menus::start_frame_callback()
{
  vvid_vil_file_manager::instance()->start_frame();
}

void vidfpl_vil_menus::end_frame_callback()
{
  vvid_vil_file_manager::instance()->end_frame();
}

void vidfpl_vil_menus::play_video_callback()
{
  vvid_vil_file_manager::instance()->play_video();
}

void vidfpl_vil_menus::stop_video_callback()
{
  vvid_vil_file_manager::instance()->stop_video();
}

void vidfpl_vil_menus::set_range_params_callback()
{
  vvid_vil_file_manager::instance()->set_range_params();
}

void vidfpl_vil_menus::quit_callback()
{
  vcl_exit(1);
}


//vidfpl_vil_menus definition
vgui_menu vidfpl_vil_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menutrack;
  vgui_menu menuprocess;
  vgui_menu menudebug;

  //file menu entries
  menufile.add( "Load", load_video_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  //view menu entries

  menuview.add( "Start Frame", start_frame_callback);
  menuview.add( "End Frame", end_frame_callback);
  menuview.add( "Play", play_video_callback);
  menuview.add( "Pause", pause_video_callback,(vgui_key)'p', vgui_CTRL);
  menuview.add( "Next", next_frame_callback,(vgui_key)'f', vgui_CTRL);
  menuview.add( "Prev", prev_frame_callback,(vgui_key)'b', vgui_CTRL);
  menuview.add( "Stop", stop_video_callback,(vgui_key)'s', vgui_CTRL);
  menuview.add( "Set Range Map", set_range_params_callback);

  //edit menu entries

  //Process menu entries

  // debug menu entries

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  menubar.add( "Track", menutrack);
  menubar.add( "Process", menuprocess);
  menubar.add( "Debug", menudebug);
  return menubar;
}
