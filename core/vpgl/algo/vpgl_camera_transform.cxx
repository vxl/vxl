#include <iostream>
#include <limits>
#include <utility>
#include "vpgl_camera_transform.h"
//:
// \file


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/algo/vnl_lsqr.h>
#include <vnl/algo/vnl_qr.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_trace.h>


vpgl_camera_transform_f::vpgl_camera_transform_f(unsigned cnt_residuals, unsigned n_unknowns,
                                                 const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                                                 std::vector< std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > >  cam_ids_img_pts,
                                                 std::vector<vnl_vector_fixed<double, 4> >  pts_3d, bool minimize_R) :
                                                 vnl_least_squares_function(n_unknowns,cnt_residuals,vnl_least_squares_function::no_gradient),
                                                 cam_ids_img_pts_(std::move(cam_ids_img_pts)), pts_3d_(std::move(pts_3d)), input_cams_(input_cams), minimize_R_(minimize_R)
{

  for (const auto & input_cam : input_cams) {
    const vpgl_calibration_matrix<double>& K = input_cam.get_calibration();
    Ks_.push_back(K);
    vnl_matrix_fixed<double, 3, 3> R = input_cam.get_rotation().as_matrix();
    Rs_.push_back(R);
    //vgl_vector_3d<double> tv = input_cams[i].get_translation();
    //vnl_vector_fixed<double, 3> tvv(tv.x(), tv.y(), tv.z());
    vgl_point_3d<double> C = input_cam.get_camera_center();
    //vnl_vector_fixed<double, 3> tvv(C.x(), C.y(), C.z());
    //ts_.push_back(tvv);
    Cs_.push_back(C);
  }
}

//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
void vpgl_camera_transform_f::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  unsigned dim = get_number_of_unknowns();

  // compute current cams
  std::vector<vpgl_perspective_camera<double> > current_cams;
  if (dim == 6)
    compute_cams(x, current_cams);
  else
    compute_cams_selective(x, current_cams);

  std::vector<vnl_matrix_fixed<double, 3, 4> > current_cam_Ms;
  current_cam_Ms.reserve(current_cams.size());
for (auto & current_cam : current_cams)
    current_cam_Ms.push_back(current_cam.get_matrix());

  // compute the new projections and the residuals
  unsigned cnt = 0;
  for (unsigned j = 0; j < cam_ids_img_pts_.size(); j++) {
    for (unsigned i = 0; i < cam_ids_img_pts_[j].size(); i++) {
      unsigned cam_id = cam_ids_img_pts_[j][i].second;

      vnl_vector_fixed<double, 2> img_pt = cam_ids_img_pts_[j][i].first;

      vnl_vector_fixed<double, 3> current_img_pt = current_cam_Ms[cam_id]*pts_3d_[j];

      // compute the residual
      double dif0 = (current_img_pt[0]/current_img_pt[2]) - img_pt[0];
      double dif1 = (current_img_pt[1]/current_img_pt[2]) - img_pt[1];
      //fx[cnt] = std::sqrt(dif0*dif0 + dif1*dif1);
      fx[cnt] = std::abs(dif0);
      cnt++;
      fx[cnt] = std::abs(dif1);

      /*if (cnt == 1 || cnt == 11) {
        std::cout << " \t\t cam id: " << cam_id << " img_pt: " << img_pt;
        std::cout << " projected pt: " << current_img_pt[0]/current_img_pt[2] << " " << current_img_pt[1]/current_img_pt[2] << std::endl;
        std::cout << " \t\t dif0: " << dif0 << " dif1: " << dif1 << " error: " << std::sqrt(dif0*dif0 + dif1*dif1) << std::endl;
        std::cout << " \t\t 3d pt: " << pts_3d_[j] << std::endl;
      }*/


      cnt++;
    }
  }
}

//: Fast conversion of rotation from Rodrigues vector to matrix
vnl_matrix_fixed<double,3,3>
vpgl_camera_transform_f::rod_to_matrix(double r0, double r1, double r2)
{
  double x2 = r0*r0, y2 = r1*r1, z2 = r2*r2;
  double m = x2 + y2 + z2;
  double theta = std::sqrt(m);
  double s = std::sin(theta) / theta;
  double c = (1 - std::cos(theta)) / m;

  vnl_matrix_fixed<double,3,3> R(0.0);
  R(0,0) = R(1,1) = R(2,2) = 1.0;
  if (m == 0.0)
    return R;

  R(0,0) -= (y2 + z2) * c;
  R(1,1) -= (x2 + z2) * c;
  R(2,2) -= (x2 + y2) * c;
  R(0,1) = R(1,0) = r0*r1*c;
  R(0,2) = R(2,0) = r0*r2*c;
  R(1,2) = R(2,1) = r1*r2*c;

  double t = r0*s;
  R(1,2) -= t;
  R(2,1) += t;
  t = r1*s;
  R(0,2) += t;
  R(2,0) -= t;
  t = r2*s;
  R(0,1) -= t;
  R(1,0) += t;

  return R;
}


void vpgl_camera_transform_f::compute_cams(vnl_vector<double> const& x, std::vector<vpgl_perspective_camera<double> >& output_cams)
{
  // current rotation
  //vnl_vector<double> w(3);
  //w[0] = x[0]; w[1] = x[1]; w[2] = x[2];
  //vgl_rotation_3d <double> Rw(w);  // create from Rodriguez params
  //vnl_matrix_fixed<double,3,3> R = Rw.as_matrix();
  vnl_matrix_fixed<double,3,3> R = rod_to_matrix(x[0], x[1], x[2]);

  // current t
  //vnl_vector_fixed<double, 3> t(x[3], x[4], x[5]);

  // construct the new cameras
  for (unsigned i = 0; i < Ks_.size(); i++) {
    //compose rotations
    vnl_matrix_fixed<double, 3, 3> Rt = R*Rs_[i];
    vgl_rotation_3d<double> Rtr(Rt);

    //compute new center
    vgl_point_3d<double> Cg(Cs_[i].x() + x[3], Cs_[i].y() + x[4], Cs_[i].z() + x[5]);

    //construct transformed camera
    vpgl_perspective_camera<double> camt(Ks_[i], Cg, Rtr);
    output_cams.push_back(camt);
  }
}

void vpgl_camera_transform_f::compute_cams_selective(vnl_vector<double> const& x, std::vector<vpgl_perspective_camera<double> >& output_cams)
{
  if (minimize_R_) {  // minimize only R,

    // current rotation
    vnl_matrix_fixed<double,3,3> R = rod_to_matrix(x[0], x[1], x[2]);

    // construct the new cameras
    for (unsigned i = 0; i < Ks_.size(); i++) {
      //compose rotations
      vnl_matrix_fixed<double, 3, 3> Rt = R*Rs_[i];
      vgl_rotation_3d<double> Rtr(Rt);

      //construct transformed camera
      vpgl_perspective_camera<double> camt(Ks_[i], Cs_[i], Rtr);
      output_cams.push_back(camt);

      /*if (i==0) {
        std::cout << "input cam:\n " << input_cams_[i] << std::endl;
        std::cout << "output cam with x: " << x[0] << " " << x[1] << " " << x[2] << ":\n " << camt << std::endl;
      }*/
    }
  } else {  // minimize only t
    // construct the new cameras
    for (unsigned i = 0; i < Ks_.size(); i++) {
      vgl_rotation_3d<double> Rtr(Rs_[i]);

      //compute new translation
      //vnl_vector_fixed<double, 3> tt = ts_[i] + Rs_[i]*t;
      //vgl_vector_3d<double> ttg(tt[0], tt[1], tt[2]);
      vgl_point_3d<double> Cg(Cs_[i].x() + x[0], Cs_[i].y() + x[1], Cs_[i].z() + x[2]);

      //construct transformed camera
      //vpgl_perspective_camera<double> camt(Ks_[i], Rtr, ttg);
      vpgl_perspective_camera<double> camt(Ks_[i], Cg, Rtr);
      output_cams.push_back(camt);

      /*if (i==0) {
        std::cout << "input cam:\n " << input_cams_[i] << std::endl;
        std::cout << "input cam center: " << Cs_[i] << std::endl;
        std::cout << "output cam with x: " << x[0] << " " << x[1] << " " << x[2] << ":\n " << camt << std::endl;
        std::cout << "output cam center: " << Cg << std::endl;
        std::cout << "input output dif: " << (Cs_[i]-Cg).length() << std::endl;
      }*/
    }
  }
}

void vpgl_camera_transform::normalize_img_pts(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                              const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                              std::vector<vpgl_perspective_camera<double> >& input_cams_norm,
                                              std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_norm_img_pts)
{
  // normalize the image points
  unsigned cnt_residuals = 0;
  double nx = 0, ny = 0, ns = 0;

  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      double x = cam_ids_img_pt[i].first[0];
      double y = cam_ids_img_pt[i].first[1];
      nx += x;
      ny += y;
      ns += x*x + y*y;
      cnt_residuals++;
    }
  }
  nx /= cnt_residuals;
  ny /= cnt_residuals;
  ns /= cnt_residuals;
  ns -= nx*nx + ny*ny;
  ns /= 2;
  ns = std::sqrt(ns);
  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > cam_pts;
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      double x = cam_ids_img_pt[i].first[0];
      double y = cam_ids_img_pt[i].first[1];
      vnl_vector_fixed<double, 2> new_pt;
      new_pt[0] = (x-nx)/ns;
      new_pt[1] = (y-ny)/ns;
      std::pair<vnl_vector_fixed<double, 2>, unsigned> pair(new_pt, cam_ids_img_pt[i].second);
      cam_pts.push_back(pair);
    }
    cam_ids_norm_img_pts.push_back(cam_pts);
  }

  // normalize the K matrices
  for (const auto & input_cam : input_cams) {
    const vpgl_calibration_matrix<double>& Ki = input_cam.get_calibration();

    vnl_vector<double> K_vals(5,0.0);
    K_vals[0] = Ki.focal_length()*Ki.x_scale();
    K_vals[1] = Ki.y_scale() / Ki.x_scale();
    K_vals[2] = Ki.principal_point().x();
    K_vals[3] = Ki.principal_point().y();
    K_vals[4] = Ki.skew();

    vpgl_calibration_matrix<double> Knew(K_vals[0]/ns,
                                      vgl_point_2d<double>((K_vals[2]-nx)/ns,(K_vals[3]-ny)/ns),
                                      1.0,
                                      K_vals[1],
                                      K_vals[4]);

    vpgl_perspective_camera<double> cnew(Knew, input_cam.get_rotation(), input_cam.get_translation());
    input_cams_norm.push_back(cnew);
  }

}

//: compute the fixed transformation as R and t
bool vpgl_camera_transform::compute_fixed_transformation(const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                                                         const std::vector< std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                                         const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                                         std::vector<vpgl_perspective_camera<double>  >& output_cams)
{
  // find the number of residuals
  unsigned cnt_residuals = 0;
  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      cnt_residuals++;
    }
  }
  std::cout << "number of residuals: " << cnt_residuals << std::endl;

  // normalize the image points
  std::vector<vpgl_perspective_camera<double> > input_cams_norm;
  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > > cam_ids_norm_img_pts;
  normalize_img_pts(input_cams, cam_ids_img_pts, input_cams_norm, cam_ids_norm_img_pts);

  // setup the minimization problem

  //unsigned n_unknowns = 6;  // if 6, minimize both R and t params
  unsigned n_unknowns = 3;  // if 3 minimize only R or t params (depends on stepsizes, set stepsize negative to discard)

  vpgl_camera_transform_f f(cnt_residuals*2, n_unknowns, input_cams_norm, cam_ids_norm_img_pts, pts_3d);

  vnl_levenberg_marquardt minimizer(f);
  //minimizer.set_x_tolerance(1e-16);
  //minimizer.set_f_tolerance(1.0);
  //minimizer.set_g_tolerance(1e-3);
  minimizer.set_trace(true);
  //minimizer.set_max_iterations(50);

  // setup initial parameters, rodriguez vector is (0,0,0) for identity rotation, also set translation to 0,0,0
  vnl_vector<double> x(n_unknowns, 0.0);

  //std::cout << "initial params: " << x << std::endl;
  minimizer.minimize(x);
  //std::cout << "end_error: " << minimizer.get_end_error() << " end params: " << x << std::endl;

  std::vector<vpgl_perspective_camera<double>  > output_cams_denorm;
  if (n_unknowns == 6)
    f.compute_cams(x, output_cams_denorm);
  else
    f.compute_cams_selective(x, output_cams_denorm);

  // denormalize output_cams
  for (unsigned i = 0; i < output_cams_denorm.size(); i++) {
    vpgl_calibration_matrix<double> K = input_cams[i].get_calibration();
    vpgl_perspective_camera<double> cnew(K, output_cams_denorm[i].get_rotation(), output_cams_denorm[i].get_translation());

    output_cams.push_back(cnew);
  }

  return true;
}

//: sample offsets for camera centers in a box with the given dimensions (e.g. plus/minus dim_x) in meters
std::vector<vnl_vector_fixed<double, 3> > vpgl_camera_transform::sample_centers(double dim_x, double dim_y, double dim_z, double step)
{
  std::vector<vnl_vector_fixed<double, 3> > out;
  for (double z = -dim_z; z <= dim_z; z+=step)
    for (double x = -dim_x; x <= dim_x; x+=step)
      for (double y = -dim_y; y <= dim_y; y+=step) {
        vnl_vector_fixed<double, 3> o(x,y,z);
        out.push_back(o);
      }
  return out;
}

//: compute the fixed transformation by sampling centers in a given box and then optimizing for rotation
bool vpgl_camera_transform::compute_fixed_transformation_sample(const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                                                                const std::vector< std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                                                const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                                                std::vector<vpgl_perspective_camera<double>  >& output_cams)
{
  // find the number of residuals
  unsigned cnt_residuals = 0;
  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      cnt_residuals++;
    }
  }
  std::cout << "number of residuals: " << cnt_residuals << std::endl;

  // normalize the image points
  std::vector<vpgl_perspective_camera<double> > input_cams_norm;
  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > > cam_ids_norm_img_pts;
  normalize_img_pts(input_cams, cam_ids_img_pts, input_cams_norm, cam_ids_norm_img_pts);

  // setup the minimization problems
  double dim_x = 20.0, dim_y = 20.0, dim_z = 12.0; // offsets around the original cam center in meters
  double step = 1.0;
  std::vector<vnl_vector_fixed<double, 3> > offsets = sample_centers(dim_x, dim_y, dim_z, step);

  double error_min = 1000000000.0;
  output_cams = input_cams;
  vnl_vector_fixed<double, 3> offset_min;
  for (unsigned i = 0; i < offsets.size(); i++) {
    if (i%500 == 0) std::cout << i << " ";

    std::vector<vpgl_perspective_camera<double> > input_cams_norm_off;
    for (auto & j : input_cams_norm) {
      const vpgl_calibration_matrix<double>& K = j.get_calibration();
      const vgl_rotation_3d<double>& R = j.get_rotation();
      vgl_point_3d<double> C = j.get_camera_center();
      vgl_point_3d<double> Cnew(C.x() + offsets[i][0], C.y() + offsets[i][1], C.z() + offsets[i][2]);
      vpgl_perspective_camera<double> cnew(K, Cnew, R);
      input_cams_norm_off.push_back(cnew);
    }

    unsigned n_unknowns = 3;  // if 3 minimize only R or t params (depends on stepsizes, set stepsize negative to discard)
    vpgl_camera_transform_f f(cnt_residuals*2, n_unknowns, input_cams_norm_off, cam_ids_norm_img_pts, pts_3d);

    vnl_levenberg_marquardt minimizer(f);
    //minimizer.set_trace(true);

    // setup initial parameters, rodriguez vector is (0,0,0) for identity rotation, also set translation to 0,0,0
    vnl_vector<double> x(n_unknowns, 0.0);

    //std::cout << "initial params: " << x << std::endl;
    minimizer.minimize(x);
    double enderror = minimizer.get_end_error();
    //std::cout << "end_error: " << enderror << " end params: " << x << std::endl;

    std::vector<vpgl_perspective_camera<double>  > output_cams_denorm;
    f.compute_cams_selective(x, output_cams_denorm);

    if (enderror < error_min) {
      std::cout << " setting output cams for offset: " << offsets[i] << " with min error: " << error_min << std::endl;
      error_min = enderror;
      offset_min = offsets[i];
      // denormalize output_cams
      for (unsigned j = 0; j < output_cams_denorm.size(); j++) {
        vpgl_calibration_matrix<double> K = input_cams[j].get_calibration();
        vpgl_perspective_camera<double> cnew(K, output_cams_denorm[j].get_rotation(), output_cams_denorm[j].get_translation());
        output_cams[j] = cnew;
      }
    }
  }
  std::cout << " final error min: " << error_min << " final offset: " << offset_min << " tried: " << offsets.size() << " offsets!\n";
  return true;
}


bool vpgl_camera_transform::compute_initial_transformation(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                                           const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                                           const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                                           std::vector<vpgl_perspective_camera<double> >& output_cams)
{
   // find the number of residuals
  unsigned cnt_residuals = 0;
  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      cnt_residuals++;
    }
  }
  std::cout << "number of residuals: " << cnt_residuals << std::endl;

  // normalize the image points
  std::vector<vpgl_perspective_camera<double> > input_cams_norm;
  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > > cam_ids_norm_img_pts;
  normalize_img_pts(input_cams, cam_ids_img_pts, input_cams_norm, cam_ids_norm_img_pts);

  //////////////////////////////////////////////////////////////////////////////////////////////
  // compute X_cam (get rid of K matrix)
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<vnl_matrix_fixed<double, 3, 3> > input_cam_K_invs;
  for (auto & i : input_cams_norm) {
    const vpgl_calibration_matrix<double>& K = i.get_calibration();
    vnl_matrix_fixed<double, 3, 3> Km = K.get_matrix();
    vnl_matrix_fixed<double, 3, 3> Kinv = vnl_inverse(Km);
    input_cam_K_invs.push_back(Kinv);
  }

  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 3>, unsigned> > > cam_ids_norm_cam_pts;
  for (auto & cam_ids_norm_img_pt : cam_ids_norm_img_pts) {
    std::vector < std::pair<vnl_vector_fixed<double, 3>, unsigned> > cam_pts;
    for (unsigned i = 0; i < cam_ids_norm_img_pt.size(); i++) {
      vnl_vector_fixed<double, 3> pt;
      pt[0] = cam_ids_norm_img_pt[i].first[0];
      pt[1] = cam_ids_norm_img_pt[i].first[1];
      pt[2] = 1.0;
      vnl_vector_fixed<double, 3> cam_pt;
      cam_pt = input_cam_K_invs[cam_ids_norm_img_pt[i].second]*pt;
      std::pair<vnl_vector_fixed<double, 3>, unsigned> pair(cam_pt, cam_ids_norm_img_pt[i].second);
      cam_pts.push_back(pair);
    }
    cam_ids_norm_cam_pts.push_back(cam_pts);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // setup the problem Ax = b
  ////////////////////////////////////////////////////////////////////////
  // two equations per correspondence
  unsigned m = cnt_residuals*2;
  // there are 12 unknowns in R matrix and t vector
  unsigned n = 12;
  vnl_sparse_matrix<double> A(m,n); vnl_vector<double> B(m);
  // setup rows of A and b
  unsigned cnt = 0;
  for (unsigned j = 0; j < cam_ids_norm_cam_pts.size(); j++) {
    double X = pts_3d[j][0], Y = pts_3d[j][1], Z = pts_3d[j][2];

    for (auto & i : cam_ids_norm_cam_pts[j]) {
      vnl_vector_fixed<double, 3> pt = i.first;
      double x = pt[0]; double y = pt[1]; double z = pt[2];
      double a = x/z;  double b = y/z;

      vnl_matrix_fixed<double, 3, 3> R = input_cams_norm[i.second].get_rotation().as_matrix();
      double r1 = R[0][0], r2 = R[0][1], r3 = R[0][2];
      double r4 = R[1][0], r5 = R[1][1], r6 = R[1][2];
      double r7 = R[2][0], r8 = R[2][1], r9 = R[2][2];
      vgl_vector_3d<double> t = input_cams_norm[i.second].get_translation();
      double t1 = t.x(), t2 = t.y(), t3 = t.z();

      A(cnt, 0) = r7*X*a-r1*X;   A(cnt, 1)  = r7*Y*a-r1*Y; A(cnt, 2)  = r7*Z*a-r1*Z;
      A(cnt, 3) = r8*X*a-r2*X;   A(cnt, 4)  = r8*Y*a-r2*Y; A(cnt, 5)  = r8*Z*a-r2*Z;
      A(cnt, 6) = r9*X*a-r3*X;   A(cnt, 7)  = r9*Y*a-r3*Y; A(cnt, 8)  = r9*Z*a-r3*Z;
      A(cnt, 9) = r7*a-r1;       A(cnt, 10) = r8*a-r2;     A(cnt, 11) = r9*a-r3;
      B[cnt] = t1 - t3*a;
      cnt++;

      A(cnt, 0) = r7*X*b-r4*X;   A(cnt, 1)  = r7*Y*b-r4*Y; A(cnt, 2)  = r7*Z*b-r4*Z;
      A(cnt, 3) = r8*X*b-r5*X;   A(cnt, 4)  = r8*Y*b-r5*Y; A(cnt, 5)  = r8*Z*b-r5*Z;
      A(cnt, 6) = r9*X*b-r6*X;   A(cnt, 7)  = r9*Y*b-r6*Y; A(cnt, 8)  = r9*Z*b-r6*Z;
      A(cnt, 9) = r7*b-r4;       A(cnt, 10) = r8*b-r5;     A(cnt, 11) = r9*b-r6;
      B[cnt] = t2 - t3*b;
      cnt++;
    }
  }

  vnl_sparse_matrix_linear_system<double> ls(A,B);
  vnl_vector<double> unknowns(n, 0.0);
  unknowns[0] = 1; unknowns[4] = 1; unknowns[8] = 1;
  vnl_lsqr lsqr(ls); lsqr.minimize(unknowns);
  std::cout << "unknowns: " << unknowns << std::endl;

  vnl_matrix_fixed<double, 3, 3> R_fixed;
  R_fixed[0][0] = unknowns[0]; R_fixed[0][1] = unknowns[1]; R_fixed[0][2] = unknowns[2];
  R_fixed[1][0] = unknowns[3]; R_fixed[1][1] = unknowns[4]; R_fixed[1][2] = unknowns[5];
  R_fixed[2][0] = unknowns[6]; R_fixed[2][1] = unknowns[7]; R_fixed[2][2] = unknowns[8];

  std::cout << " R_fixed: " << R_fixed << std::endl; std::cout << " det of R_fixed: " << vnl_det(R_fixed) << std::endl;

  //vnl_matrix_fixed<double, 3, 3> R_fixed_norm;
  //normalize_to_rotation_matrix(R_fixed, R_fixed_norm);

  //std::cout << " R_fixed_norm: " << R_fixed_norm << std::endl; std::cout << " det of R_fixed_norm: " << vnl_det(R_fixed_norm) << std::endl;

  vnl_vector_fixed<double, 3> t_fixed(unknowns[9], unknowns[10], unknowns[11]);
  std::cout << " t_fixed: " << t_fixed << std::endl;
  /////////////////////////////////////////////////////////////////////////

  // denormalize and compute R and t
  for (const auto & input_cam : input_cams) {
    const vpgl_calibration_matrix<double>& K = input_cam.get_calibration();
    vnl_matrix_fixed<double, 3, 3> R = input_cam.get_rotation().as_matrix();
    vgl_vector_3d<double> t = input_cam.get_translation();
    vnl_vector_fixed<double, 3> tv;
    tv[0] = t.x(); tv[1] = t.y(); tv[2] = t.z();

    //vnl_matrix_fixed<double, 3, 3> R_new = R*R_fixed_norm;
    vnl_matrix_fixed<double, 3, 3> R_new = R*R_fixed;
    vnl_vector_fixed<double, 3> t_new = R*t_fixed + tv;

    vgl_rotation_3d<double> R_newr(R_new);
    vgl_vector_3d<double> t_newv(t_new[0], t_new[1], t_new[2]);

    vpgl_perspective_camera<double> cnew(K, R_newr, t_newv);
    output_cams.push_back(cnew);

    std::cout << " old center: " << input_cam.get_camera_center() << " new center: " << cnew.get_camera_center();
    std::cout << " move by: " << (input_cam.get_camera_center()-cnew.get_camera_center()).length() << std::endl;
  }

  return true;
}


bool vpgl_camera_transform::compute_initial_transformation_t(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                                             const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                                             const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                                             std::vector<vpgl_perspective_camera<double> >& output_cams)
{
   // find the number of residuals
  unsigned cnt_residuals = 0;
  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      cnt_residuals++;
    }
  }
  std::cout << "number of residuals: " << cnt_residuals << std::endl;

  // normalize the image points
  std::vector<vpgl_perspective_camera<double> > input_cams_norm;
  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > > cam_ids_norm_img_pts;
  normalize_img_pts(input_cams, cam_ids_img_pts, input_cams_norm, cam_ids_norm_img_pts);

  //////////////////////////////////////////////////////////////////////////////////////////////
  // compute X_cam (get rid of K matrix)
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<vnl_matrix_fixed<double, 3, 3> > input_cam_K_invs;
  for (auto & i : input_cams_norm) {
    const vpgl_calibration_matrix<double>& K = i.get_calibration();
    vnl_matrix_fixed<double, 3, 3> Km = K.get_matrix();
    vnl_matrix_fixed<double, 3, 3> Kinv = vnl_inverse(Km);
    input_cam_K_invs.push_back(Kinv);
  }

  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 3>, unsigned> > > cam_ids_norm_cam_pts;
  for (auto & cam_ids_norm_img_pt : cam_ids_norm_img_pts) {
    std::vector < std::pair<vnl_vector_fixed<double, 3>, unsigned> > cam_pts;
    for (unsigned i = 0; i < cam_ids_norm_img_pt.size(); i++) {
      vnl_vector_fixed<double, 3> pt;
      pt[0] = cam_ids_norm_img_pt[i].first[0];
      pt[1] = cam_ids_norm_img_pt[i].first[1];
      pt[2] = 1.0;
      vnl_vector_fixed<double, 3> cam_pt;
      cam_pt = input_cam_K_invs[cam_ids_norm_img_pt[i].second]*pt;
      std::pair<vnl_vector_fixed<double, 3>, unsigned> pair(cam_pt, cam_ids_norm_img_pt[i].second);
      cam_pts.push_back(pair);
    }
    cam_ids_norm_cam_pts.push_back(cam_pts);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // setup the problem Ax = b
  ////////////////////////////////////////////////////////////////////////
  // two equations per correspondence
  unsigned m = cnt_residuals*2;
  // there are 3 unknowns in t vector
  unsigned n = 3;
  vnl_sparse_matrix<double> A(m,n); vnl_vector<double> B(m);
  // setup rows of A and b
  unsigned cnt = 0;
  for (unsigned j = 0; j < cam_ids_norm_cam_pts.size(); j++) {
    double X = pts_3d[j][0], Y = pts_3d[j][1], Z = pts_3d[j][2];

    for (auto & i : cam_ids_norm_cam_pts[j]) {
      vnl_vector_fixed<double, 3> pt = i.first;
      double x = pt[0]; double y = pt[1]; double z = pt[2];
      double a = x/z;  double b = y/z;

      vnl_matrix_fixed<double, 3, 3> R = input_cams_norm[i.second].get_rotation().as_matrix();
      double r1 = R[0][0], r2 = R[0][1], r3 = R[0][2];
      double r4 = R[1][0], r5 = R[1][1], r6 = R[1][2];
      double r7 = R[2][0], r8 = R[2][1], r9 = R[2][2];
      vgl_vector_3d<double> t = input_cams_norm[i.second].get_translation();
      double t1 = t.x(), t2 = t.y();

      A(cnt, 0) = r7*a-r1;
      A(cnt, 1) = r8*a-r2;
      A(cnt, 2) = r9*a-r3;

      B[cnt] = r1*X + r2*Y + r3*Z + t1 - r7*X*a - r8*Y*a - r9*Z*a;
      cnt++;

      A(cnt, 0) = r7*b-r4;
      A(cnt, 1) = r8*b-r5;
      A(cnt, 2) = r9*b-r6;
      B[cnt] = r4*X + r5*Y + r6*Z + t2 - r7*X*b - r8*Y*b - r9*Z*b;
      cnt++;
    }
  }

  vnl_sparse_matrix_linear_system<double> ls(A,B);
  vnl_vector<double> unknowns(n, 0.0);
  vnl_lsqr lsqr(ls); lsqr.minimize(unknowns);
  std::cout << "unknowns: " << unknowns << std::endl;

  vnl_vector_fixed<double, 3> t_fixed(unknowns[0], unknowns[1], unknowns[2]);
  std::cout << " t_fixed: " << t_fixed << std::endl;
  /////////////////////////////////////////////////////////////////////////

  // denormalize and compute R and t
  for (const auto & input_cam : input_cams) {
    const vpgl_calibration_matrix<double>& K = input_cam.get_calibration();
    const vgl_rotation_3d<double>& R = input_cam.get_rotation();
    vgl_vector_3d<double> t = input_cam.get_translation();
    vnl_vector_fixed<double, 3> tv;
    tv[0] = t.x(); tv[1] = t.y(); tv[2] = t.z();

    vnl_vector_fixed<double, 3> t_new = R*t_fixed + tv;
    vgl_vector_3d<double> t_newv(t_new[0], t_new[1], t_new[2]);

    vpgl_perspective_camera<double> cnew(K, R, t_newv);

    std::cout << " old center: " << input_cam.get_camera_center() << " new center: " << cnew.get_camera_center();
    std::cout << " move by: " << (input_cam.get_camera_center()-cnew.get_camera_center()).length() << std::endl;

    output_cams.push_back(cnew);
  }

  return true;
}

bool vpgl_camera_transform::normalize_to_rotation_matrix(const vnl_matrix_fixed<double, 3, 3>& R, vnl_matrix_fixed<double, 3, 3>& R_norm)
{
  vnl_matrix<double> temp = R.transpose()*R;  // this is symmetric

  vnl_matrix<double> Dreal(3,3,0.0), Vreal(3,3,0.0);
  vnl_vector<double> D(3, 0.0);

  // find square root of temp using eigendecomposition
  if (!vnl_symmetric_eigensystem_compute(temp, Vreal, D)) {
    std::cerr << "In vpgl_camera_transform::normalize_to_rotation_matrix() -- cannot compute eigendecomposition!\n";
    return false;
  }
  for (unsigned i = 0; i < 3; i++) {
    if (std::abs(D[i]) < std::numeric_limits<double>::epsilon())
      D[i] = 1.0;
    Dreal[i][i] = 1.0/std::sqrt(D[i]);
  }
  std::cout << "D real:\n " << Dreal << std::endl;
  std::cout << "V real:\n " << Vreal << std::endl;

  vnl_matrix<double> out;
  out = Vreal*Dreal*Vreal.transpose();
  std::cout << "( (R^t*R)^(1/2) )^-1:\n " << out << std::endl;
  out = R*out;

  for (unsigned i = 0; i < 3; i++)
    for (unsigned j = 0; j < 3; j++)
      R_norm[i][j] = out[i][j];

  return true;
}

// use quaternions
bool vpgl_camera_transform::normalize_to_rotation_matrix_q(const vnl_matrix_fixed<double, 3, 3>& R, vnl_matrix_fixed<double, 3, 3>& R_norm)
{
  vnl_quaternion<double> q(R);
  std::cout << "initial q: " << q << std::endl;
  vnl_quaternion<double> q_n = q.normalize();
  std::cout << "norm q: " << q_n << std::endl;
  vgl_rotation_3d<double> Rn(q_n);
  R_norm = Rn.as_matrix();
  return true;
}

bool vpgl_camera_transform::compute_initial_transformation_R(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                                             const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                                             const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                                             std::vector<vpgl_perspective_camera<double> >& output_cams)
{
  // find the number of residuals
  unsigned cnt_residuals = 0;
  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      cnt_residuals++;
    }
  }
  std::cout << "number of residuals: " << cnt_residuals << std::endl;

  // normalize the image points
  std::vector<vpgl_perspective_camera<double> > input_cams_norm;
  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > > cam_ids_norm_img_pts;
  normalize_img_pts(input_cams, cam_ids_img_pts, input_cams_norm, cam_ids_norm_img_pts);

  //////////////////////////////////////////////////////////////////////////////////////////////
  // compute X_cam (get rid of K matrix)
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<vnl_matrix_fixed<double, 3, 3> > input_cam_K_invs;
  for (auto & i : input_cams_norm) {
    const vpgl_calibration_matrix<double>& K = i.get_calibration();
    vnl_matrix_fixed<double, 3, 3> Km = K.get_matrix();
    vnl_matrix_fixed<double, 3, 3> Kinv = vnl_inverse(Km);
    input_cam_K_invs.push_back(Kinv);
  }

  std::vector< std::vector < std::pair<vnl_vector_fixed<double, 3>, unsigned> > > cam_ids_norm_cam_pts;
  for (auto & cam_ids_norm_img_pt : cam_ids_norm_img_pts) {
    std::vector < std::pair<vnl_vector_fixed<double, 3>, unsigned> > cam_pts;
    for (unsigned i = 0; i < cam_ids_norm_img_pt.size(); i++) {
      vnl_vector_fixed<double, 3> pt;
      pt[0] = cam_ids_norm_img_pt[i].first[0];
      pt[1] = cam_ids_norm_img_pt[i].first[1];
      pt[2] = 1.0;
      vnl_vector_fixed<double, 3> cam_pt;
      cam_pt = input_cam_K_invs[cam_ids_norm_img_pt[i].second]*pt;
      std::pair<vnl_vector_fixed<double, 3>, unsigned> pair(cam_pt, cam_ids_norm_img_pt[i].second);
      cam_pts.push_back(pair);
    }
    cam_ids_norm_cam_pts.push_back(cam_pts);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // setup the problem Ax = b
  ////////////////////////////////////////////////////////////////////////
  // two equations per correspondence
  unsigned m = cnt_residuals*2;
  // there are 9 unknowns in R matrix
  unsigned n = 9;
  vnl_sparse_matrix<double> A(m,n);
  //vnl_matrix<double> A(m,n);
  vnl_vector<double> B(m);
  // setup rows of A and b
  unsigned cnt = 0;
  for (unsigned j = 0; j < cam_ids_norm_cam_pts.size(); j++) {
    double X = pts_3d[j][0], Y = pts_3d[j][1], Z = pts_3d[j][2];

    for (auto & i : cam_ids_norm_cam_pts[j]) {
      vnl_vector_fixed<double, 3> pt = i.first;
      double x = pt[0]; double y = pt[1]; double z = pt[2];
      double a = x/z;  double b = y/z;

      vnl_matrix_fixed<double, 3, 3> R = input_cams_norm[i.second].get_rotation().as_matrix();
      double r1 = R[0][0], r2 = R[0][1], r3 = R[0][2];
      double r4 = R[1][0], r5 = R[1][1], r6 = R[1][2];
      double r7 = R[2][0], r8 = R[2][1], r9 = R[2][2];
      vgl_vector_3d<double> t = input_cams_norm[i.second].get_translation();
      double t1 = t.x(), t2 = t.y(), t3 = t.z();

      A(cnt, 0) = r7*X*a-r1*X;   A(cnt, 1)  = r7*Y*a-r1*Y; A(cnt, 2)  = r7*Z*a-r1*Z;
      A(cnt, 3) = r8*X*a-r2*X;   A(cnt, 4)  = r8*Y*a-r2*Y; A(cnt, 5)  = r8*Z*a-r2*Z;
      A(cnt, 6) = r9*X*a-r3*X;   A(cnt, 7)  = r9*Y*a-r3*Y; A(cnt, 8)  = r9*Z*a-r3*Z;
      B[cnt] = t1 - t3*a;
      cnt++;

      A(cnt, 0) = r7*X*b-r4*X;   A(cnt, 1)  = r7*Y*b-r4*Y; A(cnt, 2)  = r7*Z*b-r4*Z;
      A(cnt, 3) = r8*X*b-r5*X;   A(cnt, 4)  = r8*Y*b-r5*Y; A(cnt, 5)  = r8*Z*b-r5*Z;
      A(cnt, 6) = r9*X*b-r6*X;   A(cnt, 7)  = r9*Y*b-r6*Y; A(cnt, 8)  = r9*Z*b-r6*Z;
      B[cnt] = t2 - t3*b;
      cnt++;
    }
  }
  vnl_vector<double> unknowns(n, 0.0);

  vnl_sparse_matrix_linear_system<double> ls(A,B);
  unknowns[0] = 1; unknowns[4] = 1; unknowns[8] = 1;
  vnl_lsqr lsqr(ls); lsqr.minimize(unknowns);
  /*vnl_qr<double> ls(A);
  unknowns = ls.solve(B);*/
  std::cout << "unknowns: " << unknowns << std::endl;

  vnl_matrix_fixed<double, 3, 3> R_fixed;
  R_fixed[0][0] = unknowns[0]; R_fixed[0][1] = unknowns[1]; R_fixed[0][2] = unknowns[2];
  R_fixed[1][0] = unknowns[3]; R_fixed[1][1] = unknowns[4]; R_fixed[1][2] = unknowns[5];
  R_fixed[2][0] = unknowns[6]; R_fixed[2][1] = unknowns[7]; R_fixed[2][2] = unknowns[8];

  std::cout << " R_fixed: " << R_fixed << std::endl; std::cout << " det of R_fixed: " << vnl_det(R_fixed) << std::endl;

  vnl_matrix_fixed<double, 3, 3> R_fixed_norm;
  normalize_to_rotation_matrix(R_fixed, R_fixed_norm);
  //normalize_to_rotation_matrix_q(R_fixed, R_fixed_norm);

  std::cout << " R_fixed_norm: " << R_fixed_norm << std::endl; std::cout << " det of R_fixed_norm: " << vnl_det(R_fixed_norm) << std::endl;

  /////////////////////////////////////////////////////////////////////////

  // denormalize and compute R and t
  for (const auto & input_cam : input_cams) {
    const vpgl_calibration_matrix<double>& K = input_cam.get_calibration();
    vnl_matrix_fixed<double, 3, 3> R = input_cam.get_rotation().as_matrix();
    vgl_vector_3d<double> t = input_cam.get_translation();
    vnl_vector_fixed<double, 3> tv;
    tv[0] = t.x(); tv[1] = t.y(); tv[2] = t.z();


    vnl_matrix_fixed<double, 3, 3> R_new = R*R_fixed_norm;
    //vnl_matrix_fixed<double, 3, 3> R_new = R*R_fixed;

    vgl_rotation_3d<double> R_newr(R_new);

    vpgl_perspective_camera<double> cnew(K, input_cam.get_camera_center(), R_newr);
    output_cams.push_back(cnew);

    std::cout << " old center: " << input_cam.get_camera_center() << " new center: " << cnew.get_camera_center();
    std::cout << " move by: " << (input_cam.get_camera_center()-cnew.get_camera_center()).length() << std::endl;
    std::cout << " old t: " << input_cam.get_translation() << " new t: " << cnew.get_translation() << std::endl;
  }

  return true;
}

//: apply fixeld transformation
void vpgl_camera_transform::apply_fixed_transformation(const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                                                       vnl_matrix_fixed<double,3,3>& R_fixed, vgl_point_3d<double>& t_fixed,
                                                       std::vector<vpgl_perspective_camera<double>  >& output_cams)
{
  for (const auto & input_cam : input_cams) {
    const vpgl_calibration_matrix<double>& K = input_cam.get_calibration();
    vnl_matrix_fixed<double, 3, 3> R = input_cam.get_rotation().as_matrix();
    vgl_vector_3d<double> t = input_cam.get_translation();
    vnl_vector_fixed<double, 3> tv;
    tv[0] = t.x(); tv[1] = t.y(); tv[2] = t.z();


    vnl_matrix_fixed<double, 3, 3> R_new = R_fixed*R;

    vgl_rotation_3d<double> R_newr(R_new);

    vgl_point_3d<double> cent(input_cam.get_camera_center().x() + t_fixed.x(),
                              input_cam.get_camera_center().x() + t_fixed.y(),
                              input_cam.get_camera_center().x() + t_fixed.z());

    vpgl_perspective_camera<double> cnew(K, cent, R_newr);
    output_cams.push_back(cnew);

    std::cout << " old center: " << input_cam.get_camera_center() << " new center: " << cnew.get_camera_center();
    std::cout << " move by: " << (input_cam.get_camera_center()-cnew.get_camera_center()).length() << std::endl;
    std::cout << " old t: " << input_cam.get_translation() << " new t: " << cnew.get_translation() << std::endl;
  }

}

//: normalize the points using the inverse of the K matrix
void vpgl_camera_transform::K_normalize_img_pts(const std::vector<vpgl_perspective_camera<double> >& input_cams, vnl_matrix_fixed<double, 3, 3> const& input_correspondence_covariance,
                                                const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                                std::vector< std::vector < std::pair< std::pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> >, unsigned> > >& cam_ids_img_pts_norm)
{
  //////////////////////////////////////////////////////////////////////////////////////////////
  // compute X_cam (get rid of K matrix)
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<vnl_matrix_fixed<double, 3, 3> > input_cam_K_invs;
  for (const auto & input_cam : input_cams) {
    const vpgl_calibration_matrix<double>& K = input_cam.get_calibration();
    vnl_matrix_fixed<double, 3, 3> Km = K.get_matrix();
    vnl_matrix_fixed<double, 3, 3> Kinv = vnl_inverse(Km);
    input_cam_K_invs.push_back(Kinv);
  }

  //std::vector< std::vector < std::pair<vnl_vector_fixed<double, 3>, unsigned> > > cam_ids_norm_cam_pts;
  for (const auto & cam_ids_img_pt : cam_ids_img_pts) {
    std::vector < std::pair< std::pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> >, unsigned> > cam_pts;
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      vnl_vector_fixed<double, 3> pt;
      pt[0] = cam_ids_img_pt[i].first[0];
      pt[1] = cam_ids_img_pt[i].first[1];
      pt[2] = 1.0;
      vnl_vector_fixed<double, 3> cam_pt;
      cam_pt = input_cam_K_invs[cam_ids_img_pt[i].second]*pt;
      //std::pair<vnl_vector_fixed<double, 3>, unsigned> pair(cam_pt, cam_ids_img_pts[j][i].second);

      // also compute the covariance matrix of the normalized point using error propagation
      vnl_matrix_fixed<double, 3, 3> KinvT = input_cam_K_invs[cam_ids_img_pt[i].second].transpose();
      vnl_matrix_fixed<double, 3, 3> temp = input_correspondence_covariance*KinvT;
      vnl_matrix_fixed<double, 3, 3> correspondence_covariance = input_cam_K_invs[cam_ids_img_pt[i].second]*temp;

      std::pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> > pair(cam_pt, correspondence_covariance);
      std::pair< std::pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> >, unsigned> pair2(pair, cam_ids_img_pt[i].second);

      cam_pts.push_back(pair2);
    }
    cam_ids_img_pts_norm.push_back(cam_pts);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////

}

//: pass the ids of cams in the input_cams vector, this method computes the variance between these two using their image correspondences
bool vpgl_camera_transform::compute_covariance(unsigned cam_i, unsigned cam_j, const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                               const std::vector< std::vector < std::pair< std::pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> >, unsigned> > >& cam_ids_img_pts,
                                               vnl_matrix_fixed<double, 3, 3>& rot_variance)
{
  vgl_rotation_3d<double> R = vpgl_persp_cam_relative_orientation(input_cams[cam_i], input_cams[cam_j]);
  vnl_matrix_fixed<double, 3, 3> Rm = R.as_matrix();
  vgl_vector_3d<double> h = vpgl_persp_cam_base_line_vector(input_cams[cam_i], input_cams[cam_j]);
  vnl_vector_fixed<double, 3> hv(h.x(), h.y(), h.z());

  std::vector<vnl_vector_fixed<double, 3> > cam_i_pts;
  std::vector<vnl_matrix_fixed<double, 3, 3> > cam_i_pts_cov;
  std::vector<vnl_vector_fixed<double, 3> > cam_j_pts;
  std::vector<vnl_matrix_fixed<double, 3, 3> > cam_j_pts_cov;

  // for each 3d point
  for (const auto & cam_ids_img_pt : cam_ids_img_pts)
    // for each frame
    for (unsigned i = 0; i < cam_ids_img_pt.size(); i++) {
      //std::cout << "pt: " << cam_ids_img_pts[j][i].first.first << " from cam: " << cam_ids_img_pts[j][i].second << std::endl;
      if (cam_ids_img_pt[i].second == cam_i) {
        cam_i_pts.push_back(cam_ids_img_pt[i].first.first);
        cam_i_pts_cov.push_back(cam_ids_img_pt[i].first.second);
      } if (cam_ids_img_pt[i].second == cam_j) {
        cam_j_pts.push_back(cam_ids_img_pt[i].first.first);
        cam_j_pts_cov.push_back(cam_ids_img_pt[i].first.second);
      }
    }

  if (cam_i_pts.size() != cam_j_pts.size())
   return false;

  rot_variance.fill(0.0);

  for (unsigned i = 0; i < cam_i_pts.size(); i++) {
    //std::cout << cam_i_pts[i] << " corresponds to " << cam_j_pts[i] << std::endl;

    // compute b vector for this correspondences b = (x,Rx')h - (h,Rx')x
    vnl_vector_fixed<double, 3> temp = Rm*(cam_j_pts[i]);
    double prod = dot_product(cam_i_pts[i],temp);
    vnl_vector_fixed<double, 3> hv_new = prod*hv;

    double prod2 = dot_product(hv,temp);
    vnl_vector_fixed<double, 3> hv_new2 = prod2*cam_i_pts[i];

    vnl_vector_fixed<double, 3> b = hv_new - hv_new2;
    std::cout << " \t b: " << b << std::endl;

    vnl_matrix_fixed<double, 3, 3> bb = outer_product(b, b);
    std::cout << " \t bb: \n" << bb << std::endl;

    vnl_vector<double> term11 = vnl_cross_3d(hv, temp);
    vnl_vector<double> term12 = cam_i_pts_cov[i]*term11;
    double t1 = dot_product(term11, term12);

    vnl_vector<double> term21 = vnl_cross_3d(hv, cam_i_pts[i]);
    vnl_matrix_fixed<double, 3, 3> term22_temp = cam_j_pts_cov[i]*Rm.transpose();
    vnl_matrix_fixed<double, 3, 3> term22_m = Rm*term22_temp;
    vnl_vector<double> term22 = term22_m*term21;
    double t2 = dot_product(term21, term22);

    vnl_matrix_fixed<double, 3, 3> hvM(0.0);
    hvM(0,0) = 0.0;   hvM(0,1) = -hv[2]; hvM(0,2) = hv[1];
    hvM(1,0) = hv[2];  hvM(1,1) = 0.0;   hvM(1,2) = -hv[0];
    hvM(2,0) = -hv[1]; hvM(2,1) = hv[0];  hvM(2,2) = 0.0;

    vnl_matrix_fixed<double, 3, 3> term31 = hvM*Rm;
    vnl_matrix_fixed<double, 3, 3> term32 = cam_i_pts_cov[i]*term31;
    vnl_matrix_fixed<double, 3, 3> term33 = term31*cam_j_pts_cov[i];
    vnl_matrix_fixed<double, 3, 3> term34 = term32.transpose()*term33;
    double t3 = vnl_trace(term34); // term34[0][0] + term34[1][1] + term34[2][2];  // trace

    double weight = 1.0/(t1 + t2 + t3);
    std::cout << " t1: " << t1 << " t2: " << t2 << " t3: " << t3 << " weight: " << weight << std::endl;

    rot_variance += weight*bb;
  }

  return true;
}
