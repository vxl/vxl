// This is brl/bseg/boxm2/pro/processes/boxm2_persp_cam_from_scene_process.cxx
//:
// \file
// \brief  A process for computing a perspective camera, given a scene, camera center, and desired image resolution
//
// \author J. L. Mundy
// \date May 24, 2011

#include <bprb/bprb_func_process.h>
#include <boxm2/boxm2_scene.h>
#include <bpgl/algo/bpgl_camera_from_box.h>
namespace boxm2_persp_cam_from_scene_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 1;
}
bool boxm2_persp_cam_from_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_persp_cam_from_scene_process_globals;

  //process takes 1 input, the scene
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "float"; //Xc
  input_types_[2] = "float"; //Yc
  input_types_[3] = "float"; //Zc
  input_types_[4] = "unsigned"; //ni
  input_types_[5] = "unsigned"; //nj

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";// longitude

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_persp_cam_from_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_persp_cam_from_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  float xc  = pro.get_input<float>(1);
  float yc  = pro.get_input<float>(2);
  float zc  = pro.get_input<float>(3);
  unsigned ni = pro.get_input<unsigned>(4);
  unsigned nj = pro.get_input<unsigned>(5);
  // get the scene bounding box
  vgl_box_3d<double> box = scene->bounding_box();
  vgl_point_3d<double> cam_center(xc, yc, zc);
  vpgl_perspective_camera<double> cam = 
    bpgl_camera_from_box::persp_camera_from_box(box, cam_center, ni, nj);
  
  //set the output
  pro.set_output_val<vpgl_camera_double_sptr>(0, new vpgl_perspective_camera<double>(cam));
  return true;
}
