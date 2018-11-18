// This is brl/bseg/bstm/pro/processes/bstm_clear_cache_process.cxx
//:
// \file
// \brief  Clear cpu cache
//
// \author Ali Osman Ulusoy
// \date Nov 27, 2012

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>


namespace bstm_clear_cache_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 0;
}
bool bstm_clear_cache_process_cons(bprb_func_process& pro)
{
  using namespace bstm_clear_cache_process_globals;

  //process takes 2 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_cache_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);


  return good;
}

bool bstm_clear_cache_process(bprb_func_process& pro)
{
  using namespace bstm_clear_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_cache_sptr cache = pro.get_input<bstm_cache_sptr>(i++);

  cache->clear_cache();
  return true;
}
