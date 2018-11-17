#ifndef boxm2_ocl_kernel_filter_process_h_
#define boxm2_ocl_kernel_filter_process_h_
//:
// \file
// \brief A process to filter a boxm2 scene with a bvpl_kernel. this header is used in boxm_ocl_tests
// \author Isabel Restrepo
// \date April 12, 2012


#include <string>
#include <iostream>
#include <map>
#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <bvpl/kernels/bvpl_kernel.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//:global variables
namespace boxm2_ocl_kernel_filter_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;

  bool compile_filter_kernel(const bocl_device_sptr& device, bocl_kernel * filter_kernel, std::string opts);

  bool process(const bocl_device_sptr& device, const boxm2_scene_sptr& scene, const boxm2_opencl_cache_sptr& opencl_cache, const bvpl_kernel_sptr& filter);

  static std::map<std::string,bocl_kernel* > kernels;
}


//: Process related functions
bool boxm2_ocl_kernel_filter_process_cons(bprb_func_process& pro);
bool boxm2_ocl_kernel_filter_process(bprb_func_process& pro);


#endif // boxm2_ocl_kernel_filter_process_h_
