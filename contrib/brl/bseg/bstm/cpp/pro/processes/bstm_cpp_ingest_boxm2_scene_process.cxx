// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_ingest_boxm2_scene_process.cxx
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
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bstm/cpp/algo/bstm_ingest_boxm2_scene_function.h>

//directory utility
#include <vcl_where_root_dir.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>

#include <bstm/bstm_util.h>
#include <boxm2/boxm2_util.h>

namespace bstm_cpp_ingest_boxm2_scene_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_cpp_ingest_boxm2_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_ingest_boxm2_scene_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "boxm2_scene_sptr";
  input_types_[3] = "boxm2_cache_sptr";
  input_types_[4] = "double"; //time
  input_types_[5] = "double"; //p_threshold
  input_types_[6] = "double"; //app_threshold


  // process has 0 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_ingest_boxm2_scene_process(bprb_func_process& pro)
{

  using namespace bstm_cpp_ingest_boxm2_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  boxm2_scene_sptr boxm2_scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr boxm2_cache= pro.get_input<boxm2_cache_sptr>(i++);
  auto time =pro.get_input<double>(i++);
  auto p_threshold =pro.get_input<double>(i++);
  auto app_threshold =pro.get_input<double>(i++);

  //bstm app query
  std::string data_type;
  int apptypesize;
  std::vector<std::string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());

  if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
    std::cout<<"bstm_cpp_ingest_boxm2_scene_process ERROR: bstm scene doesn't have one of the following data types:"<<std::endl;
    for(std::vector<std::string>::const_iterator iter = valid_types.begin(); iter != valid_types.end(); ++iter){
      std::cout << *iter << std::endl;
    }
    return false;
  }

  std::string boxm2_data_type;
  int boxm2_apptypesize;
  std::vector<std::string> boxm2_valid_types;
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());

  if ( !boxm2_util::verify_appearance( *boxm2_scene, boxm2_valid_types, boxm2_data_type, boxm2_apptypesize ) ) {
    std::cout<<"bstm_cpp_ingest_boxm2_scene_process ERROR: boxm2 scene doesn't have one of the following data types: "<<std::endl;
    for(std::vector<std::string>::const_iterator iter = boxm2_valid_types.begin(); iter != boxm2_valid_types.end(); ++iter){
      std::cout << *iter << std::endl;
    }
    return false;
  }

  //next check individual block meta data
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata> boxm2_blocks = boxm2_scene->blocks();

  //iter over boxm2 blocks to make sure meta data and id's are consistent
  std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter = boxm2_blocks.begin();
  for(; iter != boxm2_blocks.end(); iter++)
  {
    //given a boxm2 block and metadata
    boxm2_block_id boxm2_id = iter->first;
    boxm2_block_metadata boxm2_metadata = iter->second;

    //search for it in the bstm scene
    std::map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
    for(; bstm_iter != blocks.end() ; bstm_iter++)
    {
      bstm_block_id bstm_id = bstm_iter->first;
      bstm_block_metadata bstm_metadata = bstm_iter->second;
      if(bstm_id == boxm2_id)
      {
        //found same block id, check block metadata
         bstm_block_metadata bstm_metadata = bstm_iter->second;
         if(! (bstm_metadata == boxm2_metadata)) {
           std::cerr << "bstm scene and boxm2 scene are not consistent! block " << boxm2_id << " metadata not consistent!\n";
           std::cerr << "Exiting..." << std::endl;
           return false;
         }

         double local_time;
         if(!bstm_metadata.contains_t(time,local_time)) {
           continue;
         }

         //now do the work
         std::cout << "Ingesting " << boxm2_id << " into " <<  bstm_metadata.id_  << " with local time = " << local_time << std::endl;

         //get data from bstm scene
         bstm_block* blk = cache->get_block(bstm_metadata.id_);
         bstm_time_block* blk_t = cache->get_time_block(bstm_metadata.id_);
         bstm_data_base * alph = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
         bstm_data_base * mog = cache->get_data_base(bstm_metadata.id_, data_type);


         std::map<std::string, bstm_data_base*> datas;
         datas[bstm_data_traits<BSTM_ALPHA>::prefix()] = alph;
         datas[data_type] = mog;

         //get data from boxm2 scene
         boxm2_block *     boxm2_blk = boxm2_cache->get_block(boxm2_scene, bstm_metadata.id_);
         boxm2_data_base * boxm2_alph = boxm2_cache->get_data_base(boxm2_scene, bstm_metadata.id_,boxm2_data_traits<BOXM2_ALPHA>::prefix());
         boxm2_data_base * boxm2_mog = boxm2_cache->get_data_base(boxm2_scene, bstm_metadata.id_,boxm2_data_type);



         std::map<std::string, boxm2_data_base*> boxm2_datas;
         boxm2_datas[boxm2_data_traits<BOXM2_ALPHA>::prefix()] = boxm2_alph;
         boxm2_datas[boxm2_data_type] = boxm2_mog;


         if(boxm2_data_type == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() &&  data_type == bstm_data_traits<BSTM_MOG3_GREY>::prefix()  )
         {
           bstm_ingest_boxm2_scene_function<BSTM_MOG3_GREY, BOXM2_MOG3_GREY>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         }
         else if( boxm2_data_type == boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix() && data_type == bstm_data_traits<BSTM_MOG6_VIEW>::prefix()  )
         {
           bstm_ingest_boxm2_scene_function<BSTM_MOG6_VIEW, BOXM2_MOG6_VIEW>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         }
         else if( boxm2_data_type == boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix() && data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix()  ) {
           bstm_ingest_boxm2_scene_function<BSTM_MOG6_VIEW_COMPACT, BOXM2_MOG6_VIEW_COMPACT>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         }
         else if( boxm2_data_type == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() && data_type == bstm_data_traits<BSTM_GAUSS_RGB>::prefix()  ) {
           bstm_ingest_boxm2_scene_function<BSTM_GAUSS_RGB, BOXM2_GAUSS_RGB>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         }
         else if( boxm2_data_type == boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix() && data_type == bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix()  )
           bstm_ingest_boxm2_scene_function<BSTM_GAUSS_RGB_VIEW_COMPACT, BOXM2_GAUSS_RGB_VIEW_COMPACT>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         else
           std::cout << "bstm_cpp_ingest_boxm2_scene_process ERROR! appearance models do not match, boxm2_data_type: " << boxm2_data_type << " bstm_data_type: " <<  data_type << std::endl;
      }
    }
  }


  std::cout << "Finished ingesting scene..." << std::endl;
  return true;
}
