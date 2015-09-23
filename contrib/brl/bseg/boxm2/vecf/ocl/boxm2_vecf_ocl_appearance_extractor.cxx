
#include "boxm2_vecf_ocl_appearance_extractor.h"
#include <boct/boct_bit_tree.h>
#include "../boxm2_vecf_composite_head_parameters.h"
#include <vul/vul_timer.h>
void boxm2_vecf_ocl_appearance_extractor::reset(bool is_right){
  boxm2_vecf_orbit_scene orbit = is_right ? head_model_.right_orbit_ : head_model_.left_orbit_;
  boxm2_scene_sptr scene = orbit.scene();

  total_sclera_app_.fill(0); vis_sclera_ = 0.0f;
  total_pupil_app_.fill(0); vis_pupil_ =0.0f;
  total_iris_app_.fill(0); vis_iris_ = 0.0f;
  total_lower_lid_app_.fill(0); vis_lower_lid_ = 0.0f;
  total_upper_lid_app_.fill(0); vis_upper_lid_ = 0.0f;
  total_eyelid_crease_app_.fill(0); vis_eyelid_crease_ = 0.0f;


  vcl_vector<boxm2_block_id> blocks = scene->get_block_ids();
  boxm2_data_base* gray_app_db  = boxm2_cache::instance()->get_data_base(scene, blocks[0],gray_APM_prefix);
  boxm2_data_base* color_app_db = boxm2_cache::instance()->get_data_base(scene, blocks[0], color_APM_prefix + "_" + (head_model_.color_apm_id_));
  boxm2_block_metadata m_data = scene->get_block_metadata(blocks[0]);
  gray_app_db->set_default_value(gray_APM_prefix,   m_data);
  color_app_db->set_default_value(color_APM_prefix, m_data);
}
bool boxm2_vecf_ocl_appearance_extractor::extract_data(boxm2_scene_sptr scene,boxm2_block_id& block,float * &alpha,gray_APM* &gray_app, color_APM* &color_app){
  boxm2_data_base* gray_app_db  = boxm2_cache::instance()->get_data_base(scene, block,gray_APM_prefix);
  boxm2_data_base* alpha_db     = boxm2_cache::instance()->get_data_base(scene, block,"alpha");
  boxm2_data_base* color_app_db = boxm2_cache::instance()->get_data_base(scene, block, color_APM_prefix + "_" + (head_model_.color_apm_id_));
  gray_app  = reinterpret_cast<gray_APM*>(gray_app_db->data_buffer());
  color_app = reinterpret_cast<color_APM*>(color_app_db->data_buffer());
  alpha     = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(alpha_db->data_buffer());
  if (gray_app && alpha && color_app)
    return true;

  return false;
}
void boxm2_vecf_ocl_appearance_extractor::extract_head_appearance(){
  vul_timer t;
  boxm2_scene_sptr base_model = head_model_.scene();
  // for each block of the target scene
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();
  vcl_vector<boxm2_block_id> source_blocks = base_model->get_block_ids();

    // for each block of the target scene
  vgl_rotation_3d<double> id; id.set_identity();
  vgl_vector_3d<double> null; null.set(0, 0, 0);

  scene_transformer_.transform_1_blk_interp_trilin(base_model,id, null, head_model_.scale_ ,true);

  vcl_cout<<"extracted head appearance model from target in "<<t.real()/1000<<" seconds"<<vcl_endl;
}


void boxm2_vecf_ocl_appearance_extractor::extract_orbit_appearance(){

  vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();
  if (target_blocks.size()>1){
    vcl_cout<< "visibility info cannot be used in current implementation if scene contains more than one block"<<vcl_endl;
    current_vis_score_ = 0;
  }else{
    boxm2_data_base* vis_score_db = boxm2_cache::instance()->get_data_base(target_scene_,target_blocks[0],boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(head_model_.color_apm_id_));
    current_vis_score_ = reinterpret_cast<vis_score_t*>(vis_score_db->data_buffer());
  }
  this->reset(true);
  this->reset(false);

  //extract and compute the mean appearance of each anatomy label
  this->extract_eye_appearance(true,true);
  this->extract_eye_appearance(false,true);

  this->extract_iris_appearance(true,true);
  this->extract_iris_appearance(false,true);

  this->extract_pupil_appearance(true,true);
  this->extract_pupil_appearance(false,true);

  this->extract_eyelid_crease_appearance(true,true);
  this->extract_eyelid_crease_appearance(false,true);

  this->extract_lower_lid_appearance(true,true);
  this->extract_lower_lid_appearance(false,true);

  this->extract_upper_lid_appearance(true,true);
  this->extract_upper_lid_appearance(false,true);


  // set the appearance of occluded eye voxels to the mean appearance
  this->extract_eye_appearance(true,false);
  this->extract_eye_appearance(false,false);

  this->extract_iris_appearance(true,false);
  this->extract_iris_appearance(false,false);

  this->extract_pupil_appearance(true,false);
  this->extract_pupil_appearance(false,false);

  this->extract_eyelid_crease_appearance(true,false);
  this->extract_eyelid_crease_appearance(false,false);

  this->extract_lower_lid_appearance(true,false);
  this->extract_lower_lid_appearance(false,false);

  this->extract_upper_lid_appearance(true,false);
  this->extract_upper_lid_appearance(false,false);


  this->bump_up_vis_scores();

}
void boxm2_vecf_ocl_appearance_extractor::extract_iris_appearance(bool is_right,bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
  if (!is_right){
    orbit =  head_model_.left_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    the_other_orbit_params = head_params.r_orbit_params_;
  }else{
    orbit = head_model_.right_orbit_;
    orbit_params           = head_params.r_orbit_params_;
    the_other_orbit_params = head_params.l_orbit_params_;
  }

  color_APM color =orbit.random_color();
  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> to_dir(orbit_params.eye_pointing_dir_.x(),
                                     orbit_params.eye_pointing_dir_.y(),
                                     orbit_params.eye_pointing_dir_.z());

  vnl_vector_fixed<double, 3> other_to_dir(the_other_orbit_params.eye_pointing_dir_.x(),
                                           the_other_orbit_params.eye_pointing_dir_.y(),
                                           the_other_orbit_params.eye_pointing_dir_.z());

  vgl_rotation_3d<double>       rot(Z, to_dir);
  vgl_rotation_3d<double> other_rot(Z, other_to_dir);


  float sum_vis = 0;
  color_APM& curr_iris  = is_right ? right_iris_app_ : left_iris_app_;
  color_APM& other_iris = is_right ? left_iris_app_  : right_iris_app_;
  color_APM final_iris_app;

  float8 weighted_sum; weighted_sum.fill(0);
  if(!extract && vis_iris_!= 0 ){
    float8 final_iris_app_f = total_iris_app_ / vis_iris_;
    final_iris_app = to_apm_t(final_iris_app_f);
    curr_iris =  individual_appearance_ ? curr_iris : final_iris_app;
  }


  boxm2_scene_sptr source_model = orbit.scene();
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();

  unsigned n_source_cells = static_cast<unsigned>(orbit.iris_cell_centers_.size());
  vcl_cout<<"iris cell centers: "<<n_source_cells<<vcl_endl;
  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.iris_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::IRIS))
        continue;
      //is a sphere voxel cell so define the vector field
       vgl_point_3d<double>    local_reflected_p = p;
       local_reflected_p = vgl_point_3d<double>(-1 * local_reflected_p.x(), local_reflected_p.y(), local_reflected_p.z()) ;

      vgl_point_3d<double>    mapped_p =       rot * p + orbit_params.offset_ ;
      vgl_point_3d<double> reflected_p =       other_rot * local_reflected_p + the_other_orbit_params.offset_ ;
      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_endl;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          target_blk->data_index( reflected_p, reflected_target_data_idx);
          unsigned source_data_idx = orbit.iris_cell_data_index_[i];

          source_app_data[source_data_idx]  = target_app_data[target_data_idx];
          double prob =1 - exp (- target_alpha_data[target_data_idx] * target_side_len);
          if(extract){
            if (current_vis_score_ && prob > 0.8){
              float8 curr_float_col = to_float8(target_color_data[target_data_idx]);
              weighted_sum +=  curr_float_col * current_vis_score_[target_data_idx];
              sum_vis += current_vis_score_[target_data_idx];
            }
          }else{
            // float8 appearance = to_float8(target_color_data[target_data_idx]) * current_vis_score_[target_data_idx] + to_float8(curr_iris) * (1 - current_vis_score_[target_data_idx]);
            float8  appearance = weight_intesities(current_vis_score_[target_data_idx], current_vis_score_[reflected_target_data_idx],                                               to_float8(target_color_data[target_data_idx]),
                                                   to_float8(target_color_data[reflected_target_data_idx]),
                                                   to_float8(curr_iris),
                                                   to_float8(other_iris));

            source_color_data[source_data_idx]=  faux_ ? color : to_apm_t(appearance);

          }
        }
      }
    }
  }
  if(sum_vis != 0 && extract){
    vis_iris_ += sum_vis;
    total_iris_app_ +=weighted_sum;
    weighted_sum /= sum_vis;
    curr_iris = to_apm_t( weighted_sum );
}
    //  vcl_cout<<"Extracted iris appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;

}
void boxm2_vecf_ocl_appearance_extractor::extract_pupil_appearance(bool is_right, bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
  if (!is_right){
    orbit =  head_model_.left_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    the_other_orbit_params = head_params.r_orbit_params_;
  }else{
    orbit = head_model_.right_orbit_;
    orbit_params = head_params.r_orbit_params_;
    the_other_orbit_params = head_params.l_orbit_params_;
  }


  color_APM color =orbit.random_color();
  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> to_dir(orbit_params.eye_pointing_dir_.x(),
                                     orbit_params.eye_pointing_dir_.y(),
                                     orbit_params.eye_pointing_dir_.z());
  vnl_vector_fixed<double, 3> other_to_dir(the_other_orbit_params.eye_pointing_dir_.x(),
                                           the_other_orbit_params.eye_pointing_dir_.y(),
                                           the_other_orbit_params.eye_pointing_dir_.z());

  vgl_rotation_3d<double>       rot(Z, to_dir);
  vgl_rotation_3d<double> other_rot(Z, other_to_dir);



  float sum_vis = 0;
  color_APM& curr_pupil  = is_right ? right_pupil_app_ : left_pupil_app_;
  color_APM& other_pupil = is_right ? left_pupil_app_  : right_pupil_app_;
  float8 weighted_sum; weighted_sum.fill(0);

  color_APM final_pupil_app;
  if(!extract && vis_pupil_!= 0 ){
    float8 final_pupil_app_f = total_pupil_app_ / vis_pupil_;
    final_pupil_app = to_apm_t(final_pupil_app_f);
    curr_pupil =  individual_appearance_ ? curr_pupil : final_pupil_app;
  }

  boxm2_scene_sptr source_model = orbit.scene();
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();


  unsigned n_source_cells = static_cast<unsigned>(orbit.pupil_cell_centers_.size());
  vcl_cout<<"pupil cell centers: "<<n_source_cells<<vcl_endl;
  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.pupil_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::PUPIL))
        continue;
      //is a sphere voxel cell so define the vector field
       vgl_point_3d<double>    local_reflected_p = p;
       local_reflected_p = vgl_point_3d<double>(-1 * local_reflected_p.x(), local_reflected_p.y(), local_reflected_p.z()) ;

      vgl_point_3d<double>    mapped_p =       rot * p + orbit_params.offset_ ;
      vgl_point_3d<double> reflected_p =       other_rot * local_reflected_p + the_other_orbit_params.offset_ ;
      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_endl;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          target_blk->data_index( reflected_p, reflected_target_data_idx);

          unsigned source_data_idx = orbit.pupil_cell_data_index_[i];
          uchar8 appearance = faux_ ? color : target_color_data[target_data_idx];
          source_app_data[source_data_idx]  = target_app_data[target_data_idx];
          double prob = 1 - exp (- target_alpha_data[target_data_idx] * target_side_len);
          if(extract){
            if (current_vis_score_ && prob > 0.8){
              float8 curr_float_col = to_float8(target_color_data[target_data_idx]);
              weighted_sum +=  curr_float_col * current_vis_score_[target_data_idx];
              sum_vis += current_vis_score_[target_data_idx];
            }
          }else{
            // float8 appearance = to_float8(target_color_data[target_data_idx]) * current_vis_score_[target_data_idx]+
            //                     to_float8(curr_pupil)                    * (1 - current_vis_score_[target_data_idx]);
            float8  appearance = weight_intesities(current_vis_score_[target_data_idx], current_vis_score_[reflected_target_data_idx],                                               to_float8(target_color_data[target_data_idx]),
                                                   to_float8(target_color_data[reflected_target_data_idx]),
                                                   to_float8(curr_pupil),
                                                   to_float8(other_pupil));
            source_color_data[source_data_idx] =  faux_ ? color : to_apm_t(appearance);
          }
        }
      }
    }
  }
  if(sum_vis != 0 && extract){
    vis_pupil_ += sum_vis;
    total_pupil_app_ +=weighted_sum;
    weighted_sum /= sum_vis;
    curr_pupil = to_apm_t( weighted_sum );
}
  if(extract)
    vcl_cout<<"Sum vis for current pupil is "<<sum_vis<<vcl_endl;
  //  vcl_cout<<"Extracted pupil appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;

}
void boxm2_vecf_ocl_appearance_extractor::extract_eye_appearance(bool is_right, bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
  if (!is_right){
          orbit =  head_model_.left_orbit_;
    other_orbit =  head_model_.right_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    the_other_orbit_params = head_params.r_orbit_params_;
  }else{
          orbit = head_model_.right_orbit_;
    other_orbit = head_model_.left_orbit_;
    orbit_params           = head_params.r_orbit_params_;
    the_other_orbit_params = head_params.l_orbit_params_;
  }
  color_APM color =orbit.random_color();
  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> to_dir(orbit_params.eye_pointing_dir_.x(),
                                     orbit_params.eye_pointing_dir_.y(),
                                     orbit_params.eye_pointing_dir_.z());

  vnl_vector_fixed<double, 3> other_to_dir(the_other_orbit_params.eye_pointing_dir_.x(),
                                           the_other_orbit_params.eye_pointing_dir_.y(),
                                           the_other_orbit_params.eye_pointing_dir_.z());

  vgl_rotation_3d<double>       rot(Z, to_dir);
  vgl_rotation_3d<double> other_rot(Z, other_to_dir);

  float sum_vis = 0;
  color_APM& curr_sclera  = is_right ? right_sclera_app_ : left_sclera_app_;
  color_APM& other_sclera = is_right ? left_sclera_app_  : right_sclera_app_;
  float8 weighted_sum; weighted_sum.fill(0);

  color_APM final_sclera_app;
  if(!extract && vis_sclera_!= 0 ){
    float8 final_sclera_app_f = total_sclera_app_ / vis_sclera_;
    final_sclera_app = to_apm_t(final_sclera_app_f);
    curr_sclera =  individual_appearance_ ? curr_sclera : final_sclera_app;
  }


  boxm2_scene_sptr source_model = orbit.scene();

  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();
  unsigned n_source_cells = static_cast<unsigned>(orbit.sphere_cell_centers_.size());
  vcl_cout<<"sphere cell centers: "<<n_source_cells<<vcl_endl;
  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.sphere_cell_centers_[i]);
      vgl_point_3d<double> test_p = (other_orbit.sphere_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::SPHERE) ||
         orbit.is_type_global(p, boxm2_vecf_orbit_scene::IRIS  )  ||
         orbit.is_type_global(p, boxm2_vecf_orbit_scene::PUPIL )    )
        continue;
      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double>    local_reflected_p = p;
       local_reflected_p = vgl_point_3d<double>(-1 * local_reflected_p.x(), local_reflected_p.y(), local_reflected_p.z()) ;

      vgl_point_3d<double>    mapped_p =       rot * p + orbit_params.offset_ ;
      vgl_point_3d<double> reflected_p =       other_rot * local_reflected_p + the_other_orbit_params.offset_ ;
      if((p-test_p).length()>0.01)
        vcl_cout<<"point difference is "<<p<<" "<<test_p<<vcl_endl;
      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_endl;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          target_blk->data_index( reflected_p, reflected_target_data_idx);

          unsigned source_data_idx = orbit.sphere_cell_data_index_[i];
          unsigned other_source_data_idx = other_orbit.sphere_cell_data_index_[i];
          if(source_data_idx - other_source_data_idx != 0)
            vcl_cout<<" data indices are different "<<other_source_data_idx<<" "<<source_data_idx<<vcl_endl;
          uchar8 appearance = faux_ ? color : target_color_data[target_data_idx];
          source_app_data  [source_data_idx] = target_app_data[target_data_idx];
          double prob =1 - exp (- target_alpha_data[target_data_idx] * target_side_len);
          if(extract){
            if (current_vis_score_ && prob > 0.8){
              float8 curr_float_col = to_float8(target_color_data[target_data_idx]);
              weighted_sum +=  curr_float_col * current_vis_score_[target_data_idx];
              sum_vis += current_vis_score_[target_data_idx];
            }
          }else{
            float8  appearance = weight_intesities(current_vis_score_[target_data_idx], current_vis_score_[reflected_target_data_idx],
                                                   to_float8(target_color_data[target_data_idx]),
                                                   to_float8(target_color_data[reflected_target_data_idx]),
                                                   to_float8(final_sclera_app),
                                                   to_float8(other_sclera));

            source_color_data[source_data_idx]=  faux_ ? color : to_apm_t(appearance);
          }
        }
      }
    }
  }
  if(sum_vis != 0 && extract){
    vis_sclera_ += sum_vis;
    total_sclera_app_ +=weighted_sum;
    weighted_sum /= sum_vis;
    curr_sclera = to_apm_t( weighted_sum );
}
  //  vcl_cout<<"Extracted eye sphere appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;
}
void boxm2_vecf_ocl_appearance_extractor::extract_eyelid_crease_appearance(bool is_right, bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
  if (!is_right){
          orbit =  head_model_.left_orbit_;
    other_orbit =  head_model_.right_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    the_other_orbit_params = head_params.r_orbit_params_;
  }else{
          orbit = head_model_.right_orbit_;
    other_orbit = head_model_.left_orbit_;
    orbit_params           = head_params.r_orbit_params_;
    the_other_orbit_params = head_params.l_orbit_params_;
  }
  color_APM color =orbit.random_color();
  boxm2_scene_sptr source_model = orbit.scene();
  float sum_vis = 0;
  color_APM& curr_eyelid_crease  = is_right ? right_eyelid_crease_app_ : left_eyelid_crease_app_ ;
  color_APM& other_eyelid_crease = is_right ? left_eyelid_crease_app_  : right_eyelid_crease_app_ ;
  float8 weighted_sum; weighted_sum.fill(0);
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();
  unsigned n_source_cells = static_cast<unsigned>(orbit.eyelid_crease_cell_centers_.size());
  vcl_cout<<"eyelid crease cell centers "<<n_source_cells<<vcl_endl;

  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.eyelid_crease_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::EYELID_CREASE))
        continue;
      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> local_reflected = p;
      if (is_right){
        local_reflected =vgl_point_3d<double>(-1 * local_reflected.x(),  local_reflected.y() , local_reflected.z());
      }

      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> mapped_p = (local_reflected + orbit_params.offset_) ;
      vgl_vector_3d<double> flip(-2 * mapped_p.x(),0,0);
      vgl_point_3d <double> reflected_p = mapped_p + flip;

      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_endl;
            return ;
          }
          target_blk->data_index(    mapped_p,           target_data_idx);
          target_blk->data_index( reflected_p, reflected_target_data_idx);
          unsigned source_data_idx = orbit.eyelid_crease_cell_data_index_[i];
          double prob =1 - exp (- target_alpha_data[target_data_idx] * target_side_len);
          source_app_data[source_data_idx] = target_app_data[target_data_idx];
          if(extract){
            if (current_vis_score_ && prob > 0.8){
              float8 curr_float_col = to_float8(target_color_data[target_data_idx]);
              weighted_sum +=  curr_float_col * current_vis_score_[target_data_idx];
              sum_vis += current_vis_score_[target_data_idx];
            }
          }else{
             float8  appearance = weight_intesities(current_vis_score_[target_data_idx], current_vis_score_[reflected_target_data_idx],
                                                   to_float8(target_color_data[target_data_idx]),
                                                   to_float8(target_color_data[reflected_target_data_idx]),
                                                   to_float8(curr_eyelid_crease),
                                                   to_float8(other_eyelid_crease));
            source_color_data[source_data_idx] = faux_ ? color : to_apm_t(appearance);
          }
        }
      }
    }
  }
  if(sum_vis != 0 && extract){
    vis_eyelid_crease_       += sum_vis;
    total_eyelid_crease_app_ += weighted_sum;
    weighted_sum /= sum_vis;
    curr_eyelid_crease = to_apm_t( weighted_sum );
}
    //  vcl_cout<<"Extracted eyelid crease appearance in "<<t.real()/1000<<" seconds and sum_vis is "<<sum_vis<<vcl_endl;
}

void boxm2_vecf_ocl_appearance_extractor::extract_lower_lid_appearance(bool is_right,bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
  if (!is_right){
          orbit =  head_model_.left_orbit_;
    other_orbit =  head_model_.right_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    the_other_orbit_params = head_params.r_orbit_params_;
  }else{
          orbit = head_model_.right_orbit_;
    other_orbit = head_model_.left_orbit_;
    orbit_params           = head_params.r_orbit_params_;
    the_other_orbit_params = head_params.l_orbit_params_;
  }
  color_APM& curr_lower_eyelid  = is_right ? right_lower_lid_app_: left_lower_lid_app_;
  color_APM& other_lower_eyelid = is_right ? left_lower_lid_app_ : right_lower_lid_app_;

 color_APM final_lower_lid_app;
  if(!extract && vis_lower_lid_!= 0 ){
    float8 final_lower_lid_app_f = total_lower_lid_app_ / vis_lower_lid_;
    final_lower_lid_app = to_apm_t(final_lower_lid_app_f);
    curr_lower_eyelid =  individual_appearance_ ? curr_lower_eyelid : final_lower_lid_app;
  }
  color_APM color =orbit.random_color();
  boxm2_scene_sptr source_model = orbit.scene();
  float8 weighted_sum; weighted_sum.fill(0); float sum_vis =0.0f;
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();

  unsigned n_source_cells = static_cast<unsigned>(orbit.lower_eyelid_cell_centers_.size());
  vcl_cout<<"lower lid cell centers "<<n_source_cells<<vcl_endl;
  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_endl;
      return;
    }

    for(unsigned i = 0; i< n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.lower_eyelid_cell_centers_[i]);

      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::LOWER_LID))
        continue;

      vgl_point_3d<double> local_reflected = p;
      if (is_right){
        local_reflected =vgl_point_3d<double>(-1 * local_reflected.x(),  local_reflected.y() , local_reflected.z());
      }

      //is a sphere voxel cell so define the vector field
      vgl_point_3d <double> mapped_p = (local_reflected + orbit_params.offset_) ;
      vgl_point_3d <double> reflected_p = vgl_point_3d<double>(-1 * mapped_p.x(),  mapped_p.y() , mapped_p.z());

      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_endl;
            return ;
          }
          target_blk->data_index(    mapped_p,           target_data_idx);
          target_blk->data_index( reflected_p, reflected_target_data_idx);
          unsigned source_data_idx = orbit.lower_eyelid_cell_data_index_[i];
          source_app_data[source_data_idx] = target_app_data[target_data_idx];

          uchar8 appearance = faux_ ? color : target_color_data[target_data_idx];
          source_color_data[source_data_idx]  = appearance;
          double prob = 1 - exp (- target_alpha_data[target_data_idx] * target_side_len);
         if(extract){
            if (current_vis_score_ && prob > 0.8){
              float8 curr_float_col = to_float8(target_color_data[target_data_idx]);
              weighted_sum +=  curr_float_col * current_vis_score_[target_data_idx];
              sum_vis += current_vis_score_[target_data_idx];
            }
          }else{
         float8  appearance = weight_intesities(current_vis_score_[target_data_idx], current_vis_score_[reflected_target_data_idx],
                                                   to_float8(target_color_data[target_data_idx]),
                                                   to_float8(target_color_data[reflected_target_data_idx]),
                                                   to_float8(curr_lower_eyelid),
                                                   to_float8(other_lower_eyelid));
            source_color_data[source_data_idx] = faux_ ? color : to_apm_t(appearance);
            vis_cells_.push_back(target_data_idx); //need to set the vis of this index to 1
          }
        }
      }
    }
  }
  if(sum_vis != 0 && extract){
    vis_lower_lid_ += sum_vis;
    total_lower_lid_app_ += weighted_sum;
    weighted_sum /= sum_vis;
    curr_lower_eyelid = to_apm_t( weighted_sum );
  }
    //  vcl_cout<<"Extracted lower lid appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;
}

void boxm2_vecf_ocl_appearance_extractor::extract_upper_lid_appearance(bool is_right,bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_orbit_scene orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
  if (!is_right){
          orbit =  head_model_.left_orbit_;
    other_orbit =  head_model_.right_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    the_other_orbit_params = head_params.r_orbit_params_;
  }else{
          orbit = head_model_.right_orbit_;
    other_orbit = head_model_.left_orbit_;
    orbit_params           = head_params.r_orbit_params_;
    the_other_orbit_params = head_params.l_orbit_params_;
  }


  uchar8 red,blue; red.fill(0);blue.fill(0); red[0] =255; red[2]=255; blue[1] = 255;
  color_APM color =orbit.random_color();

  double dt = 0.95;
  color_APM eyelid_color = is_right? red : blue;
  color[0]=0; color[1]=0; color[2]=255;
  float8 weighted_sum; weighted_sum.fill(0); float sum_vis =0.0f;
  boxm2_scene_sptr source_model = orbit.scene();

  color_APM& curr_lower_lid      = is_right ? right_lower_lid_app_     : left_lower_lid_app_ ;
  color_APM& curr_upper_lid      = is_right ? right_upper_lid_app_     : left_upper_lid_app_ ;
  color_APM& curr_eyelid_crease  = is_right ? right_eyelid_crease_app_ : left_eyelid_crease_app_ ;

  color_APM& other_lower_lid     = is_right ? left_lower_lid_app_     : right_lower_lid_app_ ;
  color_APM& other_upper_lid     = is_right ? left_upper_lid_app_     : right_upper_lid_app_ ;
  color_APM& other_eyelid_crease = is_right ? left_eyelid_crease_app_ : right_eyelid_crease_app_ ;

  color_APM final_upper_lid_app,final_lower_lid_app,final_eyelid_crease_app;

  if(!extract && vis_upper_lid_!= 0 ){
    float8 final_upper_lid_app_f = total_upper_lid_app_ / vis_upper_lid_;
    final_upper_lid_app = to_apm_t(final_upper_lid_app_f);
    curr_upper_lid =  individual_appearance_ ? curr_upper_lid : final_upper_lid_app;
  }

  float8 final_lower_lid_app_f = total_lower_lid_app_ / vis_lower_lid_;
  final_lower_lid_app = to_apm_t(final_lower_lid_app_f);

  float8 final_eyelid_crease_app_f = total_eyelid_crease_app_ / vis_eyelid_crease_;
  final_eyelid_crease_app = to_apm_t(final_eyelid_crease_app_f);


  uchar8 curr_lower_lid_scaled     = final_lower_lid_app;
  uchar8 curr_eyelid_crease_scaled = final_eyelid_crease_app;
  vcl_vector<boxm2_block_id> source_blocks = source_model->get_block_ids();

  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin(); sblk != source_blocks.end(); ++sblk) {
    boxm2_block *source_blk = boxm2_cache::instance()->get_block(source_model, *sblk);
    unsigned n_source_cells = static_cast<unsigned>(orbit.eyelid_cell_centers_.size());
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,*sblk,source_alpha_data,source_app_data,source_color_data)){
      vcl_cout<<"Data extraction failed for scene "<< source_model << " in block "<<*sblk<<vcl_endl;
      return;
    }

    for(unsigned i = 0; i < n_source_cells; ++i){
      bool skip = false;vgl_point_3d<double> loc_p;
      vgl_point_3d<double> p = (orbit.eyelid_cell_centers_[i]);


      if(!orbit.is_type_global(p, boxm2_vecf_orbit_scene::UPPER_LID)  ){
        //#if _DEBUG
        if(is_right)
          vcl_cout<<"this right eyelid point "<<p<<" was not ok w.r.t label type"<<vcl_endl;
        else
          vcl_cout<<"this left eyelid point "<<p<<" was not ok w.r.t label type"<<vcl_endl;
        continue;
        //#endif
      }

      if(!(source_blk->contains(p, loc_p) )){
#if _DEBUG
        if(is_right)
          vcl_cout<<"this right eyelid point "<<p<<" was not in bounds"<<vcl_endl;
        else
          vcl_cout<<"this left eyelid point "<<p<<" was not in bounds"<<vcl_endl;
#endif
        continue;

      }
      //is a sphere voxel cell so define the vector field

      double tc = orbit.eyelid_geo_.t(p.x(), p.y());
      if(!orbit.eyelid_geo_.valid_t(tc)){
#if _DEBUG
        vcl_cout<<"this eyelid point "<<p<<" was not ok w.r.t tc"<<vcl_endl;
        continue;
#endif
      }
      // inverse field so negate dt
      double ti = tc - dt;
      if(!orbit.eyelid_geo_.valid_t(ti)){
        skip = true;
      }

      if (is_right){
        p =vgl_point_3d<double>(-1 * p.x(),  p.y() , p.z());
      }

      float intensity_scale =  tc > 0.7f ? 0.8f : 1.0f;
      curr_lower_lid_scaled[0]   = static_cast<unsigned char>((0.96 - tc) * final_eyelid_crease_app[0] +
                                                                     (tc) * final_lower_lid_app    [0]);
      curr_lower_lid_scaled[0] = static_cast<unsigned char>( intensity_scale * (float) curr_lower_lid_scaled[0] );


      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> mapped_p = (p + orbit_params.offset_) ;
      vgl_vector_3d<double> flip(-2 * mapped_p.x(),0,0);
      vgl_point_3d <double> reflected_p = mapped_p + flip;


      // find closest sphere voxel cell
      vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, *tblk);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx, reflected_target_data_idx;
          if(!this->extract_data(target_scene_,*tblk,target_alpha_data,target_app_data,target_color_data)){
            vcl_cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<*tblk<<vcl_endl;
            return ;
          }

          target_blk->data_index(    mapped_p,           target_data_idx);
          target_blk->data_index( reflected_p, reflected_target_data_idx);
          unsigned source_data_idx = orbit.eyelid_cell_data_index_[i];
          source_app_data  [source_data_idx] = target_app_data  [target_data_idx];

          double prob = 1 - exp (- target_alpha_data[target_data_idx] * target_side_len);
          if(extract){
            if (current_vis_score_ && prob > 0.8 && !skip ){
              float8 curr_float_col = to_float8(target_color_data[target_data_idx]);
              weighted_sum +=  curr_float_col * current_vis_score_[target_data_idx];
              sum_vis += current_vis_score_[target_data_idx];
            }
          }else{
            float8  appearance = weight_intesities(current_vis_score_[target_data_idx], current_vis_score_[reflected_target_data_idx],
                                                   to_float8(target_color_data[target_data_idx]),
                                                   to_float8(target_color_data[reflected_target_data_idx]),
                                                   to_float8(curr_lower_lid_scaled),
                                                   to_float8(other_lower_lid));
            source_color_data[source_data_idx]=  faux_ ? color : to_apm_t(appearance);
            if(skip)
              source_color_data[source_data_idx]=  faux_ ? color: curr_lower_lid_scaled;
            vis_cells_.push_back(target_data_idx); //need to set the vis of this index to 1
          }
        }
      }
    }
  }
  if(sum_vis != 0 && extract){
    vis_upper_lid_ += sum_vis;
    total_upper_lid_app_ += weighted_sum;
    weighted_sum /= sum_vis;
    curr_upper_lid = to_apm_t( weighted_sum );
  }
    //  vcl_cout<<"Extracted upper lid appearance in "<<t.real()/1000<<" seconds"<<vcl_endl;
}
void boxm2_vecf_ocl_appearance_extractor::bump_up_vis_scores(){
    // for (vcl_vector<unsigned>::iterator it =vis_cells_.begin() ; it != vis_cells_.end(); it++)
    // current_vis_score_[*it] = 1;
}

float8 boxm2_vecf_ocl_appearance_extractor::weight_intesities(float& vis_A,float& vis_B,float8 int_A,float8 int_B,float8 mean_A,float8 mean_B){
  bool the_dan_way = true;
  float dominant_vis    = vis_A > vis_B ?  vis_A :  vis_B;
  float8& dominant_mean = vis_A > vis_B ? mean_A : mean_B;
  float8 ret_val;
  if (the_dan_way){
    float s = 10.0f;
    float m = 1.1;
    float v = 0.6f;
    float inv_weight1 = 1.0/(1.0 + std::exp( -s * (vis_B - m * vis_A)));
    float inv_weight2 = 1.0/(1.0 + std::exp( -s * (v - vis_A)));
    float weight = 1.0f - inv_weight1 * inv_weight2;
    float8 observed_intensity =  int_A * weight + int_B *  (1.0f - weight);



    s = 16 ; v = 0.3;
    weight = 1.0/(1.0 + std::exp( -s * ( dominant_vis -v )));
    ret_val = observed_intensity * weight + mean_A * (1.0f - weight);

  }else{
    ret_val = vis_A * int_A + (1 - vis_A) * mean_A;
  }
  return ret_val;
}
