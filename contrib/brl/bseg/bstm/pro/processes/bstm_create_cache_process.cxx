// This is brl/bseg/bstm/pro/processes/bstm_create_cache_process.cxx
//:
// \file
// \brief  A process for creating cache.
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



namespace bstm_create_cache_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}
bool bstm_create_cache_process_cons(bprb_func_process& pro)
{
  using namespace bstm_create_cache_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "bstm_cache_sptr";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_create_cache_process(bprb_func_process& pro)
{
  using namespace bstm_create_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene= pro.get_input<bstm_scene_sptr>(i++);
  std::string cache_type= pro.get_input<std::string>(i++);
  if(cache_type=="lru")
    bstm_lru_cache::create(scene);
  else
  {
    std::cerr << "Don't recognize cache type " << cache_type << " exiting..." << std::endl;
    return false;
  }

  // store scene smaprt pointer
  pro.set_output_val<bstm_cache_sptr>(0, bstm_cache::instance());
  return true;
}
