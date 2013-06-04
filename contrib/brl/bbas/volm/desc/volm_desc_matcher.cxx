#include "volm_desc_matcher.h"
#include <vul/vul_file.h>
#include <vgl/vgl_point_3d.h>

bool volm_desc_matcher::matcher(volm_desc_sptr const& query,
                                   vcl_string const& geo_hypo_folder,
                                   vcl_string const& desc_index_folder,
                                   unsigned const& tile_id)
{
  // suppose the created indices have same binary file structure as before

  // load the volm_geo_index for this tile
  vcl_stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  index_file_name_ << desc_index_folder << "desc_index_tile_" << tile_id;

  unsigned nleaves = leaves.size();
  // loop over all leaves to match each location with the query
  for (unsigned l_idx = 0; l_idx < nleaves; l_idx++) {
    vcl_string index_file = index_file_name_.str() + "_" + leaves[l_idx]->get_string() + "_" + this->get_index_type_str() + ".bin";
    if (!vul_file::exists(index_file)) {
      vcl_cout << " ERROR: can not find index file: " << index_file << vcl_endl;
      return false;
    }
    //volm_desc_indexer_sptr index = new volm_desc_ex_indexer();
    vgl_point_3d<double> h_pt;
    
    // loop over all location in current leaf
    while (leaves[l_idx]->hyps_->get_next(0,1,h_pt)) {
      unsigned h_idx = leaves[l_idx]->hyps_->current_-1;
      // index->get_next(); --> create a volm_desc_ex for current index
      float max_score = 0;
      // max_score = query->similarity(desc_index);

      // put the score 
      vcl_vector<unsigned> cam_ids;
      cam_ids.push_back(0);
      score_all_.push_back(new volm_score(l_idx, h_idx, max_score, 0, cam_ids) );
    }
  }
  return true;
}
