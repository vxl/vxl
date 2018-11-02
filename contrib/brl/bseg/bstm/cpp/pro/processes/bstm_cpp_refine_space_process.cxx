// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_refine_space_process.cxx
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
#include <bstm/cpp/algo/bstm_refine_blk_in_space_function.h>

#include <bstm/bstm_util.h>

namespace bstm_cpp_refine_space_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_cpp_refine_space_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_refine_space_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //change prob threshold
  input_types_[3] = "float"; //time


  // process has 0 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_refine_space_process(bprb_func_process& pro)
{

  using namespace bstm_cpp_refine_space_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  auto change_prob_t =pro.get_input<float>(i++);
  auto time =pro.get_input<float>(i++);


  //bstm app query
  std::string app_data_type;
  int apptypesize;
  std::vector<std::string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, app_data_type, apptypesize ) ) {
    std::cout<<"bstm_cpp_refine_space_process ERROR: scene doesn't have BSTM_MOG6_VIEW_COMPACT or BSTM_MOG3_GREY or BSTM_GAUSS_RGB data type"<<std::endl;
    return false;
  }

  std::string nobs_data_type;
  int nobstypesize;
  valid_types.empty();
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS_SINGLE>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, nobs_data_type, nobstypesize ) ) {
    std::cout<<"bstm_cpp_refine_space_process ERROR: scene doesn't have BSTM_NUM_OBS or BSTM_NUM_OBS_VIEW_COMPACT or BSTM_NUM_OBS_SINGLE data type"<<std::endl;
    return false;
  }


  std::cout<<"Refining in space..."<<std::endl;

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
    int tree_buffer_len = blk_t->tree_buff_length();
    bstm_data_base * mog = cache->get_data_base(id, app_data_type, apptypesize * num_el);
    bstm_data_base * num_obs = cache->get_data_base(id, nobs_data_type,nobstypesize * num_el );
    bstm_data_base * change = cache->get_data_base(id,bstm_data_traits<BSTM_CHANGE>::prefix(), bstm_data_traits<BSTM_CHANGE>::datasize() *tree_buffer_len );

    std::vector<bstm_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);
    datas.push_back(change);

    //refine block and datas
    if(app_data_type == bstm_data_traits<BSTM_MOG3_GREY>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS>::prefix()  )
      bstm_refine_blk_in_space_function<BSTM_MOG3_GREY, BSTM_NUM_OBS> ( blk_t, blk, datas, change_prob_t);
    else if (app_data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix()  )
      bstm_refine_blk_in_space_function<BSTM_MOG6_VIEW_COMPACT, BSTM_NUM_OBS_VIEW_COMPACT> ( blk_t, blk, datas, change_prob_t);
    else if (app_data_type == bstm_data_traits<BSTM_GAUSS_RGB>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS_SINGLE>::prefix()  )
      bstm_refine_blk_in_space_function<BSTM_GAUSS_RGB, BSTM_NUM_OBS_SINGLE> ( blk_t, blk, datas, change_prob_t);
    else {
      std::cerr << "bstm_refine_block_space ERROR! Types don't match...." << std::endl;
      std::cerr << "App type: " << app_data_type << " and nobs: " << nobs_data_type << std::endl;
    }
  }

  std::cout << "Finished refining scene..." << std::endl;
  return true;
}
