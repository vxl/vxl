// This is brl/vvid/vvid_live_video_manager.cxx
#include "vvid_live_video_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
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
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <vtol/vtol_face_2d.h>
#include <sdet/sdet_detector_params.h>
#include <vvid/cmu_1394_camera_params.h>
#include <vvid/vvid_video_process.h>
#include <vvid/vvid_edge_process.h>
#include <vvid/vvid_region_process.h>

//static live_video_manager instance
vvid_live_video_manager *vvid_live_video_manager::instance_ = 0;


vvid_live_video_manager *vvid_live_video_manager::instance()
{
  if (!instance_)
    {
      instance_ = new vvid_live_video_manager();
      instance_->init();
    }
  return vvid_live_video_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
vvid_live_video_manager::
vvid_live_video_manager() :
  cp_(cmu_1394_camera_params())
{
  width_ = 960;
  height_ = 480;
  win_ = 0;
  live_capture_ = false;
  video_process_ = 0;
  init_successful_ = false;
}
vvid_live_video_manager::~vvid_live_video_manager()
{
}
//----------------------------------------------------------
// determine the number of active cameras and install the reduced
// resolution views accordingly.
//
void vvid_live_video_manager::init()
{
  //Determine the number of active cameras
  // for now we assume use a pre-defined _N_views
  init_successful_ = true;
  edges_ = true;
  vtab_ = vvid_live_video_tableau_new(0, 2, cmu_1394_camera_params());
  init_successful_ = init_successful_&&vtab_->attach_live_video();
  if (!init_successful_)
    {
      vcl_cout << "In vvid_live_video_manager::init() - bad camera"
               << " initialization\n";
      return;
    }
  vt2D_ =  bgui_vtol2D_tableau_new(vtab_);
  vt2D_->disable_highlight();
  vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(vt2D_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2d);
  this->add_child(shell);
  sdet_detector_params dp;
  video_process_  = new vvid_edge_process(dp);
}

//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool vvid_live_video_manager::handle(const vgui_event &e)
{
  //nothing special here yet
  //just pass the event back to the base class
  return vgui_wrapper_tableau::handle(e);
}
void vvid_live_video_manager::set_camera_params()
{
  if (!vtab_)
    {
      vcl_cout << "in vvid_live_video_manager::set_camera_params() - no live"
               << " video tableau \n";
      return;
    }
  cp_._rgb=false;
  vgui_dialog cam_dlg("Camera Parameters");
  cam_dlg.field("video_format",cp_._video_format);
  cam_dlg.field("video_mode",cp_._video_mode);
  cam_dlg.field("frame_rate",cp_._frame_rate);
  cam_dlg.field("brightness",cp_._brightness);
  cam_dlg.field("sharpness",cp_._sharpness);
  cam_dlg.field("exposure",cp_._exposure);
  cam_dlg.field("gain",cp_._gain);
  cam_dlg.checkbox("image capture(acquisition) ", cp_._capture);
  cam_dlg.checkbox("RGB(monochrome) ", cp_._rgb);
  if (!cam_dlg.ask())
    return;
  cp_.constrain();//constrain the parameters to be consistent
  vtab_->set_camera_params(cp_);
}
void vvid_live_video_manager::set_detection_params()
{
  if (!vtab_)
    {
      vcl_cout << "in vvid_live_video_manager::set_camera_params() - no live"
               << " video tableau \n";
      return;
    }
  //cache the live video state to restore
  bool live = vtab_->get_video_live();
  if (live)
    this->stop_live_video();
  static bool agr = false;
  static sdet_detector_params dp;
  static float max_gap = 0;
  vgui_dialog det_dialog("Video Edges");
  det_dialog.field("Gaussian sigma", dp.smooth);
  det_dialog.field("Noise Threshold", dp.noise_multiplier);
  det_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  det_dialog.checkbox("Agressive Closure", agr);
  det_dialog.checkbox("Compute Junctions", dp.junctionp);
  det_dialog.checkbox("Edges vs Regions", edges_);
  if (!det_dialog.ask())
    return;
  if (agr)
    dp.aggressive_junction_closure=0;
  else
    dp.aggressive_junction_closure=-1;
  dp.maxGap = max_gap;

  if (edges_)
    video_process_  = new vvid_edge_process(dp);
  else
    video_process_  = new vvid_region_process(dp);
  if (live)
    this->start_live_video();
}

void vvid_live_video_manager::run_frames()
{
  if (!init_successful_)
    return;
  while (live_capture_) {
    vul_timer t;
    vtab_->update_frame();
    if (!cp_._rgb&&video_process_)//i.e. grey scale
      {
        vil_memory_image_of<unsigned char> image;
        video_process_->clear_input();

        if (vtab_->get_current_mono_image(2,image))
          video_process_->add_input_image(image);
        else return;
        if (video_process_->execute())
          {
            vt2D_->clear_all();
            vcl_vector<vtol_topology_object_sptr> const & seg =
                                             video_process_->get_segmentation();

            for (vcl_vector<vtol_topology_object_sptr>::const_iterator ti=seg.begin();
                 ti != seg.end(); ti++)
              if (edges_)
                {
                  vtol_edge_2d_sptr e=(*ti)->cast_to_edge()->cast_to_edge_2d();
                  if (e)
                    vt2D_->add_edge(e);
                }
              else
                {
                  vtol_face_2d_sptr f=(*ti)->cast_to_face()->cast_to_face_2d();
                  if (f)
                    vt2D_->add_face(f);
                }
          }
      }
    vt2D_->post_redraw();
    vgui::run_till_idle();
    float ft = float(t.real())/1000.0, rate=0;
    if (ft)
      rate = 1.0/ft;
    vgui::out << "Tf = " << ft << " sec/frame  = " << rate << " frs/sec\n";
  }
}

void vvid_live_video_manager::start_live_video()
{
  if (!init_successful_||!vtab_)
    return;
  vtab_->start_live_video();

  live_capture_=true;
  this->run_frames();
}

void vvid_live_video_manager::stop_live_video()
{
  live_capture_=false;
  if (!init_successful_)
    return;
  vtab_->stop_live_video();
}

void vvid_live_video_manager::quit()
{
  this->stop_live_video();
  vcl_exit(1);
}
bool vvid_live_video_manager::
get_current_rgb_image(int pix_sample_interval,
                      vil_memory_image_of< vil_rgb<unsigned char> >& im)
{
  if (!init_successful_||!vtab_)
    {
      vcl_cout << "In vvid_live_video_manger::get_current_rgb_imge(..) -"
               << " bad initialization\n";
      return false;
    }

  return vtab_->get_current_rgb_image(pix_sample_interval, im);
}

bool vvid_live_video_manager::
get_current_mono_image(int pix_sample_interval,
                       vil_memory_image_of<unsigned char>& im)
{
  if (!init_successful_||!vtab_)
    {
      vcl_cout << "In vvid_live_video_manger::get_current_mono_imge(..) -"
               << " bad initialization\n";
      return false;
    }

  return vtab_->get_current_mono_image(pix_sample_interval, im);
}

void vvid_live_video_manager::
set_process_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im)
{
  process_rgb_ = im;
}

void vvid_live_video_manager::
set_process_mono_image(vil_memory_image_of<unsigned char>& im)
{
  process_mono_ = im;
}
