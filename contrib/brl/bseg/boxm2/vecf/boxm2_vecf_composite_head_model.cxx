#include "boxm2_vecf_composite_head_model.h"
#include "boxm2_vecf_eye_params.h"


boxm2_vecf_composite_head_model::boxm2_vecf_composite_head_model( vcl_string const& head_model_path, vcl_string const& eye_model_path)
: right_eye_(eye_model_path, false), left_eye_(eye_model_path, false), head_(head_model_path),
  params_()
{
  set_parameters(params_);
}

void boxm2_vecf_composite_head_model::set_parameters(boxm2_vecf_composite_head_parameters const& params)
{
  params_ = params;

  boxm2_vecf_eye_params leye_params, reye_params;
  double eye_offset_x = 0.0;
  double eye_offset_y = 21; // 31.4
  double eye_offset_z = 40; // 65.7;
  double eye_dist_x = 35.0;

  eye_offset_x *= params_.head_scale_.x();
  eye_offset_y *= params_.head_scale_.y();
  eye_offset_z *= params_.head_scale_.z();
  eye_dist_x *= params_.head_scale_.x();

  leye_params.offset_ = vgl_vector_3d<double>(eye_offset_x + eye_dist_x, eye_offset_y, eye_offset_z);
  reye_params.offset_ = vgl_vector_3d<double>(eye_offset_x + -eye_dist_x, eye_offset_y, eye_offset_z);

  leye_params.eye_pointing_dir_ = params.look_dir_;
  reye_params.eye_pointing_dir_ = params.look_dir_;

  head_.set_scale( params_.head_scale_ );
  right_eye_.set_params(reye_params);
  left_eye_.set_params(leye_params);
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
  // right eye model
  right_eye_.map_to_target(target, app_id);
  // left eye model
  left_eye_.map_to_target(target, app_id);

  return true;
}
