// This is brl/vvid/vvid_file_manager.cxx
#include "vvid_file_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
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
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_grid_finder_params.h>
#include <bdgl/bdgl_curve_tracker.h>
#include <bdgl/bdgl_curve_matcher.h>

#include <vidl/vidl_io.h>
#include <vidl/vidl_frame.h>
#include <vpro/vpro_frame_diff_process.h>
#include <vpro/vpro_motion_process.h>
#include <vpro/vpro_lucas_kanade_process.h>
#include <vpro/vpro_harris_corner_process.h>
#include <vpro/vpro_edge_process.h>
#include <vpro/vpro_edge_line_process.h>
#include <vpro/vpro_grid_finder_process.h>
#include <vpro/vpro_curve_tracking_process.h>

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
  easy0_->disable_highlight();
  v2D0_ = vgui_viewer2D_tableau_new(easy0_);
  grid_->add_at(v2D0_, 0,0);

  itab1_ = vgui_image_tableau_new();
  easy1_ = bgui_vtol2D_tableau_new(itab1_);
  easy1_->disable_highlight();
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
  track_ = false;
  color_label_ = false;
  window_ = 0;
  frame_trail_.clear();
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
    video_process_->get_output_spatial_objects();
  if (easy0_)
  {
    easy0_->clear_all();
    if (color_label_) {
      float r,g,b;
      //If tracking is on then we maintain a queue of points
      if (track_) {
        frame_trail_.add_spatial_objects(sos);
        vcl_vector<vsol_spatial_object_2d_sptr> temp;
        frame_trail_.get_spatial_objects(temp);
        for (unsigned int i=0;i<temp.size();i++) {
          set_changing_colors( temp[i]->get_tag_id() , &r, &g, &b );
          easy0_->set_vsol_spatial_object_2d_style(temp[i], r, g, b, 1.0, 2.0 );
          easy0_->add_spatial_object(temp[i]);
        }
      } else {
        for (unsigned int i=0;i<sos.size();i++) {
          set_changing_colors( sos[i]->get_tag_id() , &r, &g, &b );
          //vcl_cout<<"("<<sos[i]->get_tag_id()<<")\n";
          easy0_->set_vsol_spatial_object_2d_style(sos[i], r, g, b, 1.0, 2.0 );
          easy0_->add_spatial_object(sos[i]);
        }
      }
    } else {
      //If tracking is on then we maintain a queue of points
      if (track_)
        {
          frame_trail_.add_spatial_objects(sos);
          vcl_vector<vsol_spatial_object_2d_sptr> temp;
          frame_trail_.get_spatial_objects(temp);
          easy0_->add_spatial_objects(temp);
        }
      else
        easy0_->add_spatial_objects(sos);
    }
  }
}

// set changing colors for labelling curves, points, etc
//-----------------------------------------------------------------------------
void vvid_file_manager::set_changing_colors(int num, float *r, float *g, float *b)
{
  int strength = num/6;
  int pattern  = num%6;
  strength %= 20;
  float s = 1.0f - strength * 0.05f;

  switch(pattern)
  {
    case 0 : (*r) = s; (*g) = 0; (*b) = 0; break;
    case 1 : (*r) = 0; (*g) = s; (*b) = 0; break;
    case 2 : (*r) = 0; (*g) = 0; (*b) = s; break;
    case 3 : (*r) = s; (*g) = s; (*b) = 0; break;
    case 4 : (*r) = 0; (*g) = s; (*b) = s; break;
    case 5 : (*r) = s; (*g) = 0; (*b) = s; break;
    default: (*r) = 0; (*g) = 0; (*b) = 0; break; // this will never happen
  }
  //vcl_cout<<"color : "<<(*r)<<" : "<<(*g)<<" : "<<(*b)<<"\n";

  return;
}


//-------------------------------------------------------------
//: Display topology objects
//
void vvid_file_manager::display_topology()
{
  if (!easy0_)
    return;
 vul_timer t;
  vcl_vector<vtol_topology_object_sptr> const & topos =
    video_process_->get_output_topology();
  easy0_->clear_all();
    //If tracking is on then we maintain a queue of points
  if (track_)
    {
      frame_trail_.add_topology_objects(topos);
      vcl_vector<vtol_topology_object_sptr> temp;
      frame_trail_.get_topology_objects(temp);
      easy0_->add_topology_objects(temp);
    }
  else
    easy0_->add_topology_objects(topos);
  vcl_cout << "display " << topos.size()
           << " topology objs in " << t.real() << " msecs.\n";
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
  frame_trail_.clear();
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
  vil1_image img = pframe->get_image();
  vil1_image second = (my_movie_->first()+1)->get_image();
  height_ = img.height();
  width_ = img.width();
  vcl_cout << "Video Height " << height_ << vcl_endl
           << " Video Width " << width_ << vcl_endl;
  if (win_)
    win_->reshape(2*width_, height_);
  int i = 0;
  int inc = 40;
  if (cache_frames_)
    {
      while (pframe!=my_movie_->last())
        {
          vil1_image img = pframe->get_image();
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
      itab0_->set_image(second);
      v2D0_->child.assign(easy0_);
      itab1_->set_image(second);
    }
  grid_->post_redraw();
  vgui::run_till_idle();
}

//----------------------------------------------
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

//----------------------------------------------
void vvid_file_manager::un_cached_play()
{
  if (!my_movie_)
  {
    vcl_cout << "No movie has been loaded\n";
    return;
  }
  vidl_movie::frame_iterator pframe(my_movie_);
  for (pframe=my_movie_->first(); pframe!=my_movie_->last()&&play_video_;
       ++pframe)
    {
      //pause by repeating the same frame
      if (pause_video_&&play_video_)
        {
          if (next_frame_&&pframe!=my_movie_->last()-2)
            {
              ++pframe;
              next_frame_ = false;
            }
          if (prev_frame_&&pframe!=my_movie_->first()+2)
            {
              --pframe;
              prev_frame_ = false;
            }
          --pframe;
        }
      vil1_image img = pframe->get_image();
      itab0_->set_image(img);
      if (video_process_&&!pause_video_)
        {
          vil1_memory_image_of<unsigned char> image(img);
          video_process_->add_input_image(image);
          if (video_process_->execute())
            {
              if (video_process_->get_output_type()==vpro_video_process::IMAGE)
                display_image();
              if (video_process_->get_output_type()==
                  vpro_video_process::SPATIAL_OBJECT)
                display_spatial_objects();
              if (video_process_->get_output_type()==
                  vpro_video_process::TOPOLOGY)
                display_topology();
            }
        }
      vgui::out << "frame["<< pframe->get_real_frame_index()<<"]\n";
      grid_->post_redraw();
      vgui::run_till_idle();
    }
}

void vvid_file_manager::play_video()
{
  play_video_ = true;
  pause_video_ = false;
  time_interval_ = 10.0;
  easy0_->clear_all();
  frame_trail_.clear();
  //return the display to the first frame after the play is finished
  if (cache_frames_)
    {
      this->cached_play();
      v2D0_->child.assign(tabs_[0]);
    }
  else
    {
      this->un_cached_play();
      vil1_image img =my_movie_->get_image(0);
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
  video_process_ = new vpro_frame_diff_process();
}

void vvid_file_manager::compute_motion()
{
  video_process_ = new vpro_motion_process();
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
  video_process_ = new vpro_lucas_kanade_process(downsample,windowsize,thresh);
}

void vvid_file_manager::compute_harris_corners()
{
  static int track_window;
  static sdet_harris_detector_params hdp;
  vgui_dialog harris_dialog("harris");
  harris_dialog.field("sigma", hdp.sigma_);
  harris_dialog.field("thresh", hdp.thresh_);
  harris_dialog.field("N = 2n+1, (n)", hdp.n_);
  harris_dialog.field("Max No.Corners(percent)", hdp.percent_corners_);
  harris_dialog.field("scale_factor", hdp.scale_factor_);
  harris_dialog.checkbox("Tracks", track_);
  harris_dialog.field("Window", track_window);

  if (!harris_dialog.ask())
    return;

  video_process_ = new vpro_harris_corner_process(hdp);
  if (track_)
    {
      frame_trail_.clear();
      frame_trail_.set_window(track_window);
    }
}

void vvid_file_manager::compute_vd_edges()
{
  static int track_window;
  static bool agr = false;
  static sdet_detector_params dp;
  vgui_dialog det_dialog("Video Edges");
  det_dialog.field("Gaussian sigma", dp.smooth);
  det_dialog.field("Noise Threshold", dp.noise_multiplier);
  det_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  det_dialog.checkbox("Agressive Closure", agr);
  det_dialog.checkbox("Compute Junctions", dp.junctionp);
  det_dialog.checkbox("Tracks", track_);
  det_dialog.field("Window", track_window);

  if (!det_dialog.ask())
    return;

  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;

  video_process_  = new vpro_edge_process(dp);
  if (track_)
    {
      frame_trail_.clear();
      frame_trail_.set_window(track_window);
    }
}

void vvid_file_manager::compute_line_fit()
{
  static bool agr = false;
  static sdet_detector_params dp;
  dp.borderp = false;
  static sdet_fit_lines_params flp;
  vgui_dialog line_dialog("Video Line Segments");
  line_dialog.field("Gaussian sigma", dp.smooth);
  line_dialog.field("Noise Threshold", dp.noise_multiplier);
  line_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  line_dialog.checkbox("Agressive Closure", agr);
  line_dialog.checkbox("Compute Junctions", dp.junctionp);
  line_dialog.field("Min Fit Length", flp.min_fit_length_);
  line_dialog.field("RMS Distance", flp.rms_distance_);
  if (!line_dialog.ask())
    return;

  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;

  video_process_  = new vpro_edge_line_process(dp, flp);
}

void vvid_file_manager::compute_grid_match()
{
  static bool agr = false;
  static sdet_detector_params dp;
  dp.borderp = false;
  static sdet_fit_lines_params flp;
  static sdet_grid_finder_params gfp;
  dp.automatic_threshold=true;
  dp.noise_multiplier=20;
  flp.min_fit_length_=7;
  flp.rms_distance_=0.05;
  vgui_dialog grid_dialog("Grid Match");
  grid_dialog.field("Gaussian sigma", dp.smooth);
  grid_dialog.field("Noise Threshold", dp.noise_multiplier);
  grid_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  grid_dialog.checkbox("Agressive Closure", agr);
  grid_dialog.checkbox("Compute Junctions", dp.junctionp);
  grid_dialog.field("Min Fit Length", flp.min_fit_length_);
  grid_dialog.field("RMS Distance", flp.rms_distance_);
  grid_dialog.field("Angle Tolerance", gfp.angle_tol_);
  grid_dialog.field("Line Count Threshold", gfp.thresh_);

  if (!grid_dialog.ask())
    return;

  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;

  video_process_  = new vpro_grid_finder_process(dp, flp, gfp);
}

void vvid_file_manager::compute_curve_tracking()
{
  // get parameters
  static int track_window;
  static bdgl_curve_matcher_params mp(1.0, 10.0, 0.31416);
  static bdgl_curve_tracker_params tp(1e6);

  vgui_dialog* tr_dialog = new vgui_dialog("Edge Tracking");
  tr_dialog->field("Matching threshold", tp.match_thres_);
  tr_dialog->field("Pixel scale", mp.image_scale_);
  tr_dialog->field("Gradient scale", mp.grad_scale_);
  tr_dialog->field("Angle scale", mp.angle_scale_);
  tr_dialog->checkbox("Tracks", track_);
  tr_dialog->field("Window", track_window);
  if (!tr_dialog->ask())
    return;
  tp.match_params_ = mp;

  // embedded VD edges computations
  // VD parameters
  static bool agr = true;
  static sdet_detector_params dp;
  vgui_dialog* vd_dialog = new vgui_dialog("VD Edges");
  vd_dialog->field("Gaussian sigma", dp.smooth);
  vd_dialog->field("Noise Threshold", dp.noise_multiplier);
  vd_dialog->checkbox("Automatic Threshold", dp.automatic_threshold);
  vd_dialog->checkbox("Agressive Closure", agr);
  if (!vd_dialog->ask())
    return;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;

  color_label_ = true;

  video_process_  = new vpro_curve_tracking_process(tp,dp);
  if (track_)
    {
      frame_trail_.clear();
      frame_trail_.set_window(track_window);
    }
}
