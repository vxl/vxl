// This is brl/vvid/vvid_live_video_manager.cxx
#include "vvid_live_video_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vil1/vil1_memory_image_of.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <sdet/sdet_detector_params.h>
#include <vvid/cmu_1394_camera_params.h>
#include <vpro/vpro_video_process.h>
#include <vpro/vpro_edge_process.h>
#include <vpro/vpro_region_process.h>
#include <vpro/vpro_capture_process.h>
#include <bgui/bgui_histogram_tableau.h>

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
  cp_(cmu_1394_camera_params()),
  histogram_(false),
  width_(960),
  height_(480),
  min_msec_per_frame_(1000.0/30.0),
  win_(0),
  video_process_(0),
  init_successful_(false)
{}

vvid_live_video_manager::~vvid_live_video_manager()
{}
//----------------------------------------------------------
// determine the number of active cameras and install the reduced
// resolution views accordingly.
//
void vvid_live_video_manager::init()
{
  //Determine the number of active cameras
  // for now we assume use a pre-defined _N_views
  cmu_1394_camera cam;
  num_cameras_ = cam.GetNumberCameras();
  vcl_cout << "Number of Cameras Detected: " << num_cameras_ << vcl_endl;
  if (num_cameras_ <= 0) {
    vcl_cerr << "Exiting - no cameras detected\n";
    vcl_exit(0);
  }

  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(num_cameras_,2);
  grid->set_grid_size_changeable(true);
  sample_ = 1;
  init_successful_ = true;
  edges_ = true;
  vvid_live_video_tableau_sptr vtab;
  bgui_vtol2D_tableau_sptr vt2D;

  for (unsigned cam=0; cam<num_cameras_; ++cam)
  {
    vtab = vvid_live_video_tableau_new(cam, sample_, cmu_1394_camera_params());
    vtabs_.push_back(vtab);
    init_successful_ = init_successful_&&vtab->attach_live_video();
    if (!init_successful_)
    {
      vcl_cout << "In vvid_live_video_manager::init() -"
               << " bad initialization - camera #"<< cam << vcl_endl;
      return;
    }
    vt2D =  bgui_vtol2D_tableau_new(vtab);
    vt2Ds_.push_back(vt2D);

    // make a 2D viewer tableau and add it to the grid
    vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(vt2D);
    grid->add_at(v2d, cam, 0);

  // make a histogram
  bgui_histogram_tableau_new htab;
    htabs_.push_back(htab);
    vgui_viewer2D_tableau_new viewer(htab);
  grid->add_at(viewer, cam, 1);
  }

  // get the camera paramaters from the last camera (assume are are the same)
  cp_ = vtab->get_camera_params();

  // put the grid in a shell
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid);
  this->add_child(shell);
  video_process_  = (vpro_video_process*)0;
}

//: make an event handler
// Note that we have to get an adaptor and set the tableau to receive events
bool vvid_live_video_manager::handle(const vgui_event &e)
{
  //nothing special here yet
  //just pass the event back to the base class
  return vgui_wrapper_tableau::handle(e);
}
//---------------------------------------------------------
//:
//  Cameras have different possible resolutions, frame rates and color
//  sampling choices. This method sets up a vgui choice menu item
//  based on the valid video configurations supported by the camera.
//  (currently the rgb_ flag doesn't do anything but ultimately will
//   control color/vs/mono acquisition if the camera supports it.)
//  The pix_sample_interval determines the resolution of the display
//
void vvid_live_video_manager::set_camera_params()
{
  vvid_live_video_tableau_sptr vtab = vtabs_.back();
  if (vtabs_.size() != num_cameras_ || !vtab)
  {
    vcl_cout << "in vvid_live_video_manager::set_camera_params() -"
             << " no live video tableau\n";
    return;
  }
  cp_ = vtab->get_camera_params();
  static int pix_sample_interval = 1;
  vcl_vector<vcl_string> choices;
  vcl_string no_choice="CurrentConfiguration";
  choices.push_back(no_choice);
  vcl_vector<vcl_string> valid_descrs = vtab->get_capability_descriptions();
  for (vcl_vector<vcl_string>::iterator cit = valid_descrs.begin();
       cit != valid_descrs.end(); cit++)
       choices.push_back(*cit);
  static int choice=0;
  static double max_frame_rate=30.0;
  //Set up the dialog.
  vgui_dialog cam_dlg("Camera Parameters");
  cam_dlg.message(vtab->current_capability_desc().c_str());
  cam_dlg.choice("Choose Configuration", choices, choice);
  cam_dlg.checkbox("Auto Exposure ", cp_.auto_exposure_);
  cam_dlg.checkbox("Auto Gain ", cp_.auto_gain_);
  cam_dlg.field("Shutter Speed", cp_.shutter_);
  cam_dlg.field("brightness",cp_.brightness_);
  cam_dlg.field("sharpness",cp_.sharpness_);
  cam_dlg.field("exposure",cp_.exposure_);
  cam_dlg.field("gain",cp_.gain_);
  cam_dlg.field("Display Sample Interval", pix_sample_interval);
  cam_dlg.field("Maximum Frame Rate", max_frame_rate);
  cam_dlg.checkbox("image capture(acquisition) ", cp_.capture_);
  cam_dlg.checkbox("RGB(monochrome) ", cp_.rgb_);
  cam_dlg.checkbox("Auto White Balance",cp_.autowhitebalance_);
  cam_dlg.field("White Balance U",cp_.whitebalanceU_);
  cam_dlg.field("White Balance V",cp_.whitebalanceV_);
  cam_dlg.checkbox(" One push WhiteBalance",cp_.onepushWBbalance_);

  if (!cam_dlg.ask())
    return;

  // set the results to all video tableaux
  for (unsigned i=0; i<num_cameras_; ++i) {
    if (choice)
      vtabs_[i]->set_current(choice-1);
    vtabs_[i]->set_pixel_sample_interval(pix_sample_interval);
    vtabs_[i]->set_camera_params(cp_);
  }

  min_msec_per_frame_ = 1000.0/max_frame_rate;

  vcl_cout << "Current Camera Parameters\n" << cp_ << '\n';
}

void vvid_live_video_manager::set_detection_params()
{
  if (vtabs_.size() != num_cameras_)
  {
    vcl_cout << "in vvid_live_video_manager::set_camera_params() -"
             << " no live video tableau\n";
    return;
  }
  //cache the live video state to restore
  bool live = vtabs_.back()->get_video_live();
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
    video_process_  = new vpro_edge_process(dp);
  else
    video_process_  = new vpro_region_process(dp);
  sample_ = 2;
  if (live)
    this->start_live_video();
}

void vvid_live_video_manager::no_op()
{
  video_process_ = 0;
}

void vvid_live_video_manager::capture_sequence()
{
  this->stop_live_video();
  vgui_dialog save_video_dlg("Save Video Sequence");
  static vcl_string video_filename = "";
  static vcl_string ext = "*.*";
  save_video_dlg.file("Video Filename:", ext, video_filename);
  if (!save_video_dlg.ask())
    return;
  sample_ = 1;
  video_process_ = new vpro_capture_process(video_filename);
}

void vvid_live_video_manager::init_capture()
{
  this->stop_live_video();
  vgui_dialog save_video_dlg("Init Capture");
  static vcl_string video_directory = vul_file::get_cwd();
  static vcl_string ext = "*.*";
  static bool auto_increment = true;
  static vcl_string dir_prefix = "video";
  static int dir_index = 0;
  save_video_dlg.file("Video Directory:", ext, video_directory);
  save_video_dlg.checkbox("Automatically Create Incremental Subdirectories", auto_increment);
  save_video_dlg.field("Directory Prefix", dir_prefix);
  save_video_dlg.field("Current Directory Index", dir_index);
  vcl_stringstream complete_path;
  complete_path << "Complete Path: " << video_directory << '/' << dir_prefix << dir_index;
  save_video_dlg.message(complete_path.str().c_str());

  if (!save_video_dlg.ask())
    return;

  // if not a directory, use the base directory
  if (!vul_file::is_directory(video_directory))
    video_directory = vul_file::dirname(video_directory);

  vcl_string video_filename = video_directory;
  if (auto_increment){
    vcl_stringstream auto_dir;
    auto_dir << '/' << dir_prefix << dir_index++;
    video_filename += auto_dir.str();
    vul_file::make_directory(video_filename);
  }

  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vcl_stringstream camera_subdir;
    camera_subdir << "/camera" << i << '/';
    vul_file::make_directory(video_filename+camera_subdir.str());
    vtabs_[i]->start_capture(video_filename+camera_subdir.str());
  }
}

void vvid_live_video_manager::stop_capture()
{
  this->stop_live_video();
  for (unsigned i=0; i<num_cameras_; ++i)
    vtabs_[i]->stop_capture();
}

void vvid_live_video_manager::toggle_histogram()
{
  if (histogram_) {
    histogram_ = false;
    for (unsigned i=0; i<num_cameras_; ++i)
      htabs_[i]->clear();
  }
  else
    histogram_ = true;
}

void vvid_live_video_manager::display_topology()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vt2Ds_[i]->clear_all();
    vcl_vector<vtol_topology_object_sptr> const & seg = video_process_->get_output_topology();

    for (vcl_vector<vtol_topology_object_sptr>::const_iterator ti=seg.begin();
         ti != seg.end(); ti++)
    {
      if (edges_)
      {
        vtol_edge_2d_sptr e=(*ti)->cast_to_edge()->cast_to_edge_2d();
        if (e)
          vt2Ds_[i]->add_edge(e);
      }
      else
      {
        vtol_face_2d_sptr f=(*ti)->cast_to_face()->cast_to_face_2d();
        if (f)
          vt2Ds_[i]->add_face(f);
      }
    }
  }
}

void vvid_live_video_manager::display_image()
{
  //do nothing for now
}

void vvid_live_video_manager::run_frames()
{
  if (!init_successful_)
    return;
  while (vtabs_.back()->get_video_live()) {
    vul_timer t;
    for (unsigned i=0; i<num_cameras_; ++i)
    {
      vtabs_[i]->update_frame();

      if (histogram_)
        htabs_[i]->update(vtabs_[i]->get_rgb_frame());

      if (!cp_.rgb_&&video_process_)//i.e. grey scale
      {
        vil1_memory_image_of<unsigned char> image;
        video_process_->clear_input();

        if (vtabs_[i]->get_current_mono_image(sample_,image))
          video_process_->add_input_image(image);
        else return;
        if (video_process_->execute())
        {
          if (video_process_->get_output_type()==vpro_video_process::IMAGE)
            display_image();
          if (video_process_->get_output_type()==vpro_video_process::TOPOLOGY)
            display_topology();
        }
      }
      vt2Ds_[i]->post_redraw();
    }
    vgui::run_till_idle();
    // delay until the minimum time has passed for this frame
    while (t.real()<min_msec_per_frame_) ;
    float ft = float(t.real())/1000.f, rate=1e33f;
    if (ft!=0.f) rate = 1.0f/ft;
    vgui::out << "Tf = " << ft << " sec/frame  = " << rate << " frs/sec\n";
  }
}

void vvid_live_video_manager::start_live_video()
{
  if (!init_successful_ || vtabs_.size() != num_cameras_)
    return;

  for (unsigned i=0; i<num_cameras_; ++i)
    if (!vtabs_[i]->start_live_video())
    {
      vcl_cout << "In vvid_live_video_manager::start_live_video() -"
               << " start failed - camera #" << i << vcl_endl;
      return;
    }
  this->run_frames();
}

void vvid_live_video_manager::stop_live_video()
{
  if (!init_successful_)
    return;

  for (unsigned i=0; i<num_cameras_; ++i)
    if (vtabs_[i])
      vtabs_[i]->stop_live_video();
  if (video_process_)
    video_process_->finish();
}

void vvid_live_video_manager::quit()
{
  this->stop_live_video();
  vcl_exit(1);
}

bool vvid_live_video_manager::
get_current_rgb_image(unsigned camera_index,
                      int pix_sample_interval,
                      vil1_memory_image_of< vil1_rgb<unsigned char> >& im)
{
  if (!init_successful_||!vtabs_[camera_index])
  {
    vcl_cout << "In vvid_live_video_manger::get_current_rgb_imge(..) -"
             << " bad initialization\n";
    return false;
  }

  return vtabs_[camera_index]->get_current_rgb_image(pix_sample_interval, im);
}

bool vvid_live_video_manager::
get_current_mono_image(unsigned camera_index,
                       int pix_sample_interval,
                       vil1_memory_image_of<unsigned char>& im)
{
  if (!init_successful_||!vtabs_[camera_index])
  {
    vcl_cout << "In vvid_live_video_manger::get_current_mono_imge(..) -"
             << " bad initialization\n";
    return false;
  }

  return vtabs_[camera_index]->get_current_mono_image(pix_sample_interval, im);
}

void vvid_live_video_manager::
set_process_rgb_image(vil1_memory_image_of< vil1_rgb<unsigned char> >& im)
{
  process_rgb_ = im;
}

void vvid_live_video_manager::
set_process_mono_image(vil1_memory_image_of<unsigned char>& im)
{
  process_mono_ = im;
}
