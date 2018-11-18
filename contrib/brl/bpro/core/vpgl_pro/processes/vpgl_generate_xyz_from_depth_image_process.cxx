// This is brl/bpro/core/vpgl_pro/processes/vpgl_generate_xyz_from_depth_image_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view.h>

namespace vpgl_generate_xyz_from_depth_image_process_globals
{
    constexpr unsigned n_inputs_ = 2;
    constexpr unsigned n_outputs_ = 3;
}
//: Init function
bool vpgl_generate_xyz_from_depth_image_process_cons(bprb_func_process& pro)
{
    using namespace vpgl_generate_xyz_from_depth_image_process_globals;

    //process takes 2 inputs
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "vpgl_camera_double_sptr";
    input_types_[1] = "vil_image_view_base_sptr";

    // process has 3 outputs
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "vil_image_view_base_sptr";  // x image
    output_types_[1] = "vil_image_view_base_sptr";  // y image
    output_types_[2] = "vil_image_view_base_sptr";  // z image

    return pro.set_input_types(input_types_) &&
           pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_generate_xyz_from_depth_image_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_generate_xyz_from_depth_image_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr  cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr depth_img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);


  auto* depth_img = dynamic_cast<vil_image_view<float>*>(depth_img_ptr.ptr());
  if ( !depth_img ) {
    std::cout<<"Depth image cannot be converted to float image"<<std::endl;
    return false;
  }
  unsigned ni = depth_img->ni();
  unsigned nj = depth_img->nj();

  auto* out_img_x = new vil_image_view<float>(ni, nj);
  auto* out_img_y = new vil_image_view<float>(ni, nj);
  auto* out_img_z = new vil_image_view<float>(ni, nj);
  if(auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr()))
    {
    for (unsigned int u=0; u < ni; ++u)
      {
      for (unsigned int v=0; v < nj; ++v)
        {
        vgl_ray_3d<double> ray = cam->backproject_ray(vgl_point_2d<double>(u,v));
        //std::cout<<ray.origin()<<std::endl;
        float t = (*depth_img)(u,v);
        vgl_point_3d<double> pt3d = ray.origin()+ray.direction()*t;
        (*out_img_x)(u,v) = (float)pt3d.x();
        (*out_img_y)(u,v) = (float)pt3d.y();
        (*out_img_z)(u,v) = (float)pt3d.z();
        }
      }
    }
  else if(auto* cam = dynamic_cast<vpgl_generic_camera<double>*>(cam_ptr.ptr()))
    {
    for (unsigned int u=0; u < ni; ++u)
      {
      for (unsigned int v=0; v < nj; ++v)
        {
        vgl_ray_3d<double> ray = cam->ray(u,v);
        //std::cout<<ray.origin()<<std::endl;
        float t = (*depth_img)(u,v);
        vgl_point_3d<double> pt3d = ray.origin()+ray.direction()*t;
        (*out_img_x)(u,v) = (float)pt3d.x();
        (*out_img_y)(u,v) = (float)pt3d.y();
        (*out_img_z)(u,v) = (float)pt3d.z();
        }
      }
    }
  else
    {
    std::cerr << "vpgl_generate_xyz_from_depth_image_process: couldn't cast camera\n";
    return false;
    }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);

  return true;

}
