// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_filter_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A median filtering process
//
// \author Andrew Miller
// \date May 16, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_filter_block_function.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_filter_process_globals
{
  const unsigned n_inputs_ =  2;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_filter_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_filter_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  //input_types_[2] = "float";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_filter_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_filter_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);

  //zip through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout<<"Filtering Block: "<<id<<vcl_endl;

    boxm2_block *     blk     = cache->get_block(id);
    boxm2_data_base * alph    = cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());

    //refine block and datas
    boxm2_block_metadata data = blk_iter->second;
    boxm2_filter_block_function(data, blk,alph);
  }

  return true;
}
