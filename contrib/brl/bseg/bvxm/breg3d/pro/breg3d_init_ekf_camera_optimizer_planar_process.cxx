#include "breg3d_init_ekf_camera_optimizer_planar_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_plane_3d.h>

#include <bvxm/bvxm_voxel_world.h>

#include <breg3d/breg3d_ekf_camera_optimizer_state.h>
#include <breg3d/breg3d_homography_generator.h>
#include <breg3d/breg3d_lm_direct_homography_generator.h>

breg3d_init_ekf_camera_optimizer_planar_process::breg3d_init_ekf_camera_optimizer_planar_process()
{
  // process takes 4 inputs:
  //input[0]: The first estimated camera (should be of type vgpl_perspective_camera)
  //input[1]: The first frame
  //input[2]: The second frame
  //input[3]: The voxel world
  input_data_.resize(4,brdb_value_sptr(nullptr));
  input_types_.resize(4);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "bvxm_voxel_world_sptr";

  // process has 6 outputs:
  // output[0]: The initial state estimate for the kalman filter
  // output[1]: The second camera's estimated position
  // outputs[2-5]: The plane parameters a,b,c,d  (ax + by + cz + d = 0)
  output_data_.resize(6,brdb_value_sptr(nullptr));
  output_types_.resize(6);
  output_types_[0] = "breg3d_ekf_camera_optimizer_state";
  output_types_[1] = "vpgl_camera_double_sptr";
  output_types_[2] = "double";
  output_types_[3] = "double";
  output_types_[4] = "double";
  output_types_[5] = "double";

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


bool breg3d_init_ekf_camera_optimizer_planar_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  auto* input0 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[0].ptr());

  auto* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());

  auto* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());

  auto* input3 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[3].ptr());

  // get first camera
  vpgl_perspective_camera<double> *cam0;
  if (!(cam0 = dynamic_cast<vpgl_perspective_camera<double>*>(input0->value().ptr()))) {
       std::cerr << "error: process expects camera to be a vpgl_perspective_camera.\n";
      return false;
  }

  // get first and second images
  vil_image_view_base_sptr img0 = input1->value();
  vil_image_view_base_sptr img1 = input2->value();
  // get voxel world
  bvxm_voxel_world_sptr vox_world = input3->value();

  // get parameters
  double rot_var=0.0, pos_var=0.0, t_scale=0.0; // dummy initialisations to avoid compiler warnings
  if (!parameters()->get_value(std::string("position_measurement_variance"), rot_var)) {
    std::cout << "breg3d_init_ekf_camera_optimizer_planar_process::execute() -- problem in retrieving parameter rotation_variance\n";
    return false;
  }
  if (!parameters()->get_value(std::string("position_measurement_variance"), pos_var)) {
    std::cout << "breg3d_init_ekf_camera_optimizer_planar_process::execute() -- problem in retrieving parameter position_variance\n";
    return false;
  }

  if (!parameters()->get_value(std::string("translation_scale"), t_scale)) {
    std::cout << "breg3d_init_ekf_camera_optimizer_planar_process::execute() -- problem in retrieving parameter translation_scale\n";
    return false;
  }

  // compute homography from img0 to img1
  breg3d_homography_generator *h_gen = new breg3d_lm_direct_homography_generator();
  float dummy = 0.0f;
  vil_image_view_base_sptr img0_float_sptr = vil_convert_cast(dummy,img0);
  vil_image_view_base_sptr img1_float_sptr = vil_convert_cast(dummy,img1);
  auto* img0_float = dynamic_cast<vil_image_view<float>*>(img0_float_sptr.ptr());
  auto* img1_float = dynamic_cast<vil_image_view<float>*>(img1_float_sptr.ptr());

  h_gen->set_image0(img0_float);
  h_gen->set_image1(img1_float);
  ihog_transform_2d H = h_gen->compute_homography();

  std::cout << "H =\n" << H.get_matrix() << std::endl;

  // TEMP
  auto *cam1 = new vpgl_perspective_camera<double>(cam0->get_calibration(),cam0->get_camera_center(),cam0->get_rotation());

  // compute position of plane and camera1
  vgl_plane_3d<double> plane_est = vox_world->fit_plane();

  std::cout << "plane est = " << plane_est << std::endl;

  breg3d_ekf_camera_optimizer_state init_state(t_scale,cam0->camera_center(),cam0->get_rotation(),pos_var,rot_var);

  //store output
  // initial state
  brdb_value_sptr output0 =
    new brdb_value_t<breg3d_ekf_camera_optimizer_state>(init_state);
  output_data_[0] = output0;

  // camera1 estimate
  brdb_value_sptr output1 =
    new brdb_value_t<vpgl_camera_double_sptr>(cam1);
  output_data_[1] = output1;

  // plane estimate
  brdb_value_sptr output2 =
    new brdb_value_t<double>(plane_est.a());
  output_data_[2] = output2;
  brdb_value_sptr output3 =
    new brdb_value_t<double>(plane_est.b());
  output_data_[3] = output3;
  brdb_value_sptr output4 =
    new brdb_value_t<double>(plane_est.c());
  output_data_[4] = output4;
  brdb_value_sptr output5 =
    new brdb_value_t<double>(plane_est.d());
  output_data_[5] = output5;

  return true;
}
