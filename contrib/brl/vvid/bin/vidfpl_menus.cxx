#include "vidfpl_menus.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <vvid/vvid_file_manager.h>

#ifdef HAS_MPEG2
# include <vidl_vil1/vidl_vil1_mpegcodec.h>
# include <vidl_vil1/vidl_vil1_io.h>

//define mpeg callback here
//this dialog box queries the user for info
//necessary to initialize the codec. normally, this
//would be done by reading the header, but that is
//not implemented here.
static void
vidfpl_load_mpegcodec_callback(vidl_vil1_codec * vc)
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
    vcl_cout << "vidfpl_load_mpegcodec_callback. did not initialize codec.\n";
  }

  vidl_vil1_mpegcodec * mpegcodec = vc->castto_vidl_vil1_mpegcodec();
  if (!mpegcodec) return;

  mpegcodec->set_grey_scale(grey_scale);
  if (demux_video) mpegcodec->set_demux_video();
  mpegcodec->set_pid(pid.c_str());
  mpegcodec->set_number_frames(numframes);
  mpegcodec->init();
}

#endif // HAS_MPEG2

//Static munu callback functions
void vidfpl_menus::load_video_callback()
{
#ifdef HAS_MPEG2
   //need to define callbacks
   vidl_vil1_io::load_mpegcodec_callback = &vidfpl_load_mpegcodec_callback;
#endif
   vvid_file_manager::instance()->load_video_file();
}

void vidfpl_menus::pause_video_callback()
{
  vvid_file_manager::instance()->pause_video();
}

void vidfpl_menus::next_frame_callback()
{
  vvid_file_manager::instance()->next_frame();
}

void vidfpl_menus::prev_frame_callback()
{
  vvid_file_manager::instance()->prev_frame();
}


void vidfpl_menus::start_frame_callback()
{
  vvid_file_manager::instance()->start_frame();
}

void vidfpl_menus::end_frame_callback()
{
  vvid_file_manager::instance()->end_frame();
}

void vidfpl_menus::play_video_callback()
{
  vvid_file_manager::instance()->play_video();
}

void vidfpl_menus::stop_video_callback()
{
  vvid_file_manager::instance()->stop_video();
}

void vidfpl_menus::easy2D_tableau_demo_callback()
{
  vvid_file_manager::instance()->easy2D_tableau_demo();
}

void vidfpl_menus::no_op_callback()
{
  vvid_file_manager::instance()->no_op();
}

void vidfpl_menus::difference_frames_callback()
{
  vvid_file_manager::instance()->difference_frames();
}

void vidfpl_menus::compute_motion_callback()
{
  vvid_file_manager::instance()->compute_motion();
}

void vidfpl_menus::compute_lucas_kanade_callback()
{
  vvid_file_manager::instance()->compute_lucas_kanade();
}

void vidfpl_menus::compute_harris_corners_callback()
{
  vvid_file_manager::instance()->compute_harris_corners();
}

void vidfpl_menus::compute_vd_edges_callback()
{
  vvid_file_manager::instance()->compute_vd_edges();
}

void vidfpl_menus::compute_line_fit_callback()
{
  vvid_file_manager::instance()->compute_line_fit();
}

void vidfpl_menus::compute_grid_match_callback()
{
  vvid_file_manager::instance()->compute_grid_match();
}

void vidfpl_menus::compute_corr_tracking_callback()
{
  vvid_file_manager::instance()->compute_corr_tracking();
}

void vidfpl_menus::compute_info_tracking_callback()
{
  vvid_file_manager::instance()->compute_info_tracking();
}

void vidfpl_menus::display_poly_track_callback()
{
  vvid_file_manager::instance()->display_poly_track();
}

void vidfpl_menus::display_art_model_track_callback()
{
  vvid_file_manager::instance()->display_art_model_track();
}

void vidfpl_menus::start_save_display_callback()
{
  vvid_file_manager::instance()->start_save_display();
}

void vidfpl_menus::end_save_display_callback()
{
  vvid_file_manager::instance()->end_save_display();
}

void vidfpl_menus::generate_basis_sequence_callback()
{
  vvid_file_manager::instance()->generate_basis_sequence();
}

void vidfpl_menus::compute_fourier_transform_callback()
{
  vvid_file_manager::instance()->compute_fourier_transform();
}

void vidfpl_menus::spatial_filter_callback()
{
  vvid_file_manager::instance()->spatial_filter();
}

void vidfpl_menus::create_box_callback()
{
  vvid_file_manager::instance()->create_box();
}

void vidfpl_menus::create_polygon_callback()
{
  vvid_file_manager::instance()->create_polygon();
}

void vidfpl_menus::create_stem_callback()
{
  vvid_file_manager::instance()->create_stem();
}

void vidfpl_menus::create_long_arm_tip_callback()
{
  vvid_file_manager::instance()->create_long_arm_tip();
}

void vidfpl_menus::create_short_arm_tip_callback()
{
  vvid_file_manager::instance()->create_short_arm_tip();
}

void vidfpl_menus::create_background_model_callback()
{
  vvid_file_manager::instance()->create_background_model();
}


void vidfpl_menus::exercise_art_model_callback()
{
  vvid_file_manager::instance()->exercise_art_model();
}

void vidfpl_menus::track_art_model_callback()
{
  vvid_file_manager::instance()->track_art_model();
}

void vidfpl_menus::display_ihs_callback()
{
  vvid_file_manager::instance()->display_ihs();
}

void vidfpl_menus::save_frame_callback()
{
  vvid_file_manager::instance()->save_frame();
}

void vidfpl_menus::save_half_res_callback()
{
  vvid_file_manager::instance()->save_half_res();
}

void vidfpl_menus::create_c_and_g_tracking_face_callback()
{
  vvid_file_manager::instance()->create_c_and_g_tracking_face();
}

void vidfpl_menus::display_tracked_hist_data_callback()
{
  vvid_file_manager::instance()->display_tracked_hist_data();
}

void vidfpl_menus::capture_feature_data_callback()
{
  vvid_file_manager::instance()->capture_feature_data();
}


void vidfpl_menus::quit_callback()
{
  vcl_exit(1);
}


//vidfpl_menus definition
vgui_menu vidfpl_menus::get_menu()
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
  menufile.add( "Start Save Display", start_save_display_callback);
  menufile.add( "End Save Display", end_save_display_callback,
                (vgui_key)'e', vgui_CTRL);
  menufile.add( "Save Current Frame", save_frame_callback);
  menufile.add( "Save Half Resolution Video", save_half_res_callback);

  //view menu entries

  menuview.add( "Start Frame", start_frame_callback);
  menuview.add( "End Frame", end_frame_callback);
  menuview.add( "Play", play_video_callback);
  menuview.add( "Pause", pause_video_callback,(vgui_key)'p', vgui_CTRL);
  menuview.add( "Next", next_frame_callback,(vgui_key)'f', vgui_CTRL);
  menuview.add( "Prev", prev_frame_callback,(vgui_key)'b', vgui_CTRL);
  menuview.add( "Stop", stop_video_callback,(vgui_key)'s', vgui_CTRL);
  menuview.add( "Display Track", display_poly_track_callback);
  menuview.add( "Display Histogram Track Image",
                display_tracked_hist_data_callback);

  menuview.add( "Display Art Model Track", display_art_model_track_callback);

  //edit menu entries
  menuedit.add( "Create Box", create_box_callback);
  menuedit.add( "Create Polygon", create_polygon_callback,(vgui_key)'a', vgui_CTRL);
  menuedit.add( "Click and Go Polygon", create_c_and_g_tracking_face_callback);

  menuedit.add( "Create Background Model", create_background_model_callback,
                (vgui_key)'4', vgui_CTRL);

  menuedit.add( "Create Stem", create_stem_callback,
                (vgui_key)'1', vgui_CTRL);
  menuedit.add( "Create Long Arm Tip", create_long_arm_tip_callback,
                (vgui_key)'2', vgui_CTRL);
  menuedit.add( "Create Short Arm Tip", create_short_arm_tip_callback,
                (vgui_key)'3', vgui_CTRL);

  //Process menu entries
  menuprocess.add( "No Op", no_op_callback);
  menuprocess.add( "Frame Difference", difference_frames_callback);
  menuprocess.add( "Compute Motion", compute_motion_callback);
  menuprocess.add( "Compute Lucas-Kanade Flow", compute_lucas_kanade_callback);
  menuprocess.add( "Compute Harris Corners", compute_harris_corners_callback);
  menuprocess.add( "Compute VD Edges", compute_vd_edges_callback);
  menuprocess.add( "Compute Line Fit", compute_line_fit_callback);
  menuprocess.add( "Compute Grid Match", compute_grid_match_callback);
  menuprocess.add( "Generate Basis ", generate_basis_sequence_callback);
  menuprocess.add( "Compute Fourier Transform ",
                   compute_fourier_transform_callback);
  menuprocess.add( "Spatial Filter ", spatial_filter_callback);
  menuprocess.add( "Capture Feature Data ", capture_feature_data_callback);

  //Tracking menu entries
  menutrack.add( "Compute Corr Tracking", compute_corr_tracking_callback);
  menutrack.add( "Compute Info Tracking", compute_info_tracking_callback);
  menutrack.add( "Exercise Art Model", exercise_art_model_callback);
  menutrack.add( "Track Art Model", track_art_model_callback);

  // debug menu entries
  menudebug.add( "Display IHS", display_ihs_callback);
  menudebug.add( "easy2D Demo", easy2D_tableau_demo_callback);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  menubar.add( "Track", menutrack);
  menubar.add( "Process", menuprocess);
  menubar.add( "Debug", menudebug);
  return menubar;
}
