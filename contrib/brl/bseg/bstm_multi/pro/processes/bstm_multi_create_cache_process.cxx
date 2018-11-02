// This is brl/bseg/bstm_multi/pro/processes/bstm_multi_create_cache_process.cxx
//:
// \file
// \brief  A process for creating a cache for a bstm_multi scene.
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/io/block_simple_cache.h>
#include <bstm_multi/space_time_scene.h>

namespace {
constexpr unsigned n_inputs_ = 2;
constexpr unsigned n_outputs_ = 1;
}

bool bstm_multi_create_cache_process_cons(bprb_func_process &pro) {
  std::vector<std::string> input_types_(::n_inputs_);
  input_types_[0] = "bstm_multi_scene_sptr"; // scene sptr
  input_types_[1] = "vcl_string";            // cache type

  std::vector<std::string> output_types_(::n_outputs_);
  output_types_[0] = "bstm_cache_sptr";
  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bstm_multi_create_cache_process(bprb_func_process &pro) {

  if (pro.n_inputs() < ::n_inputs_) {
    std::cout << pro.name() << ": The input number should be " << ::n_inputs_
             << std::endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(i++);
  std::string cache_type = pro.get_input<std::string>(i++);
  if (cache_type == "simple")
    block_simple_cache<bstm_multi_scene, bstm_multi_block>::create(scene);
  else {
    std::cerr << "Don't recognize cache type " << cache_type << " exiting..."
             << std::endl;
    return false;
  }

  pro.set_output_val<bstm_multi_cache_sptr>(0, bstm_multi_cache::instance());
  return true;
}
