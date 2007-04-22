// This is brl/vvid/vvid_vil_file_manager.cxx
#include "vvid_vil_file_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vil/vil_image_list.h>
#include <vil/vil_image_resource.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_io.h>
#include <vgui/vgui.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_bargraph_clipon_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vpro/vpro_vil_video_process.h>
#include <vpro/vpro_roi_process.h>
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
  easy0_ = vgui_easy2D_tableau_new(itab0_);
  picktab0_ = bgui_picker_tableau_new(easy0_);
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

void vvid_vil_file_manager::quit()
{
  vgui::quit();
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
  bool cache = true;
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
  if (!img)
    return;
  unsigned max = 255;
  if (img->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
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
  if (display_width>1000)
    display_width = 1000;
  if (display_height>900)
    display_height = 900;
  if (win_)
    win_->reshape(display_width, display_height);
  itab0_->set_image_resource(img);
  itab0_->set_mapping(rmps_);
  grid_->post_redraw();
  vgui::run_till_idle();
}

//-----------------------------------------------------------------------------
//: Loads a pyramid video
//-----------------------------------------------------------------------------
void vvid_vil_file_manager::load_pyramid_video()
{
  play_video_ = true;
  pause_video_ = false;
  next_frame_ = false;
  prev_frame_ = false;
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = true;
  bool cache = false;
  vgui_dialog load_video_dlg("Pyramid Video Dir");
  static vcl_string video_dir = "";
  static vcl_string ext = "";
  load_video_dlg.file("Filename:", ext, video_dir);
  if (!load_video_dlg.ask())
    return;
  vil_image_list il(video_dir.c_str());
  pyramid_movie_ = il.pyramids();
  if (pyramid_movie_.size()==0)
  {
    vgui_error_dialog("Failed to load pyramid movie");
    return;
  }

  unsigned n_frames = pyramid_movie_.size();
  start_frame_ = 0;
  end_frame_ = n_frames-1;
  vil_image_resource_sptr img = pyramid_movie_[0].ptr();
  if (!img)
    return;
  unsigned max = 255;
  rmps_=
    new vgui_range_map_params(0, max, gamma, invert,
                              gl_map, cache);
  if (img->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
    max =4095;
  //  vil1_image second = (++pframe)->get_image();
  height_ = img->ni();
  width_ = img->nj();
  vcl_cout << "Video Height " << height_ << vcl_endl
           << "Video Width  " << width_ << vcl_endl;
  unsigned display_width =2*width_, display_height = height_;
  if (display_width>1000)
    display_width = 1000;
  if (display_height>900)
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
  if (video_process_)
    video_process_->set_n_frames(my_movie_->length());
  vidl_movie::frame_iterator pframe = my_movie_->begin();
  vidl_movie::frame_iterator lframe = pframe;
  pframe += start_frame_;
  lframe += end_frame_+1;
  for ( ; pframe!=my_movie_->end() && pframe!=lframe && play_video_; ++pframe)
  {
    int frame_index = pframe->get_real_frame_index();
    vgui::out << "frame["<< frame_index <<"]\n";
    vil_image_resource_sptr imgr = pframe->get_resource();
    if (!imgr)
      continue;
    itab0_->set_image_resource(imgr);
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
    else if (video_process_)
    {
      video_process_->set_frame_index(frame_index);
      video_process_->add_input_image(imgr);
      if (video_process_->execute())
      {
        vcl_cout << "Executing Frame " << frame_index << '\n';
      }
    }
    grid_->post_redraw();
    vgui::run_till_idle();
  }
  if (video_process_)
    video_process_->finish();
 }

void vvid_vil_file_manager::pyramid_play()
{
  unsigned n_frames = pyramid_movie_.size();
  if (n_frames == 0)
  {
    vcl_cout << "No pyramid movie has been loaded\n";
    return;
  }
  for (unsigned pframe = 0 ; pframe<n_frames && play_video_; ++pframe)
  {
    vgui::out << "frame["<< pframe <<"]\n";
    vil_image_resource_sptr imgr = pyramid_movie_[pframe];
    if (!imgr)
      continue;
    itab0_->set_image_resource(imgr);
    itab0_->set_mapping(rmps_);

    // pause by repeating the same frame
    if (pause_video_)
    {
      if (next_frame_)
      {
        if (pframe!=n_frames-1) ++pframe;
        next_frame_ = false;
      }
      if (prev_frame_)
      {
        if (pframe!=0) --pframe;
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
  easy0_->clear();

  //return the display to the first frame after the play is finished
  this->un_cached_play();
  if (!my_movie_)
    return;
  this->post_redraw();
}

void vvid_vil_file_manager::play_pyramid()
{
  play_video_ = true;
  pause_video_ = false;
  time_interval_ = 10.0;
  easy0_->clear();

  //return the display to the first frame after the play is finished
  this->pyramid_play();
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
  static double min = 0, max = 255;
  static float gamma = 1.0;
  static bool invert = false;
  static bool gl_map = false;
  static bool cache = false;
  if (rmps_)
  {
    invert = rmps_->invert_;
    gl_map = rmps_->use_glPixelMap_;
    cache = rmps_->cache_mapped_pix_;
    if (rmps_->max_L_ > 0)
    {
      min = rmps_->min_L_;
      max = rmps_->max_L_;
      gamma = rmps_->gamma_L_;
    }
    else
    {
      min = rmps_->min_R_;
      max = rmps_->max_R_;
      gamma = rmps_->gamma_R_;
    }
  }
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

void vvid_vil_file_manager::create_box()
{
  float x1, y1, x2, y2;
  itab0_->unset_handle_motion();
  picktab0_->pick_box(&x1, &y1, &x2, &y2);
#if 0
  vsol_point_2d_sptr p0 = new vsol_point_2d(x1, y1);
  vsol_point_2d_sptr p1 = new vsol_point_2d(x2, y1);
  vsol_point_2d_sptr p2 = new vsol_point_2d(x2, y2);
  vsol_point_2d_sptr p3 = new vsol_point_2d(x1, y2);
  vcl_vector<vsol_point_2d_sptr> pts;
  pts.push_back(p0);   pts.push_back(p1);
  pts.push_back(p2);   pts.push_back(p3);
  box_ = new vsol_polygon_2d(pts);
#endif
  //: draw the box in easy2D tableau
  float x[4], y[4];
  x[0]=x[3] = x1; x[1]=x[2]=x2;
  y[0]=y[1] = y1; y[2]=y[3]=y2;
  easy0_->add_polygon(4, x, y);
  v2D0_->post_redraw();
  x0_=int(x1); y0_=int(y1);
  if (x2>=x1)
    xsize_ = (unsigned) (x2-x1 +1);
  else
    xsize_ = (unsigned) (x1-x2 +1);
  if (y2>y1)
   ysize_ = (unsigned) (y2-y1 +1);
  else
   ysize_ = (unsigned) (y1 -y2 +1);
  itab0_->set_handle_motion();
}

void vvid_vil_file_manager::save_roi()
{
  vgui_dialog file_dialog("ROI Directory");
  static vcl_string video_file;
  static vcl_string ext = "";
  file_dialog.file("Video File:", ext, video_file);
  if (!file_dialog.ask())
    return;
  vpro_roi_process* rop = new vpro_roi_process(video_file, x0_, y0_, xsize_, ysize_);
  video_process_ = rop;
}
