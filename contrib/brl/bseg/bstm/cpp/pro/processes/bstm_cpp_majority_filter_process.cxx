// This is brl/bseg/bstm/pro/processes/bstm_cpp_majority_filter_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file

//
// \author Ali Osman Ulusoy
// \date June 25, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>
#include <bstm/bstm_util.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/bstm_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <bstm/cpp/algo/bstm_majority_filter.h>

namespace bstm_cpp_majority_filter_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_cpp_majority_filter_process_cons (bprb_func_process& pro)
{
  using namespace bstm_cpp_majority_filter_process_globals;

  //process takes 3 inputs, no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //time

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool bstm_cpp_majority_filter_process (bprb_func_process& pro)
{
  using namespace bstm_cpp_majority_filter_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache = pro.get_input<bstm_cache_sptr>(i++);
  auto time = pro.get_input<float>(i++);

  //zip through each block
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    bstm_block_id id = blk_iter->first;
    bstm_block_metadata data = blk_iter->second;
    double local_time;
    if(!data.contains_t(time,local_time))
      continue;
    int time_tree_index = std::floor(local_time);

    std::cout << "Filtering " << id << std::endl;

    bstm_block *     blk = cache->get_block(id);
    bstm_time_block *     blk_t = cache->get_time_block(id);

    bstm_data_base * change = cache->get_data_base(id,bstm_data_traits<BSTM_CHANGE>::prefix());
    bstm_majority_filter(data, blk, blk_t,change);
  }

  return true;
}
