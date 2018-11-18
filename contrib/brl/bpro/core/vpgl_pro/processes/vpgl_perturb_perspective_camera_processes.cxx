// This is brl/bpro/core/vpgl_pro/processes/vpgl_perturb_perspective_camera_processes.cxx
#include <iostream>
#include <limits>
#include <fstream>
#include <ctime>
#include <cmath>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vgl/algo/vgl_p_matrix.h>
#include <bsta/bsta_random_wrapper.h>
#if 0
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

struct ang_pair {
  double theta;
  double phi;
};

//: generate samples from a 3-d von mises distribution.
// The mean vector is along the z axis.
ang_pair sample_3d(double kappa)
{
  vnl_random rng(std::clock());
  ang_pair ap;
  ap.phi = vnl_math::twopi*rng.drand32();
  double p = rng.drand32();
  double exk = std::exp(2.0*kappa);
  //double exk = std::exp(2.0*kappa);
  double t1 = exk-1.0;
  double t2 =exk/t1;
  double t3 = std::log(t1*(t2-p));
  double temp_sum = -kappa + t3;
  double ratio = temp_sum/kappa;
  ap.theta = std::acos(ratio);
  return ap;
}

//: perturb the orientation of the given camera randomly in a cone around its pointing direction by the given angle
namespace vpgl_perturb_persp_cam_orient_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 3;
}

//: Init function
bool vpgl_perturb_persp_cam_orient_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_perturb_persp_cam_orient_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "float";  // kappa value, its standard deviation if normal distribution is used
  input_types_[2] = "bsta_random_wrapper_sptr";

  // process has 3 outputs
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr"; // perturbed cam
  output_types_[1] = "float";  // theta
  output_types_[2] = "float";  // phi

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_perturb_persp_cam_orient_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_get_view_direction_at_point_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto kappa = pro.get_input<float>(i++);
  bsta_random_wrapper_sptr rngw = pro.get_input<bsta_random_wrapper_sptr>(i++);

  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_get_view_direction_at_point_process: couldn't cast camera\n";
    return false;
  }
# if 0
  // sample using von mises distribution
  ang_pair ap = sample_3d(kappa);
#endif
  ang_pair ap;
  // first angle is sample within the radius of the cone of uncertainty, input to the process
  ap.theta = rngw->rng_.normal64()*(kappa/180.0*vnl_math::pi);
  // normalize the distribution by throwing away samples that are out of valid bounds and resampling
  while (ap.theta > vnl_math::pi || ap.theta < -vnl_math::pi) {
    ap.theta = rngw->rng_.normal64()*(kappa/180.0*vnl_math::pi);
    std::cout << "..." << std::endl;
  }
  // second angle is the amount of turn around z axis (up vector of camera is y axis)
  ap.phi = (rngw->rng_.drand64(0.0, 360.0)/180.0)*vnl_math::pi;

  std::cout << "sampled theta: " << 180.0*(ap.theta/vnl_math::pi) << " phi: " << 180.0*(ap.phi/vnl_math::pi) << std::endl;

  vgl_vector_3d<double> y_vec(0.0, 1.0, 0.0);
  vgl_rotation_3d<double> R2(0.0, 0.0, ap.phi);
  // first rotate up vector by phi
  vgl_vector_3d<double> y_vec_r = R2*y_vec;
  vnl_vector_fixed<double,3> v(y_vec_r.x(), y_vec_r.y(), y_vec_r.z());
  // then rotate axis by theta around up vector
  vnl_quaternion<double> q(v,ap.theta);
  vgl_rotation_3d<double> composed_from_q(q);

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  out_cam.set_rotation(composed_from_q*cam->get_rotation());
  out_cam.set_camera_center(cam->get_camera_center());
# if 0
  // calculate delta_x on ground
  double h = cam->get_camera_center().z();  // actually ground plane of the scene is not necessarily z = 0
  double tan_gamma = cam->get_camera_center().x()/h;
  std::cout << " cam h: " << h << " x: " << cam->get_camera_center().x() << " tan_gamma: " << tan_gamma << std::endl;
  double theta = kappa/180.0*vnl_math::pi;
  double tan_theta = std::tan(theta);
  std::cout << "theta in deg: " << kappa << " in rad: " << theta << " tan_theta: " << tan_theta << std::endl;
  double h_in_cm = 30400;
  double delta_x = h_in_cm*(std::pow(tan_gamma, 2.0)*tan_theta+tan_theta)/(1+tan_gamma*tan_theta);
  std::cout << "delta_x with theta: " << kappa << " at height: " << h << " is: " << delta_x << " cm." << std::endl;

  tan_theta = std::tan(ap.theta);
  delta_x = h_in_cm*(std::pow(tan_gamma, 2.0)*tan_theta+tan_theta)/(1+tan_gamma*tan_theta);
  std::cout << "delta_x with sampled theta: " << ap.theta << " at height: " << h << " is: " << delta_x << " cm." << std::endl;
#endif
  pro.set_output_val<vpgl_camera_double_sptr>(0, new vpgl_perspective_camera<double>(out_cam));
  pro.set_output_val<float>(1,float(ap.theta));
  pro.set_output_val<float>(2,float(ap.phi));
  return true;
}


//: perturb the orientation of the given camera by the given angles in a cone around its pointing direction
namespace vpgl_rotate_persp_cam_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

//: Init function
bool vpgl_rotate_persp_cam_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_rotate_persp_cam_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "float";  // theta, rotation aroun y axis
  input_types_[2] = "float";  // phi, rotation around z axis

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr"; // rotated cam

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_rotate_persp_cam_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_get_view_direction_at_point_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  ang_pair ap;
  ap.theta = pro.get_input<float>(i++);
  ap.phi = pro.get_input<float>(i++);

  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_get_view_direction_at_point_process: couldn't cast camera\n";
    return false;
  }

  vgl_vector_3d<double> y_vec(0.0, 1.0, 0.0);
  vgl_rotation_3d<double> R2(0.0, 0.0, ap.phi);
  // first rotate up vector by phi
  vgl_vector_3d<double> y_vec_r = R2*y_vec;
  vnl_vector_fixed<double,3> v(y_vec_r.x(), y_vec_r.y(), y_vec_r.z());
  // then rotate axis by theta around up vector
  vnl_quaternion<double> q(v,ap.theta);
  vgl_rotation_3d<double> composed_from_q(q);

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  out_cam.set_rotation(composed_from_q*cam->get_rotation());
  out_cam.set_camera_center(cam->get_camera_center());

  pro.set_output_val<vpgl_camera_double_sptr>(0, new vpgl_perspective_camera<double>(out_cam));
  return true;
}

// perturb the orientation of the given camera randomly in a cone around it's pointing direction by the given angle
namespace vpgl_perturb_loc_persp_cam_orient_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

//: Init function
bool vpgl_perturb_loc_persp_cam_orient_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_perturb_loc_persp_cam_orient_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "float";  // variance of normal distribution
  input_types_[2] = "bsta_random_wrapper_sptr";

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr"; // perturbed cam

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_perturb_persp_loc_cam_orient_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_get_view_direction_at_point_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto var = pro.get_input<float>(i++);
  bsta_random_wrapper_sptr rngw = pro.get_input<bsta_random_wrapper_sptr>(i++);

  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_get_view_direction_at_point_process: couldn't cast camera\n";
    return false;
  }
  double x = rngw->rng_.normal64()*var;
  double y = rngw->rng_.normal64()*var;
  double z = rngw->rng_.normal64()*var;

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  out_cam.set_rotation(cam->get_rotation());
  vgl_point_3d<double> cent = cam->get_camera_center();
  vgl_point_3d<double> cent2(cent.x() + x, cent.y() + y, cent.z() + z);
  out_cam.set_camera_center(cent2);

  pro.set_output_val<vpgl_camera_double_sptr>(0, new vpgl_perspective_camera<double>(out_cam));
  return true;
}

// perturb the orientation of the given camera randomly in a cone around it's pointing direction by the given angle
// sample the angle in the cone uniformly as opposed to normally in vpgl_perturb_persp_cam_orient_process
namespace vpgl_perturb_uniform_persp_cam_orient_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 3;
}

//: Init function
bool vpgl_perturb_uniform_persp_cam_orient_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_perturb_uniform_persp_cam_orient_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "float";  // range of uniform distribution for the angle
  input_types_[2] = "bsta_random_wrapper_sptr";

  // process has 3 outputs
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr"; // perturbed cam
  output_types_[1] = "float";  // theta
  output_types_[2] = "float";  // phi

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool vpgl_perturb_uniform_persp_cam_orient_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_get_view_direction_at_point_process: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i=0;
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto kappa = pro.get_input<float>(i++);
  bsta_random_wrapper_sptr rngw = pro.get_input<bsta_random_wrapper_sptr>(i++);

  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_get_view_direction_at_point_process: couldn't cast camera\n";
    return false;
  }

  ang_pair ap;
  // first angle is sample within the radius of the cone of uncertainty, input to the process
  ap.theta = (rngw->rng_.drand64(0.0, kappa)/180.0)*vnl_math::pi;
  // second angle is the amount of turn around z axis (up vector of camera is y axis)
  ap.phi = (rngw->rng_.drand64(0.0, 360.0)/180.0)*vnl_math::pi;

  std::cout << "sampled theta: " << 180.0*(ap.theta/vnl_math::pi) << " phi: " << 180.0*(ap.phi/vnl_math::pi) << std::endl;

  vgl_vector_3d<double> y_vec(0.0, 1.0, 0.0);
  vgl_rotation_3d<double> R2(0.0, 0.0, ap.phi);
  // first rotate up vector by phi
  vgl_vector_3d<double> y_vec_r = R2*y_vec;
  vnl_vector_fixed<double,3> v(y_vec_r.x(), y_vec_r.y(), y_vec_r.z());
  // then rotate axis by theta around up vector
  vnl_quaternion<double> q(v,ap.theta);
  vgl_rotation_3d<double> composed_from_q(q);

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  out_cam.set_rotation(composed_from_q*cam->get_rotation());
  out_cam.set_camera_center(cam->get_camera_center());

  pro.set_output_val<vpgl_camera_double_sptr>(0, new vpgl_perspective_camera<double>(out_cam));
  pro.set_output_val<float>(1,float(ap.theta));
  pro.set_output_val<float>(2,float(ap.phi));
  return true;
}
