// This is brl/bpro/core/vpgl_pro/processes/vpgl_compute_affine_from_rational_process.cxx
//:
// \file
// \brief A process for computing an affine camera approximation of a "local" rational camera.
//        i.e. use case: create a bvxm/boxm2 scene, crop a satellite image using the scene and create a "local" rational camera (cropped camera) using its local bounding box in 3d
//        The process samples random points from the input bounding box
//        projects these points into the image using local rational camera to find image points of these 3d points
//        then uses vpgl_affine_camera_compute to fit an affine camera to these projections
//        the output affine camera is in the local vertical coordinte system given by the LVCS of the local rational camera
// \author Ozge C. Ozcanli
// \date Nov 26, 2013

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/algo/vpgl_affine_rectification.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vnl/vnl_random.h>
#include <vgl/vgl_box_3d.h>

bool vpgl_compute_affine_from_rat_process_cons(bprb_func_process& pro)
{
  //set output types
  std::vector<std::string> input_types_(8);
  int i=0;
  input_types_[i++] = "vpgl_camera_double_sptr";  // camera  -- pass local rational camera
  input_types_[i++] = "double";    // min point x (e.g. lower left corner of a scene bbox)
  input_types_[i++] = "double";    // min point y
  input_types_[i++] = "double";    // min point z
  input_types_[i++] = "double";    // max point x (e.g. upper right corner of a scene bbox)
  input_types_[i++] = "double";    // max point y
  input_types_[i++] = "double";    // max point z
  input_types_[i++] = "unsigned";    // n_points -- randomly sample this many points form the voxel volume, e.g. 100
  if (!pro.set_input_types(input_types_))
    return false;
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vpgl_camera_double_sptr";  // output affine camera
  return pro.set_output_types(output_types_);
}

bool vpgl_compute_affine_from_rat_process(bprb_func_process& pro)
{
  unsigned i = 0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto min_x = pro.get_input<double>(i++);
  auto min_y = pro.get_input<double>(i++);
  auto min_z = pro.get_input<double>(i++);
  auto max_x = pro.get_input<double>(i++);
  auto max_y = pro.get_input<double>(i++);
  auto max_z = pro.get_input<double>(i++);
  auto n_points = pro.get_input<unsigned>(i++);
  if (n_points <= 3)
    n_points = 10;   // make it minimum 10 points

  if (!camera) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  auto* rat_camera = dynamic_cast<vpgl_local_rational_camera<double>*> (camera.as_pointer());
  if (!rat_camera) {
    std::cout << pro.name() <<" :--  Input camera is not a local rational camera!\n";
    return false;
  }

  vgl_point_3d<double> pmin(min_x, min_y, min_z);
  vgl_point_3d<double> pmax(max_x, max_y, max_z);
  vgl_box_3d<double> bb;
  bb.add(pmin); bb.add(pmax);

  vpgl_affine_camera<double>* acam_ptr = new vpgl_affine_camera<double>;
  bool good = vpgl_affine_camera_convert::convert(*rat_camera, bb, *acam_ptr, n_points);
  if(!good)
    return false;

  pro.set_output_val<vpgl_camera_double_sptr>(0, acam_ptr);
  return true;
}
