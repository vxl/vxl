// This is
// brl/bseg/bstm_multi/cpp/pro/processes/bstm_multi_cpp_ingest_boxm2_scene_process.cxx

//:
// \file
// \brief  A process for adding a BOXM2 scene as a frame of a multi-BSTM scene.
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vcl_where_root_dir.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bstm/bstm_util.h>

#include <bstm_multi/block_data_base.h>
#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/cpp/algo/bstm_multi_ingest_boxm2_scene_function.h>
#include <bstm_multi/io/block_cache.h>
#include <bstm_multi/io/block_simple_cache.h>
#include <bstm_multi/space_time_scene.h>

namespace {
const unsigned n_inputs_ = 7;
const unsigned n_outputs_ = 0;
}

bool bstm_multi_cpp_ingest_boxm2_scene_process_cons(bprb_func_process &pro) {
  vcl_vector<vcl_string> input_types_(::n_inputs_);
  input_types_[0] = "bstm_multi_scene_sptr"; // multi-BSTM scene
  input_types_[1] = "bstm_multi_cache_sptr"; // multi-BSTM cache
  input_types_[2] = "boxm2_scene_sptr";      // BOXM2 scene
  input_types_[3] = "boxm2_cache_sptr";      // BOXM2 cache
  input_types_[4] = "double";                // time
  input_types_[5] = "double";                // p_threshold
  input_types_[6] = "double";                // app_threshold

  vcl_vector<vcl_string> output_types_(::n_outputs_);

  bool good =
      pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_multi_cpp_ingest_boxm2_scene_process(bprb_func_process &pro) {
  if (pro.n_inputs() < ::n_inputs_) {
    vcl_cout << pro.name() << ": The number of inputs should be " << ::n_inputs_
             << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(i++);
  bstm_multi_cache_sptr cache = pro.get_input<bstm_multi_cache_sptr>(i++);
  boxm2_scene_sptr boxm2_scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr boxm2_cache = pro.get_input<boxm2_cache_sptr>(i++);
  double time = pro.get_input<double>(i++);
  double p_threshold = pro.get_input<double>(i++);
  double app_threshold = pro.get_input<double>(i++);

  // bstm_multi app query
  vcl_string data_type;
  int apptypesize;
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(
      bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());

  if (!bstm_util::verify_appearance(
          scene->appearances(), valid_types, data_type, apptypesize)) {
    vcl_cout << pro.name() << " ERROR: bstm_multi scene "
                              "doesn't have one of the following data types:"
             << vcl_endl;
    for (vcl_vector<vcl_string>::const_iterator iter = valid_types.begin();
         iter != valid_types.end();
         ++iter) {
      vcl_cout << *iter << vcl_endl;
    }
    return false;
  }

  vcl_string boxm2_data_type;
  int boxm2_apptypesize;
  vcl_vector<vcl_string> boxm2_valid_types;
  boxm2_valid_types.push_back(
      boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix());
  boxm2_valid_types.push_back(
      boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix());
  boxm2_valid_types.push_back(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());

  if (!boxm2_util::verify_appearance(*boxm2_scene,
                                     boxm2_valid_types,
                                     boxm2_data_type,
                                     boxm2_apptypesize)) {
    vcl_cout << pro.name() << " ERROR: boxm2 scene "
                              "doesn't have one of the following data types: "
             << vcl_endl;
    for (vcl_vector<vcl_string>::const_iterator iter =
             boxm2_valid_types.begin();
         iter != boxm2_valid_types.end();
         ++iter) {
      vcl_cout << *iter << vcl_endl;
    }
    return false;
  }

  // compare BOXM2 and BSTM data types without their prefixes
  if (boxm2_data_type.substr(vcl_string("boxm2").size()) !=
      data_type.substr(vcl_string("bstm").size())) {
    vcl_cout << "BOXM2 data type " << boxm2_data_type << " and BSTM data type "
             << data_type << " do not match." << vcl_endl;
    return false;
  }

  // iterate over blocks of both scenes
  const vcl_map<bstm_block_id, bstm_multi_block_metadata> &blocks =
      scene->blocks();
  const vcl_map<boxm2_block_id, boxm2_block_metadata> &boxm2_blocks =
      boxm2_scene->blocks();
  // iter over boxm2 blocks to make sure meta data and id's are consistent
  for (vcl_map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter =
           boxm2_blocks.begin();
       iter != boxm2_blocks.end();
       ++iter) {
    boxm2_block_id boxm2_id = iter->first;
    boxm2_block_metadata boxm2_metadata = iter->second;

    // search for block in the bstm_multi scene with same ID
    for (vcl_map<bstm_block_id, bstm_multi_block_metadata>::const_iterator
             bstm_multi_iter = blocks.begin();
         bstm_multi_iter != blocks.end();
         ++bstm_multi_iter) {
      if (!(bstm_multi_iter->first == boxm2_id)) {
        continue;
      }

      // found same block id, check block metadata
      bstm_block_id bstm_multi_id = bstm_multi_iter->first;
      bstm_multi_block_metadata bstm_multi_metadata = bstm_multi_iter->second;
      if (!(bstm_multi_metadata == boxm2_metadata)) {
        vcl_cerr
            << "bstm_multi scene and boxm2 scene are not consistent! block "
            << boxm2_id << " metadata not consistent!\n";
        vcl_cerr << "Exiting..." << vcl_endl;
        return false;
      }

      double local_time;
      if (!bstm_multi_metadata.contains_t(time, local_time)) {
        continue;
      }

      // now do the work
      vcl_cout << "Ingesting " << boxm2_id << " into "
               << bstm_multi_metadata.id_ << " with local time = " << local_time
               << vcl_endl;

      // get data from bstm_multi scene
      bstm_multi_block *blk = cache->get_block(scene, bstm_multi_metadata.id_);
      block_data_base *alph =
          cache->get_data_base(scene,
                               bstm_multi_metadata.id_,
                               bstm_data_traits<BSTM_ALPHA>::prefix());
      block_data_base *mog =
          cache->get_data_base(scene, bstm_multi_metadata.id_, data_type);

      vcl_map<vcl_string, block_data_base *> datas;
      datas[bstm_data_traits<BSTM_ALPHA>::prefix()] = alph;
      datas[data_type] = mog;

      // get data from boxm2 scene
      boxm2_block *boxm2_blk =
          boxm2_cache->get_block(boxm2_scene, bstm_multi_metadata.id_);
      boxm2_data_base *boxm2_alph =
          boxm2_cache->get_data_base(boxm2_scene,
                                     bstm_multi_metadata.id_,
                                     boxm2_data_traits<BOXM2_ALPHA>::prefix());
      boxm2_data_base *boxm2_mog = boxm2_cache->get_data_base(
          boxm2_scene, bstm_multi_metadata.id_, boxm2_data_type);

      vcl_map<vcl_string, boxm2_data_base *> boxm2_datas;
      boxm2_datas[boxm2_data_traits<BOXM2_ALPHA>::prefix()] = boxm2_alph;
      boxm2_datas[boxm2_data_type] = boxm2_mog;

      bstm_multi_ingest_boxm2_scene_function(blk,
                                             datas,
                                             boxm2_blk,
                                             boxm2_datas,
                                             local_time,
                                             p_threshold,
                                             app_threshold);
    }
  }

  vcl_cout << "Finished ingesting scene...." << vcl_endl;
  return true;
}
