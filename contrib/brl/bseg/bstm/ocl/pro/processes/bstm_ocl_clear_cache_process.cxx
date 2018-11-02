// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_clear_cache_process.cxx
//:
// \file


#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/ocl/bstm_opencl_cache.h>

namespace bstm_clear_opencl_cache_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_clear_opencl_cache_process_cons(bprb_func_process& pro)
{
  using namespace bstm_clear_opencl_cache_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_opencl_cache_sptr";

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_clear_opencl_cache_process(bprb_func_process& pro)
{
  using namespace bstm_clear_opencl_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_opencl_cache_sptr ocl_cache = pro.get_input<bstm_opencl_cache_sptr>(i++);
  ocl_cache->clear_cache();
  return true;
}
