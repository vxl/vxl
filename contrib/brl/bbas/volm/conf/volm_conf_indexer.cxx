#include <volm/conf/volm_conf_indexer.h>
//:
// \file
#include <vul/vul_file.h>
#include <vgl/vgl_point_3d.h>

bool volm_conf_indexer::write_params_file()
{
  return true;
}

bool volm_conf_indexer::load_loc_hypos(std::string const& geo_hypo_folder, unsigned const& tile_id)
{
  tile_id_ = tile_id;
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id_;
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    std::cerr << "In volm_conf_indexer::load_tile_hypos() -- location file does not exist: " << file_name_pre.str() << ".txt!\n";
    return false;
  }

  // load the tree structure
  float min_size;
  loc_root_ = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  // load the location database
  volm_geo_index::read_hyps(loc_root_, file_name_pre.str());
  // get the leaves that have locations
  loc_leaves_.clear();
  volm_geo_index::get_leaves_with_hyps(loc_root_, loc_leaves_);
  // check
  if (!loc_leaves_.size()) {
    std::cerr << "In volm_conf_indexer::load_tile_hypos() -- location geo index has 0 leaves with a hypos!\n";
    return false;
  }
  // clear previous out_file_name_pre_
  out_file_name_pre_.str("");
  out_file_name_pre_ << out_index_folder_ << "conf_index_tile_" << tile_id_;

  return true;
}

bool volm_conf_indexer::index(float const& buffer_capacity, int const& min_leaf_id, int const& max_leaf_id)
{
  // write a parameter file
  if (!this->write_params_file()) {
    std::cerr << "In volm_conf_indexer::index -- can not write params file to " << out_file_name_pre_.str() + ".params!\n";
    return false;
  }
  // loop over each leaf to construct index in each location
  for (current_leaf_id_ = 0; current_leaf_id_ < loc_leaves_.size(); current_leaf_id_++)
  {
    //std::cout << " current_leaf_id = " << current_leaf_id_ << " min_leaf_id = " << min_leaf_id << ", max_leaf_id = " << max_leaf_id << std::endl;
    if ((int)current_leaf_id_ < min_leaf_id || (int)current_leaf_id_ >= max_leaf_id)
      continue;
    std::cout << " current_leaf_id = " << current_leaf_id_ << " satisfies " << loc_leaves_[current_leaf_id_]->extent_ << std::endl;

    if (!this->get_next()) {
      std::cerr << "In volm_conf_indexer::index -- get next database failed!\n";
      return false;
    }

    // create a binary index file for each hypo set in a  leaf
    volm_conf_buffer<volm_conf_object> ind(buffer_capacity);

    std::string out_file_name = out_file_name_pre_.str() + "_" + loc_leaves_[current_leaf_id_]->get_string() + "_" + this->get_index_name() + ".bin";
    std::cout << "out_file_name: " << out_file_name << std::endl;
    if (!ind.initialize_write(out_file_name)) {
      std::cerr << "In volm_conf_indexer::index -- can not initialize " << out_file_name << " for write!\n";
      return false;
    }
    std::cout << loc_leaves_[current_leaf_id_]->hyps_->locs_.size() << " locations in current leaf" << std::endl;
    unsigned indexed_cnt = 0;
    vgl_point_3d<double> h_pt;
    while ( loc_leaves_[current_leaf_id_]->hyps_->get_next(0, 1, h_pt) )
    {
      std::vector<volm_conf_object> values;
      if ( !this->extract(h_pt.x(), h_pt.y(), h_pt.z(), values) ) {
        std::cerr << "In volm_conf_indexer::index -- extract index from location " << h_pt << " failed!\n";
        return false;
      }
      // create a invalid index for location has no values to ensure index size consistent with location size
      if (values.empty())
        values.emplace_back(0.0f, 0.0f, -1.0f, 0);
      if (!ind.add_to_index(values)) {
        std::cerr << "In volm_conf_indexer::index -- add index to buffer failed for location " << h_pt << " failed!\n";
        return false;
      }
#if 0
      std::cout << "\t indexed_cnt " << indexed_cnt << ", loc: " << h_pt.x() << ", " << h_pt.y() << " has " << values.size() << " land_objects. " << std::endl;
      for (unsigned i = 0; i < values.size(); i++) {
        std::cout << "\t\t";  values[i].print(std::cout);
      }
#endif
      indexed_cnt++;
      if (indexed_cnt%1000==0)  std::cerr << indexed_cnt << '.' << std::flush;
    }
    // finalize the index
   ind.finalize();

  }  // end of loop over leaves
  return true;
}
