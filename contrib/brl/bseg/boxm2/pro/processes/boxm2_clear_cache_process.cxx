// This is brl/bseg/boxm2/pro/processes/boxm2_clear_cache_process.cxx
//:
// \file
// \brief  A process to clear cpu cache.
//
// \author Ali Osman Ulusoy
// \date Oct 05, 2011

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_nn_cache.h>


namespace boxm2_clear_cache_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_clear_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_clear_cache_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_cache_sptr";

  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_clear_cache_process(bprb_func_process& pro)
{
  using namespace boxm2_clear_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  cache->clear_cache();
  return true;
}
