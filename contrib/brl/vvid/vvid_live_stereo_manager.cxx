//this-sets-emacs-to-*-c++-*-mode

//:
// \file
// \author J.L. Mundy

#include "vvid_live_stereo_manager.h"
#include <math.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vil/vil_memory_image_of.h>
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

#include <vidl/vidl_io.h>
#include <vidl/vidl_frame.h>
#include <gevd/gevd_float_operators.h>
#include <vvid/cmu_1394_camera_params.h>
#include <vvid/vvid_video_process.h>
#include <vvid/vvid_epipolar_space_process.h>

//static live_video_manager instance
vvid_live_stereo_manager *vvid_live_stereo_manager::_instance = 0;


vvid_live_stereo_manager *vvid_live_stereo_manager::instance()
{
  if (!_instance)
    _instance = new vvid_live_stereo_manager();
  return vvid_live_stereo_manager::_instance;
}

//-----------------------------------------------------------
// constructors/destructor
//
vvid_live_stereo_manager::
vvid_live_stereo_manager() : vgui_grid_tableau(2,2),
                            _cp(cmu_1394_camera_params())
{
  width_ = 960;
  height_ = 480;
  _win = 0;
  _live_capture = false;
  _N_views = 2;
  _video_process = (vvid_video_process*)new vvid_epipolar_space_process();
  _init_successful = false;
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
  if (!_vframes.size())
    {
      vcl_cout << "in vvid_live_stereo_manager::set_camera_params() - no live"
               << " video frames\n";
      return;
    }
  vgui_dialog cam_dlg("Camera Parameters");
  cam_dlg.field("video_format",_cp._video_format);
  cam_dlg.field("video_mode",_cp._video_mode);
  cam_dlg.field("frame_rate",_cp._frame_rate);
  cam_dlg.field("brightness",_cp._brightness);
  cam_dlg.field("sharpness",_cp._sharpness);
  cam_dlg.field("exposure",_cp._exposure);
  cam_dlg.field("gain",_cp._gain);
  cam_dlg.checkbox("image capture(acquisition) ", _cp._capture);
  cam_dlg.checkbox("RGB(monochrome) ", _cp._rgb);
  if (!cam_dlg.ask())
    return;
  _cp.constrain();//constrain the parameters to be consistent
  for (int i = 0; i<_N_views; i++)
    _vframes[i]->set_camera_params(_cp);
}
//----------------------------------------------------------
// determine the number of active cameras and install the reduced
// resolution views accordingly.
//
void vvid_live_stereo_manager::setup_views()
{
  //Determine the number of active cameras
  // for now we assume use a pre-defined _N_views
  _init_successful = true;
  _vframes.clear();
  for (int i = 0; i<_N_views; i++)
    {
      vvid_live_video_tableau_sptr vf =
        vvid_live_video_tableau_new(i, 2, cmu_1394_camera_params());
      _vframes.push_back(vf);
      _init_successful = _init_successful&&vf->attach_live_video();
      if (!_init_successful)
        {
          vcl_cout << "In vvid_live_stereo_manager::setup_views() - bad camera"
                   << " initialization\n";
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

  _it = vgui_image_tableau_new();  
  _v2D = vgui_viewer2D_tableau_new(_it);
  this->add_at(_v2D, 0,0);
}

void vvid_live_stereo_manager::run_frames()
{
  if (!_init_successful)
    return;
  while (_live_capture){
    for (int i=0; i<_N_views; i++)
    _vframes[i]->update_frame();

    if (!_cp._rgb&&_N_views==2)//i.e. grey scale
      {
        vil_memory_image_of<unsigned char> i1, i2;
        vil_memory_image_of<vil_rgb<unsigned char> > im;

        _video_process->clear_input();

        if (_vframes[0]->get_current_mono_image(2,i1))
          _video_process->add_input_image(i1);
        else return;
        if (_vframes[1]->get_current_mono_image(2,i2))
          _video_process->add_input_image(i2);
        else
          return;
       if (_video_process->execute())
         _it->set_image(_video_process->get_output_image());
       else
         return;
      }
    _v2D->post_redraw();
    vgui::run_till_idle();
  }
}
void vvid_live_stereo_manager::start_live_video()
{
  if (!_init_successful)
    this->setup_views();
  if (!_init_successful)
    return;

  for (int i=0; i<_N_views; i++)
    _vframes[i]->start_live_video();

  _live_capture=true;
  this->run_frames();
}

void vvid_live_stereo_manager::stop_live_video()
{
  _live_capture=false;
  if (!_init_successful)
    return;
  for (int i=0; i<_N_views; i++)
    _vframes[i]->stop_live_video();
}

void vvid_live_stereo_manager::quit()
{
  this->stop_live_video();
  vcl_exit(1);
}
bool
vvid_live_stereo_manager::get_current_rgb_image(int view_no,
                                               int pix_sample_interval,
                                               vil_memory_image_of< vil_rgb<unsigned char> >& im)
{
  if (!_init_successful)
    return false;
  if (_vframes.size()< view_no+1)
    {
      vcl_cout << "In vvid_live_video_manger::get_current_rgb_imge(..) -"
               << " view_no out of range\n";
      return false;
    }

  return _vframes[view_no]->get_current_rgb_image(pix_sample_interval, im);
}

bool vvid_live_stereo_manager::
get_current_mono_image(int view_no, int pix_sample_interval,
                       vil_memory_image_of<unsigned char>& im)
{
  if (!_init_successful)
    return false;
  if (_vframes.size()< view_no+1)
    {
      vcl_cout << "In vvid_live_video_manger::get_current_mono_imge(..) -"
               << " view_no out of range\n";
      return false;
    }

  return _vframes[view_no]->get_current_mono_image(pix_sample_interval, im);
}

void vvid_live_stereo_manager::
set_process_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im)
{
  _process_rgb = im;
}

void vvid_live_stereo_manager::
set_process_mono_image(vil_memory_image_of<unsigned char>& im)
{
  _process_mono = im;
}
