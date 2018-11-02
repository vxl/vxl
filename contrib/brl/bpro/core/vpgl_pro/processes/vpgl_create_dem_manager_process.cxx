// This is brl/bpro/core/vpgl_pro/processes/vpgl_create_dem_manager_process.cxx
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
bool vpgl_create_dem_manager_process_cons(bprb_func_process& pro)
{
  //this process takes three inputs: the dem image resource pointer, zmin, zmax
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_resource_sptr");
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // the process has one output, the dem_manager
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_dem_manager_sptr");  // the dem manager
  output_types.emplace_back("double");  // zmin
  output_types.emplace_back("double");  // zmax
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;

}

//: Execute the process
bool vpgl_create_dem_manager_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 3) {
    std::cout << "vpgl_create_dem_manager_process: The input number should be 3" << std::endl;
    return false;
  }

  // get the inputs
  vil_image_resource_sptr resc = pro.get_input<vil_image_resource_sptr>(0);
  auto zmin = pro.get_input<double>(1);
  auto zmax = pro.get_input<double>(2);
  if(!resc){
    std::cout << "vpgl_create_dem_manager_process: Null input image resource" << std::endl;
    return false;
  }
  vpgl_dem_manager_sptr dem_mgr = new vpgl_dem_manager(resc,zmin,zmax);

  pro.set_output_val<vpgl_dem_manager_sptr>(0, dem_mgr);
  pro.set_output_val<double>(1, (double)dem_mgr->zmin());
  pro.set_output_val<double>(2, (double)dem_mgr->zmax());

  return true;
}
