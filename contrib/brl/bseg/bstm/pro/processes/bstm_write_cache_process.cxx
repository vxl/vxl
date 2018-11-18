// This is brl/bseg/bstm/pro/processes/bstm_write_cache_process.cxx
//:
// \file
// \brief  Writes cache out to disk.
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


namespace bstm_write_cache_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}
bool bstm_write_cache_process_cons(bprb_func_process& pro)
{
  using namespace bstm_write_cache_process_globals;

  //process takes 2 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_cache_sptr";
  input_types_[1] = "bool";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);

  // in case the 2nd input is not set
  brdb_value_sptr idx = new brdb_value_t<bool>(false);
  pro.set_input(1, idx);
  return good;
}

bool bstm_write_cache_process(bprb_func_process& pro)
{
  using namespace bstm_write_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_cache_sptr cache = pro.get_input<bstm_cache_sptr>(i++);
  bool clear_cache = pro.get_input<bool>(i++);

  cache->write_to_disk();
  if (clear_cache)
    cache->clear_cache();
  return true;
}
