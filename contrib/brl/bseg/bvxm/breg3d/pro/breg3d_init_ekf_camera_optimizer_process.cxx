#include "breg3d_init_ekf_camera_optimizer_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_interpolate.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>


#include <breg3d/breg3d_ekf_camera_optimizer_state.h>

breg3d_init_ekf_camera_optimizer_process::breg3d_init_ekf_camera_optimizer_process()
{
  // process takes 1 inputs: 
  //input[0]: The first estimated camera (eg GPS/INS reading) 
  // camera should be of type vgpl_perspective_camera
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0] = "vpgl_camera_double_sptr";

  // process has 1 output:
  // output[0]: The initial state estimate for the kalman filter
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "breg3d_ekf_camera_optimizer_state";

  // parameters
  // default corresponds to roughly 1 degree std deviation
  if (!parameters()->add("Translation Scale Factor", "translation_scale", 0.005))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  // default corresponds to roughly 1 degree std deviation
  if (!parameters()->add("Rotation Measurement Variance", "rotation_measurement_variance", 3e-4))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  // default corresponds to roughly 0.5 meter std deviation
  if (!parameters()->add("Position Measurement Variance", "position_measurement_variance", 0.25))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

}


bool breg3d_init_ekf_camera_optimizer_process::execute()
{

  // Sanity check
  if(!this->verify_inputs())
    return false;

  brdb_value_t<vpgl_camera_double_sptr>* input0 = 
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[0].ptr());


  vpgl_perspective_camera<double> *cam0;
  if (!(cam0 = dynamic_cast<vpgl_perspective_camera<double>*>(input0->value().ptr()))) {
       vcl_cerr << "error: process expects camera to be a vpgl_perspective_camera." << vcl_endl;
      return false;
  }
  
  // get parameters
  double rot_var, pos_var, t_scale;
  if (!parameters()->get_value(vcl_string("position_measurement_variance"), rot_var)) {
    vcl_cout << "breg3d_init_ekf_camera_optimizer_process::execute() -- problem in retrieving parameter rotation_variance\n";
    return false;
  }
  if (!parameters()->get_value(vcl_string("position_measurement_variance"), pos_var)) {
    vcl_cout << "breg3d_init_ekf_camera_optimizer_process::execute() -- problem in retrieving parameter position_variance\n";
    return false;
  }
    if (!parameters()->get_value(vcl_string("translation_scale"), t_scale)) {
    vcl_cout << "breg3d_init_ekf_camera_optimizer_process::execute() -- problem in retrieving parameter translation_scale\n";
    return false;
  }

  breg3d_ekf_camera_optimizer_state init_state(t_scale,cam0->camera_center(),cam0->get_rotation(),pos_var,rot_var);
  
  //store output
  brdb_value_sptr output0 = 
    new brdb_value_t<breg3d_ekf_camera_optimizer_state>(init_state);
  output_data_[0] = output0;

  return true;
}



