#include "vvid_live_video_frame.h"
#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

vvid_live_video_frame::vvid_live_video_frame(int node,
                                             int pixel_sample_interval,
                                             const cmu_1394_camera_params& cp)
  : cam_(node, cp)
{
  live_ = false;
  pixel_sample_interval_ = pixel_sample_interval;
  itab_ = 0;
  e2d_ = 0;
}

vvid_live_video_frame::~vvid_live_video_frame()
{
}

void vvid_live_video_frame::set_camera_params(const cmu_1394_camera_params& cp)
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::set_camera_params() - warning, "
               << "no camera present, but param values were reassigned\n";
      return;
    }
  bool live =live_;
  if (live)
    this->stop_live_video();
  cam_.set_params(cp);
  cam_.update_settings();

  if (live)
    this->start_live_video();
}

bool vvid_live_video_frame::attach_live_video()
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::attach_live_video() - "
               << "no camera present\n";
      return false;
    }

  //Build the tableaux
  itab_= vgui_image_tableau_new();
  e2d_ = vgui_easy2D_tableau_new(itab_);
  v2d_ = vgui_viewer2D_tableau_new(e2d_);
  return true;
}

void vvid_live_video_frame::start_live_video()
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::start_live_video() - "
               << "no camera present\n";
      return;
    }
  cam_.start();
  live_ = true;

  if (cam_.rgb_)
    {
    cam_.get_rgb_image(rgb_frame_, pixel_sample_interval_, true);
    itab_->set_image(rgb_frame_);
    }
  else
    {
      cam_.get_monochrome_image(mono_frame_, pixel_sample_interval_, true);
      itab_->set_image(mono_frame_);
    }
}

void vvid_live_video_frame::update_frame()
{
  if (cam_.rgb_)
    cam_.get_rgb_image(rgb_frame_, pixel_sample_interval_, true);
  else
    cam_.get_monochrome_image(mono_frame_, pixel_sample_interval_, true);
  itab_->reread_image();
  v2d_->post_redraw();
  vgui::run_till_idle();
}

void vvid_live_video_frame::stop_live_video()
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::stop_live_video() - "
               << "no camera present\n";
      return;
    }
  cam_.stop();
  live_=false;
}

void vvid_live_video_frame::
get_camera_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im,
                     int pix_sample_interval)
{
  cam_.get_rgb_image(im, pix_sample_interval, false);
}

vil_memory_image_of< vil_rgb<unsigned char> >
vvid_live_video_frame::get_current_rgb_image(int pix_sample_interval)
{
  vil_memory_image_of< vil_rgb<unsigned char> > im;
  cam_.get_rgb_image(im, pix_sample_interval, false);
  return im;
}

bool vvid_live_video_frame::
get_current_rgb_image(int pix_sample_interval,
                      vil_memory_image_of< vil_rgb<unsigned char> >& im)
{
 return  cam_.get_rgb_image(im, pix_sample_interval, false);
}

vil_memory_image_of<unsigned char>
vvid_live_video_frame::get_current_mono_image(int pix_sample_interval)
{
  vil_memory_image_of<unsigned char> im;
  cam_.get_monochrome_image(im, pix_sample_interval, false);
  return im;
}

bool vvid_live_video_frame::
get_current_mono_image(int pix_sample_interval,
                       vil_memory_image_of<unsigned char>& im )
{
  return cam_.get_monochrome_image(im, pix_sample_interval, false);
}
