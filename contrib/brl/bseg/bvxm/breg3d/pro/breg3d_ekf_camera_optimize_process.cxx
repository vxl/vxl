#include "breg3d_ekf_camera_optimize_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>

#include <vnl/vnl_matrix.h>


#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <breg3d/breg3d_ekf_camera_optimizer.h>
#include <breg3d/breg3d_ekf_camera_optimizer_state.h>


breg3d_ekf_camera_optimize_process::breg3d_ekf_camera_optimize_process()
{
  // process takes 7 inputs: 
  //input[0]: The previous state estimate
  //input[1]: The previous image
  //input[2]: The current image
  //input[3]: The current camera estimate (eg from GPS/INS)
  //input[4]: The voxel world
  //input[5]: The appearance model type to use for expected images
  //input[6]: The bin index to use for expected images
  input_data_.resize(7,brdb_value_sptr(0));
  input_types_.resize(7);
  input_types_[0] = "breg3d_ekf_camera_optimizer_state";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "bvxm_voxel_world_sptr";
  input_types_[5] = "vcl_string";
  input_types_[6] = "unsigned";

  // process has 2 outputs:
  // output[0]: The estimate for the current state
  // output[1]: The optimized camera
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "breg3d_ekf_camera_optimizer_state";
  output_types_[1] = "vpgl_camera_double_sptr";

  // parameters
  // default corresponds to roughly 1 degree std deviation
  if (!parameters()->add("Rotation Prediction Variance", "rotation_prediction_variance", 0.2))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  // default corresponds to roughly 1.4 meter std deviation
  if (!parameters()->add("Position Prediction Variance", "position_prediction_variance", 3.0))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

    // default corresponds to roughly 1 degree std deviation
  if (!parameters()->add("Rotation Measurement Variance", "rotation_measurement_variance", 3e-4))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  // default corresponds to roughly 1.0 meter std deviation
  if (!parameters()->add("Position Measurement Variance", "position_measurement_variance", 1.0))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

    // 
  if (!parameters()->add("Homography terms Variance", "homography_term_variance", 0.08))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

      // 
  if (!parameters()->add("Homography translation terms Variance", "homography_translation_term_variance", 0.08))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;


}


bool breg3d_ekf_camera_optimize_process::execute()
{

  // Sanity check
  if(!this->verify_inputs())
    return false;

  brdb_value_t<breg3d_ekf_camera_optimizer_state>* input0 = 
      static_cast<brdb_value_t<breg3d_ekf_camera_optimizer_state>* >(input_data_[0].ptr());

  brdb_value_t<vil_image_view_base_sptr>* input1 = 
      static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());

    brdb_value_t<vil_image_view_base_sptr>* input2 = 
      static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());

  brdb_value_t<vpgl_camera_double_sptr>* input3 = 
      static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[3].ptr());

  brdb_value_t<bvxm_voxel_world_sptr>* input4 = 
      static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[4].ptr());

  brdb_value_t<vcl_string>* input5 = 
      static_cast<brdb_value_t<vcl_string>* >(input_data_[5].ptr());

  brdb_value_t<unsigned>* input6 = 
      static_cast<brdb_value_t<unsigned>* >(input_data_[6].ptr());

  // get previous state
  breg3d_ekf_camera_optimizer_state prev_state = input0->value();
  // get previous image
  vil_image_view_base_sptr prev_img = input1->value();
  // get current image
  vil_image_view_base_sptr curr_img = input2->value();
  // get current camera measurement
  vpgl_perspective_camera<double> *cam_in = dynamic_cast<vpgl_perspective_camera<double>*>(input3->value().ptr());
  // copy so original doesnt get modified by optimizer
  vpgl_camera_double_sptr cam_est = new vpgl_perspective_camera<double>(*cam_in);
  // get voxel world
  bvxm_voxel_world_sptr vox_world = input4->value();
  // get appearance model type
  vcl_string apm_type = input5->value();
  // get bin index
  unsigned bin_idx = input6->value();

  // get parameters
  double rot_var_measure, pos_var_measure;
  if (!parameters()->get_value(vcl_string("position_measurement_variance"), rot_var_measure)) {
    vcl_cout << "breg3d_init_ekf_camera_optimize_process::execute() -- problem in retrieving parameter rotation_variance\n";
    return false;
  }
  if (!parameters()->get_value(vcl_string("position_measurement_variance"), pos_var_measure)) {
    vcl_cout << "breg3d_init_ekf_camera_optimize_process::execute() -- problem in retrieving parameter position_variance\n";
    return false;
  }
  double rot_var_predict, pos_var_predict;
  if (!parameters()->get_value(vcl_string("position_prediction_variance"), rot_var_predict)) {
    vcl_cout << "breg3d_init_ekf_camera_optimize_process::execute() -- problem in retrieving parameter rotation_variance\n";
    return false;
  }
  if (!parameters()->get_value(vcl_string("position_prediction_variance"), pos_var_predict)) {
    vcl_cout << "breg3d_init_ekf_camera_optimize_process::execute() -- problem in retrieving parameter position_variance\n";
    return false;
  }
  double homography_var, homography_t_var;
  if (!parameters()->get_value(vcl_string("homography_term_variance"), homography_var)) {
    vcl_cout << "breg3d_init_ekf_camera_optimize_process::execute() -- problem in retrieving parameter homography_variance\n";
    return false;
  }
  if (!parameters()->get_value(vcl_string("homography_translation_term_variance"), homography_t_var)) {
    vcl_cout << "breg3d_init_ekf_camera_optimize_process::execute() -- problem in retrieving parameter homography_variance\n";
    return false;
  }


  breg3d_ekf_camera_optimizer optimizer(pos_var_predict,rot_var_predict,
                                      pos_var_measure,rot_var_measure,
                                      homography_var,homography_t_var,false,true,false);
  

  bvxm_image_metadata curr_metadata(curr_img,cam_est);
  breg3d_ekf_camera_optimizer_state curr_state = optimizer.optimize(vox_world,prev_img,curr_metadata,prev_state,apm_type);

  //store output
  brdb_value_sptr output0 = 
    new brdb_value_t<breg3d_ekf_camera_optimizer_state>(curr_state);
  output_data_[0] = output0;

  brdb_value_sptr output1 = 
    new brdb_value_t<vpgl_camera_double_sptr>(curr_metadata.camera);
  output_data_[1] = output1;

  return true;
}


