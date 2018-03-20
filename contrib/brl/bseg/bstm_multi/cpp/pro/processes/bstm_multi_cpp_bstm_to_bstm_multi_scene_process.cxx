#include <vcl_string.h>
#include <vcl_vector.h>

#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>

#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_util.h>
#include <bstm/io/bstm_cache.h>

#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/io/block_cache.h>
#include <bstm_multi/space_time_scene.h>

#include <bstm_multi/cpp/algo/bstm_multi_bstm_block_to_bstm_multi_block_function.h>

namespace {
const unsigned n_inputs_ = 7;
const unsigned n_outputs_ = 0;
}

// Calls verify_appearances with a pre-set list of valid types
bool check_data_types(const vcl_vector<vcl_string> &appearances,
                      vcl_string &data_type,
                      int &app_type_size) {
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(
      bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());

  bool success = bstm_util::verify_appearance(
      appearances, valid_types, data_type, app_type_size);
  if (!success) {
    vcl_cout << " ERROR: scene doesn't have one of the following data types:"
             << vcl_endl;
    for (vcl_vector<vcl_string>::const_iterator iter = valid_types.begin();
         iter != valid_types.end();
         ++iter) {
      vcl_cout << *iter << vcl_endl;
    }
  }
  return success;
}

bool bstm_multi_cpp_bstm_to_bstm_multi_scene_process_cons(
    bprb_func_process &pro) {
  vcl_vector<vcl_string> input_types_(::n_inputs_);
  int i = 0;
  input_types_[i++] = "bstm_multi_scene_sptr"; // multi-BSTM scene
  input_types_[i++] = "bstm_multi_cache_sptr"; // multi-BSTM cache
  input_types_[i++] = "bstm_scene_sptr";       // BSTM scene
  input_types_[i++] = "bstm_cache_sptr";       // BSTM cache
  input_types_[i++] = "double";                // p_threshold
  input_types_[i++] = "double";                // app_threshold

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
  bstm_scene_sptr bstm_scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr bstm_cache = pro.get_input<bstm_cache_sptr>(i++);
  double p_threshold = pro.get_input<double>(i++);
  double app_threshold = pro.get_input<double>(i++);

  vcl_string data_type;
  int app_type_size;
  vcl_cout << "Checking Multi-BSTM data types..." << vcl_endl;
  if (!check_data_types(scene->appearances(), data_type, app_type_size)) {
    return false;
  }

  vcl_string bstm_data_type;
  int bstm_app_type_size;
  vcl_cout << "Checking BSTM data types..." << vcl_endl;
  if (!check_data_types(
          bstm_scene->appearances(), bstm_data_type, bstm_app_type_size)) {
    return false;
  }

  // Iterate over blocks of multi-bstm scene, and ingest corresponding blocks of
  // bstm scene
  const vcl_map<bstm_block_id, bstm_multi_block_metadata> &blocks =
      scene->blocks();
  const vcl_map<bstm_block_id, bstm_block_metadata> &bstm_blocks =
      bstm_scene->blocks();
  for (vcl_map<bstm_block_id, bstm_multi_block_metadata>::const_iterator
           bstm_multi_iter = blocks.begin();
       bstm_multi_iter != blocks.end();
       ++bstm_multi_iter) {

    bstm_block_id block_id = bstm_multi_iter->first;
    bstm_multi_block_metadata bstm_multi_metadata = bstm_multi_iter->second;

    // Find BSTM block with same ID
    vcl_map<bstm_block_id, bstm_block_metadata>::const_iterator iter =
        bstm_blocks.find(block_id);
    if (iter == bstm_blocks.end()) {
      continue;
    }
    bstm_block_metadata bstm_metadata = iter->second;

    if (!(bstm_multi_metadata == bstm_metadata)) {
      vcl_cerr << "bstm_multi scene and bstm scene are not consistent! block "
               << block_id << " metadata not consistent!\n";
      vcl_cerr << "Exiting..." << vcl_endl;
      return false;
    }
    if (bstm_multi_metadata.resolution() != bstm_metadata.resolution()) {
      vcl_cerr << "bstm_multi scene and bstm scene have different resolutions! "
                  "Found in block ID "
               << block_id << vcl_endl;
      return false;
    }

    // TODO currently we want multi-BSTM scene to have same bottom-level
    // structure as BSTM scene, a penultimate space subdivision and then
    // finally a time subdivision.
    vcl_size_t n_subdivs = bstm_multi_metadata.subdivisions_.size();
    if (n_subdivs < 2 ||
        bstm_multi_metadata.subdivisions_[n_subdivs - 2] != STE_SPACE ||
        bstm_multi_metadata.subdivisions_[n_subdivs - 1] != STE_TIME) {
      vcl_cerr << "currently, bstm_multi scene must have a space and time "
                  "subdivision as its bottom two levels."
               << vcl_endl;
    }

    // now do the work
    vcl_cout << "Ingesting block " << block_id << vcl_endl;

    // get data from bstm_multi scene
    bstm_multi_block *blk = cache->get_block(scene, block_id);
    block_data_base *alph = cache->get_data_base(
        scene, bstm_multi_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
    block_data_base *mog = cache->get_data_base(scene, block_id, data_type);

    vcl_map<vcl_string, block_data_base *> datas;
    datas[bstm_data_traits<BSTM_ALPHA>::prefix()] = alph;
    datas[data_type] = mog;

    // get data from bstm scene
    bstm_block *bstm_blk = bstm_cache->get_block(block_id);
    bstm_time_block *bstm_blk_t = bstm_cache->get_time_block(block_id);
    bstm_data_base *bstm_alph = bstm_cache->get_data_base(
        bstm_multi_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
    bstm_data_base *bstm_mog =
        bstm_cache->get_data_base(bstm_multi_metadata.id_, bstm_data_type);

    vcl_map<vcl_string, bstm_data_base *> bstm_datas;
    bstm_datas[bstm_data_traits<BSTM_ALPHA>::prefix()] = bstm_alph;
    bstm_datas[bstm_data_type] = bstm_mog;

    if (!bstm_block_to_bstm_multi_block(blk,
                                        datas,
                                        bstm_blk,
                                        bstm_blk_t,
                                        bstm_datas,
                                        p_threshold,
                                        app_threshold)) {
      vcl_cerr << "Failed to ingest block ID " << block_id << vcl_endl;
      return false;
    }
  }

  vcl_cout << "Finished ingesting scene...." << vcl_endl;
  return true;
}
