// This is brl/bpro/core/vpgl_pro/processes/vpgl_compute_image_to_world_homography_process.cxx
//:
// \file
// \brief
// \date May 19, 2014

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_camera_homographies.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vnl/vnl_random.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
bool vpgl_compute_image_to_world_homography_process_cons(bprb_func_process& pro)
{
  //set output types
  std::vector<std::string> input_types_(3);
  int i=0;
  input_types_[i++] = "vpgl_camera_double_sptr";  // camera  -- pass local rational camera
  input_types_[i++] = "bbas_1d_array_float_sptr";    //world plane
  input_types_[i++] = "bool";
  if (!pro.set_input_types(input_types_))
    return false;
  std::vector<std::string> output_types_(1);
  output_types_[0] = "bbas_1d_array_float_sptr";  // output affine camera
  return pro.set_output_types(output_types_);
}

bool vpgl_compute_image_to_world_homography_process(bprb_func_process& pro)
{
  unsigned i = 0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bbas_1d_array_float_sptr plane_params_ptr = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bool inverse = pro.get_input<bool>(i++);
  vbl_array_1d<float>& plane_params = plane_params_ptr->data_array;
  vgl_plane_3d<double> plane(plane_params[0],plane_params[1],plane_params[2],plane_params[3]);

  vgl_h_matrix_2d<double> H;
  if(auto * pcam
       = dynamic_cast<vpgl_perspective_camera<double>* > (camera.ptr() ))
  {
      if(!inverse)
          H  = vpgl_camera_homographies::homography_from_camera(*pcam, plane);
      else
          H  = vpgl_camera_homographies::homography_to_camera(*pcam, plane);
  }

  vbl_array_1d<float> h_params(9,0.0);

  unsigned count = 0;
  for(unsigned k = 0 ; k < H.get_matrix().rows(); k++)
      for(unsigned j = 0 ; j < H.get_matrix().cols(); j++)
           h_params[count++] = H.get_matrix()(k,j);

  bbas_1d_array_float_sptr h_params_ptr = new bbas_1d_array_float(9);
  h_params_ptr->data_array = h_params;
  pro.set_output_val<bbas_1d_array_float_sptr>(0, h_params_ptr);
  return true;
}
