#include "vvid_live_video_frame.h"
#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

vvid_live_video_frame::vvid_live_video_frame(int node,
                                             int pixel_sample_interval,
                                             const cmu_1394_camera_params& cp)
  : _cam(node, cp)
{
  _live = false;
  _pixel_sample_interval = pixel_sample_interval;
  _itab = 0;
  _e2d = 0;
}

vvid_live_video_frame::~vvid_live_video_frame()
{
}

void vvid_live_video_frame::set_camera_params(const cmu_1394_camera_params& cp)
{
  if (!_cam.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::set_camera_params() - warning, "
               << "no camera present, but param values were reassigned\n";
      return;
    }
  bool live =_live;
  if (live)
    this->stop_live_video();
  _cam.set_params(cp);
  _cam.update_settings();

  if (live)
    this->start_live_video();
}

bool vvid_live_video_frame::attach_live_video()
{
  if (!_cam.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::attach_live_video() - "
               << "no camera present\n";
      return false;
    }

  //Build the tableaux
  _itab= vgui_image_tableau_new();
  _e2d = vgui_easy2D_tableau_new(_itab);
  _v2d = vgui_viewer2D_tableau_new(_e2d);
  return true;
}

void vvid_live_video_frame::start_live_video()
{
  if (!_cam.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::start_live_video() - "
               << "no camera present\n";
      return;
    }
  _cam.start();
  _live = true;

  if (_cam.rgb_)
    {
    _cam.get_rgb_image(_rgb_frame, _pixel_sample_interval, true);
    _itab->set_image(_rgb_frame);
    }
  else
    {
      _cam.get_monochrome_image(_mono_frame, _pixel_sample_interval, true);
      _itab->set_image(_mono_frame);
    }
}

void vvid_live_video_frame::update_frame()
{
  if (_cam.rgb_)
    _cam.get_rgb_image(_rgb_frame, _pixel_sample_interval, true);
  else
    _cam.get_monochrome_image(_mono_frame, _pixel_sample_interval, true);
  _itab->reread_image();
  _v2d->post_redraw();
  vgui::run_till_idle();
}

void vvid_live_video_frame::stop_live_video()
{
  if (!_cam.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_frame::stop_live_video() - "
               << "no camera present\n";
      return;
    }
  _cam.stop();
  _live=false;
}

void vvid_live_video_frame::
get_camera_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im,
                     int pix_sample_interval)
{
  _cam.get_rgb_image(im, pix_sample_interval, false);
}

vil_memory_image_of< vil_rgb<unsigned char> >
vvid_live_video_frame::get_current_rgb_image(int pix_sample_interval)
{
  vil_memory_image_of< vil_rgb<unsigned char> > im;
  _cam.get_rgb_image(im, pix_sample_interval, false);
  return im;
}

bool vvid_live_video_frame::
get_current_rgb_image(int pix_sample_interval,
                      vil_memory_image_of< vil_rgb<unsigned char> >& im)
{
 return  _cam.get_rgb_image(im, pix_sample_interval, false);
}

vil_memory_image_of<unsigned char>
vvid_live_video_frame::get_current_mono_image(int pix_sample_interval)
{
  vil_memory_image_of<unsigned char> im;
  _cam.get_monochrome_image(im, pix_sample_interval, false);
  return im;
}

bool vvid_live_video_frame::
get_current_mono_image(int pix_sample_interval,
                       vil_memory_image_of<unsigned char>& im )
{
  return _cam.get_monochrome_image(im, pix_sample_interval, false);
}
