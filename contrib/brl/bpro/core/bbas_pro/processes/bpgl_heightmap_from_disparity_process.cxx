//This is brl/bpro/core/bbas_pro/processes/bpgl_heightmap_from_disparity_process.cxx
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vgl/vgl_box_3d.h>

#include <bpgl/algo/bpgl_heightmap_from_disparity.h>


//: process to convert heightmap to disparity
// template<class CAM_T>
// vil_image_view<float>
// bpgl_heightmap_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
//                               vil_image_view<float> disparity, vgl_box_3d<double> heightmap_bounds,
//                               double ground_sample_distance);

namespace bpgl_heightmap_from_disparity_process_globals
{
  unsigned n_inputs_  = 5;
  unsigned n_outputs_ = 1;
}


bool bpgl_heightmap_from_disparity_process_cons(bprb_func_process& pro)
{
  using namespace bpgl_heightmap_from_disparity_process_globals;

  std::vector<std::string> input_types_;
  input_types_.emplace_back("vpgl_camera_double_sptr"); // vpgl_affine_camera<double> camera 1
  input_types_.emplace_back("vpgl_camera_double_sptr"); // vpgl_affine_camera<double> camera 2
  input_types_.emplace_back("vil_image_view_base_sptr"); // vil_image_view<float> disparity
  input_types_.emplace_back("double"); // min point x (e.g. lower left corner of a scene bbox)
  input_types_.emplace_back("double"); // min point y
  input_types_.emplace_back("double"); // min point z
  input_types_.emplace_back("double"); // max point x (e.g. upper right corner of a scene bbox)
  input_types_.emplace_back("double"); // max point y
  input_types_.emplace_back("double"); // max point z
  input_types_.emplace_back("double"); // ground sample distance

  std::vector<std::string> output_types_;
  output_types_.emplace_back("vil_image_view_base_sptr"); // vil_image_view<float> heightmap

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool bpgl_heightmap_from_disparity_process(bprb_func_process& pro)
{
  using namespace bpgl_heightmap_from_disparity_process_globals;

  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": wrong inputs!!!\n";
    return false;
  }

  // get inputs
  unsigned i = 0;
  vpgl_camera_double_sptr camera1_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr camera2_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr disparity_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  auto min_x = pro.get_input<double>(i++);
  auto min_y = pro.get_input<double>(i++);
  auto min_z = pro.get_input<double>(i++);
  auto max_x = pro.get_input<double>(i++);
  auto max_y = pro.get_input<double>(i++);
  auto max_z = pro.get_input<double>(i++);
  auto gsd = pro.get_input<double>(i++);

  // convert cameras
  auto* camera1 = dynamic_cast<vpgl_affine_camera<double>*> (camera1_sptr.as_pointer());
  if (!camera1) {
    std::cerr << pro.name() << " :-- camera 1 is not affine" << std::endl;
    return false;
  }
  auto* camera2 = dynamic_cast<vpgl_affine_camera<double>*> (camera2_sptr.as_pointer());
  if (!camera2) {
    std::cerr << pro.name() << " :-- camera 2 is not affine" << std::endl;
    return false;
  }

  // convert image
  vil_image_view<float> disparity = *vil_convert_cast(float(), disparity_sptr);

  // bounding box
  vgl_box_3d<double> heightmap_bounds(min_x,min_y,min_z, max_x,max_y,max_z);

  // process
  vil_image_view<float> heightmap = bpgl_heightmap_from_disparity(
      camera1, camera2, disparity, heightmap_bounds, gsd);

  // return
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(heightmap));
  return true;
}
