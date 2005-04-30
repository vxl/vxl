// This is brl/vvid/vvid_vil_file_manager.cxx
#include "vvid_vil_file_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_memory_image_of.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_clip.h>
#include <vidl/vidl_io.h>
#include <vidl/vidl_frame.h>
#include <vgui/vgui.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_range_map_params.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_bargraph_clipon_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <bgui/bgui_vtol2D_rubberband_client.h>
#include <vgui/vgui_composite_tableau.h>

//static manager instance
vvid_vil_file_manager *vvid_vil_file_manager::instance_ = 0;

//The vvid_vil_file_manager is a singleton class
vvid_vil_file_manager *vvid_vil_file_manager::instance()
{
  if (!instance_)
  {
    instance_ = new vvid_vil_file_manager();
    instance_->init();
  }
  return vvid_vil_file_manager::instance_;
}

//======================================================================
//: set up the tableaux at each grid cell
//======================================================================
void vvid_vil_file_manager::init()
{
  grid_ = vgui_grid_tableau_new(1,1);
  grid_->set_grid_size_changeable(true);

  //  itab0_ = vgui_image_tableau_new();
  itab0_ = bgui_image_tableau_new();
  easy0_ = bgui_vtol2D_tableau_new(itab0_);
  bgui_vtol2D_rubberband_client* cl0 =  new bgui_vtol2D_rubberband_client(easy0_);

  rubber0_ = vgui_rubberband_tableau_new(cl0);
  vgui_composite_tableau_new comp0(easy0_,rubber0_);
  picktab0_ = bgui_picker_tableau_new(comp0);
  v2D0_ = vgui_viewer2D_tableau_new(picktab0_);
  grid_->add_at(v2D0_, 0,0);
#if 0
  itab1_ = bgui_image_tableau_new();
  easy1_ = bgui_vtol2D_tableau_new(itab1_);
  v2D1_ = vgui_viewer2D_tableau_new(easy1_);
  grid_->add_at(v2D1_, 1,0);
#endif
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);
  
  display_frame_repeat_=1;
  start_frame_ = 0;
  end_frame_ = 0;
}

//-----------------------------------------------------------
// constructors/destructor
// start with a single pane
vvid_vil_file_manager::vvid_vil_file_manager(): vgui_wrapper_tableau()
{
  width_ = 512;
  height_ = 512;
  
  window_ = 0;
  
  my_movie_=(vidl_movie*)0;
  win_ = 0;
  play_video_ = true;
  pause_video_ = false;
  next_frame_ = false;
  prev_frame_ = false;
  time_interval_ = 10.0;
  rmps_ = (vgui_range_map_params*)0;
  
  display_frame_repeat_=1;
}

vvid_vil_file_manager::~vvid_vil_file_manager()
{
}


// make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool vvid_vil_file_manager::handle(const vgui_event &e)
{
  return this->child.handle(e);
}

//-----------------------------------------------------------------------------
//: Loads a video file, e.g. avi into the viewer
//-----------------------------------------------------------------------------
void vvid_vil_file_manager::load_video_file()
{
  play_video_ = true;
  pause_video_ = false;
  next_frame_ = false;
  prev_frame_ = false;
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = true;
  bool cache = false;
  vgui_dialog load_video_dlg("Load video file");
  static vcl_string image_filename = "";
  static vcl_string ext = "";
  load_video_dlg.file("Filename:", ext, image_filename);
  load_video_dlg.checkbox("Cache Frames ", cache_frames_);
  if (!load_video_dlg.ask())
    return;

  my_movie_ = vidl_io::load_movie(image_filename.c_str());
  if (!my_movie_) {
    vgui_error_dialog("Failed to load movie file");
    return;
  }
  tabs_.clear();
  int n_frames = my_movie_->length();
  start_frame_ = 0;
  end_frame_ = n_frames-1;
  vidl_movie::frame_iterator pframe = my_movie_->first();
  vil_image_resource_sptr img = pframe->get_resource();
  unsigned max = 255;
  if(img->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
    max =4095;
  rmps_=
    new vgui_range_map_params(0, max, gamma, invert,
                              gl_map, cache);  
  //  vil1_image second = (++pframe)->get_image();
  height_ = img->ni();
  width_ = img->nj();
  vcl_cout << "Video Height " << height_ << vcl_endl
           << "Video Width  " << width_ << vcl_endl;
  unsigned display_width =2*width_, display_height = height_;
  if(display_width>1000)
    display_width = 1000;
  if(display_height>900)
    display_height = 900;
  if (win_)
    win_->reshape(display_width, display_height);
  itab0_->set_image_resource(img);
  itab0_->set_mapping(rmps_);
  grid_->post_redraw();
  vgui::run_till_idle();
}

//----------------------------------------------
void vvid_vil_file_manager::un_cached_play()
{
  if (!my_movie_)
  {
    vcl_cout << "No movie has been loaded\n";
    return;
  }
  vidl_movie::frame_iterator pframe = my_movie_->begin();
  vidl_movie::frame_iterator lframe = pframe;
  pframe += start_frame_;
  lframe += end_frame_+1;
  for ( ; pframe!=my_movie_->end() && pframe!=lframe && play_video_; ++pframe)
  {
    int frame_index = pframe->get_real_frame_index();
    vgui::out << "frame["<< frame_index <<"]\n";
    vil_image_resource_sptr img = pframe->get_resource();
    itab0_->set_image_resource(img);
    itab0_->set_mapping(rmps_);
    // pause by repeating the same frame
    if (pause_video_)
    {
      if (next_frame_)
      {
        if (pframe!=my_movie_->last()) ++pframe;
        next_frame_ = false;
      }
      if (prev_frame_)
      {
        if (pframe!=my_movie_->first()) --pframe;
        prev_frame_ = false;
      }
      // repeat the same frame by undoing the subsequent ++pframe of the iteration
      --pframe;
    }
    grid_->post_redraw();
    vgui::run_till_idle();
  }
}

void vvid_vil_file_manager::play_video()
{
  play_video_ = true;
  pause_video_ = false;
  time_interval_ = 10.0;
  easy0_->clear_all();
 
  //return the display to the first frame after the play is finished
  this->un_cached_play();
  if (!my_movie_)
    return;
  this->post_redraw();
}

//Player control functions

//Stop the video and return to the first frame
void vvid_vil_file_manager::stop_video()
{
  play_video_ = false;
}

//Cycle the play at the current frame
void vvid_vil_file_manager::pause_video()
{
  pause_video_ =!pause_video_;
  //make the time interval longer for paused state
  time_interval_ = 50.0;
}

void vvid_vil_file_manager::start_frame()
{
  vgui_dialog frame_dlg("Start Frame");
  frame_dlg.field("Frame No.", start_frame_);
  if (!frame_dlg.ask())
    return;
}

void vvid_vil_file_manager::end_frame()
{
  vgui_dialog frame_dlg("End Frame");
  frame_dlg.field("Frame No.", end_frame_);
  if (!frame_dlg.ask())
    return;
}


//While the video is paused go to the next frame
void vvid_vil_file_manager::next_frame()
{
  next_frame_ = true;
}

//While the video is paused go to the previous frame
void vvid_vil_file_manager::prev_frame()
{
  prev_frame_ = true;
}

void vvid_vil_file_manager::set_speed()
{
  //not implemented yet
}

void vvid_vil_file_manager::set_range_params()
{
  if (!itab0_||!itab0_->get_image_resource())
    return;
  static double min = 0.0, max = 255;
  static float gamma = 1.0;
  static bool invert = false;
  static bool gl_map = false;
  static bool cache = false;
  vgui_dialog range_dlg("Set Range Map Params");
  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  range_dlg.checkbox("Use GL Mapping", gl_map);
  range_dlg.checkbox("Cache Pixels", cache);
  if (!range_dlg.ask())
    return;
  vil_image_resource_sptr img = itab0_->get_image_resource();
  unsigned n_components = img->nplanes();

  if (n_components == 1)
    {
      rmps_=
        new vgui_range_map_params(min, max, gamma, invert,
                                gl_map, cache);
    }
  else if (n_components == 3)
     rmps_ =
      new vgui_range_map_params(min, max, min, max, min, max,
                                gamma, gamma, gamma, invert,
                                gl_map, cache);

  itab0_->set_mapping(rmps_);
}
