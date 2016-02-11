#include "boxm2_vecf_ocl_composite_head_model.h"
// #include "boxm2_vecf_eye_params.h"

boxm2_vecf_ocl_composite_head_model::boxm2_vecf_ocl_composite_head_model( vcl_string const& head_model_path, vcl_string const& eye_model_path,bocl_device_sptr device, boxm2_opencl_cache_sptr opencl_cache)
  : boxm2_vecf_ocl_head_model(head_model_path,device,opencl_cache),
  params_()
{
#ifdef USE_ORBIT_CL
  right_orbit_= boxm2_vecf_ocl_orbit_scene(eye_model_path,device,opencl_cache,false,true);
  left_orbit_= boxm2_vecf_ocl_orbit_scene(eye_model_path,device,opencl_cache,false);
  optimize_= true;
#else
    right_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false,true);
    left_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false);
    optimize_ = false;
#endif
}

bool boxm2_vecf_ocl_composite_head_model::set_params(boxm2_vecf_articulated_params const& params)
{
  try{
    boxm2_vecf_composite_head_parameters const& params_ref = dynamic_cast<boxm2_vecf_composite_head_parameters const &>(params);
    params_ =boxm2_vecf_composite_head_parameters(params_ref);
  }catch(std::exception e){
    vcl_cout<<" Can't downcast to composite head parameters! PARAMATER ASSIGNMENT PHAILED!"<<vcl_endl;
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
  //  vcl_cout << "@@@@@@@@@@@@@@@  clearing and re-mapping head model " << vcl_endl;
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
void boxm2_vecf_ocl_composite_head_model::update_gpu_target(boxm2_scene_sptr target_scene)
{
  // for each block of the target scene
  vcl_vector<boxm2_block_id> target_blocks = target_scene->get_block_ids();

  for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin();
       tblk != target_blocks.end(); ++tblk) {
    bocl_mem* color_app_db =  opencl_cache_->get_data(target_scene, *tblk, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
    bocl_mem* vis_score    =  opencl_cache_->get_data(target_scene, *tblk, boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(color_apm_id_));
    bocl_mem* alpha_db     =  opencl_cache_->get_data(target_scene, *tblk, boxm2_data_traits<BOXM2_ALPHA>::prefix());
    bocl_mem* gray_app_db  =  opencl_cache_->get_data(target_scene, *tblk, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());


    int status;
    cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
    color_app_db->write_to_buffer(queue);
    alpha_db    ->write_to_buffer(queue);
    gray_app_db ->write_to_buffer(queue);
    vis_score   ->write_to_buffer(queue);
    status = clFinish(queue);

  } // for each target block
}
