#include "volm_desc_indexer.h"

#include <vcl_sstream.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <vgl/vgl_point_3d.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_buffered_index.h>

bool volm_desc_indexer::load_tile_hypos(vcl_string const& geo_hypo_folder, int tile_id)
{
  vcl_stringstream file_name_pre_hypo;
  file_name_pre_hypo << geo_hypo_folder << "/geo_index_tile_" << tile_id <<".txt";
  vcl_stringstream file_name_pre_indx;
  
  if (!vul_file::exists(file_name_pre_hypo.str())) {
    vcl_cout << "In volm_desc_indexer::load_tile_hypos() -- file does not exist: " << file_name_pre_hypo.str() << "!\n";
    return false;
  }

  vcl_stringstream file_name; file_name << geo_hypo_folder << "/geo_index_tile_" << tile_id;
  float min_size;
  root_ = volm_geo_index::read_and_construct(file_name.str() + ".txt", min_size);
  
  volm_geo_index::read_hyps(root_, file_name.str());
  vcl_cout << " read hyps!\n";
  volm_geo_index::get_leaves_with_hyps(root_, leaves_);
  
  if (!leaves_.size()) {
    vcl_cout << "In volm_desc_indexer::load_tile_hypos() -- geo index has 0 leaves with a hyps!\n";
    return false;
  }
  out_file_name_ << out_index_folder_ << "desc_index_tile_" << tile_id;
  return true;
}

bool volm_desc_indexer::index(float buffer_capacity) 
{
  volm_buffered_index_params params;
  params.layer_size = this->layer_size(); 

  if (!params.write_params_file(out_file_name_.str())) { 
    vcl_cerr << "Cannot write params file to " << out_file_name_.str() + ".params!\n";
    return false;
  }
  
  for (unsigned i = 0; i < leaves_.size(); i++) {
    if (!leaves_[i]->hyps_)
      continue;
 
    // create a binary index file for each hypo set in a leaf
    volm_buffered_index_sptr ind = new volm_buffered_index(params.layer_size, buffer_capacity);
    
    vcl_string out_file_name = out_file_name_.str() + "_" + leaves_[i]->get_string() + "_" + this->get_index_type_str() + ".bin";

    if (!ind->initialize_write(out_file_name)) {
      vcl_cerr << "Cannot initialize " << out_file_name << " for write!\n";
      return false;
    }

    unsigned indexed_cnt = 0;

    vgl_point_3d<double> h_pt;
    while ((leaves_[i])->hyps_->get_next(0,1,h_pt)) {
      vcl_vector<unsigned char> values;
      this->extract(h_pt.y(), h_pt.x(), h_pt.z(), values);
      ind->add_to_index(values);
      ++indexed_cnt;
      if (indexed_cnt%1000 == 0) vcl_cout << indexed_cnt << '.';
    }

    // write all and close the binary file
    ind->finalize(); 

  }
  return true;

}

