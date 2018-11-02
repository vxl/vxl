// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_aggregate_normal_from_filter_vector_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to interpolate the responses of first order derivatives filters into a normal
// \author Ali Osman Ulusoy
// \verbatim
//  Modifications
//   April 17, 2012  Isabel Restrepo: Take the vector of filters an input. This provides filter names, number and orientation
// \endverbatim
// \date Feb 13, 2011

//process utilities
#include <brdb/brdb_value.h>

//OpenCL
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_cl.h>

//boxm2
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/algo/boxm2_ocl_aggregate_normal_from_filter_vector.h>

//filters
#include <bvpl/kernels/bvpl_kernel.h>

//utilities
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>

namespace boxm2_ocl_aggregate_normal_from_filter_vector_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;

  static std::map<std::string, boxm2_ocl_aggregate_normal_from_filter_vector*> engines;
}

bool boxm2_ocl_aggregate_normal_from_filter_vector_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_aggregate_normal_from_filter_vector_process_globals;

  // process has 4 inputs and no outputs:
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "bvpl_kernel_vector_sptr";

  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_ocl_aggregate_normal_from_filter_vector_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_aggregate_normal_from_filter_vector_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;

  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bvpl_kernel_vector_sptr filter_vector = pro.get_input<bvpl_kernel_vector_sptr>(i++);

  //cache size sanity check
  long binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  // compile the kernel if not already compiled
  std::string identifier=device->device_identifier();

  if (engines.find(identifier) == engines.end()) {
    engines[identifier] = new boxm2_ocl_aggregate_normal_from_filter_vector(scene, opencl_cache,
                                                                            device, filter_vector);
  }

  return engines[identifier]->run();
}
