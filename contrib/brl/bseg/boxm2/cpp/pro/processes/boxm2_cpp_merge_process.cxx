// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_merge_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
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
  const unsigned n_inputs_ =  3;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_merge_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_merge_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";   //scene to operate on 
  input_types_[1] = "boxm2_cache_sptr";   //cache with access to scene blocks
  input_types_[2] = "float";              //threshold occupancy probability (if all 8 children are below this, merge them)

  // process has 0 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_merge_process(bprb_func_process& pro)
{
  vul_timer t; 
  using namespace boxm2_cpp_merge_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  vcl_cout<<"Getting inputs"<<vcl_endl;
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vcl_cout<<"Getting thresh input"<<vcl_endl;
  float thresh = pro.get_input<float>(i++);

  //check datatype
  bool foundDataType = false;
  vcl_string data_type;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }
  
  //iterate through each block, run merge function
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout<<"Merging Block: "<<id<<vcl_endl;
    boxm2_block *     blk     = cache->get_block(id);
    boxm2_data_base * alph    = cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix(), 0, false);
    boxm2_data_base * mog     = cache->get_data_base(id,data_type, 0, false);
    boxm2_data_base * num_obs = cache->get_data_base(id,boxm2_data_traits<BOXM2_NUM_OBS>::prefix(), 0, false);

    vcl_vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);

    //refine block and datas
    boxm2_block_metadata data = blk_iter->second;
    boxm2_merge_block(blk,datas, thresh, false);
    blk->enable_write(); // now cache will make sure that it is written to disc
  }
  
  vcl_cout<<"  merge time: "<<t.all()/1000.0f<<" sec"<<vcl_endl;
  return true;
}
