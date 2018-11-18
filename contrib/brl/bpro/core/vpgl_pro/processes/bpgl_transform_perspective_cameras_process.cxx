// This is brl/bpro/core/vpgl_pro/processes/bpgl_transform_perspective_cameras_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/algo/bpgl_transform_camera.h>

namespace bpgl_transform_perspective_cameras_process_globals
{
    constexpr unsigned n_inputs_ = 3;
    constexpr unsigned n_outputs_ = 0;
}

//: Init function
bool bpgl_transform_perspective_cameras_process_cons(bprb_func_process& pro)
{
  using namespace bpgl_transform_perspective_cameras_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // transformation file
  input_types_[1] = "vcl_string"; // Input cam dir
  input_types_[2] = "vcl_string"; // Output cam dir

  // process has 0 outputs
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool bpgl_transform_perspective_cameras_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "bpgl_transform_perspective_cameras_process: Invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  std::string xform_file = pro.get_input<std::string>(i++);
  std::string in_dir = pro.get_input<std::string>(i++);
  std::string out_dir = pro.get_input<std::string>(i++);
  std::cout<<"out_dir "<<out_dir<<std::endl;
  // check if input directory exists
  if (!vul_file::is_directory(in_dir.c_str()))
  {
    std::cout<<"Input Camera directory does not exist"<<std::endl;
    return false;
  }

  // check if output directory exists
  if (!vul_file::is_directory(out_dir.c_str()))
  {
    std::cout<<"Output Camera directory does not exist"<<std::endl;
    return false;
  }

  // read the xform file
  // * xaxis yaxis zaxis roation angle
  // * tx ty tz
  // * scale
  std::ifstream ifile(xform_file.c_str());
  if (!ifile)
  {
    std::cout<<"Cannot open Xform file"<<std::endl;
    return false;
  }

  double xr,yr,zr;
  double tx,ty,tz,scale;

  ifile>>xr>>yr>>zr;
  ifile>>tx>>ty>>tz;
  ifile>>scale;
  vnl_quaternion<double> q(xr,yr,zr);
  vgl_rotation_3d<double> R(q);
  vnl_vector_fixed<double, 3> t;
  t[0] = tx; t[1] = ty; t[2] = tz;
  // xform and save the xformed cams
  std::string glob_in_dir = in_dir + "/*.txt";
  for (vul_file_iterator fn = glob_in_dir.c_str(); fn; ++fn) {
    std::string f = fn();
    std::ifstream is(f.c_str());
    vpgl_perspective_camera<double> cam;
    is >> cam;
    is.close();
    std::string fname = vul_file::strip_directory(f.c_str());
    vpgl_perspective_camera<double> tcam = bpgl_transform_camera::transform_perspective_camera(cam, R, t, scale);
    std::string out_file = out_dir + "/"+ fname;
    std::ofstream os(out_file.c_str());
    os << tcam;
    os.close();
  }
  return true;
}


//
//   this process is a version of bwm_3d_site_transform.cxx
//
//   input: pts0 and pts1
//     compute similarity transformation that maps space of pt0 to space of pts1
//
// the input camera is in the coordinate system of pts0. The output camera
// is the camera in the coordinate system of pts1, that is,
//
//  x1 =  K[R0|t0](Hs Hs^-1) X1, where Hs is the similarity transform.
//
// Thus, the similarity transform is applied to the camera as,
// (s = scale)
//                        _     _  _      _
//                       |s 0 0 0||        |
//  K[R' | t'] = K[R0|t0]|0 s 0 0||  Rs  ts|
//                       |0 0 s 0||        |
//                       |0 0 0 1|| 0 0 0 1|
//                        -      - -      -
// It follows that R' = R0*Rs and t' = t0/s + R0*ts
//
//

#include <core/bbas_pro/bbas_1d_array_double.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_box_3d.h>

static vpgl_perspective_camera<double>
vpgl_transform_space_process_transform_camera(vpgl_perspective_camera<double> const& cam,
                 vgl_rotation_3d<double> const& Rs,
                 vnl_vector_fixed<double, 3> const& ts,
                 const double scale)
{
  vnl_matrix_fixed<double,3,3> Rms = Rs.as_matrix();
  //Get input camera components
  //note, the homogeneous calibration matrix is unaffected by the scale
  const vpgl_calibration_matrix<double>& K = cam.get_calibration();
  vnl_matrix_fixed<double, 3, 3> R = cam.get_rotation().as_matrix();
  vgl_vector_3d<double> tv = cam.get_translation();
  vnl_vector_fixed<double, 3> t(tv.x(), tv.y(), tv.z());
  //compose rotations
  vnl_matrix_fixed<double, 3, 3> Rt = R*Rms;
  vgl_rotation_3d<double> Rtr(Rt);
  //compute new translation
  vnl_vector_fixed<double, 3> tt = (1.0/scale)*t + R*ts;
  vgl_vector_3d<double> ttg(tt[0], tt[1], tt[2]);
  //construct transformed camera
  vpgl_perspective_camera<double> camt(K, Rtr, ttg);
  return camt;
}

//: sets input and output types
bool vpgl_transform_space_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(8);
  input_types_[0] = "bbas_1d_array_double_sptr"; // x values of pts0 (points in SPACE 0)
  input_types_[1] = "bbas_1d_array_double_sptr"; // y values of pts0 (points in SPACE 0)
  input_types_[2] = "bbas_1d_array_double_sptr"; // z values of pts0 (points in SPACE 0)
  input_types_[3] = "bbas_1d_array_double_sptr"; // x values of pts1 (points in SPACE 1)
  input_types_[4] = "bbas_1d_array_double_sptr"; // y values of pts1 (points in SPACE 1)
  input_types_[5] = "bbas_1d_array_double_sptr"; // z values of pts1 (points in SPACE 1)
  input_types_[6] = "vcl_string";      // path to input camera folder
  input_types_[7] = "vcl_string"; // path to output camera folder

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(2);
  output_types_[0] = "bbas_1d_array_double_sptr"; // output 4 by 4 similarity matrix as a vector of size 16
                                                  // construct the matrix as follows
                                                  // 0  1  2  3
                                                  // 4  5  6  7
                                                  // 8  9  10 11
                                                  // 12 13 14 15
  output_types_[1] = "double"; // also output scale as a double to scale other quantities appropriately if needed, e.g. the voxel_size if a new scene.xml is needed
  return pro.set_output_types(output_types_);
}

bool vpgl_transform_space_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  bbas_1d_array_double_sptr pts0_xs = pro.get_input<bbas_1d_array_double_sptr>(0);
  bbas_1d_array_double_sptr pts0_ys = pro.get_input<bbas_1d_array_double_sptr>(1);
  bbas_1d_array_double_sptr pts0_zs = pro.get_input<bbas_1d_array_double_sptr>(2);
  bbas_1d_array_double_sptr pts1_xs = pro.get_input<bbas_1d_array_double_sptr>(3);
  bbas_1d_array_double_sptr pts1_ys = pro.get_input<bbas_1d_array_double_sptr>(4);
  bbas_1d_array_double_sptr pts1_zs = pro.get_input<bbas_1d_array_double_sptr>(5);

  std::string input_cam_path = pro.get_input<std::string>(6);
  std::string output_cam_path = pro.get_input<std::string>(7);

  std::cout << pts0_xs->data_array;

  vnl_matrix<double> pts0, pts1;
  auto n = (unsigned)(pts0_xs->data_array.size());

  pts0.set_size(3,n);
  pts1.set_size(3,n);
  for (unsigned i = 0; i<n; ++i) {
    pts0[0][i] = pts0_xs->data_array[i];  pts1[0][i] = pts1_xs->data_array[i];
    pts0[1][i] = pts0_ys->data_array[i];  pts1[1][i] = pts1_ys->data_array[i];
    pts0[2][i] = pts0_zs->data_array[i];  pts1[2][i] = pts1_zs->data_array[i];
  }

  // prepare the output matrix as an array
  bbas_1d_array_double_sptr out = new bbas_1d_array_double(16);

  // compute the similarity transformation
  vpgl_ortho_procrustes op(pts0, pts1);
  vgl_rotation_3d<double> R = op.R();
  vnl_vector_fixed<double, 3> t = op.t();
  double scale = op.s();
  if (! op.compute_ok()) {
    std::cerr << "Problems during similarity transformation computation! Check if there are enough number of points! Current number of points: " << n << ". Minimum required number of points is 3, suggested to use a minimum of 5!" << std::endl;
    pro.set_output_val<bbas_1d_array_double_sptr>(0, out);
    return false;
  }

  std::cout << "Copmuted similarity matrix! Ortho procrustes error " << std::sqrt(op.residual_mean_sq_error()) << std::endl;

  // read each camera and save the output camera in the output folder
  std::string in_dir = input_cam_path + "/*.txt";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string f = fn();
    std::ifstream is(f.c_str());
    vpgl_perspective_camera<double> cam;
    is >> cam;
    is.close();
    std::string fname = vul_file::strip_directory(f.c_str());
    std::cout << fname << std::endl;
    vpgl_perspective_camera<double> tcam =
      vpgl_transform_space_process_transform_camera(cam, R, t, scale);
    std::cout<<"CC : "<<tcam.camera_center()<<std::endl;
    std::string out_dir = output_cam_path + "/";
    std::string out_file = out_dir + fname;
    std::ofstream os(out_file.c_str());
    os << tcam;
    os.close();
  }

  // prepare the output matrix, we need the matrix that takes a point in the space of pts0 to the space of pts1,
  // this is actually the inverse of the transformation computed above!!
  vnl_matrix_fixed<double, 4, 4> S(0.0), outM(0.0);
  S[0][0] = S[1][1] = S[2][2] = 1.0/scale; S[3][3] = 1.0;
  // inverse of t is a little more complicated
  vnl_vector_fixed<double, 3> t_inverse = -R.inverse().as_matrix()*t;
  vgl_h_matrix_3d<double> RotT(R.inverse().as_matrix(), t_inverse);
  outM = RotT.get_matrix()*S;

  // copy to the output array
  out->data_array[0] = outM[0][0]; out->data_array[1] = outM[0][1]; out->data_array[2] = outM[0][2]; out->data_array[3] = outM[0][3];
  out->data_array[4] = outM[1][0]; out->data_array[5] = outM[1][1]; out->data_array[6] = outM[1][2]; out->data_array[7] = outM[1][3];
  out->data_array[8] = outM[2][0]; out->data_array[9] = outM[2][1]; out->data_array[10] = outM[2][2]; out->data_array[11] = outM[2][3];
  out->data_array[12] = outM[3][0]; out->data_array[13] = outM[3][1]; out->data_array[14] = outM[3][2]; out->data_array[15] = outM[3][3];

  pro.set_output_val<bbas_1d_array_double_sptr>(0, out);
  pro.set_output_val<double>(1,1.0/scale);
  return true;
}

// a process to take min and max points of a box in the space of pts0 then transform 8 corner points to the space of pts1
// in the transformed space, compute a new axis aligned bounding box and return its min and max points

//: sets input and output types
bool vpgl_transform_box_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(3);
  input_types_[0] = "bbas_1d_array_double_sptr"; // min pt x y z values in SPACE 0
  input_types_[1] = "bbas_1d_array_double_sptr"; // max pt x y z values in SPACE 0
  input_types_[2] = "bbas_1d_array_double_sptr"; // input 4 by 4 similarity matrix as a vector of size 16
                                                  // construct the matrix as follows
                                                  // 0  1  2  3
                                                  // 4  5  6  7
                                                  // 8  9  10 11
                                                  // 12 13 14 15


  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(2);
  output_types_[0] = "bbas_1d_array_double_sptr"; // min pt x y z of the bounding box in SPACE 1
  output_types_[1] = "bbas_1d_array_double_sptr"; // max pt x y z of the bounding box in SPACE 1
  return pro.set_output_types(output_types_);
}

bool vpgl_transform_box_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  bbas_1d_array_double_sptr min_pt_a = pro.get_input<bbas_1d_array_double_sptr>(0);
  bbas_1d_array_double_sptr max_pt_a = pro.get_input<bbas_1d_array_double_sptr>(1);
  bbas_1d_array_double_sptr matrix = pro.get_input<bbas_1d_array_double_sptr>(2);

  // construct the similarity matrix
  vnl_matrix_fixed<double, 4, 4> SM;
  SM[0][0] = matrix->data_array[0];  SM[0][1] = matrix->data_array[1];  SM[0][2] = matrix->data_array[2];  SM[0][3] = matrix->data_array[3];
  SM[1][0] = matrix->data_array[4];  SM[1][1] = matrix->data_array[5];  SM[1][2] = matrix->data_array[6];  SM[1][3] = matrix->data_array[7];
  SM[2][0] = matrix->data_array[8];  SM[2][1] = matrix->data_array[9];  SM[2][2] = matrix->data_array[10]; SM[2][3] = matrix->data_array[11];
  SM[3][0] = matrix->data_array[12]; SM[3][1] = matrix->data_array[13]; SM[3][2] = matrix->data_array[14]; SM[3][3] = matrix->data_array[15];

  // create 8 corners of the input box
  vgl_box_3d<double> box1, box2;
  box1.add(vgl_point_3d<double>(min_pt_a->data_array[0], min_pt_a->data_array[1], min_pt_a->data_array[2]));
  box1.add(vgl_point_3d<double>(max_pt_a->data_array[0], max_pt_a->data_array[1], max_pt_a->data_array[2]));
  std::vector<vgl_point_3d<double> > vertices = box1.vertices();

  // transform 8 corners of the box and add to the new box in the transformed space
  for (auto & vertice : vertices) {
    vnl_matrix_fixed<double, 4, 1> pt, new_pt;
    pt[0][0] = vertice.x(); pt[1][0] = vertice.y(); pt[2][0] = vertice.z(); pt[3][0] = 1.0;
    new_pt = SM*pt;
    box2.add(vgl_point_3d<double>(new_pt[0][0]/new_pt[3][0], new_pt[1][0]/new_pt[3][0], new_pt[2][0]/new_pt[3][0]));
  }
  vgl_point_3d<double> out_min_pt = box2.min_point();
  vgl_point_3d<double> out_max_pt = box2.max_point();

  bbas_1d_array_double_sptr outmin = new bbas_1d_array_double(3), outmax = new bbas_1d_array_double(3);
  outmin->data_array[0] = out_min_pt.x(); outmin->data_array[1] = out_min_pt.y(); outmin->data_array[2] = out_min_pt.z();
  outmax->data_array[0] = out_max_pt.x(); outmax->data_array[1] = out_max_pt.y(); outmax->data_array[2] = out_max_pt.z();

  pro.set_output_val<bbas_1d_array_double_sptr>(0, outmin);
  pro.set_output_val<bbas_1d_array_double_sptr>(1, outmax);
  return true;
}
