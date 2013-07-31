// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_ingest_boxm2_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
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
  const unsigned n_inputs_ =  7;
  const unsigned n_outputs_ = 0;
}

bool bstm_cpp_ingest_boxm2_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_ingest_boxm2_scene_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "boxm2_scene_sptr";
  input_types_[3] = "boxm2_cache_sptr";
  input_types_[4] = "double"; //time
  input_types_[5] = "double"; //p_threshold
  input_types_[6] = "double"; //app_threshold


  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_ingest_boxm2_scene_process(bprb_func_process& pro)
{

  using namespace bstm_cpp_ingest_boxm2_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  boxm2_scene_sptr boxm2_scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr boxm2_cache= pro.get_input<boxm2_cache_sptr>(i++);
  double time =pro.get_input<double>(i++);
  double p_threshold =pro.get_input<double>(i++);
  double app_threshold =pro.get_input<double>(i++);

  //bstm app query
  vcl_string data_type;
  int apptypesize;
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());

  if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
    vcl_cout<<"bstm_cpp_ingest_boxm2_scene_process ERROR: scene doesn't have BSTM_MOG6_VIEW or BSTM_MOG3_GREY data type"<<vcl_endl;
    return false;
  }

  vcl_string boxm2_data_type;
  int boxm2_apptypesize;
  vcl_vector<vcl_string> boxm2_valid_types;
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix());
  if ( !boxm2_util::verify_appearance( *boxm2_scene, boxm2_valid_types, boxm2_data_type, boxm2_apptypesize ) ) {
    vcl_cout<<"bstm_cpp_ingest_boxm2_scene_process ERROR: boxm2 scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }

  //next check individual block meta data
  vcl_map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata> boxm2_blocks = boxm2_scene->blocks();

  //iter over boxm2 blocks to make sure meta data and id's are consistent
  vcl_map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter = boxm2_blocks.begin();
  for(; iter != boxm2_blocks.end(); iter++)
  {
    //given a boxm2 block and metadata
    boxm2_block_id boxm2_id = iter->first;
    boxm2_block_metadata boxm2_metadata = iter->second;

    //search for it in the bstm scene
    vcl_map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
    for(; bstm_iter != blocks.end() ; bstm_iter++)
    {
      bstm_block_id bstm_id = bstm_iter->first;
      bstm_block_metadata bstm_metadata = bstm_iter->second;
      if(bstm_id == boxm2_id)
      {
        //found same block id, check block metadata
         bstm_block_metadata bstm_metadata = bstm_iter->second;
         if(! (bstm_metadata == boxm2_metadata)) {
           vcl_cerr << "bstm scene and boxm2 scene are not consistent! block " << boxm2_id << " metadata not consistent!\n";
           vcl_cerr << "Exiting..." << vcl_endl;
           return false;
         }

         double local_time;
         if(!bstm_metadata.contains_t(time,local_time))
           continue;


         //now do the work
         //vcl_cout << "Ingesting " << boxm2_id << " into " <<  bstm_metadata.id_ << vcl_endl;

         //get data from bstm scene
         bstm_block* blk = cache->get_block(bstm_metadata.id_);
         bstm_time_block* blk_t = cache->get_time_block(bstm_metadata.id_);
         bstm_data_base * alph    = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
         bstm_data_base * mog     = cache->get_data_base(bstm_metadata.id_, data_type);

         vcl_map<vcl_string, bstm_data_base*> datas;
         datas[bstm_data_traits<BSTM_ALPHA>::prefix()] = alph;
         datas[data_type] = mog;

         //get data from boxm2 scene
         boxm2_block *     boxm2_blk     = boxm2_cache->get_block(bstm_metadata.id_);
         boxm2_data_base * boxm2_alph    = boxm2_cache->get_data_base(bstm_metadata.id_,boxm2_data_traits<BOXM2_ALPHA>::prefix());
         boxm2_data_base * boxm2_mog     = boxm2_cache->get_data_base(bstm_metadata.id_,boxm2_data_type);

         vcl_map<vcl_string, boxm2_data_base*> boxm2_datas;
         boxm2_datas[boxm2_data_traits<BOXM2_ALPHA>::prefix()] = boxm2_alph;
         boxm2_datas[boxm2_data_type] = boxm2_mog;

         if(boxm2_data_type == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() &&  data_type == bstm_data_traits<BSTM_MOG3_GREY>::prefix()  )
         {
           bstm_ingest_boxm2_scene_function<BSTM_MOG3_GREY, BOXM2_MOG3_GREY>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         }
         else if( boxm2_data_type == boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix() && data_type == bstm_data_traits<BSTM_MOG6_VIEW>::prefix()  )
         {
           bstm_ingest_boxm2_scene_function<BSTM_MOG6_VIEW, BOXM2_MOG6_VIEW>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         } if( boxm2_data_type == boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix() &&
               data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix()  )
           bstm_ingest_boxm2_scene_function<BSTM_MOG6_VIEW_COMPACT, BOXM2_MOG6_VIEW_COMPACT>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         else if( boxm2_data_type == boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix() && data_type == bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix()  )
           bstm_ingest_boxm2_scene_function<BSTM_GAUSS_RGB_VIEW_COMPACT, BOXM2_GAUSS_RGB_VIEW_COMPACT>(blk,blk_t,datas,boxm2_blk,boxm2_datas,local_time, p_threshold, app_threshold);
         else
           vcl_cout << "bstm_cpp_ingest_boxm2_scene_process ERROR!" << vcl_endl;
      }
    }
  }


  vcl_cout << "Finished ingesting scene..." << vcl_endl;
  return true;
}
