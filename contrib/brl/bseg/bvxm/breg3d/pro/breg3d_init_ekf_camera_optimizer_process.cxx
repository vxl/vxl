#include "breg3d_init_ekf_camera_optimizer_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <breg3d/breg3d_ekf_camera_optimizer_state.h>

breg3d_init_ekf_camera_optimizer_process::breg3d_init_ekf_camera_optimizer_process()
{
  // process takes 1 input:
  //input[0]: The first estimated camera (eg GPS/INS reading)
  // camera should be of type vgpl_perspective_camera
  input_data_.resize(1,brdb_value_sptr(nullptr));
  input_types_.resize(1);
  input_types_[0] = "vpgl_camera_double_sptr";

  // process has 1 output:
  // output[0]: The initial state estimate for the kalman filter
  output_data_.resize(1,brdb_value_sptr(nullptr));
  output_types_.resize(1);
  output_types_[0]= "breg3d_ekf_camera_optimizer_state";

  // parameters
  // default corresponds to roughly 1 degree std deviation
  if (!parameters()->add("Translation Scale Factor", "translation_scale", 0.005))
    std::cerr << "ERROR: Adding parameters in " << __FILE__ << '\n';

  // default corresponds to roughly 1 degree std deviation
  if (!parameters()->add("Rotation Measurement Variance", "rotation_measurement_variance", 3e-4))
    std::cerr << "ERROR: Adding parameters in " << __FILE__ << '\n';

  // default corresponds to roughly 0.5 meter std deviation
  if (!parameters()->add("Position Measurement Variance", "position_measurement_variance", 0.25))
    std::cerr << "ERROR: Adding parameters in " << __FILE__ << '\n';
}


bool breg3d_init_ekf_camera_optimizer_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  auto* input0 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[0].ptr());

  vpgl_perspective_camera<double> *cam0;
  if (!(cam0 = dynamic_cast<vpgl_perspective_camera<double>*>(input0->value().ptr()))) {
    std::cerr << "error: process expects camera to be a vpgl_perspective_camera." << '\n';
    return false;
  }

  // get parameters
  double rot_var=0.0, pos_var=0.0, t_scale=0.0; // dummy initialisations to avoid compiler warnings
  if (!parameters()->get_value(std::string("position_measurement_variance"), rot_var)) {
    std::cout << "breg3d_init_ekf_camera_optimizer_process::execute() -- problem in retrieving parameter rotation_variance\n";
    return false;
  }
  if (!parameters()->get_value(std::string("position_measurement_variance"), pos_var)) {
    std::cout << "breg3d_init_ekf_camera_optimizer_process::execute() -- problem in retrieving parameter position_variance\n";
    return false;
  }
  if (!parameters()->get_value(std::string("translation_scale"), t_scale)) {
    std::cout << "breg3d_init_ekf_camera_optimizer_process::execute() -- problem in retrieving parameter translation_scale\n";
    return false;
  }

  breg3d_ekf_camera_optimizer_state init_state(t_scale,cam0->camera_center(),cam0->get_rotation(),pos_var,rot_var);

  //store output
  brdb_value_sptr output0 = new brdb_value_t<breg3d_ekf_camera_optimizer_state>(init_state);
  output_data_[0] = output0;

  return true;
}
