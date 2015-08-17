#include "boxm2_vecf_composite_head_model.h"
// #include "boxm2_vecf_eye_params.h"

boxm2_vecf_composite_head_model::boxm2_vecf_composite_head_model( vcl_string const& head_model_path, vcl_string const& eye_model_path)
  : boxm2_vecf_head_model(head_model_path),
  params_()
{
    right_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false,true);
    left_orbit_= boxm2_vecf_orbit_scene(eye_model_path,false);
}

bool boxm2_vecf_composite_head_model::set_params(boxm2_vecf_articulated_params const& params)
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

  params_.l_orbit_params_.offset_ = vgl_vector_3d<double>(eye_offset_x + eye_dist_x, eye_offset_y-y_off_orbit, eye_offset_z);
  params_.r_orbit_params_.offset_ = vgl_vector_3d<double>(eye_offset_x + -eye_dist_x, eye_offset_y-y_off_orbit, eye_offset_z);

  // double conic_depth = params_.conic_depth_;
  // const double eye_ang =21 * 3.141/180;
  // double eye_dist_x= conic_depth * sin(eye_ang) * params_.head_scale_.x();
  // double eye_dist_z= conic_depth * cos(eye_ang) * params_.head_scale_.z();
  // double scaled_conic_depth = sqrt(eye_dist_x * eye_dist_x + eye_dist_z * eye_dist_z);

  // vgl_point_3d<double> skull_center(0.0 , 21 , -38 );
  // double R = params_.l_orbit_params_.eye_radius_;

  // vgl_vector_3d<double> dir_r(-sin(eye_ang) , 0 , cos(eye_ang));
  // vgl_vector_3d<double> dir_l( sin(eye_ang) , 0 , cos(eye_ang));
  // vgl_point_3d<double> l_pos = skull_center + dir_l * scaled_conic_depth;
  // vgl_point_3d<double> r_pos = skull_center + dir_r * scaled_conic_depth;
  // params_.l_orbit_params_.offset_ = vgl_vector_3d<double>(l_pos.x(),l_pos.y() ,l_pos.z());
  // params_.r_orbit_params_.offset_ = vgl_vector_3d<double>(r_pos.x(),r_pos.y() ,r_pos.z());

  //currently fixed to the head look direction. This might change
  params_.l_orbit_params_.eye_pointing_dir_ = params_.look_dir_;
  params_.r_orbit_params_.eye_pointing_dir_ = params_.look_dir_; //assuming the person is not lazy eyed.

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
  //  vcl_cout << "@@@@@@@@@@@@@@@  clearing and re-mapping head model " << vcl_endl;
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
