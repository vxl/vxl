#include "boxm2_vecf_ocl_appearance_extractor.h"
#include <boct/boct_bit_tree.h>
#include "../boxm2_vecf_composite_head_parameters.h"
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
void boxm2_vecf_ocl_appearance_extractor::compile_kernels(){
  std::string options;
  // sets apptypesize_ and app_type
   scene_transformer_.get_scene_appearance(head_model_.left_orbit_.scene(), options);
  std::cout<<" compiling trans kernel with options "<<options<< std::endl;
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  std::string vecf_source_dir = std::string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "atomics_util.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "boxm2_ocl_helpers.cl");
  src_paths.push_back(vecf_source_dir + "interp_helpers.cl");
  src_paths.push_back(vecf_source_dir + "eyelid_funcs.cl");
  src_paths.push_back(vecf_source_dir + "orbit_funcs.cl");
  src_paths.push_back(vecf_source_dir + "inverse_map_orbit.cl");

  std::cout<<"compiling appearance extraction kernel with "<<options<<std::endl;
  auto* appearance_extraction_kernel = new bocl_kernel();
  appearance_extraction_kernel->create_kernel(&device_->context(),device_->device_id(), src_paths, "map_to_source_and_extract_appearance", options, "orbit_appearance_extraction");
  kernels_.push_back(appearance_extraction_kernel);

  std::string options_anatomy = options + " -D ANATOMY_CALC ";
  auto* mean_anatomical_appearance_kernel = new bocl_kernel();
  mean_anatomical_appearance_kernel->create_kernel(&device_->context(),device_->device_id(), src_paths, "map_to_source_and_extract_appearance", options_anatomy, "orbit_mean_appearance_calc");
  kernels_.push_back(mean_anatomical_appearance_kernel);

}


void boxm2_vecf_ocl_appearance_extractor::reset(bool is_right){
  ORBIT & orbit = is_right ? head_model_.right_orbit_ : head_model_.left_orbit_;
  boxm2_scene_sptr scene = orbit.scene();

  total_sclera_app_.fill(0); vis_sclera_ = 0.0f;
  total_pupil_app_.fill(0); vis_pupil_ =0.0f;
  total_iris_app_.fill(0); vis_iris_ = 0.0f;
  total_lower_lid_app_.fill(0); vis_lower_lid_ = 0.0f;
  total_upper_lid_app_.fill(0); vis_upper_lid_ = 0.0f;
  total_eyelid_crease_app_.fill(0); vis_eyelid_crease_ = 0.0f;
  orbit.reset_buffers(true);
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
  std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();
  std::vector<boxm2_block_id> source_blocks = base_model->get_block_ids();

    // for each block of the target scene
  vgl_rotation_3d<double> id; id.set_identity();
  vgl_vector_3d<double> null; null.set(0, 0, 0);

  scene_transformer_.transform_1_blk_interp_trilin(base_model,id, null, head_model_.scale_ ,true);

  std::cout<<"extracted head appearance model from target in "<<t.real()/1000<<" seconds"<<std::endl;
}


void boxm2_vecf_ocl_appearance_extractor::extract_orbit_appearance(){

  std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();
  if (target_blocks.size()>1){
    std::cout<< "visibility info cannot be used in current implementation if scene contains more than one block"<<std::endl;
    current_vis_score_ = nullptr;
  }else{
    boxm2_data_base* vis_score_db = boxm2_cache::instance()->get_data_base(target_scene_,target_blocks[0],boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(head_model_.color_apm_id_));
    current_vis_score_ = reinterpret_cast<vis_score_t*>(vis_score_db->data_buffer());
  }
  int status;
  queue_ = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
  this->reset(true);
  this->reset(false);

#ifdef USE_ORBIT_CL

  this->compute_mean_anatomical_appearance(false);
  this->compute_mean_anatomical_appearance(true);
  this->extract_appearance_one_pass(false);
  this->extract_appearance_one_pass(true);
#else
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
#endif

// #ifdef USE_ORBIT_CL
//   head_model_. left_orbit_.update_source_gpu_buffers(true);
//   head_model_.right_orbit_.update_source_gpu_buffers(true);
// #endif

}
void boxm2_vecf_ocl_appearance_extractor::extract_iris_appearance(bool is_right,bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  ORBIT  orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
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
  std::vector<boxm2_block_id> source_blocks = source_model->get_block_ids();

  auto n_source_cells = static_cast<unsigned>(orbit.iris_cell_centers_.size());
  std::cout<<"iris cell centers: "<<n_source_cells<<std::endl;
  for (auto & source_block : source_blocks) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,source_block,source_alpha_data,source_app_data,source_color_data)){
      std::cout<<"Data extraction failed for scene "<< source_model << " in block "<<source_block<<std::endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.iris_cell_centers_[i]);

      if(!orbit.is_type_global(p, ORBIT::IRIS))
        continue;
      //is a sphere voxel cell so define the vector field
       vgl_point_3d<double>    local_reflected_p = p;
       local_reflected_p = vgl_point_3d<double>(-1 * local_reflected_p.x(), local_reflected_p.y(), local_reflected_p.z()) ;

      vgl_point_3d<double>    mapped_p =       rot * p + orbit_params.offset_ ;
      vgl_point_3d<double> reflected_p =       other_rot * local_reflected_p + the_other_orbit_params.offset_ ;
      // find closest sphere voxel cell
      std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (auto & target_block : target_blocks) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, target_block);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,target_block,target_alpha_data,target_app_data,target_color_data)){
            std::cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<target_block<<std::endl;
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
    //  std::cout<<"Extracted iris appearance in "<<t.real()/1000<<" seconds"<<std::endl;

}
void boxm2_vecf_ocl_appearance_extractor::extract_pupil_appearance(bool is_right, bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  ORBIT orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
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
  std::vector<boxm2_block_id> source_blocks = source_model->get_block_ids();


  auto n_source_cells = static_cast<unsigned>(orbit.pupil_cell_centers_.size());
  std::cout<<"pupil cell centers: "<<n_source_cells<<std::endl;
  for (auto & source_block : source_blocks) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,source_block,source_alpha_data,source_app_data,source_color_data)){
      std::cout<<"Data extraction failed for scene "<< source_model << " in block "<<source_block<<std::endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.pupil_cell_centers_[i]);

      if(!orbit.is_type_global(p, ORBIT::PUPIL))
        continue;
      //is a sphere voxel cell so define the vector field
       vgl_point_3d<double>    local_reflected_p = p;
       local_reflected_p = vgl_point_3d<double>(-1 * local_reflected_p.x(), local_reflected_p.y(), local_reflected_p.z()) ;

      vgl_point_3d<double>    mapped_p =       rot * p + orbit_params.offset_ ;
      vgl_point_3d<double> reflected_p =       other_rot * local_reflected_p + the_other_orbit_params.offset_ ;
      // find closest sphere voxel cell
      std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (auto & target_block : target_blocks) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, target_block);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,target_block,target_alpha_data,target_app_data,target_color_data)){
            std::cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<target_block<<std::endl;
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
    std::cout<<"Sum vis for current pupil is "<<sum_vis<<std::endl;
  //  std::cout<<"Extracted pupil appearance in "<<t.real()/1000<<" seconds"<<std::endl;

}
void boxm2_vecf_ocl_appearance_extractor::extract_eye_appearance(bool is_right, bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  ORBIT orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
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

  std::vector<boxm2_block_id> source_blocks = source_model->get_block_ids();
  auto n_source_cells = static_cast<unsigned>(orbit.sphere_cell_centers_.size());
  std::cout<<"sphere cell centers: "<<n_source_cells<<std::endl;
  for (auto & source_block : source_blocks) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,source_block,source_alpha_data,source_app_data,source_color_data)){
      std::cout<<"Data extraction failed for scene "<< source_model << " in block "<<source_block<<std::endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.sphere_cell_centers_[i]);
      vgl_point_3d<double> test_p = (other_orbit.sphere_cell_centers_[i]);

      if(!orbit.is_type_global(p, ORBIT::SPHERE) ||
         orbit.is_type_global(p, ORBIT::IRIS  )  ||
         orbit.is_type_global(p, ORBIT::PUPIL )    )
        continue;
      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double>    local_reflected_p = p;
       local_reflected_p = vgl_point_3d<double>(-1 * local_reflected_p.x(), local_reflected_p.y(), local_reflected_p.z()) ;

      vgl_point_3d<double>    mapped_p =       rot * p + orbit_params.offset_ ;
      vgl_point_3d<double> reflected_p =       other_rot * local_reflected_p + the_other_orbit_params.offset_ ;
      if((p-test_p).length()>0.01)
        std::cout<<"point difference is "<<p<<" "<<test_p<<std::endl;
      // find closest sphere voxel cell
      std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (auto & target_block : target_blocks) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, target_block);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,target_block,target_alpha_data,target_app_data,target_color_data)){
            std::cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<target_block<<std::endl;
            return ;
          }
          target_blk->data_index( mapped_p, target_data_idx);
          target_blk->data_index( reflected_p, reflected_target_data_idx);

          unsigned source_data_idx = orbit.sphere_cell_data_index_[i];
          unsigned other_source_data_idx = other_orbit.sphere_cell_data_index_[i];
          if(source_data_idx - other_source_data_idx != 0)
            std::cout<<" data indices are different "<<other_source_data_idx<<" "<<source_data_idx<<std::endl;
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
  //  std::cout<<"Extracted eye sphere appearance in "<<t.real()/1000<<" seconds"<<std::endl;
}
void boxm2_vecf_ocl_appearance_extractor::extract_eyelid_crease_appearance(bool is_right, bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  ORBIT orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
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
  std::vector<boxm2_block_id> source_blocks = source_model->get_block_ids();
  auto n_source_cells = static_cast<unsigned>(orbit.eyelid_crease_cell_centers_.size());
  std::cout<<"eyelid crease cell centers "<<n_source_cells<<std::endl;

  for (auto & source_block : source_blocks) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,source_block,source_alpha_data,source_app_data,source_color_data)){
      std::cout<<"Data extraction failed for scene "<< source_model << " in block "<<source_block<<std::endl;
      return;
    }

    for(unsigned i = 0; i<n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.eyelid_crease_cell_centers_[i]);

      if(!orbit.is_type_global(p, ORBIT::EYELID_CREASE))
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

      std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (auto & target_block : target_blocks) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, target_block);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,target_block,target_alpha_data,target_app_data,target_color_data)){
            std::cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<target_block<<std::endl;
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
    //  std::cout<<"Extracted eyelid crease appearance in "<<t.real()/1000<<" seconds and sum_vis is "<<sum_vis<<std::endl;
}

void boxm2_vecf_ocl_appearance_extractor::extract_lower_lid_appearance(bool is_right,bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  ORBIT orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
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
  std::vector<boxm2_block_id> source_blocks = source_model->get_block_ids();

  auto n_source_cells = static_cast<unsigned>(orbit.lower_eyelid_cell_centers_.size());
  std::cout<<"lower lid cell centers "<<n_source_cells<<std::endl;
  for (auto & source_block : source_blocks) {
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,source_block,source_alpha_data,source_app_data,source_color_data)){
      std::cout<<"Data extraction failed for scene "<< source_model << " in block "<<source_block<<std::endl;
      return;
    }

    for(unsigned i = 0; i< n_source_cells; ++i){
      vgl_point_3d<double> p = (orbit.lower_eyelid_cell_centers_[i]);

      if(!orbit.is_type_global(p, ORBIT::LOWER_LID))
        continue;

      vgl_point_3d<double> local_reflected = p;
      if (is_right){
        local_reflected =vgl_point_3d<double>(-1 * local_reflected.x(),  local_reflected.y() , local_reflected.z());
      }

      //is a sphere voxel cell so define the vector field
      vgl_point_3d <double> mapped_p = (local_reflected + orbit_params.offset_) ;
      vgl_point_3d <double> reflected_p = vgl_point_3d<double>(-1 * mapped_p.x(),  mapped_p.y() , mapped_p.z());

      // find closest sphere voxel cell
      std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (auto & target_block : target_blocks) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, target_block);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx,reflected_target_data_idx;
          if(!this->extract_data(target_scene_,target_block,target_alpha_data,target_app_data,target_color_data)){
            std::cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<target_block<<std::endl;
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
    //  std::cout<<"Extracted lower lid appearance in "<<t.real()/1000<<" seconds"<<std::endl;
}

void boxm2_vecf_ocl_appearance_extractor::extract_upper_lid_appearance(bool is_right,bool extract){
  vul_timer t;
  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  ORBIT orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, the_other_orbit_params;
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
  std::vector<boxm2_block_id> source_blocks = source_model->get_block_ids();

  for (auto & source_block : source_blocks) {
    boxm2_block *source_blk = boxm2_cache::instance()->get_block(source_model, source_block);
    auto n_source_cells = static_cast<unsigned>(orbit.eyelid_cell_centers_.size());
    color_APM   * source_color_data; gray_APM* source_app_data; float* source_alpha_data;
    if(!this->extract_data(source_model,source_block,source_alpha_data,source_app_data,source_color_data)){
      std::cout<<"Data extraction failed for scene "<< source_model << " in block "<<source_block<<std::endl;
      return;
    }

    for(unsigned i = 0; i < n_source_cells; ++i){
      bool skip = false;vgl_point_3d<double> loc_p;
      vgl_point_3d<double> p = (orbit.eyelid_cell_centers_[i]);


      if(!orbit.is_type_global(p, ORBIT::UPPER_LID)  ){
        //#if _DEBUG
        if(is_right)
          std::cout<<"this right eyelid point "<<p<<" was not ok w.r.t label type"<<std::endl;
        else
          std::cout<<"this left eyelid point "<<p<<" was not ok w.r.t label type"<<std::endl;
        continue;
        //#endif
      }

      if(!(source_blk->contains(p, loc_p) )){
#if _DEBUG
        if(is_right)
          std::cout<<"this right eyelid point "<<p<<" was not in bounds"<<std::endl;
        else
          std::cout<<"this left eyelid point "<<p<<" was not in bounds"<<std::endl;
#endif
        continue;

      }
      //is a sphere voxel cell so define the vector field

      double tc = orbit.eyelid_geo_.t(p.x(), p.y());
      if(!orbit.eyelid_geo_.valid_t(tc)){
#if _DEBUG
        std::cout<<"this eyelid point "<<p<<" was not ok w.r.t tc"<<std::endl;
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
      color_APM darker_lower_lid = curr_lower_lid_scaled;
      darker_lower_lid[0] *= 0.95;
      curr_lower_lid_scaled[0]  = static_cast<unsigned char> ((0.96 - tc) * darker_lower_lid[0] +
                                                                     (tc) * final_lower_lid_app    [0]);
      curr_lower_lid_scaled[0] = static_cast<unsigned char>( intensity_scale * (float) curr_lower_lid_scaled[0] );


      //is a sphere voxel cell so define the vector field
      vgl_point_3d<double> mapped_p = (p + orbit_params.offset_) ;
      vgl_vector_3d<double> flip(-2 * mapped_p.x(),0,0);
      vgl_point_3d <double> reflected_p = mapped_p + flip;


      // find closest sphere voxel cell
      std::vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();

      vgl_point_3d<double> local_tree_coords, target_cell_center; double target_side_len;
      for (auto & target_block : target_blocks) {
        boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene_, target_block);
        if ( target_blk->contains( mapped_p, local_tree_coords, target_cell_center, target_side_len )) {
          color_APM   * target_color_data; gray_APM* target_app_data; float* target_alpha_data; unsigned target_data_idx, reflected_target_data_idx;
          if(!this->extract_data(target_scene_,target_block,target_alpha_data,target_app_data,target_color_data)){
            std::cout<<"Data extraction failed for scene "<< target_scene_ << " in block "<<target_block<<std::endl;
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
    //  std::cout<<"Extracted upper lid appearance in "<<t.real()/1000<<" seconds"<<std::endl;
}
void boxm2_vecf_ocl_appearance_extractor::bump_up_vis_scores(){
    // for (std::vector<unsigned>::iterator it =vis_cells_.begin() ; it != vis_cells_.end(); it++)
    // current_vis_score_[*it] = 1;
}
#ifdef USE_ORBIT_CL
bool boxm2_vecf_ocl_appearance_extractor::extract_appearance_one_pass(bool is_right)
{

  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_ocl_orbit_scene orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, other_orbit_params;
  if (!is_right){
          orbit =  head_model_.left_orbit_;
    other_orbit =  head_model_.right_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    other_orbit_params = head_params.r_orbit_params_;

  }else{
          orbit = head_model_.right_orbit_;
    other_orbit = head_model_.left_orbit_;
    orbit_params           = head_params.r_orbit_params_;
    other_orbit_params = head_params.l_orbit_params_;
  }

  color_APM& curr_sclera_app        = is_right ? right_sclera_app_        : left_sclera_app_;
  color_APM& curr_pupil_app         = is_right ? right_pupil_app_         : left_pupil_app_;
  color_APM& curr_iris_app          = is_right ? right_iris_app_          : left_iris_app_;
  color_APM& curr_upper_lid_app     = is_right ? right_upper_lid_app_     : left_upper_lid_app_;
  color_APM& curr_lower_lid_app     = is_right ? right_lower_lid_app_     : left_lower_lid_app_;
  color_APM& curr_eyelid_crease_app = is_right ? right_eyelid_crease_app_ : left_eyelid_crease_app_;

  float8 null ; null.fill(0);

  float8 final_sclera_app        = vis_sclera_       !=0 ? (total_sclera_app_/vis_sclera_)        : null;
  float8 final_pupil_app         = vis_pupil_        !=0 ? (total_pupil_app_/vis_pupil_)          : null;
  float8 final_iris_app          = vis_iris_         !=0 ? (total_iris_app_/vis_iris_)            : null;
  float8 final_upper_lid_app     = vis_upper_lid_    !=0 ? (total_upper_lid_app_/vis_upper_lid_)  : null;
  float8 final_lower_lid_app     = vis_lower_lid_    !=0 ? (total_lower_lid_app_/vis_lower_lid_)  : null;
  float8 final_eyelid_crease_app = vis_eyelid_crease_!=0 ? (total_eyelid_crease_app_/vis_eyelid_crease_) : null;

  float8 app[6],total_app[6];


  app[0] =  to_float8 (curr_sclera_app) ;          total_app[0] = final_sclera_app;
  app[1] =  to_float8 (curr_iris_app) ;            total_app[1] = final_iris_app;
  app[2] =  to_float8 (curr_pupil_app) ;           total_app[2] = final_pupil_app;
  app[3] =  to_float8 (curr_upper_lid_app) ;       total_app[3] = final_upper_lid_app;
  app[4] =  to_float8 (curr_lower_lid_app) ;       total_app[4] = final_lower_lid_app;
  app[5] =  to_float8 (curr_eyelid_crease_app) ;   total_app[5] = final_eyelid_crease_app;

  std::cout<<" mean eyelid intensity "<<total_app[3][0]<<" "<<total_app[3][1]<<" "<<total_app[3][2]<<std::endl;

  float offset_buff  [4],other_offset_buff  [4];
  float rotation_buff[9],other_rotation_buff[9];
  float curr_dt = - orbit_params.eyelid_dt_;
  bool good_buffs = true;
  unsigned char is_right_buf = is_right;
  vnl_vector_fixed<double, 3> other_to_dir(other_orbit_params.eye_pointing_dir_.x(),
                                           other_orbit_params.eye_pointing_dir_.y(),
                                           other_orbit_params.eye_pointing_dir_.z());

  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> to_dir(orbit_params.eye_pointing_dir_.x(),
                                     orbit_params.eye_pointing_dir_.y(),
                                     orbit_params.eye_pointing_dir_.z());
  vgl_rotation_3d<double> rot(Z, to_dir);
  vgl_rotation_3d<double> other_rot(Z, other_to_dir);

  offset_buff[0] = orbit_params.offset_.x();      other_offset_buff[0] = other_orbit_params.offset_.x();
  offset_buff[1] = orbit_params.offset_.y();      other_offset_buff[1] = other_orbit_params.offset_.y();
  offset_buff[2] = orbit_params.offset_.z();      other_offset_buff[2] = other_orbit_params.offset_.z();

  vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();
   rotation_buff[0] =(float) R(0,0);  rotation_buff[3] =(float) R(1,0);  rotation_buff[6] =(float) R(2,0);
   rotation_buff[1] =(float) R(0,1);  rotation_buff[4] =(float) R(1,1);  rotation_buff[7] =(float) R(2,1);
   rotation_buff[2] =(float) R(0,2);  rotation_buff[5] =(float) R(1,2);  rotation_buff[8] =(float) R(2,2);


  vnl_matrix_fixed<double, 3, 3> other_R = other_rot.as_matrix();
   other_rotation_buff[0] =(float) other_R(0,0); other_rotation_buff[3] =(float) other_R(1,0); other_rotation_buff[6] =(float) other_R(2,0);
   other_rotation_buff[1] =(float) other_R(0,1); other_rotation_buff[4] =(float) other_R(1,1); other_rotation_buff[7] =(float) other_R(2,1);
   other_rotation_buff[2] =(float) other_R(0,2); other_rotation_buff[5] =(float) other_R(1,2); other_rotation_buff[8] =(float) other_R(2,2);


   bocl_mem_sptr  rotation_l = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
   good_buffs &=  rotation_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  other_rotation_l = new bocl_mem(device_->context(), other_rotation_buff, sizeof(float)*9, " other_rotation " );
   good_buffs &=  other_rotation_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  offset_l = new bocl_mem(device_->context(), offset_buff, sizeof(float)*4, " offset " );
   good_buffs &=  offset_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  other_offset_l = new bocl_mem(device_->context(), other_offset_buff, sizeof(float)*4, "other offset " );
   good_buffs &=  other_offset_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  mean_app_l = new bocl_mem(device_->context(), app, sizeof(float8)*6, " mean app buff " );
   good_buffs &=  mean_app_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  total_app_l = new bocl_mem(device_->context(), total_app, sizeof(float8)*6, " total_mean app buff " );
   good_buffs &=  total_app_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  is_right_cl = new bocl_mem(device_->context(), &is_right_buf, sizeof(char), " mean app buff " );
   good_buffs &=  is_right_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  dt_l = new bocl_mem(device_->context(), &curr_dt , sizeof(float), " dt buff " );
   good_buffs &=  dt_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  max_t_l = new bocl_mem(device_->context(), &max_t_color , sizeof(float), " max t color buff " );
   good_buffs &=  max_t_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   std::size_t local_threads[1]={64};
   std::size_t global_threads[1]={1};

   std::vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
   std::vector<boxm2_block_id> blocks_source = orbit.scene()->get_block_ids();
   auto iter_blk_target = blocks_target.begin();
   auto iter_blk_source = blocks_source.begin();

   if(blocks_target.size()!=1||blocks_source.size()!=1)
     return false;

   boxm2_scene_info*    info_buffer_source      = orbit.scene()->get_blk_metadata(*iter_blk_source);
   boxm2_scene_info*    info_buffer_target      = target_scene_->get_blk_metadata(*iter_blk_target);
   auto target_data_size =  (std::size_t) orbit.target_blk_->num_cells();
   info_buffer_target->data_buffer_length = target_data_size;
   bocl_mem_sptr blk_info_target  = new bocl_mem(device_->context(), info_buffer_target, sizeof(boxm2_scene_info), " Scene Info Target" );
   good_buffs &= blk_info_target->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   blk_info_target->write_to_buffer(queue_);


  bocl_mem_sptr blk_info_source  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info Source" );
  std::size_t source_data_size = (int) orbit.blk_->num_cells();
  info_buffer_source->data_buffer_length = source_data_size;
   blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   blk_info_source->write_to_buffer(queue_);

   auto* output = new float[source_data_size];
   bocl_mem_sptr output_cl = new bocl_mem(device_->context(), output, source_data_size * sizeof(float), "Output buf" );
   good_buffs &=  output_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
   if(!good_buffs)
     return false;
//   output_cl->zero_gpu_buffer(queue_);
   global_threads[0] = (unsigned) RoundUp(info_buffer_source->scene_dims[0]*info_buffer_source->scene_dims[1]*info_buffer_source->scene_dims[2],(int)local_threads[0]);


   bocl_mem*  blk_source      = opencl_cache_->get_block(orbit.scene(), *iter_blk_source);
   bocl_mem*  blk_target      = opencl_cache_->get_block(target_scene_,*iter_blk_target);

   // for (int i = 0;i<6;i++){
   //   for(int j = 0; j<3;j++)
   //     std::cout<<app[i][j]<<" ";
   //   std::cout<<std::endl;
   //   }
   orbit.update_target_gpu_buffers(target_scene_, *iter_blk_target);
   orbit.update_source_gpu_buffers();
   bocl_kernel* map_to_source_kern = kernels_[0];
   map_to_source_kern->set_arg(orbit.centerX_.ptr());
   map_to_source_kern->set_arg(orbit.centerY_.ptr());
   map_to_source_kern->set_arg(orbit.centerZ_.ptr());
   map_to_source_kern->set_arg(orbit.lookup_.ptr());
   map_to_source_kern->set_arg(blk_info_target.ptr());
   map_to_source_kern->set_arg(blk_info_source.ptr());
   map_to_source_kern->set_arg(blk_target);
   map_to_source_kern->set_arg(blk_source);
   map_to_source_kern->set_arg(orbit.target_alpha_base_);
   map_to_source_kern->set_arg(orbit.target_color_base_);
   map_to_source_kern->set_arg(orbit.target_vis_base_);
   map_to_source_kern->set_arg(orbit.source_color_base_);
   map_to_source_kern->set_arg(rotation_l.ptr());
   map_to_source_kern->set_arg(other_rotation_l.ptr());
   map_to_source_kern->set_arg(offset_l.ptr());
   map_to_source_kern->set_arg(other_offset_l.ptr());
   map_to_source_kern->set_arg(orbit.sphere_base_);
   map_to_source_kern->set_arg(orbit.iris_base_);
   map_to_source_kern->set_arg(orbit.pupil_base_);
   map_to_source_kern->set_arg(orbit.eyelid_base_);
   map_to_source_kern->set_arg(orbit.eyelid_crease_base_);
   map_to_source_kern->set_arg(orbit.lower_eyelid_base_);
   map_to_source_kern->set_arg(output_cl.ptr());
   map_to_source_kern->set_arg(orbit.eyelid_geo_cl_.ptr());
   map_to_source_kern->set_arg(dt_l.ptr());
   map_to_source_kern->set_arg(max_t_l.ptr());
   map_to_source_kern->set_arg(total_app_l.ptr());
   map_to_source_kern->set_arg(mean_app_l.ptr());
   map_to_source_kern->set_arg(is_right_cl.ptr());
   map_to_source_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
   map_to_source_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
   map_to_source_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
   map_to_source_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // reflected local trees source
   map_to_source_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // neighbor trees

   if(!map_to_source_kern->execute(queue_, 1, local_threads, global_threads))
     {
       std::cout<<"Kernel Failed to Execute "<<std::endl;
       return false;
    }


   //   orbit.source_color_base_->read_to_buffer(queue_);
//   output_cl->read_to_buffer(queue_);
   int status = clFinish(queue_);
   bool good_kern = check_val(status, CL_SUCCESS, "TRANSFORMATION KERNEL (INTERP) FAILED: " + error_to_string(status));
   map_to_source_kern->clear_args();

   if(!good_kern)
     return false;
   // for (unsigned i = 0;i<source_data_size;i++){
   //   if( output[i]!=0)
   //     std::cout<<output[i]<<" ";
   // }

   delete [] output;
   return true;

  }

bool  boxm2_vecf_ocl_appearance_extractor::compute_mean_anatomical_appearance(bool is_right){

  boxm2_vecf_composite_head_parameters const& head_params = head_model_.get_params();
  boxm2_vecf_ocl_orbit_scene orbit,other_orbit; boxm2_vecf_orbit_params orbit_params, other_orbit_params;
  if (!is_right){
          orbit =  head_model_.left_orbit_;
    other_orbit =  head_model_.right_orbit_;
    orbit_params           = head_params.l_orbit_params_;
    other_orbit_params = head_params.r_orbit_params_;

  }else{
          orbit = head_model_.right_orbit_;
    other_orbit = head_model_.left_orbit_;
    orbit_params           = head_params.r_orbit_params_;
    other_orbit_params = head_params.l_orbit_params_;
  }
   float8 null ; null.fill(0);
  color_APM& curr_sclera_app        = is_right ? right_sclera_app_        : left_sclera_app_;
  color_APM& curr_pupil_app         = is_right ? right_pupil_app_         : left_pupil_app_;
  color_APM& curr_iris_app          = is_right ? right_iris_app_          : left_iris_app_;
  color_APM& curr_upper_lid_app     = is_right ? right_upper_lid_app_     : left_upper_lid_app_;
  color_APM& curr_lower_lid_app     = is_right ? right_lower_lid_app_     : left_lower_lid_app_;
  color_APM& curr_eyelid_crease_app = is_right ? right_eyelid_crease_app_ : left_eyelid_crease_app_;

  float8 app[6];
  float  vis[6];

for(unsigned i = 0;i<6;i++){
  app[i] = null;
  vis[i] = 0;
 }

  float offset_buff  [4],other_offset_buff  [4];
  float rotation_buff[9],other_rotation_buff[9];
  max_t_color = 100;
  bool good_buffs = true;
  unsigned char is_right_buf = is_right;
  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> other_to_dir(other_orbit_params.eye_pointing_dir_.x(),
                                           other_orbit_params.eye_pointing_dir_.y(),
                                           other_orbit_params.eye_pointing_dir_.z());
  vnl_vector_fixed<double, 3> to_dir(orbit_params.eye_pointing_dir_.x(),
                                     orbit_params.eye_pointing_dir_.y(),
                                     orbit_params.eye_pointing_dir_.z());
  vgl_rotation_3d<double> rot(Z, to_dir);
  vgl_rotation_3d<double> other_rot(Z, other_to_dir);
  float curr_dt = - orbit_params.eyelid_dt_;

  offset_buff[0] = orbit_params.offset_.x();      other_offset_buff[0] = other_orbit_params.offset_.x();
  offset_buff[1] = orbit_params.offset_.y();      other_offset_buff[1] = other_orbit_params.offset_.y();
  offset_buff[2] = orbit_params.offset_.z();      other_offset_buff[2] = other_orbit_params.offset_.z();

  vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();
   rotation_buff[0] =(float) R(0,0);  rotation_buff[3] =(float) R(1,0);  rotation_buff[6] =(float) R(2,0);
   rotation_buff[1] =(float) R(0,1);  rotation_buff[4] =(float) R(1,1);  rotation_buff[7] =(float) R(2,1);
   rotation_buff[2] =(float) R(0,2);  rotation_buff[5] =(float) R(1,2);  rotation_buff[8] =(float) R(2,2);


  vnl_matrix_fixed<double, 3, 3> other_R = other_rot.as_matrix();
   other_rotation_buff[0] =(float) other_R(0,0); other_rotation_buff[3] =(float) other_R(1,0); other_rotation_buff[6] =(float) other_R(2,0);
   other_rotation_buff[1] =(float) other_R(0,1); other_rotation_buff[4] =(float) other_R(1,1); other_rotation_buff[7] =(float) other_R(2,1);
   other_rotation_buff[2] =(float) other_R(0,2); other_rotation_buff[5] =(float) other_R(1,2); other_rotation_buff[8] =(float) other_R(2,2);


   bocl_mem_sptr  rotation_l = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
   good_buffs &=  rotation_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  other_rotation_l = new bocl_mem(device_->context(), other_rotation_buff, sizeof(float)*9, " other_rotation " );
   good_buffs &=  other_rotation_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  offset_l = new bocl_mem(device_->context(), offset_buff, sizeof(float)*4, " offset " );
   good_buffs &=  offset_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  other_offset_l = new bocl_mem(device_->context(), other_offset_buff, sizeof(float)*4, "other offset " );
   good_buffs &=  other_offset_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  vis_l = new bocl_mem(device_->context(), vis, sizeof(float)*6, " vis buff " );
   good_buffs &=  vis_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  mean_app_l = new bocl_mem(device_->context(), app, sizeof(float8)*6, " mean app buff " );
   good_buffs &=  mean_app_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  is_right_cl = new bocl_mem(device_->context(), &is_right_buf, sizeof(char), " mean app buff " );
   good_buffs &=  is_right_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  dt_l = new bocl_mem(device_->context(), &curr_dt , sizeof(float), " dt buff " );
   good_buffs &=  dt_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  max_t_l = new bocl_mem(device_->context(), &max_t_color , sizeof(float), " max t color buff " );
   good_buffs &=  max_t_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );


   std::size_t local_threads[1]={64};
   std::size_t global_threads[1]={1};

    std::vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
    std::vector<boxm2_block_id> blocks_source = orbit.scene()->get_block_ids();
    auto iter_blk_target = blocks_target.begin();
    auto iter_blk_source = blocks_source.begin();

     if(blocks_target.size()!=1||blocks_source.size()!=1)
       return false;

   boxm2_scene_info*    info_buffer_source      = orbit.scene()->get_blk_metadata(*iter_blk_source);
   boxm2_scene_info*    info_buffer_target      = target_scene_->get_blk_metadata(*iter_blk_target);
   auto target_data_size =  (std::size_t) orbit.target_blk_->num_cells();
   info_buffer_target->data_buffer_length = target_data_size;
   bocl_mem_sptr blk_info_target  = new bocl_mem(device_->context(), info_buffer_target, sizeof(boxm2_scene_info), " Scene Info Target" );
  good_buffs &= blk_info_target->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  blk_info_target->write_to_buffer(queue_);


  bocl_mem_sptr blk_info_source  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info Source" );
  std::size_t source_data_size = (int) orbit.blk_->num_cells();
  info_buffer_source->data_buffer_length = source_data_size;
   blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   blk_info_source->write_to_buffer(queue_);

   auto* output = new float[source_data_size];
   bocl_mem_sptr output_cl = new bocl_mem(device_->context(), output, source_data_size * sizeof(float), "Output buf" );
   good_buffs &=  output_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
   if(!good_buffs)
     return false;
//   output_cl->zero_gpu_buffer(queue_);

     global_threads[0] = (unsigned) RoundUp(info_buffer_source->scene_dims[0]*info_buffer_source->scene_dims[1]*info_buffer_source->scene_dims[2],(int)local_threads[0]);


   bocl_mem*  blk_source      = opencl_cache_->get_block(orbit.scene(), *iter_blk_source);
   bocl_mem*  blk_target      = opencl_cache_->get_block(target_scene_,*iter_blk_target);
   orbit.update_target_gpu_buffers(target_scene_, *iter_blk_target);
   orbit.update_source_gpu_buffers();

   bocl_kernel* calc_mean_orbit_app_kern = kernels_[1];
   calc_mean_orbit_app_kern->set_arg(orbit.centerX_.ptr());
   calc_mean_orbit_app_kern->set_arg(orbit.centerY_.ptr());
   calc_mean_orbit_app_kern->set_arg(orbit.centerZ_.ptr());
   calc_mean_orbit_app_kern->set_arg(orbit.lookup_.ptr());
   calc_mean_orbit_app_kern->set_arg(blk_info_target.ptr());
   calc_mean_orbit_app_kern->set_arg(blk_info_source.ptr());
   calc_mean_orbit_app_kern->set_arg(blk_target);
   calc_mean_orbit_app_kern->set_arg(blk_source);
   calc_mean_orbit_app_kern->set_arg(orbit.target_alpha_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.target_color_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.target_vis_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.source_color_base_);
   calc_mean_orbit_app_kern->set_arg(rotation_l.ptr());
   calc_mean_orbit_app_kern->set_arg(other_rotation_l.ptr());
   calc_mean_orbit_app_kern->set_arg(offset_l.ptr());
   calc_mean_orbit_app_kern->set_arg(other_offset_l.ptr());
   calc_mean_orbit_app_kern->set_arg(orbit.sphere_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.iris_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.pupil_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.eyelid_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.eyelid_crease_base_);
   calc_mean_orbit_app_kern->set_arg(orbit.lower_eyelid_base_);
   calc_mean_orbit_app_kern->set_arg(output_cl.ptr());
   calc_mean_orbit_app_kern->set_arg(orbit.eyelid_geo_cl_.ptr());
   calc_mean_orbit_app_kern->set_arg(dt_l.ptr());
   calc_mean_orbit_app_kern->set_arg(max_t_l.ptr());
   calc_mean_orbit_app_kern->set_arg(vis_l.ptr());
   calc_mean_orbit_app_kern->set_arg(mean_app_l.ptr());
   calc_mean_orbit_app_kern->set_arg(is_right_cl.ptr());
   calc_mean_orbit_app_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
   calc_mean_orbit_app_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
   calc_mean_orbit_app_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
   calc_mean_orbit_app_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // reflected local trees source
   calc_mean_orbit_app_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // neighbor trees

   if(!calc_mean_orbit_app_kern->execute(queue_, 1, local_threads, global_threads))
     {
       std::cout<<"Calc Mean Orbit Kernel Failed to Execute "<<std::endl;
       return false;
    }
   vis_l->read_to_buffer(queue_);
   mean_app_l->read_to_buffer(queue_);
   max_t_l->read_to_buffer(queue_);
   std::cout<<" max t color was determined to be " <<max_t_color<< " and t min and t max are  "<< orbit_params.eyelid_tmin_ <<" "<<orbit_params.eyelid_tmax_ << std::endl;
   std::cout<<" dt is "<< -curr_dt<<std::endl;
//   output_cl->read_to_buffer(queue_);
   int status = clFinish(queue_);
   bool good_kern = check_val(status, CL_SUCCESS, "Calc Mean Orbit Kernel Failed: " + error_to_string(status));
   calc_mean_orbit_app_kern->clear_args();

   if(!good_kern)
     return false;
   // for (unsigned i = 0;i < source_data_size ; i++){
   //   if( output[i]!=0)
   //     std::cout<<output[i]<<" ";
   // }

   float8 sclera   = vis[0]  != 0.0f ? app[0]/vis[0] : null; vis_sclera_        += vis[0];
   float8 iris     = vis[1]  != 0.0f ? app[1]/vis[1] : null; vis_iris_          += vis[1];
   float8 pupil    = vis[2]  != 0.0f ? app[2]/vis[2] : null; vis_pupil_         += vis[2];
   float8 u_lid    = vis[3]  != 0.0f ? app[3]/vis[3] : null; vis_upper_lid_     += vis[3];
   float8 l_lid    = vis[4]  != 0.0f ? app[4]/vis[4] : null; vis_lower_lid_     += vis[4];
   float8 l_crease = vis[5]  != 0.0f ? app[5]/vis[5] : null; vis_eyelid_crease_ += vis[5];

   total_sclera_app_       += app[0];          curr_sclera_app        = to_apm_t(sclera);
   total_iris_app_         += app[1];          curr_iris_app          = to_apm_t(iris);
   total_pupil_app_        += app[2];          curr_pupil_app         = to_apm_t(pupil);
   total_upper_lid_app_    += app[3];          curr_upper_lid_app     = to_apm_t(u_lid);
   total_lower_lid_app_    += app[4];          curr_lower_lid_app     = to_apm_t(l_lid);
   total_eyelid_crease_app_+= app[5];          curr_eyelid_crease_app = to_apm_t(l_crease);



   // for(unsigned i = 0; i<6;i++)
   //   std::cout<<vis[i]<<" ";
   // std::cout<<std::endl;
   delete [] output;
   return true;
  }
#endif
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
