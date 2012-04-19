#ifndef boxm2_ocl_kernel_filter_process_h_
#define boxm2_ocl_kernel_filter_process_h_
//:
// \file
// \brief A process to filter a boxm2 scene with a bvpl_kernel. this header is used in boxm_ocl_tests
// \author Isabel Restrepo
// \date April 12, 2012


#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <bvpl/kernels/bvpl_kernel.h>

#include <vcl_string.h>
#include <vcl_map.h>


//:global variables
namespace boxm2_ocl_kernel_filter_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;

  bool compile_filter_kernel(bocl_device_sptr device, bocl_kernel * filter_kernel, vcl_string opts);

  bool process(bocl_device_sptr device, boxm2_scene_sptr scene, boxm2_opencl_cache_sptr opencl_cache, bvpl_kernel_sptr filter);

  static vcl_map<vcl_string,bocl_kernel* > kernels;
}


//: Process related functions
bool boxm2_ocl_kernel_filter_process_cons(bprb_func_process& pro);
bool boxm2_ocl_kernel_filter_process(bprb_func_process& pro);


#endif // boxm2_ocl_kernel_filter_process_h_
