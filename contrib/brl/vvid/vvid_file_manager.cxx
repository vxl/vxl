//this-sets-emacs-to-*-c++-*-mode

//:
// \file
// \author J.L. Mundy

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <sdet/sdet_harris_detector_params.h>

#include <vidl/vidl_io.h>
#include <vidl/vidl_frame.h>
#include <vvid/vvid_frame_diff_process.h>
#include <vvid/vvid_motion_process.h>
#include <vvid/vvid_lucas_kanade_process.h>
#include <vvid/vvid_harris_corner_process.h>
#include <vvid/vvid_file_manager.h>

//static manager instance
vvid_file_manager *vvid_file_manager::instance_ = 0;

//The vvid_file_manager is a singleton class
vvid_file_manager *vvid_file_manager::instance()
{
  if (!instance_)
    {
      instance_ = new vvid_file_manager();
      instance_->init();
    }
  return vvid_file_manager::instance_;
}

//======================================================================
//: set up the tableaux at each grid cell
//======================================================================
void vvid_file_manager::init()
{
  grid_ = vgui_grid_tableau_new(2,1);
  grid_->set_grid_size_changeable(true);

  itab0_ = vgui_image_tableau_new();
  easy0_ = bgui_vtol2D_tableau_new(itab0_);
  v2D0_ = vgui_viewer2D_tableau_new(easy0_);
  grid_->add_at(v2D0_, 0,0);

  itab1_ = vgui_image_tableau_new();
  easy1_ = bgui_vtol2D_tableau_new(itab1_);
  v2D1_ = vgui_viewer2D_tableau_new(easy1_);
  grid_->add_at(v2D1_, 1,0);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);
}

//-----------------------------------------------------------
// constructors/destructor
// start with a single pane
vvid_file_manager::vvid_file_manager(): vgui_wrapper_tableau()
{
  width_ = 512;
  height_ = 512;
  my_movie_=(vidl_movie*)0;
  win_ = 0;
  cache_frames_ = false;
  play_video_ = true;
  pause_video_ = false;
  next_frame_ = false;
  prev_frame_ = false;
  time_interval_ = 10.0;
  video_process_ = 0;
}
vvid_file_manager::~vvid_file_manager()
{
}


// make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool vvid_file_manager::handle(const vgui_event &e)
{
  return this->child.handle(e);
}
//-------------------------------------------------------------
//: Display a processed image
//
void vvid_file_manager::display_image()
{
  if (!video_process_)
    return;
  if (itab1_)
    itab1_->set_image(video_process_->get_output_image());
}

//-------------------------------------------------------------
//: Display a set of spatial objects
//
void vvid_file_manager::display_spatial_objects()
{
  if (!video_process_)
    return;
  vcl_vector<vsol_spatial_object_2d_sptr> const& sos = 
    video_process_->get_spatial_objects();
  if (easy0_)
  {
	  easy0_->clear_all();
    easy0_->add_spatial_objects(sos);
  }
}

//-----------------------------------------------------------------------------
//: Loads a video file, e.g. avi into the viewer
//-----------------------------------------------------------------------------
void vvid_file_manager::load_video_file()
{
  play_video_ = true;
  pause_video_ = false;
  next_frame_ = false;
  prev_frame_ = false;
  video_process_ = 0;
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

  vidl_movie::frame_iterator pframe(my_movie_);
  pframe = my_movie_->first();

  vil_image img = pframe->get_image();
  height_ = img.height();
  width_ = img.width();
  vcl_cout << "Video Height " << height_ << vcl_endl
           << " Video Width " << width_ << vcl_endl;
  if (win_)
    win_->reshape(width_, height_);

  int i = 0;
  int inc = 40;
  if (cache_frames_)
    {
      while (pframe!=my_movie_->last())
        {
          vil_image img = pframe->get_image();
          vgui_image_tableau_sptr itab = vgui_image_tableau_new(img);
          bgui_vtol2D_tableau_new  e(itab);
          tabs_.push_back(e);
          ++pframe;//next video frame
          vcl_cout << "Loading Frame[" << i << "]:(" <<width_ <<" "<<height_ << ")\n";
          ++inc;
          ++i;
        }
      v2D0_->child.assign(tabs_[0]);
      itab1_->set_image(tabs_[0]->get_image_tableau()->get_image());
    }
  else
    {
      itab0_->set_image(img);
      v2D0_->child.assign(easy0_);
      itab1_->set_image(img);
    }
  grid_->post_redraw();
  vgui::run_till_idle();
}
void vvid_file_manager::cached_play()
{
  vul_timer t;
  for (vcl_vector<bgui_vtol2D_tableau_sptr>::iterator vit = tabs_.begin();
       vit != tabs_.end()&&play_video_; vit++)
    {
      //pause by repeating the same frame
      if (pause_video_&&play_video_)
        {
          if (next_frame_&&vit!=tabs_.end()-2)
            {
              vit+=2;
              next_frame_ = false;
            }
          if (prev_frame_&&vit!=tabs_.begin()+2)
            {
              vit--;
              prev_frame_ = false;
            }
          vit--;
        }

      v2D0_->child.assign(*vit);
        //Here we can put some stuff to control the frame rate. Hard coded to
        //a delay of 10 for now
      while (t.all()<time_interval_) ;
      //force the new frame to be displayed
      grid_->post_redraw();
      vgui::run_till_idle();
      t.mark();
    }
}

void vvid_file_manager::un_cached_play()
{
  vidl_movie::frame_iterator pframe(my_movie_);
  for (pframe=my_movie_->first(); pframe!=my_movie_->last()&&play_video_;
       ++pframe)
    {
      //pause by repeating the same frame
      if (pause_video_&&play_video_)
        {
          if (next_frame_&&pframe!=my_movie_->last()-2)
            {
              ++pframe;++pframe;
              next_frame_ = false;
            }
          if (prev_frame_&&pframe!=my_movie_->first()+2)
            {
              prev_frame_ = false;
            }
          --pframe;
        }
      vil_image img = pframe->get_image();
      itab0_->set_image(img);
      if (video_process_&&!pause_video_)
        {
          vil_memory_image_of<unsigned char> image(img);
          video_process_->add_input_image(image);
          if (video_process_->execute())
            if (video_process_->get_output_type()==vvid_video_process::IMAGE)
              display_image();
            else if (video_process_->get_output_type()==
                     vvid_video_process::SPATIAL_OBJECT)
              display_spatial_objects();
        }
      grid_->post_redraw();
      vgui::run_till_idle();
    }
}
void vvid_file_manager::play_video()
{
  play_video_ = true;
  pause_video_ = false;
  time_interval_ = 10.0;
  //return the display to the first frame after the play is finished
  if (cache_frames_)
    {
      this->cached_play();
      v2D0_->child.assign(tabs_[0]);
    }
  else
    {
      this->un_cached_play();
      vil_image img =my_movie_->get_image(0);
      itab1_->set_image(img);
    }
  this->post_redraw();
}

//Player control functions

//Stop the video and return to the first frame
void vvid_file_manager::stop_video()
{
  play_video_ = false;
}

//Cycle the play at the current frame
void vvid_file_manager::pause_video()
{
  pause_video_ =!pause_video_;
  //make the time interval longer for paused state
  time_interval_ = 50.0;
}

void vvid_file_manager::go_to_frame()
{
  //not implemented yet
}

//While the video is paused go to the next frame
void vvid_file_manager::next_frame()
{
  next_frame_ = true;
}

//While the video is paused go to the previous frame
void vvid_file_manager::prev_frame()
{
  prev_frame_ = true;
}

void vvid_file_manager::set_speed()
{
  //not implemented yet
}

void vvid_file_manager::easy2D_tableau_demo()
{
  int inc = 40;
  for (vcl_vector<bgui_vtol2D_tableau_sptr>::iterator eit = tabs_.begin();
       eit != tabs_.end(); eit++, ++inc)
    {
      (*eit)->clear();
      (*eit)->set_foreground(0,1,1);
      (*eit)->set_point_radius(5);
      if (inc>60)
        inc = 40;
      for (unsigned int j = 0; j<=height_; j+=inc)
        for (unsigned int k=0; k<=width_; k+=inc)
          (*eit)->add_point(k,j);
    }
}

void vvid_file_manager::no_op()
{
  video_process_ = 0;
}

void vvid_file_manager::difference_frames()
{
  video_process_ = new vvid_frame_diff_process();
}

void vvid_file_manager::compute_motion()
{
  video_process_ = new vvid_motion_process();
}

void vvid_file_manager::compute_lucas_kanade()
{
  static bool downsample = false;
  static int windowsize=2;
  static double thresh=20000;
  vgui_dialog downsample_dialog("Lucas-Kanade Params");
  downsample_dialog.checkbox("Downsample", downsample);
  downsample_dialog.field("WindowSize(2n+1) n=",windowsize);
  downsample_dialog.field("Motion Factor Threshold", thresh);

  if (!downsample_dialog.ask())
    return;
  video_process_ = new vvid_lucas_kanade_process(downsample,windowsize,thresh);
}

void vvid_file_manager::compute_harris_corners()
{
  static sdet_harris_detector_params hdp(1.0f, 100.0f, 2);
  vgui_dialog harris_dialog("harris");
  harris_dialog.field("sigma", hdp.sigma_);
  harris_dialog.field("thresh", hdp.thresh_);
  harris_dialog.field("N = 2n+1, (n)", hdp.n_);
  harris_dialog.field("scale_factor", hdp.scale_factor_);
  if (!harris_dialog.ask())
    return;

  video_process_ = new vvid_harris_corner_process(hdp);
}
