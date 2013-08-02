// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_refine_space_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Ali Osman Ulusoy
// \date June 06, 2013

#include <vcl_fstream.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bstm/cpp/algo/bstm_refine_blk_in_space_function.h>

#include <bstm/bstm_util.h>

namespace bstm_cpp_refine_space_process_globals
{
  const unsigned n_inputs_ =  4;
  const unsigned n_outputs_ = 0;
}

bool bstm_cpp_refine_space_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_refine_space_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //change prob threshold
  input_types_[3] = "float"; //time


  // process has 0 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_refine_space_process(bprb_func_process& pro)
{

  using namespace bstm_cpp_refine_space_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  float change_prob_t =pro.get_input<float>(i++);
  float time =pro.get_input<float>(i++);


  //bstm app query
  vcl_string app_data_type;
  int apptypesize;
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, app_data_type, apptypesize ) ) {
    vcl_cout<<"bstm_cpp_refine_space_process ERROR: scene doesn't have BSTM_MOG6_VIEW_COMPACT or BSTM_MOG3_GREY data type"<<vcl_endl;
    return false;
  }

  vcl_string nobs_data_type;
  int nobstypesize;
  valid_types.empty();
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, nobs_data_type, nobstypesize ) ) {
    vcl_cout<<"bstm_cpp_refine_space_process ERROR: scene doesn't have BSTM_NUM_OBS or BSTM_NUM_OBS_VIEW_COMPACT data type"<<vcl_endl;
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
    vcl_cout<<"Refining Block: "<<id<<vcl_endl;

    bstm_block     * blk     = cache->get_block(id);
    bstm_time_block* blk_t   = cache->get_time_block(id);
    bstm_data_base * alph    = cache->get_data_base(id,bstm_data_traits<BSTM_ALPHA>::prefix());
    int num_el = alph->buffer_length() / bstm_data_traits<BSTM_ALPHA>::datasize();
    int tree_buffer_len = blk_t->tree_buff_length();
    bstm_data_base * mog     = cache->get_data_base(id, app_data_type, apptypesize * num_el);
    bstm_data_base * num_obs = cache->get_data_base(id, nobs_data_type,nobstypesize * num_el );
    bstm_data_base * change = cache->get_data_base(id,bstm_data_traits<BSTM_CHANGE>::prefix(), bstm_data_traits<BSTM_CHANGE>::datasize() *tree_buffer_len );

    vcl_vector<bstm_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);
    datas.push_back(change);

    //refine block and datas
    if(app_data_type == bstm_data_traits<BSTM_MOG3_GREY>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS>::prefix()  )
      bstm_refine_blk_in_space_function<BSTM_MOG3_GREY, BSTM_NUM_OBS> ( blk_t, blk, datas, change_prob_t);
    else if (app_data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix()  )
      bstm_refine_blk_in_space_function<BSTM_MOG6_VIEW_COMPACT, BSTM_NUM_OBS_VIEW_COMPACT> ( blk_t, blk, datas, change_prob_t);
    else
      vcl_cout << "bstm_refine_block_space ERROR! Types don't match...." << vcl_endl;
  }

  vcl_cout << "Finished refining scene..." << vcl_endl;
  return true;
}
