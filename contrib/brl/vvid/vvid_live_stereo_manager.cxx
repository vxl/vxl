// This is brl/vvid/vvid_live_stereo_manager.cxx
#include "vvid_live_stereo_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vil1/vil1_memory_image_of.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>

#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vvid/cmu_1394_camera_params.h>
#include <vpro/vpro_video_process.h>
#include <vpro/vpro_epipolar_space_process.h>

//static live_video_manager instance
vvid_live_stereo_manager *vvid_live_stereo_manager::instance_ = 0;


vvid_live_stereo_manager *vvid_live_stereo_manager::instance()
{
  if (!instance_)
    instance_ = new vvid_live_stereo_manager();
  return vvid_live_stereo_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
vvid_live_stereo_manager::
vvid_live_stereo_manager() : vgui_grid_tableau(2,2),
                            cp_(cmu_1394_camera_params())
{
  width_ = 960;
  height_ = 480;
  win_ = 0;
  live_capture_ = false;
  N_views_ = 2;
  video_process_ = (vpro_video_process*)new vpro_epipolar_space_process();
  init_successful_ = false;
  this->set_grid_size_changeable(true);
}
vvid_live_stereo_manager::~vvid_live_stereo_manager()
{
}

//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool vvid_live_stereo_manager::handle(const vgui_event &e)
{
  //nothing special here yet
  //just pass the event back to the base class
  return vgui_grid_tableau::handle(e);
}
void vvid_live_stereo_manager::set_camera_params()
{
  if (!vframes_.size())
    {
      vcl_cout << "in vvid_live_stereo_manager::set_camera_params() -"
               << " no live video frames\n";
      return;
    }
  vgui_dialog cam_dlg("Camera Parameters");
  cam_dlg.field("video_format",cp_.video_format_);
  cam_dlg.field("video_mode",cp_.video_mode_);
  cam_dlg.field("frame_rate",cp_.frame_rate_);
  cam_dlg.field("brightness",cp_.brightness_);
  cam_dlg.field("sharpness",cp_.sharpness_);
  cam_dlg.field("exposure",cp_.exposure_);
  cam_dlg.field("gain",cp_.gain_);
  cam_dlg.checkbox("image capture(acquisition) ", cp_.capture_);
  cam_dlg.checkbox("RGB(monochrome) ", cp_.rgb_);
  if (!cam_dlg.ask())
    return;
  cp_.constrain();//constrain the parameters to be consistent
  for (int i = 0; i<N_views_; i++)
    vframes_[i]->set_camera_params(cp_);
}
//----------------------------------------------------------
// determine the number of active cameras and install the reduced
// resolution views accordingly.
//
void vvid_live_stereo_manager::setup_views()
{
  //Determine the number of active cameras
  // for now we assume use a pre-defined N_views_
  init_successful_ = true;
  vframes_.clear();
  for (int i = 0; i<N_views_; i++)
    {
      vvid_live_video_tableau_sptr vf =
        vvid_live_video_tableau_new(i, 2, cmu_1394_camera_params());
      vframes_.push_back(vf);
      init_successful_ = init_successful_&&vf->attach_live_video();
      if (!init_successful_)
        {
          vcl_cout << "In vvid_live_stereo_manager::setup_views() -"
                   << " bad camera initialization\n";
          return;
        }
//       //Experimental Kludge
//       //draw green line at 1/2 the height
//       float h2 = 120.0; //240/2
//       vgui_easy2D_tableau_sptr e = vf->get_easy2D_tableau();
//       e->set_foreground(0.0, 1.0, 0.0);
//       e->set_line_width(3.0);
//       e->add_line(0.0,h2, 320., h2);
      //end kludge
      this->add_at(vgui_viewer2D_tableau_new(vf), 1,i);
    }

  it_ = vgui_image_tableau_new();
  v2D_ = vgui_viewer2D_tableau_new(it_);
  this->add_at(v2D_, 0,0);
}

void vvid_live_stereo_manager::run_frames()
{
  if (!init_successful_)
    return;
  while (live_capture_){
    for (int i=0; i<N_views_; i++)
    vframes_[i]->update_frame();

    if (!cp_.rgb_&&N_views_==2)//i.e. grey scale
      {
        vil1_memory_image_of<unsigned char> i1, i2;
        vil1_memory_image_of<vil1_rgb<unsigned char> > im;

        video_process_->clear_input();

        if (vframes_[0]->get_current_mono_image(2,i1))
          video_process_->add_input_image(i1);
        else return;
        if (vframes_[1]->get_current_mono_image(2,i2))
          video_process_->add_input_image(i2);
        else
          return;
       if (video_process_->execute())
         it_->set_image(video_process_->get_output_image());
       else
         return;
      }
    v2D_->post_redraw();
    vgui::run_till_idle();
  }
}
void vvid_live_stereo_manager::start_live_video()
{
  if (!init_successful_)
    this->setup_views();
  if (!init_successful_)
    return;

  for (int i=0; i<N_views_; i++)
    vframes_[i]->start_live_video();

  live_capture_=true;
  this->run_frames();
}

void vvid_live_stereo_manager::stop_live_video()
{
  live_capture_=false;
  if (!init_successful_)
    return;
  for (int i=0; i<N_views_; i++)
    vframes_[i]->stop_live_video();
}

void vvid_live_stereo_manager::quit()
{
  this->stop_live_video();
  vcl_exit(1);
}
bool
vvid_live_stereo_manager::get_current_rgb_image(int view_no,
                                                int pix_sample_interval,
                                                vil1_memory_image_of< vil1_rgb<unsigned char> >& im)
{
  if (!init_successful_)
    return false;
  if (vframes_.size()< view_no+1)
    {
      vcl_cout << "In vvid_live_video_manger::get_current_rgb_imge(..) -"
               << " view_no out of range\n";
      return false;
    }

  return vframes_[view_no]->get_current_rgb_image(pix_sample_interval, im);
}

bool vvid_live_stereo_manager::
get_current_mono_image(int view_no, int pix_sample_interval,
                       vil1_memory_image_of<unsigned char>& im)
{
  if (!init_successful_)
    return false;
  if (vframes_.size()< view_no+1)
    {
      vcl_cout << "In vvid_live_video_manger::get_current_mono_imge(..) -"
               << " view_no out of range\n";
      return false;
    }

  return vframes_[view_no]->get_current_mono_image(pix_sample_interval, im);
}

void vvid_live_stereo_manager::
set_process_rgb_image(vil1_memory_image_of< vil1_rgb<unsigned char> >& im)
{
  process_rgb_ = im;
}

void vvid_live_stereo_manager::
set_process_mono_image(vil1_memory_image_of<unsigned char>& im)
{
  process_mono_ = im;
}
