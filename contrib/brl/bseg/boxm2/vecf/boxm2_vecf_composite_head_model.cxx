#include "boxm2_vecf_composite_head_model.h"
// #include "boxm2_vecf_eye_params.h"

boxm2_vecf_composite_head_model::boxm2_vecf_composite_head_model( std::string const& head_model_path, std::string const& eye_model_path)
  : boxm2_vecf_head_model(head_model_path),
  params_()
{
    right_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false,true);
    left_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false);
}

bool boxm2_vecf_composite_head_model::set_params(boxm2_vecf_articulated_params const& params)
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
    boxm2_vecf_head_model::set_intrinsic_change(true);
    }
  //finally, propagate the params to each orbit
  left_orbit_. set_params( params_.l_orbit_params_);
  right_orbit_.set_params( params_.r_orbit_params_);
  this->set_scale( params_.head_scale_ );
  return true;
}


void boxm2_vecf_composite_head_model::map_to_target(boxm2_scene_sptr target)
{
  //  std::cout << "@@@@@@@@@@@@@@@  clearing and re-mapping head model " << std::endl;
  // clear target

  if (boxm2_vecf_head_model::intrinsic_change_){
    this->clear_target(target);
  // head model
    boxm2_vecf_head_model::map_to_target(target);
  }
  //orbit model
  right_orbit_.map_to_target(target);
  left_orbit_.map_to_target(target);

}
