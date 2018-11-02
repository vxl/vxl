// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_sun_visibilities_process.cxx
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_sun_visibilities.h>
//brdb stuff
#include <brdb/brdb_value.h>

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_sun_visibilities_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_ocl_update_sun_visibilities_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_sun_visibilities_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "boxm2_cache_sptr";
  input_types_[4] = "vpgl_camera_double_sptr";
  input_types_[5] = "unsigned";
  input_types_[6] = "unsigned";
  input_types_[7] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_update_sun_visibilities_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_sun_visibilities_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs: device, scene, opencl_cache, cam, img
  unsigned argIdx = 0;
  bocl_device_sptr          device = pro.get_input<bocl_device_sptr>(argIdx++);
  boxm2_scene_sptr          scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(argIdx++);
  boxm2_cache_sptr          cache = pro.get_input<boxm2_cache_sptr>(argIdx++);
  vpgl_camera_double_sptr   cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  auto                  ni = pro.get_input<unsigned>(argIdx++);
  auto                  nj = pro.get_input<unsigned>(argIdx++);
  std::string                prefix_name = pro.get_input<std::string>(argIdx++);

  return boxm2_ocl_update_sun_visibilities::update(scene, device, opencl_cache, cache, cam, ni, nj, prefix_name);
}
