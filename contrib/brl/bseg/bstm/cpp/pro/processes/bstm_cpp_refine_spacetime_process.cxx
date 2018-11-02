// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_refine_spacetime_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Ali Osman Ulusoy
// \date June 06, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bstm/cpp/algo/bstm_refine_blk_in_spacetime_function.h>

#include <boxm2/basic/boxm2_array_1d.h>

namespace bstm_cpp_refine_spacetime_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_cpp_refine_spacetime_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_refine_spacetime_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //p_threshold
  input_types_[3] = "float"; //time


  // process has 0 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_refine_spacetime_process(bprb_func_process& pro)
{

  using namespace bstm_cpp_refine_spacetime_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  auto p_threshold =pro.get_input<float>(i++);
  auto time =pro.get_input<float>(i++);


  bool foundAppDataType = false, foundNumobsDataType = false;

  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix() )
    {
      foundAppDataType = true;
    }
    else if ( app == bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix() )
    {
      foundNumobsDataType = true;
    }
  }
  if (!foundAppDataType || !foundNumobsDataType ) {
    std::cout<<"BSTM_CPP_REFINE_SPACETIME_PROCESS_ERROR: scene doesn't have BSTM_MOG6_VIEW_COMPACT or BSTM_NUM_OBS_VIEW_COMPACT data type"<<std::endl;
    return false;
  }

  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    bstm_block_id id = blk_iter->first;

    //skip block if it doesn't contain curr time.
    bstm_block_metadata mdata =  blk_iter->second;
    double local_time;
    if(!mdata.contains_t(time,local_time))
      continue;
    std::cout<<"Refining Block: "<<id<<std::endl;

    bstm_block     * blk = cache->get_block(id);
    bstm_time_block* blk_t = cache->get_time_block(id);
    bstm_data_base * alph = cache->get_data_base(id,bstm_data_traits<BSTM_ALPHA>::prefix());
    int num_el = alph->buffer_length() / bstm_data_traits<BSTM_ALPHA>::datasize();
    bstm_data_base * mog = cache->get_data_base(id,bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix(), bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datasize() * num_el);
    bstm_data_base * num_obs = cache->get_data_base(id,bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix(),bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datasize() * num_el );

    std::vector<bstm_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);

    //refine block and datas
    bstm_refine_block_spacetime( blk_t, blk, datas, p_threshold);
  }

  std::cout << "Finished refining scene..." << std::endl;
  return true;
}
