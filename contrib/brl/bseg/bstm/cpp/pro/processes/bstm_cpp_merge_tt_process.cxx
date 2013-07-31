// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_merge_tt_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for merging the time trees.
//
// \author Ali Osman Ulusoy
// \date June 17, 2013

#include <vcl_fstream.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bstm/cpp/algo/bstm_merge_tt_function.h>

#include <boxm2/basic/boxm2_array_1d.h>

namespace bstm_cpp_merge_tt_process_globals
{
  const unsigned n_inputs_ =  4;
  const unsigned n_outputs_ = 0;
}

bool bstm_cpp_merge_tt_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_merge_tt_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //p_threshold
  input_types_[3] = "float"; //time


  // process has 0 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_merge_tt_process(bprb_func_process& pro)
{

  using namespace bstm_cpp_merge_tt_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  float p_threshold =pro.get_input<float>(i++);
  float time =pro.get_input<float>(i++);


  bool foundAppDataType = false, foundNumobsDataType = false;

  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix() )
    {
      foundAppDataType = true;
    }
    else if ( apps[i] == bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix() )
    {
      foundNumobsDataType = true;
    }
  }
  if (!foundAppDataType || !foundNumobsDataType ) {
    vcl_cout<<"bstm_cpp_merge_tt_process_ERROR: scene doesn't have BSTM_MOG6_VIEW_COMPACT or BSTM_NUM_OBS_VIEW_COMPACT data type"<<vcl_endl;
    return false;
  }

  vcl_map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  vcl_map<bstm_block_id, bstm_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    bstm_block_id id = blk_iter->first;

    //skip block if it doesn't contain curr time.
    bstm_block_metadata mdata =  blk_iter->second;
    double local_time;
    if(!mdata.contains_t(time,local_time))
      continue;
    vcl_cout<<"Merging Block: "<<id<<vcl_endl;

    bstm_block     * blk     = cache->get_block(id);
    bstm_time_block* blk_t   = cache->get_time_block(id);
    bstm_data_base * alph    = cache->get_data_base(id,bstm_data_traits<BSTM_ALPHA>::prefix());
    bstm_data_base * mog     = cache->get_data_base(id,bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
    bstm_data_base * num_obs = cache->get_data_base(id,bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix() );

    vcl_vector<bstm_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);

    //refine block and datas
    bstm_merge_tt_blk( blk_t, blk, datas, p_threshold);
  }

  vcl_cout << "Finished merging scene..." << vcl_endl;
  return true;
}
