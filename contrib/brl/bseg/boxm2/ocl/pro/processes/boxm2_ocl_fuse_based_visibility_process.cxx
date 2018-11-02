// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_fuse_based_visibility_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for fusing the scene based on visibility
//
// \author Vishal Jain
// \date Nov 13, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>


#include <boxm2/ocl/algo/boxm2_ocl_fuse_based_visibility.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_fuse_based_visibility_process_globals
{
  constexpr unsigned int n_inputs_ = 4;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_fuse_based_visibility_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_fuse_based_visibility_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr"; // scene A
  input_types_[2] = "boxm2_scene_sptr"; // scene B
  input_types_[3] = "boxm2_opencl_cache_sptr";
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_fuse_based_visibility_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_fuse_based_visibility_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         sceneA        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_scene_sptr         sceneB        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vul_timer t;
  t.mark();
  boxm2_ocl_fuse_based_visibility::fuse_based_visibility(sceneA,sceneB, device, opencl_cache);
  std::cout<<"Total time taken is "<<t.all()<<std::endl;
  return true;
}
namespace boxm2_ocl_fuse_based_orientation_process_globals
{
  constexpr unsigned int n_inputs_ = 4;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_fuse_based_orientation_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_fuse_based_orientation_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr"; // scene A
  input_types_[2] = "boxm2_scene_sptr"; // scene B
  input_types_[3] = "boxm2_opencl_cache_sptr";
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_fuse_based_orientation_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_fuse_based_orientation_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         sceneA        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_scene_sptr         sceneB        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vul_timer t;
  t.mark();
  boxm2_ocl_fuse_based_orientation::fuse_based_orientation(sceneA,sceneB, device, opencl_cache);
  std::cout<<"Total time taken is "<<t.all()<<std::endl;
  return true;
}
namespace boxm2_ocl_fuse_surface_density_process_globals
{
  constexpr unsigned int n_inputs_ = 4;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_fuse_surface_density_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_fuse_surface_density_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr"; // scene A
  input_types_[2] = "boxm2_scene_sptr"; // scene B
  input_types_[3] = "boxm2_opencl_cache_sptr";
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_fuse_surface_density_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_fuse_surface_density_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         sceneA        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_scene_sptr         sceneB        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vul_timer t;
  t.mark();
  boxm2_ocl_fuse_surface_density::fuse_surface_density(sceneA,sceneB, device, opencl_cache);
  std::cout<<"Total time taken is "<<t.all()<<std::endl;

  return true;
}
