// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_scene_uncertainty_map_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the uncertainty of the scene.
//
// \author Vishal Jain
// \date Mar 12, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_scene_uncertainty_map.h>


namespace boxm2_ocl_render_scene_uncertainty_map_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 2;
}

bool boxm2_ocl_render_scene_uncertainty_map_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_scene_uncertainty_map_process_globals;

  //process takes 8 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";
  input_types_[5] = "vcl_string";
  input_types_[6] = "vcl_string"; // directory of the cameras used
  input_types_[7] = "vcl_string"; // directory of the cameras unused

  // process has 2 outputs
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; // scene sptr
  output_types_[1] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 7th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(6, idx);
  return good;
}

bool boxm2_ocl_render_scene_uncertainty_map_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_scene_uncertainty_map_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  auto ni=pro.get_input<unsigned>(i++);
  auto nj=pro.get_input<unsigned>(i++);
  std::string ident = pro.get_input<std::string>(i++);
  std::string cam_dir_1 = pro.get_input<std::string>(i++);
  std::string cam_dir_2 = pro.get_input<std::string>(i++);
  auto * exp_img_out = new vil_image_view<float>(ni,nj);
  auto * radial_img_out = new vil_image_view<unsigned char>(ni,ni,3);
  //: render scene uncertainty
  boxm2_ocl_render_scene_uncertainty_map::render_scene_uncertainty_map(scene,device,opencl_cache,ni,nj,ident,exp_img_out,radial_img_out,cam_dir_1,cam_dir_2);
  i=0;
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, radial_img_out);
  return true;
}
