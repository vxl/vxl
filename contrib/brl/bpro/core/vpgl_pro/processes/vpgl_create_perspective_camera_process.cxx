// This is brl/bpro/core/vpgl_pro/processes/vpgl_create_perspective_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vsl/vsl_binary_io.h>
#include <bpgl/bpgl_camera_utils.h>
#include <bpgl/algo/bpgl_transform_camera.h>

//: Init function
bool vpgl_create_perspective_camera_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types(10);
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

  vcl_vector<vcl_string> output_types(1);
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
    vcl_cerr << "vpgl_scale_perspective_camera_process: Invalid inputs\n";
    return false;
  }
  // get the inputs

  double k00 = pro.get_input<double>(0), k02 = pro.get_input<double>(1);
  double k11 = pro.get_input<double>(2), k12 = pro.get_input<double>(3);
  vnl_matrix_fixed<double ,3,3> m(0.0);
  m[0][0]=k00;   m[0][2]=k02;   m[1][1]=k11;   m[1][2]=k12; m[2][2]=1.0;
  vpgl_calibration_matrix<double> K(m);

  vgl_homg_point_3d<double> cent(pro.get_input<double>(4), pro.get_input<double>(5), pro.get_input<double>(6));
  vgl_rotation_3d<double> I; // no rotation initially
  vpgl_perspective_camera<double> camera(K, cent, I);

  vgl_homg_point_3d<double> look(pro.get_input<double>(7), pro.get_input<double>(8), pro.get_input<double>(9));
  camera.look_at(look);

  vpgl_perspective_camera<double>* ncam = new vpgl_perspective_camera<double>(camera);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}

//: Init function
bool vpgl_create_perspective_camera_process2_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types(4);
  input_types[0] = "vpgl_camera_double_sptr";// get everything from this camera
  input_types[1] = "float";// center x  Except the center
  input_types[2] = "float";// center y
  input_types[3] = "float";// center z
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types(1);
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
    vcl_cerr << "vpgl_create_perspective_camera_process2: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    vcl_cerr << "vpgl_create_perspective_camera_process2: couldn't cast camera\n";
    return false;
  }

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  out_cam.set_rotation(cam->get_rotation());
  vgl_point_3d<double> cent(pro.get_input<float>(1), pro.get_input<float>(2), pro.get_input<float>(3));
  out_cam.set_camera_center(cent);

  vpgl_perspective_camera<double>* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}

//: Init function
bool vpgl_create_perspective_camera_process3_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types(6);
  input_types[0] = "vpgl_camera_double_sptr";// get R from this camera
  input_types[1] = "float"; // phi, first rotate up vector by phi around z, then rotate principal axis of camera by theta around rotated up vector, then rotate principal axis by R
  input_types[2] = "float"; // theta  pass angles in degrees, process converts to radians
  input_types[3] = "float";// center x  Except the center
  input_types[4] = "float";// center y
  input_types[5] = "float";// center z
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types(1);
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
    vcl_cerr << "vpgl_create_perspective_camera_process2: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    vcl_cerr << "vpgl_create_perspective_camera_process2: couldn't cast camera\n";
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

  vpgl_perspective_camera<double>* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}


//: Init function
bool vpgl_create_perspective_camera_process4_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types(7);
  input_types[0] = "vpgl_camera_double_sptr";// get everything from this camera
  input_types[1] = "float";// center x  Except the center, and initialize orientation with respect to direction of motion 
  input_types[2] = "float";// center y
  input_types[3] = "float";// center z
  input_types[4] = "float"; // center x of next camera
  input_types[5] = "float"; // center y of next camera
  input_types[6] = "float"; // center z of next camera
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types(1);
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
    vcl_cerr << "vpgl_create_perspective_camera_process4: Invalid inputs\n";
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    vcl_cerr << "vpgl_create_perspective_camera_process2: couldn't cast camera\n";
    return false;
  }
  float cent_x = pro.get_input<float>(1);
  float cent_y = pro.get_input<float>(2);
  float cent_z = pro.get_input<float>(3);

  float cent2_x = pro.get_input<float>(4);
  float cent2_y = pro.get_input<float>(5);
  float cent2_z = pro.get_input<float>(6);

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
  double theta_c=vcl_atan2(cy-py,cx-px);
  double theta_n=vcl_atan2(ny-cy,nx-cx);
  vcl_cout << "theta_c: " << theta_c*rad_to_deg << " theta_n: " << theta_n*rad_to_deg << " theta dif: " << (theta_c-theta_n)*rad_to_deg << vcl_endl; 
  double theta = (theta_n - theta_c)/2.0;
  vcl_cout << "theta: " << theta << " which is: " << theta*rad_to_deg << " in degrees, using 5 degree which is: " << 5*vnl_math::pi/180.0 << " in radians!\n";
  //theta = -5*vnl_math::pi/180.0;
  vnl_vector_fixed<double,3> v(up_vec.x(), up_vec.y(), up_vec.z());
  //: then rotate axis by theta around up vector
  vnl_quaternion<double> q(v,theta);
  vgl_rotation_3d<double> composed_from_q(q);

  vpgl_perspective_camera<double> out_cam;
  out_cam.set_calibration(cam->get_calibration());
  
  out_cam.set_rotation(composed_from_q*cam->get_rotation());
  out_cam.set_camera_center(pt2);

  vpgl_perspective_camera<double>* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);

  return true;
}

// create camera from kml parameters
bool vpgl_create_perspective_camera_from_kml_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types(10);
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

  vcl_vector<vcl_string> output_types(1);
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
    vcl_cerr << "vpgl_create_perspective_camera_process4: Invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned ni = pro.get_input<unsigned>(0);
  unsigned nj = pro.get_input<unsigned>(1);
  double right_fov = pro.get_input<double>(2);
  double top_fov = pro.get_input<double>(3);
  double alt = pro.get_input<double>(4);
  double heading = pro.get_input<double>(5);
  double tilt = pro.get_input<double>(6);
  double roll = pro.get_input<double>(7);
  double cent_x = pro.get_input<double>(8);
  double cent_y = pro.get_input<double>(9);

  vpgl_perspective_camera<double> out_cam = bpgl_camera_utils::camera_from_kml((double)ni, (double)nj, right_fov, top_fov, 1.6, heading, tilt, roll);  
  out_cam.set_camera_center(vgl_point_3d<double>(cent_x, cent_y, alt));
  vpgl_perspective_camera<double>* ncam = new vpgl_perspective_camera<double>(out_cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, ncam);
  return true;
}
