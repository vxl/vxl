#include "boxm2_volm_desc_ex_indexer.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vul/vul_file.h>

volm_desc_ex_indexer::volm_desc_ex_indexer(vcl_string const& index_folder,
                                           vcl_string const& out_index_folder,
                                           vcl_vector<double> const& radius,
                                           float ind_buffer,
                                           unsigned const& norients,
                                           unsigned const& nlands,
                                           unsigned char const& initial_mag)
 : volm_desc_indexer(out_index_folder)
{
  ind_buffer_ = ind_buffer;
  index_folder_ = index_folder;
  norients_ = norients;
  nlands_ = nlands;
  out_index_folder_ = out_index_folder;
  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  // sort the radius to ensure the bin order
  vcl_sort(radius_.begin(), radius_.end());
  unsigned ndists = (unsigned)radius.size() + 1;
  layer_size_ = ndists * norients_ * nlands_;
}

bool volm_desc_ex_indexer::load_tile_hypos(vcl_string const& geo_hypo_folder, int tile_id)
{
  // given the tile id and geo_hypo_folder, load the volm_geo_index and initialize associated indice for the tile
  vcl_stringstream file_name_pre_hypo;
  file_name_pre_hypo << geo_hypo_folder << "/geo_index_tile_" << tile_id << ".txt";
  vcl_stringstream file_name_pre_indx;

  if (!vul_file::exists(file_name_pre_hypo.str())) {
    vcl_cout << "ERROR: in volm_desc_ex_indexer -- file does not exist: "
             << file_name_pre_hypo.str() << vcl_endl;
    return false;
  }
  // load the volm_geo_index
  vcl_stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
  float min_size;
  root_ = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root_, file_name_pre.str());
  vcl_cout << " read hyps!\n";
  volm_geo_index::get_leaves_with_hyps(root_, leaves_);

  if (leaves_.empty()) {
    vcl_cout << "ERROR: in volm_desc_ex_indexer -- geo index has 0 leaves with a hyps for tile " << tile_id << vcl_endl;
    return false;
  }
  
  // initialize the indices
  boxm2_volm_wr3db_index_params params;
  vcl_stringstream params_file;
  params_file << index_folder_ << "geo_index_tile_" << tile_id << "_index.params";
  if (!params.read_params_file(params_file.str())) {
    vcl_cout << "ERROR: in volm_desc_ex_indexer -- param file does not exist: " << params_file.str() << vcl_endl;
    return false;
  }
  ind_dist_ = new boxm2_volm_wr3db_index(params.layer_size, ind_buffer_);
  ind_comb_ = new boxm2_volm_wr3db_index(params.layer_size, ind_buffer_);

  // defined the out put file name pre
  out_file_name_pre_ << out_index_folder_ << "desc_index_tile_" << tile_id;

  return true;
}

bool volm_desc_ex_indexer::write_params_file()
{
  volm_buffered_index_params params;
  params.radius = radius_;
  params.layer_size = layer_size_;
  params.nlands = nlands_;
  params.norients = norients_;
  if (!params.write_ex_param_file(out_file_name_pre_.str()))
    return false;
  return true;
}

bool volm_desc_ex_indexer::extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values)
{
  
  return true;
}
