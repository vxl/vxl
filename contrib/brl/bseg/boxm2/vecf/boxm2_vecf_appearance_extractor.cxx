#include "boxm2_vecf_appearance_extractor.h"
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include "boxm2_vecf_composite_head_parameters.h"
#include <vul/vul_timer.h>
bool boxm2_vecf_appearance_extractor::extract_data(boxm2_scene_sptr scene,boxm2_block_id& block,float * &alpha,gray_APM* &gray_app, color_APM* &color_app){
  boxm2_data_base*   gray_app_db = boxm2_cache::instance()->get_data_base(scene, block,gray_APM_prefix);
  boxm2_data_base* alpha_db = boxm2_cache::instance()->get_data_base(scene, block,"alpha");
  boxm2_data_base* color_app_db = boxm2_cache::instance()->get_data_base(scene, block, color_APM_prefix + "_" + (head_model_.color_apm_id_));
  gray_app = reinterpret_cast<gray_APM*>(gray_app_db->data_buffer());
  color_app = reinterpret_cast<color_APM*>(color_app_db->data_buffer());
  alpha = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(alpha_db->data_buffer());
  if (gray_app && alpha && color_app)
    return true;

  return false;
}
void boxm2_vecf_appearance_extractor::extract_head_appearance(){
  vul_timer t;
  boxm2_scene_sptr base_model = head_model_.scene();
  // for each block of the target scene
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();
  vcl_vector<boxm2_block_id> source_blocks = base_model->get_block_ids();

  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin();
       sblk != source_blocks.end(); ++sblk) {

    boxm2_block * source_blk = boxm2_cache::instance()->get_block(base_model, *sblk);
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(base_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
        vcl_cout<<"Data extraction failed for scene "<< base_model << " in block "<<*sblk<<vcl_cout;
        return;
      }

    //3d array of trees
    typedef boxm2_block::uchar16 uchar16;
    const boxm2_array_3d<uchar16>& trees = source_blk->trees();
    // for each block of the base model
    for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
      boxm2_block * target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
      color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data;
    if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
        vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_cout;
        return;
      }

      //iterate through each block, filtering the root level first
      for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
        for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
          for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
            //load current tree
            uchar16 tree = trees(x, y, z);
            boct_bit_tree bit_tree((unsigned char*) tree.data_block(), source_blk->max_level());

            //FOR ALL LEAVES IN CURRENT TREE
            vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
            vcl_vector<int>::iterator iter;
            for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
              int bit_idx = (*iter);
              int data_idx = bit_tree.get_data_index(bit_idx);

              vgl_point_3d<double> cell_center_norm = bit_tree.cell_center( bit_idx );
              double source_side_len = 1.0/(1 << bit_tree.depth_at(bit_idx));
              // convert to global coordinates
              vgl_vector_3d<double> subblk_dims = source_blk->sub_block_dim();
              vgl_vector_3d<double> subblk_offset(x*subblk_dims.x(),
                                                  y*subblk_dims.y(),
                                                  z*subblk_dims.z());

              vgl_vector_3d<double> cell_offset(cell_center_norm.x()*subblk_dims.x(),
                                                cell_center_norm.y()*subblk_dims.y(),
                                                cell_center_norm.z()*subblk_dims.z());

              vgl_point_3d<double> cell_center = source_blk->local_origin() + subblk_offset + cell_offset;

              // apply (inverse) scaling
              vgl_point_3d<double> fwd_scaled_cell_center(cell_center.x() * head_params.head_scale_.x(),
                                                          cell_center.y() * head_params.head_scale_.y(),
                                                          cell_center.z() * head_params.head_scale_.z());

              // retreive the correct cell from the source data
              vgl_point_3d<double> local_tree_coords, target_cell_center;
              double side_len;

              if ( target_blk->contains( fwd_scaled_cell_center, local_tree_coords, target_cell_center, side_len )) {

                unsigned target_data_idx;
                target_blk->data_index( fwd_scaled_cell_center, target_data_idx);
                float alpha = target_alpha_data[target_data_idx];
                float src_prob = static_cast<float>(1.0 - vcl_exp( - source_alpha_data[data_idx] * source_side_len));
                float prob = static_cast<float>(1.0 - vcl_exp(-alpha*side_len));
                const float prob_thresh = 0.0;

                if (src_prob >= prob_thresh) {
                  // get data can copy from source to target
                  source_app_data[data_idx] = target_app_data[target_data_idx];
                  source_color_data[data_idx] =  target_color_data[target_data_idx];
                }
              }
            }
          }
        }
      }
    } // for each source block
  } // for each target block
  vcl_cout<<"extracted head appearance model from target in "<<t.real()/1000<<" seconds"<<vcl_endl;
}


void boxm2_vecf_appearance_extractor::extract_orbit_appearance(){
  this->extract_eye_appearance(true);
  this->extract_eye_appearance(false);

  this->extract_upper_lid_appearance(true);
  this->extract_upper_lid_appearance(false);

  this->extract_eyelid_crease_appearance(true);
  this->extract_eyelid_crease_appearance(false);

  this->extract_lower_lid_appearance(true);
  this->extract_lower_lid_appearance(false);

}
void boxm2_vecf_appearance_extractor::extract_eye_appearance(bool is_right){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit; boxm2_vecf_orbit_params orbit_params;
  if (!is_right){
    orbit =  head_model_.left_orbit_;
    orbit_params = head_params.l_orbit_params_;
  }else{
    orbit = head_model_.right_orbit_;
    orbit_params = head_params.r_orbit_params_;
  }
  color_APM color =orbit.random_color();
  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> to_dir(head_params.look_dir_.x(),
                                     head_params.look_dir_.y(),
                                     head_params.look_dir_.z());
  vgl_rotation_3d<double> rot(Z, to_dir);
  vcl_vector<vgl_vector_3d<double> >vf;

  boxm2_scene_sptr source_model = orbit.scene();
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();
  unsigned n_source_cells = static_cast<unsigned>(orbit.sphere_cell_centers_.size());
  vcl_cout<<"sphere cell centers: "<<n_source_cells;
  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_cout;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.sphere_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::SPHERE))
        continue;
      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> mapped_p = rot * (p + orbit_params.offset_) ;
      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_cout;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          unsigned source_data_idx = orbit.sphere_cell_data_index_[i];

          source_app_data[source_data_idx] = target_app_data[target_data_idx];
          source_color_data[source_data_idx]= target_color_data[target_data_idx];
          //          source_color_data[source_data_idx]= color;
        }
      }
    }
  }
  vcl_cout<<"Extracted eye sphere appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;
}
void boxm2_vecf_appearance_extractor::extract_eyelid_crease_appearance(bool is_right){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit; boxm2_vecf_orbit_params orbit_params;
  if (!is_right){
    orbit =  head_model_.left_orbit_;
    orbit_params = head_params.l_orbit_params_;
  }else{
    orbit = head_model_.right_orbit_;
    orbit_params = head_params.r_orbit_params_;
  }
  color_APM color =orbit.random_color();
  boxm2_scene_sptr source_model = orbit.scene();
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();
  unsigned n_source_cells = static_cast<unsigned>(orbit.eyelid_crease_cell_centers_.size());

  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_cout;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.eyelid_crease_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::EYELID_CREASE))
        continue;
      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> mapped_p = (p + orbit_params.offset_) ;
      if (is_right){
        vgl_vector_3d<double> flip(-2 * p.x(),0,0);
        mapped_p = mapped_p + flip;
      }

      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_cout;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          unsigned source_data_idx = orbit.eyelid_crease_cell_data_index_[i];

          source_app_data[source_data_idx] = target_app_data[target_data_idx];
          source_color_data[source_data_idx]= target_color_data[target_data_idx];
          //          source_color_data[source_data_idx] = color;
        }
      }
    }
  }
  vcl_cout<<"Extracted eyelid crease appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;
}

void boxm2_vecf_appearance_extractor::extract_lower_lid_appearance(bool is_right){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit; boxm2_vecf_orbit_params orbit_params;
  if (!is_right){
    orbit =  head_model_.left_orbit_;
    orbit_params = head_params.l_orbit_params_;
  }else{
    orbit = head_model_.right_orbit_;
    orbit_params = head_params.r_orbit_params_;
  }
  color_APM color =orbit.random_color();
  boxm2_scene_sptr source_model = orbit.scene();
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();
  unsigned n_source_cells = static_cast<unsigned>(orbit.lower_eyelid_cell_centers_.size());

  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_cout;
      return;
    }

    for(unsigned i = 0; i< n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.lower_eyelid_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::LOWER_LID))
        continue;
      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> mapped_p = (p + orbit_params.offset_) ;
      if (is_right){
        vgl_vector_3d<double> flip(-2 * p.x(),0,0);
        mapped_p = mapped_p + flip;
      }

      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_cout;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          unsigned source_data_idx = orbit.lower_eyelid_cell_data_index_[i];
          source_app_data[source_data_idx] = target_app_data[target_data_idx];
          //          float prob = 1 - vcl_exp( - target_alpha_data[target_data_idx] * target_side_len);
          source_color_data[source_data_idx]= target_color_data[target_data_idx];
          //          source_color_data[source_data_idx] = color;
        }
      }
    }
  }
  vcl_cout<<"Extracted lower lid appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;
}

void boxm2_vecf_appearance_extractor::extract_upper_lid_appearance(bool is_right){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit; boxm2_vecf_orbit_params orbit_params;
  if (!is_right){
    orbit =  head_model_.left_orbit_;
    orbit_params = head_params.l_orbit_params_;
  }else{
    orbit = head_model_.right_orbit_;
    orbit_params = head_params.r_orbit_params_;
  }
  color_APM color =orbit.random_color();
  color[0]=255; color[2]=0; color[4]=0;
  double dt = orbit_params.eyelid_dt_;
  boxm2_scene_sptr source_model = orbit.scene();
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();

  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    boxm2_block *source_blk = boxm2_cache::instance()->get_block(source_model, *sblk);
    unsigned n_source_cells = static_cast<unsigned>(orbit.eyelid_cell_centers_.size());
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_cout;
      return;
    }

    for(unsigned i = 0; i < n_source_cells; ++i){
      bool skip = false;vgl_point_3d<double> loc_p;
      vgl_point_3d<double> p = (orbit.eyelid_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::UPPER_LID) || !(source_blk->contains(p, loc_p) ) )
        continue;
      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> mapped_p = (p + orbit_params.offset_) ;
      if (is_right){
        vgl_vector_3d<double> flip(-2 * p.x(),0,0);
        mapped_p = mapped_p + flip;
      }
      double tc = orbit.eyelid_geo_.t(p.x(), p.y());
      if(!orbit.eyelid_geo_.valid_t(tc))
        continue;
      // inverse field so negate dt
      double ti = dt-tc;
      if(!orbit.eyelid_geo_.valid_t(ti)){
        skip = true;
      }
      mapped_p +=orbit.eyelid_geo_.vf(mapped_p.x(), tc, dt);


      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_cout;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          unsigned source_data_idx = orbit.eyelid_cell_data_index_[i];

          source_app_data[source_data_idx] = target_app_data[target_data_idx];
          source_color_data[source_data_idx]= target_color_data[target_data_idx];

        }
      }
    }
  }
  vcl_cout<<"Extracted upper lid appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;
}
