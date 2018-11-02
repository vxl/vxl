// This is brl/bpro/core/vpgl_pro/processes/vpgl_create_perspective_camera_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vsl/vsl_binary_io.h>
#include <bpgl/bpgl_camera_utils.h>
#include <bpgl/algo/bpgl_transform_camera.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>

//: Init function
bool vpgl_create_perspective_camera_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(10);
  // the revised K matrix (no skew)
  input_types[0] = "double";// K[0][0] - u scale
  input_types[1] = "double";// K[0][2] - principal point (u)
  input_types[2] = "double";// K[1][1] - v scale
  input_types[3] = "double";// K[1][2] - principal point (v)
  input_types[4] = "double";// center x
  input_types[5] = "double";// center y
  input_types[6] = "double";// center z
  input_types[7] = "double";// look at point x
  input_types[8] = "double";// look at point y
  input_types[9] = "double";// look at point z
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_perspective_camera_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_scale_perspective_camera_process: Invalid inputs\n";
    return false;
  }
  // get the inputs

  auto k00 = pro.get_input<double>(0), k02 = pro.get_input<double>(1);
  auto k11 = pro.get_input<double>(2), k12 = pro.get_input<double>(3);
  vnl_matrix_fixed<double ,3,3> m(0.0);
  m[0][0]=k00;   m[0][2]=k02;   m[1][1]=k11;   m[1][2]=k12; m[2][2]=1.0;
  vpgl_calibration_matrix<double> K(m);

  vgl_homg_point_3d<double> cent(pro.get_input<double>(4), pro.get_input<double>(5), pro.get_input<double>(6));
  vgl_rotation_3d<double> I; // no rotation initially
  vpgl_perspective_camera<double> camera(K, cent, I);

  vgl_homg_point_3d<double> look(pro.get_input<double>(7), pro.get_input<double>(8), pro.get_input<double>(9));
  camera.look_at(look);

  auto* ncam = new vpgl_perspective_camera<double>(camera);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}

//: Init function
bool vpgl_create_perspective_camera_process2_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(4);
  input_types[0] = "vpgl_camera_double_sptr";// get everything from this camera
  input_types[1] = "float";// center x  Except the center
  input_types[2] = "float";// center y
  input_types[3] = "float";// center z
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_perspective_camera_process2(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_create_perspective_camera_process2: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_create_perspective_camera_process2: couldn't cast camera\n";
    return false;
  }

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  out_cam.set_rotation(cam->get_rotation());
  vgl_point_3d<double> cent(pro.get_input<float>(1), pro.get_input<float>(2), pro.get_input<float>(3));
  out_cam.set_camera_center(cent);

  auto* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}

//: Init function
bool vpgl_create_perspective_camera_process3_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(6);
  input_types[0] = "vpgl_camera_double_sptr";// get R from this camera
  input_types[1] = "float"; // phi, first rotate up vector by phi around z, then rotate principal axis of camera by theta around rotated up vector, then rotate principal axis by R
  input_types[2] = "float"; // theta  pass angles in degrees, process converts to radians
  input_types[3] = "float";// center x  Except the center
  input_types[4] = "float";// center y
  input_types[5] = "float";// center z
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_perspective_camera_process3(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_create_perspective_camera_process2: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_create_perspective_camera_process2: couldn't cast camera\n";
    return false;
  }
  double phi = pro.get_input<float>(1)/180.0*vnl_math::pi;
  double theta = pro.get_input<float>(2)/180.0*vnl_math::pi;

  //: assumes that the camera has it's up vector along x axis in the standard coordinates
  vgl_vector_3d<double> up_vec(1.0, 0.0, 0.0);
  vgl_rotation_3d<double> R2(0.0, 0.0, phi);
  //: first rotate up vector by phi
  vgl_vector_3d<double> up_vec_r = R2*up_vec;
  vnl_vector_fixed<double,3> v(up_vec_r.x(), up_vec_r.y(), up_vec_r.z());
  //: then rotate axis by theta around up vector
  vnl_quaternion<double> q(v,theta);
  vgl_rotation_3d<double> composed_from_q(q);

  vgl_rotation_3d<double> R3(theta, 0.0, 0.0);

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  //out_cam.set_rotation(composed_from_q*cam->get_rotation());
  out_cam.set_rotation(R2*R3*cam->get_rotation());
  vgl_point_3d<double> cent(pro.get_input<float>(3), pro.get_input<float>(4), pro.get_input<float>(5));
  out_cam.set_camera_center(cent);

  auto* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}


//: Init function
bool vpgl_create_perspective_camera_process4_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(7);
  input_types[0] = "vpgl_camera_double_sptr";// get everything from this camera
  input_types[1] = "float";// center x  Except the center, and initialize orientation with respect to direction of motion
  input_types[2] = "float";// center y
  input_types[3] = "float";// center z
  input_types[4] = "float"; // center x of next camera
  input_types[5] = "float"; // center y of next camera
  input_types[6] = "float"; // center z of next camera
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_perspective_camera_process4(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_create_perspective_camera_process4: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    std::cerr << "vpgl_create_perspective_camera_process2: couldn't cast camera\n";
    return false;
  }
  auto cent_x = pro.get_input<float>(1);
  auto cent_y = pro.get_input<float>(2);
  auto cent_z = pro.get_input<float>(3);

  auto cent2_x = pro.get_input<float>(4);
  auto cent2_y = pro.get_input<float>(5);
  auto cent2_z = pro.get_input<float>(6);

  //: assumes that the camera has it's up vector along x axis in the standard coordinates
  vgl_vector_3d<double> up_vec(0.0, 1.0, 0.0);

  //: now find the angle that the camera needs to be rotated around it's up vector
  vgl_point_3d<double> pt1 = cam->get_camera_center();
  vgl_point_3d<double> pt2(cent_x, cent_y, cent_z);
  vgl_point_3d<double> pt3(cent2_x, cent2_y, cent2_z);
  /*vgl_vector_3d<double> l1 = pt2 - pt1;
  vgl_vector_3d<double> l2 = pt3 - pt2;
  double theta = inner_product(l1,l2);*/

  double rad_to_deg = 180.0/vnl_math::pi;
  double px=pt1.x();
  double py=pt1.y();
  double cx=pt2.x();
  double cy=pt2.y();
  double nx=pt3.x();
  double ny=pt3.y();
  double theta_c=std::atan2(cy-py,cx-px);
  double theta_n=std::atan2(ny-cy,nx-cx);
  std::cout << "theta_c: " << theta_c*rad_to_deg << " theta_n: " << theta_n*rad_to_deg << " theta dif: " << (theta_c-theta_n)*rad_to_deg << std::endl;
  double theta = (theta_n - theta_c)/2.0;
  std::cout << "theta: " << theta << " which is: " << theta*rad_to_deg << " in degrees, using 5 degree which is: " << 5*vnl_math::pi/180.0 << " in radians!\n";
  //theta = -5*vnl_math::pi/180.0;
  vnl_vector_fixed<double,3> v(up_vec.x(), up_vec.y(), up_vec.z());
  //: then rotate axis by theta around up vector
  vnl_quaternion<double> q(v,theta);
  vgl_rotation_3d<double> composed_from_q(q);

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());

  out_cam.set_rotation(composed_from_q*cam->get_rotation());
  out_cam.set_camera_center(pt2);

  auto* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}

// create camera from kml parameters
bool vpgl_create_perspective_camera_from_kml_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(10);
  input_types[0] = "unsigned";// ni
  input_types[1] = "unsigned";// nj
  input_types[2] = "double";  // right field of view
  input_types[3] = "double";  // top field of view
  input_types[4] = "double";  // altitude
  input_types[5] = "double";  // heading
  input_types[6] = "double";  // tilt
  input_types[7] = "double";  // roll
  input_types[8] = "double";  // x
  input_types[9] = "double";  // y
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_perspective_camera_from_kml_process(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_create_perspective_camera_process4: Invalid inputs\n";
    return false;
  }
  // get the inputs
  auto ni = pro.get_input<unsigned>(0);
  auto nj = pro.get_input<unsigned>(1);
  auto right_fov = pro.get_input<double>(2);
  auto top_fov = pro.get_input<double>(3);
  auto alt = pro.get_input<double>(4);
  auto heading = pro.get_input<double>(5);
  auto tilt = pro.get_input<double>(6);
  auto roll = pro.get_input<double>(7);
  auto cent_x = pro.get_input<double>(8);
  auto cent_y = pro.get_input<double>(9);

  vpgl_perspective_camera<double> out_cam = bpgl_camera_utils::camera_from_kml((double)ni, (double)nj, right_fov, top_fov, 1.6, heading, tilt, roll);
  out_cam.set_camera_center(vgl_point_3d<double>(cent_x, cent_y, alt));
  auto* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);
  return true;
}

//: Init function
bool vpgl_create_perspective_camera_process5_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(3);
  // the revised K matrix (no skew)
  input_types[0] = "bbas_1d_array_double_sptr";// K
  input_types[1] = "bbas_1d_array_double_sptr";// R
  input_types[2] = "bbas_1d_array_double_sptr";// t
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types(1);
  output_types[0] = "vpgl_camera_double_sptr";  // output camera
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_create_perspective_camera_process5(bprb_func_process& pro)
{
   // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "vpgl_scale_perspective_camera_process5: Invalid inputs\n";
    return false;
  }
  // get the inputs

  bbas_1d_array_double_sptr K_input = pro.get_input<bbas_1d_array_double_sptr>(0);
  bbas_1d_array_double_sptr R_input = pro.get_input<bbas_1d_array_double_sptr>(1);
  bbas_1d_array_double_sptr t_input = pro.get_input<bbas_1d_array_double_sptr>(2);


  if(K_input->data_array.size() != 9 ||
     R_input->data_array.size() != 9 ||
     t_input->data_array.size() != 3)
    return false;

  vnl_matrix_fixed<double ,3,3> K_temp(0.0);
  vnl_matrix_fixed<double ,3,3> R_temp(0.0);
  vnl_vector_fixed<double ,3> t_temp(0.0);

  for(int x=0; x<3; x++)
  {
    for(int y=0; y<3; y++)
    {
      K_temp[x][y] = K_input->data_array[x*3+y];
      R_temp[x][y] = R_input->data_array[x*3+y];
    }
    t_temp[x] = t_input->data_array[x];
  }

  vpgl_calibration_matrix<double>K(K_temp);
  vgl_rotation_3d<double>R(R_temp);
  vgl_vector_3d<double>t(t_temp[0], t_temp[1], t_temp[2]);

  auto* camera = new vpgl_perspective_camera<double>(K, R, t);

  pro.set_output_val<vpgl_camera_double_sptr>(0, camera);

  return true;
}
