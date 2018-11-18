#include <iostream>
#include <sstream>
#include "volm_desc_indexer.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <vgl/vgl_point_3d.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_buffered_index.h>

bool volm_desc_indexer::load_tile_hypos(std::string const& geo_hypo_folder, int tile_id)
{
  tile_id_ = tile_id;
  std::stringstream file_name_pre_hypo;
  file_name_pre_hypo << geo_hypo_folder << "/geo_index_tile_" << tile_id_ <<".txt";
  std::stringstream file_name_pre_indx;

  if (!vul_file::exists(file_name_pre_hypo.str())) {
    std::cout << "In volm_desc_indexer::load_tile_hypos() -- file does not exist: " << file_name_pre_hypo.str() << "!\n";
    return false;
  }

  std::stringstream file_name; file_name << geo_hypo_folder << "/geo_index_tile_" << tile_id_;
  float min_size;
  root_ = volm_geo_index::read_and_construct(file_name.str() + ".txt", min_size);

  volm_geo_index::read_hyps(root_, file_name.str());
  std::cout << " read hyps!\n";
  leaves_.clear();
  volm_geo_index::get_leaves_with_hyps(root_, leaves_);

  if (!leaves_.size()) {
    std::cout << "In volm_desc_indexer::load_tile_hypos() -- geo index has 0 leaves with a hyps!\n";
    return false;
  }
  // clear previous out_file_name_pre_
  out_file_name_pre_.str("");
  // refill out_file_name_pre_
  out_file_name_pre_ << out_index_folder_ << "desc_index_tile_" << tile_id_;
  return true;
}

bool volm_desc_indexer::write_params_file()
{
  volm_buffered_index_params params;
  params.layer_size = this->layer_size();

  if (!params.write_params_file(out_file_name_pre_.str()))
    return false;
  return true;
}

bool volm_desc_indexer::index(float buffer_capacity, int min_leaf_id, int max_leaf_id)
{
  if (!this->write_params_file()) {
    std::cerr << "Cannot write params file to " << out_file_name_pre_.str() + ".params!\n";
    return false;
  }
#if 0
  volm_buffered_index_params params;
  params.layer_size = this->layer_size();

  if (!params.write_params_file(out_file_name_pre_.str())) {
    std::cerr << "cannot write params file to " << out_file_name_pre_.str() + ".params!\n";
    return false;
  }
#endif

  for (current_leaf_id_ = 0; current_leaf_id_ < leaves_.size(); current_leaf_id_++) {
    std::cout << " current_leaf_id = " << current_leaf_id_ << " min_leaf_id = " << min_leaf_id << ", max_leaf_id = " << max_leaf_id << std::endl;
    if ((int)current_leaf_id_ < min_leaf_id || (int)current_leaf_id_ >= max_leaf_id)
      continue;

    std::cout << " current_leaf_id = " << current_leaf_id_ << " satisfies " << leaves_[current_leaf_id_]->extent_ << std::endl;
    if (!this->get_next())
      return false;

    // create a binary index file for each hypo set in a leaf
    volm_buffered_index_sptr ind = new volm_buffered_index(this->layer_size(), buffer_capacity);

    std::string out_file_name = out_file_name_pre_.str() + "_" + leaves_[current_leaf_id_]->get_string() + "_" + this->get_index_type_str() + ".bin";

    if (!ind->initialize_write(out_file_name)) {
      std::cerr << "Cannot initialize " << out_file_name << " for write!\n";
      return false;
    }

    unsigned indexed_cnt = 0;

    vgl_point_3d<double> h_pt;
    while ((leaves_[current_leaf_id_])->hyps_->get_next(0,1,h_pt)) {
      std::vector<unsigned char> values;
      this->extract(h_pt.y(), h_pt.x(), h_pt.z(), values);
      ind->add_to_index(values);
#if 0
      std::cout << "for location: " << h_pt << ", index is:\n";
      for (std::vector<unsigned char>::iterator vit = values.begin();  vit != values.end(); ++vit)
        std::cout << (int)(*vit) << ' ';
      std::cout << '\n';
#endif
      ++indexed_cnt;
      if (indexed_cnt%1000 == 0) std::cerr << indexed_cnt << '.';
    }

    // write all and close the binary file
    ind->finalize();

  }
  return true;

}
