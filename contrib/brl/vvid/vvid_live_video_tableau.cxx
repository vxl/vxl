// This is brl/vvid/vvid_live_video_tableau.cxx
#include "vvid_live_video_tableau.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>

vvid_live_video_tableau::vvid_live_video_tableau()
  : live_(false), pixel_sample_interval_(1)
{}


vvid_live_video_tableau::vvid_live_video_tableau(int node,
                                                 int pixel_sample_interval,
                                                 const cmu_1394_camera_params& cp)
  : cam_(node, cp)
{
  node_ = node;
  live_ = false;
  pixel_sample_interval_ = pixel_sample_interval;
}

vvid_live_video_tableau::~vvid_live_video_tableau()
{
  node_ = 0;
  live_ = false;
  pixel_sample_interval_ = 1;
}

vcl_string vvid_live_video_tableau::type_name() const
{
  return "xcv_image_tableau";//this name is hard coded in the vgui_viewer_tab
                             //bad arrangement but shouldn't cause problems
                             //if we don't use two specalized image tableaux at
                             //the same time
}

//: Handle all events for this tableau.
bool vvid_live_video_tableau::handle(vgui_event const &e)
{
  return base::handle(e);//currently just let the parent do the work
}

void vvid_live_video_tableau::set_camera_params(const cmu_1394_camera_params& cp)
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_tableau::set_camera_params() - warning, "
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

bool vvid_live_video_tableau::attach_live_video()
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_tableau::attach_live_video() - "
               << "no camera present\n";
      return false;
    }
  cam_.init(node_);
  return true;
}

void vvid_live_video_tableau::start_live_video()
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_tableau::startlive_video() - "
               << "no camera present\n";
      return;
    }
  cam_.start();
  live_ = true;

  if (cam_.rgb_)
    {
    cam_.get_rgb_image(rgb_frame_, pixel_sample_interval_, true);
    this->set_image(rgb_frame_);
    }
  else
    {
      vcl_cout << "get image\n";
      cam_.get_monochrome_image(mono_frame_, pixel_sample_interval_, true);
      vcl_cout << "got image\n";
      this->set_image(mono_frame_);
    }
}

void vvid_live_video_tableau::update_frame()
{
  if (cam_.rgb_)
    cam_.get_rgb_image(rgb_frame_, pixel_sample_interval_, true);
  else
    cam_.get_monochrome_image(mono_frame_, pixel_sample_interval_, true);
  this->reread_image();
  this->post_redraw();
  vgui::run_till_idle();
}

void vvid_live_video_tableau::stop_live_video()
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_tableau::stop_live_video() - "
               << "no camera present\n";
      return;
    }
  cam_.stop();
  live_=false;
}

void vvid_live_video_tableau::
get_camera_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im,
                     int pix_sample_interval)
{
  cam_.get_rgb_image(im, pix_sample_interval, false);
}

vil_memory_image_of< vil_rgb<unsigned char> >
vvid_live_video_tableau::get_current_rgb_image(int pix_sample_interval)
{
  vil_memory_image_of< vil_rgb<unsigned char> > im;
  cam_.get_rgb_image(im, pix_sample_interval, false);
  return im;
}

bool vvid_live_video_tableau::
get_current_rgb_image(int pix_sample_interval,
                      vil_memory_image_of< vil_rgb<unsigned char> >& im)
{
 return  cam_.get_rgb_image(im, pix_sample_interval, false);
}

vil_memory_image_of<unsigned char>
vvid_live_video_tableau::get_current_mono_image(int pix_sample_interval)
{
  vil_memory_image_of<unsigned char> im;
  cam_.get_monochrome_image(im, pix_sample_interval, false);
  return im;
}

bool vvid_live_video_tableau::
get_current_mono_image(int pix_sample_interval,
                       vil_memory_image_of<unsigned char>& im )
{
  return cam_.get_monochrome_image(im, pix_sample_interval, false);
}
