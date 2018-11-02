#include <iostream>
#include <algorithm>
#include "boxm2_volm_desc_ex_land_only_indexer.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

std::string volm_desc_ex_land_only_indexer::get_name()
{
  std::string name = "ex_land_only";
  return name;
}

volm_desc_ex_land_only_indexer::volm_desc_ex_land_only_indexer(std::string const& index_folder,
                                                               std::string const& out_index_folder,
                                                               std::vector<double> const& radius,
                                                               std::vector<double> const& depth_interval,
                                                               unsigned index_layer_size,
                                                               float ind_buffer,
                                                               unsigned const& nlands,
                                                               unsigned char const&  /*initial_mag*/)
 : volm_desc_indexer(out_index_folder)
{
  depth_interval_ = depth_interval;
  ind_buffer_ = ind_buffer;
  index_layer_size_ = index_layer_size;
  index_folder_ = index_folder;
  nlands_ = nlands;
  out_index_folder_ = out_index_folder;
  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  // sort the radius to ensure the bin order
  std::sort(radius_.begin(), radius_.end());
  unsigned ndists = (unsigned)radius.size() + 1;
  layer_size_ = ndists * nlands_;

  // create wr3db indice
  ind_dist_ = new boxm2_volm_wr3db_index(index_layer_size, ind_buffer_);
  ind_comb_ = new boxm2_volm_wr3db_index(index_layer_size, ind_buffer_);
}

bool volm_desc_ex_land_only_indexer::write_params_file()
{
  volm_buffered_index_params params;
  params.radius = radius_;
  params.layer_size = layer_size_;
  params.nlands = nlands_;
  params.norients = 0;
  if (!params.write_ex_param_file(out_file_name_pre_.str()))
    return false;
  return true;
}

bool volm_desc_ex_land_only_indexer::get_next()
{
  if (current_leaf_id_ != 0) {
    // clear previous initalization
    ind_dist_->finalize();
    ind_comb_->finalize();
  }
  // initialize index given new leaf
  std::stringstream file_name_pre;
  file_name_pre << index_folder_ << "geo_index_tile_" << tile_id_;
  std::string index_dist_file = leaves_[current_leaf_id_]->get_index_name(file_name_pre.str());
  std::string index_comb_file = leaves_[current_leaf_id_]->get_label_index_name(file_name_pre.str(), "combined");
  if (!vul_file::exists(index_dist_file) || !vul_file::exists(index_comb_file)) {
    std::cerr << " In volm_desc_ex_indexer::get_next -- can not find index file: " << index_dist_file << " or "
              << index_comb_file << '\n';
    return false;
  }
  ind_dist_->initialize_read(index_dist_file);
  ind_comb_->initialize_read(index_comb_file);

  return true;
}

bool volm_desc_ex_land_only_indexer::extract(double  /*lat*/, double  /*lon*/, double  /*elev*/, std::vector<unsigned char>& values)
{
  // fetch previous wr3db index for current location
  std::vector<unsigned char> values_dist(index_layer_size_);
  std::vector<unsigned char> values_comb(index_layer_size_);
  ind_dist_->get_next(values_dist);
  ind_comb_->get_next(values_comb);

  // construct existance histogram from wr3db index
  volm_desc_sptr desc = new volm_desc_ex_land_only(values_dist, values_comb, depth_interval_, radius_, nlands_, (unsigned char)0);
  desc->get_char_array(values);
  return true;
}
