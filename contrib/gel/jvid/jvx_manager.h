#ifndef jvx_manager_h_
#define jvx_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Video player
//   the manager for playing video sequences
// \author 
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Apr 14, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_codec_callbacks.h>
#include <vidl/vidl_mpegcodec.h>

extern void (*load_mpegcodec_callback)(vidl_codec*);

//define some callbacks here
void
jvid_load_mpegcodec_callback(vidl_codec * vc)
{
  vgui_dialog dialog( "MPEG player setup");

  bool grey_scale = true;
  bool demux_video = true;
  vcl_string pid = "0x00";
  int numframes = -1;
  
  dialog.checkbox("gray scale",grey_scale);
  dialog.checkbox("demux",demux_video);
  dialog.field("pid",pid);
  dialog.field("total frames. if not known, leave it.",numframes);

  if( !dialog.ask())
    {
      vcl_cout << "jvid_load_mpegcodec_callback. did not initialize codec." << vcl_endl;
    }

  vidl_mpegcodec * mpegcodec = vc->castto_vidl_mpegcodec();
  if (!mpegcodec) return;

  mpegcodec->set_grey_scale(grey_scale);
  if(demux_video) mpegcodec->set_demux_video();
  mpegcodec->set_pid(pid.c_str());
  mpegcodec->set_number_frames(numframes);
	
  mpegcodec->init();
}

class jvx_manager : public vgui_grid_tableau
{
 public:
  jvx_manager();
  ~jvx_manager();
  virtual bool handle(const vgui_event&);
  unsigned get_height(){return _height;}
  unsigned get_width(){return _width;}
  void load_video_file();
  void play();
  void play_video();
  void stop_video();
  void go_to_frame();
  void next_frame();
  void prev_frame();
  void set_speed();
 protected:
  vgui_viewer2D_sptr get_vgui_viewer2D_at(unsigned col, unsigned row);
 private:
  unsigned _width;
  unsigned _height;
  vidl_movie_sptr _my_movie;
  vcl_vector<vgui_viewer2D_sptr> _tabs;
};
#endif // jvx_manager_h_
