// This is brl/bseg/bstm_multi/pro/processes/bstm_multi_create_cache_process.cxx
//:
// \file
// \brief  A process for creating a cache for a bstm_multi scene.
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <bprb/bprb_func_process.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/io/block_simple_cache.h>
#include <bstm_multi/space_time_scene.h>

namespace {
const unsigned n_inputs_ = 2;
const unsigned n_outputs_ = 1;
}

bool bstm_multi_create_cache_process_cons(bprb_func_process &pro) {
  vcl_vector<vcl_string> input_types_(::n_inputs_);
  input_types_[0] = "bstm_multi_scene_sptr"; // scene sptr
  input_types_[1] = "vcl_string";            // cache type

  vcl_vector<vcl_string> output_types_(::n_outputs_);
  output_types_[0] = "bstm_cache_sptr";
  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bstm_multi_create_cache_process(bprb_func_process &pro) {

  if (pro.n_inputs() < ::n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << ::n_inputs_
             << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(i++);
  vcl_string cache_type = pro.get_input<vcl_string>(i++);
  if (cache_type == "simple")
    block_simple_cache<bstm_multi_scene, bstm_multi_block>::create(scene);
  else {
    vcl_cerr << "Don't recognize cache type " << cache_type << " exiting..."
             << vcl_endl;
    return false;
  }

  pro.set_output_val<bstm_multi_cache_sptr>(0, bstm_multi_cache::instance());
  return true;
}
