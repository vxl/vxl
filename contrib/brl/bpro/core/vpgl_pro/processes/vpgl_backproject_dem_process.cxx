// This is brl/bpro/core/vpgl_pro/processes/vpgl_backproject_dem_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vil/vil_image_resource_sptr.h>
#include "../vpgl_dem_manager.h"
//: initialization
bool vpgl_backproject_dem_process_cons(bprb_func_process& pro)
{
  //this process takes 5 inputs: dem_manager, camera, u, v, error tolerance
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_dem_manager_sptr");
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("double");  // x
  output_types.emplace_back("double");  // y
  output_types.emplace_back("double");  // z
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;

}

//: Execute the process
bool vpgl_backproject_dem_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 5) {
    std::cout << "vpgl_backproject_dem_process: The input number should be 5" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vpgl_dem_manager_sptr dem_mgr = pro.get_input<vpgl_dem_manager_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto u = pro.get_input<double>(i++);
  auto v = pro.get_input<double>(i++);
  auto err_tol = pro.get_input<double>(i);
  if(!dem_mgr || !cam){
   std::cout << "vpgl_backproject_dem_process: Null input dem_mgr or camera" << std::endl;
    return false;
  }
  double x, y, z;
  // if err_tol = -1, use default
  bool good = true;
  std::cout << "[u,v]=[" << u << ',' << v << "], err tol=" << err_tol << std::endl;
  if(err_tol>0){
    good = dem_mgr->back_project(cam.ptr(), u, v, x, y, z, err_tol);
    std::cout << "PX,PY,PZ " << x << ' ' << y << ' ' << z << std::endl;
  }else {
    good = dem_mgr->back_project(cam.ptr(), u, v, x, y, z);
    std::cout << "DPX,DPY,DPZ " << x << ' ' << y << ' ' << z << std::endl;
  }
    if(!good){
    std::cout << "vpgl_backproject_dem_process: backproject onto dem failed" << std::endl;
    return false;
  }
  std::cout << "XYZ " << x << ' ' << y << ' ' << z << std::endl;
  pro.set_output_val<double>(0, (double)x);
  pro.set_output_val<double>(1, (double)y);
  pro.set_output_val<double>(2, (double)z);
  return true;
}
