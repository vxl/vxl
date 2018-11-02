// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_merge_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_merge_block_function.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <vul/vul_timer.h>

namespace boxm2_cpp_merge_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_merge_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_merge_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";   //scene to operate on
  input_types_[1] = "boxm2_cache_sptr";   //cache with access to scene blocks
  input_types_[2] = "float";              //threshold occupancy probability (if all 8 children are below this, merge them)

  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_merge_process(bprb_func_process& pro)
{
  vul_timer t;
  using namespace boxm2_cpp_merge_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  std::cout<<"Getting inputs"<<std::endl;
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  std::cout<<"Getting thresh input"<<std::endl;
  auto thresh = pro.get_input<float>(i++);

  //check datatype
  bool foundDataType = false;
  std::string data_type;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_CPP_MERGE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  //iterate through each block, run merge function
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout<<"Merging Block: "<<id<<std::endl;
    boxm2_block *     blk = cache->get_block(scene, id);
    boxm2_data_base * alph = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix(), 0, false);
    boxm2_data_base * mog = cache->get_data_base(scene,id,data_type, 0, false);
    boxm2_data_base * num_obs = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_NUM_OBS>::prefix(), 0, false);

    std::vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);

    //refine block and datas
    boxm2_block_metadata data = blk_iter->second;
    boxm2_merge_block(scene, blk,datas, thresh, false);
    blk->enable_write(); // now cache will make sure that it is written to disc
  }

  std::cout<<"  merge time: "<<t.all()/1000.0f<<" sec"<<std::endl;
  return true;
}
