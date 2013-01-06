// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_create_cache_process.cxx
//:
// \file

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <bstm/bstm_scene.h>
#include <bocl/bocl_device.h>
#include <bstm/ocl/bstm_opencl_cache.h>

namespace bstm_create_opencl_cache_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

bool bstm_create_opencl_cache_process_cons(bprb_func_process& pro)
{
  using namespace bstm_create_opencl_cache_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bstm_opencl_cache_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_create_opencl_cache_process(bprb_func_process& pro)
{
  using namespace bstm_create_opencl_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_opencl_cache_sptr opencl_cache= new bstm_opencl_cache(scene, device);
  i=0;

  // store scene smart pointer
  pro.set_output_val<bstm_opencl_cache_sptr>(i++, opencl_cache);
  return true;
}
