// This is gel/jvid/jvx_manager.h
#ifndef jvx_manager_h_
#define jvx_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Video player - the manager for playing video sequences
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Apr 14, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vidl_vil1/vidl_vil1_movie.h>

#ifdef HAS_MPEG2
#include <vidl_vil1/vidl_vil1_mpegcodec.h>

//define mpeg callback here
//this dialog box queries the user for info
//necessary to initialize the codec. normally, this
//would be done by reading the header, but that is
//not implemented here.
inline void
jvid_load_mpegcodec_callback(vidl_vil1_codec * vc)
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
    vcl_cout << "jvid_load_mpegcodec_callback. did not initialize codec.\n";

  vidl_vil1_mpegcodec * mpegcodec = vc->castto_vidl_vil1_mpegcodec();
  if (!mpegcodec) return;

  mpegcodec->set_grey_scale(grey_scale);
  if (demux_video) mpegcodec->set_demux_video();
  mpegcodec->set_pid(pid.c_str());
  mpegcodec->set_number_frames(numframes);
  mpegcodec->init();
}

#endif // HAS_MPEG2

class jvx_manager : public vgui_grid_tableau
{
 public:
  jvx_manager();
  ~jvx_manager();
  virtual bool handle(const vgui_event&);
  unsigned get_height(){return height_;}
  unsigned get_width(){return width_;}
  void load_video_file();
  void play();
  void play_video();
  void stop_video();
  void go_to_frame();
  void next_frame();
  void prev_frame();
  void set_speed();
 protected:
  vgui_viewer2D_tableau_sptr get_vgui_viewer2D_at(unsigned col, unsigned row);
 private:
  unsigned width_;
  unsigned height_;
  vidl_vil1_movie_sptr my_movie_;
  vcl_vector<vgui_viewer2D_tableau_sptr> tabs_;
};

#endif // jvx_manager_h_
