#include "boxm2_vecf_composite_head_model.h"
#include "boxm2_vecf_eye_params.h"


boxm2_vecf_composite_head_model::boxm2_vecf_composite_head_model( vcl_string const& head_model_path, vcl_string const& eye_model_path)
  : head_(head_model_path),
  params_()
{
  use_orbit = true;
  if(use_orbit){
    right_orbit_= boxm2_vecf_orbit_scene(eye_model_path,true);
    left_orbit_= boxm2_vecf_orbit_scene(eye_model_path,true);
  }else{
    right_eye_ = boxm2_vecf_eye_scene(eye_model_path, true);
    left_eye_ = boxm2_vecf_eye_scene(eye_model_path, true);}

  set_parameters(params_);
}

void boxm2_vecf_composite_head_model::set_parameters(boxm2_vecf_composite_head_parameters const& params)
{
  params_ = params;

  double eye_offset_x = 0.0;
  double eye_offset_y = 21; // 31.4
  double eye_offset_z = 43.5; // 65.7;
  double eye_dist_x = 35;

  eye_offset_x *= params_.head_scale_.x();
  eye_offset_y *= params_.head_scale_.y();
  eye_offset_z *= params_.head_scale_.z();
  eye_dist_x *= params_.head_scale_.x();

  if(!use_orbit){
    boxm2_vecf_eye_params leye_params, reye_params;
    leye_params.offset_ = vgl_vector_3d<double>(eye_offset_x + eye_dist_x, eye_offset_y, eye_offset_z);
    reye_params.offset_ = vgl_vector_3d<double>(eye_offset_x + -eye_dist_x, eye_offset_y, eye_offset_z);
    leye_params.eye_pointing_dir_ = params.look_dir_;
    reye_params.eye_pointing_dir_ = params.look_dir_;
    right_eye_.set_params(reye_params);
    left_eye_.set_params(leye_params);
  }else{
    boxm2_vecf_orbit_params lorbit_params,rorbit_params;
    lorbit_params.offset_ = vgl_vector_3d<double>(eye_offset_x + eye_dist_x,eye_offset_y, eye_offset_z);
    rorbit_params.offset_ = vgl_vector_3d<double>(eye_offset_x + -eye_dist_x, eye_offset_y, eye_offset_z);
    lorbit_params.eye_pointing_dir_ = params.look_dir_;
    rorbit_params.eye_pointing_dir_ = params.look_dir_;
    rorbit_params.eyelid_dt_ = 0.9 ;
    lorbit_params.eyelid_dt_ = 0.9 ;
    left_orbit_.set_params(lorbit_params);
    right_orbit_.set_params(rorbit_params);
  }

  head_.set_scale( params_.head_scale_ );

}


bool boxm2_vecf_composite_head_model::map_to_target(boxm2_scene_sptr target, vcl_string const& app_id, bool eyes_only)
{
  if (!eyes_only) {
    vcl_cout << "@@@@@@@@@@@@@@@  clearing and re-mapping head model " << vcl_endl;
    // clear target
    head_.clear_target(target);
    // head model
    head_.map_to_target(target);
  }

  if(!use_orbit){
    right_eye_.map_to_target(target, app_id);
    left_eye_.map_to_target(target, app_id);
  }else{
    right_orbit_.map_to_target(target);
    left_orbit_.map_to_target(target);
  }

  return true;
}
