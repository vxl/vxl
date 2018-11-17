#include "boxm2_vecf_ocl_composite_head_model.h"
// #include "boxm2_vecf_eye_params.h"

    boxm2_vecf_ocl_composite_head_model::boxm2_vecf_ocl_composite_head_model( std::string const& head_model_path, std::string const& eye_model_path,const bocl_device_sptr& device, const boxm2_opencl_cache_sptr& opencl_cache,bool optimize)
    : boxm2_vecf_ocl_head_model(head_model_path,device,opencl_cache,optimize),

  params_()
{
#ifdef USE_ORBIT_CL
  right_orbit_= boxm2_vecf_ocl_orbit_scene(eye_model_path,device,opencl_cache,false,true);
  left_orbit_= boxm2_vecf_ocl_orbit_scene(eye_model_path,device,opencl_cache,false);
#else
    right_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false,true);
    left_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false);
    optimize_ = false;
#endif
}

bool boxm2_vecf_ocl_composite_head_model::set_params(boxm2_vecf_articulated_params const& params)
{
  try{
    auto const& params_ref = dynamic_cast<boxm2_vecf_composite_head_parameters const &>(params);
    params_ =boxm2_vecf_composite_head_parameters(params_ref);
  }catch(std::exception e){
    std::cout<<" Can't downcast to composite head parameters! PARAMATER ASSIGNMENT PHAILED!"<<std::endl;
    return false;
  }

  double y_off_orbit = params_.l_orbit_params_.y_off_;

  double eye_offset_x = 0.0;
  double eye_offset_y = 21; // 31.4
  double eye_offset_z = 44; // 65.7;
  double eye_dist_x = 34;

  eye_offset_x *= params_.head_scale_.x();
  eye_offset_y *= params_.head_scale_.y();
  eye_offset_z *= params_.head_scale_.z();
  eye_dist_x   *= params_.head_scale_.x();

  double   eye_offset_z_left  =  eye_offset_z - (params_.l_orbit_params_.eye_radius_ -12);
  double   eye_offset_z_right =  eye_offset_z - (params_.r_orbit_params_.eye_radius_ -12);

  params_.l_orbit_params_.offset_ = vgl_vector_3d<double>(eye_offset_x + eye_dist_x, eye_offset_y-y_off_orbit, eye_offset_z_left);
  params_.r_orbit_params_.offset_ = vgl_vector_3d<double>(eye_offset_x + -eye_dist_x, eye_offset_y-y_off_orbit, eye_offset_z_right);

  //if the intrinsic eye parameters changed, clear target
  if(left_orbit_.vfield_params_change_check(params_.l_orbit_params_) || right_orbit_.vfield_params_change_check(params_.r_orbit_params_) ){
    boxm2_vecf_ocl_head_model::set_intrinsic_change(true);
    }
  //finally, propagate the params to each orbit
  left_orbit_. set_params( params_.l_orbit_params_);
  right_orbit_.set_params( params_.r_orbit_params_);
  this->set_scale( params_.head_scale_ );
  return true;
}


void boxm2_vecf_ocl_composite_head_model::map_to_target(boxm2_scene_sptr target)
{
  //  std::cout << "@@@@@@@@@@@@@@@  clearing and re-mapping head model " << std::endl;
  // clear target

  if (boxm2_vecf_ocl_head_model::intrinsic_change_){
    this->clear_target(target);
  // head model
    boxm2_vecf_ocl_head_model::map_to_target(target);
  }
  //orbit model
  right_orbit_.map_to_target(target);
  left_orbit_.map_to_target (target);
#ifndef USE_ORBIT_CL
  this->update_gpu_target   (target);
#endif
}
void boxm2_vecf_ocl_composite_head_model::update_gpu_target(const boxm2_scene_sptr& target_scene)
{
  // for each block of the target scene
  std::vector<boxm2_block_id> target_blocks = target_scene->get_block_ids();

  for (auto & target_block : target_blocks) {
    bocl_mem* color_app_db =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
    bocl_mem* vis_score    =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(color_apm_id_));
    bocl_mem* alpha_db     =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_ALPHA>::prefix());
    bocl_mem* gray_app_db  =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());


    int status;
    cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
    color_app_db->write_to_buffer(queue);
    alpha_db    ->write_to_buffer(queue);
    gray_app_db ->write_to_buffer(queue);
    vis_score   ->write_to_buffer(queue);
    status = clFinish(queue);

  } // for each target block
}
// void boxm2_vecf_ocl_composite_head_model::inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) {
// }
// int boxm2_vecf_ocl_composite_head_model::prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) {
//   return true;
// }
// bool boxm2_vecf_ocl_composite_head_model::inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) {
//   return true;
// }
// bool boxm2_vecf_ocl_composite_head_model::apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) {
//   return true;
// }
