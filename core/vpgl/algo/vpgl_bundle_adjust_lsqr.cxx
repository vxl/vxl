// This is vpgl/algo/vpgl_bundle_adjust_lsqr.cxx
#include <algorithm>
#include <iostream>
#include <utility>
#include "vpgl_bundle_adjust_lsqr.h"
//:
// \file

#include <vnl/vnl_vector_ref.h>
#include <vnl/vnl_double_3.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//: Constructor
vpgl_bundle_adjust_lsqr::
vpgl_bundle_adjust_lsqr(unsigned int num_params_per_a,
                        unsigned int num_params_per_b,
                        unsigned int num_params_c,
                        std::vector<vgl_point_2d<double> >  image_points,
                        const std::vector<std::vector<bool> >& mask)
 : vnl_sparse_lst_sqr_function(mask.size(),num_params_per_a,
                               mask[0].size(),num_params_per_b,
                               num_params_c,mask,2,use_gradient,use_weights),
   image_points_(std::move(image_points)),
   use_covars_(false),
   scale2_(1.0),
   iteration_count_(0)
{
}


//: Constructor
//  Each image point is assigned an inverse covariance (error projector) matrix
// \note image points are not homogeneous because they require finite points to
//       measure projection error
vpgl_bundle_adjust_lsqr::
vpgl_bundle_adjust_lsqr(unsigned int num_params_per_a,
                        unsigned int num_params_per_b,
                        unsigned int num_params_c,
                        const std::vector<vgl_point_2d<double> >& image_points,
                        const std::vector<vnl_matrix<double> >& inv_covars,
                        const std::vector<std::vector<bool> >& mask)
 : vnl_sparse_lst_sqr_function(mask.size(),num_params_per_a,
                               mask[0].size(),num_params_per_b,
                               num_params_c,mask,2,use_gradient,use_weights),
   image_points_(image_points),
   use_covars_(true),
   scale2_(1.0),
   iteration_count_(0)
{
  assert(image_points.size() == inv_covars.size());
  vnl_matrix<double> U(2,2,0.0);
  for (const auto & S : inv_covars)
  {
    if (S(0,0) > 0.0) {
      U(0,0) = std::sqrt(S(0,0));
      U(0,1) = S(0,1)/U(0,0);
      double U11 = S(1,1)-S(0,1)*S(0,1)/S(0,0);
      U(1,1) = (U11>0.0)?std::sqrt(U11):0.0;
    }
    else if (S(1,1) > 0.0) {
      assert(S(0,1) == 0.0);
      U(0,0) = 0.0;
      U(0,1) = 0.0;
      U(1,1) = std::sqrt(S(1,1));
    }
    else {
      std::cout << "warning: not positive definite"<<std::endl;
      U.fill(0.0);
    }
    factored_inv_covars_.push_back(U);
  }
}


//: Compute all the reprojection errors
//  Given the parameter vectors a, b, and c, compute the vector of residuals e.
//  e has been sized appropriately before the call.
//  The parameters in a for each camera are {wx, wy, wz, tx, ty, tz}
//  where w is the Rodrigues vector of the rotation and t is the translation.
//  The parameters in b for each 3D point are {px, py, pz}
//  the non-homogeneous position.
void
vpgl_bundle_adjust_lsqr::f(vnl_vector<double> const& a,
                           vnl_vector<double> const& b,
                           vnl_vector<double> const& c,
                           vnl_vector<double>& e)
{
  for (unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vnl_double_3x4 Pi = param_to_cam_matrix(i,a,c);

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for (auto & r_itr : row)
    {
      unsigned int j = r_itr.second;
      unsigned int k = r_itr.first;

      // Construct the jth point
      vnl_vector_fixed<double,4> Xj = param_to_pt_vector(j,b,c);

      // Project jth point with the ith camera
      vnl_vector_fixed<double,3> xij = Pi*Xj;

      double* eij = e.data_block()+index_e(k);
      eij[0] = xij[0]/xij[2] - image_points_[k].x();
      eij[1] = xij[1]/xij[2] - image_points_[k].y();
      if (use_covars_){
        // multiple this error by upper triangular Sij
        const vnl_matrix<double>& Sij = factored_inv_covars_[k];
        eij[0] *= Sij(0,0);
        eij[0] += eij[1]*Sij(0,1);
        eij[1] *= Sij(1,1);
      }
    }
  }
}


//: Compute the residuals from the ith component of a and the jth component of b.
//  This is not normally used because f() has a self-contained efficient implementation
//  It is used for finite-differencing if the gradient is marked as unavailable
void
vpgl_bundle_adjust_lsqr::fij(int i, int j,
                             vnl_vector<double> const& ai,
                             vnl_vector<double> const& bj,
                             vnl_vector<double> const& c,
                             vnl_vector<double>& fij)
{
  //: Construct the ith camera
  vnl_double_3x4 Pi = param_to_cam_matrix(i,ai.data_block(),c);

  // Construct the jth point
  vnl_vector_fixed<double,4> Xj = param_to_pt_vector(j,bj.data_block(),c);

  // Project jth point with the ith camera
  vnl_vector_fixed<double,3> xij = Pi*Xj;

  int k = residual_indices_(i,j);
  fij[0] = xij[0]/xij[2] - image_points_[k].x();
  fij[1] = xij[1]/xij[2] - image_points_[k].y();
  if (use_covars_){
    // multiple this error by upper triangular Sij
    const vnl_matrix<double>& Sij = factored_inv_covars_[k];
    fij[0] *= Sij(0,0);
    fij[0] += fij[1]*Sij(0,1);
    fij[1] *= Sij(1,1);
  }
}


//: Compute the sparse Jacobian in block form.
void
vpgl_bundle_adjust_lsqr::jac_blocks(vnl_vector<double> const& a,
                                    vnl_vector<double> const& b,
                                    vnl_vector<double> const& c,
                                    std::vector<vnl_matrix<double> >& A,
                                    std::vector<vnl_matrix<double> >& B,
                                    std::vector<vnl_matrix<double> >& C)
{
  for (unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vnl_double_3x4 Pi = param_to_cam_matrix(i,a,c);

    // This is semi const incorrect - there is no vnl_vector_ref_const
    const vnl_vector_ref<double> ai(number_of_params_a(i),
                                    const_cast<double*>(a.data_block())+index_a(i));

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for (auto & r_itr : row)
    {
      unsigned int j = r_itr.second;
      unsigned int k = r_itr.first;
      // This is semi const incorrect - there is no vnl_vector_ref_const
      const vnl_vector_ref<double> bj(number_of_params_b(j),
                                      const_cast<double*>(b.data_block())+index_b(j));

      jac_Aij(i,j,Pi,ai,bj,c,A[k]); // compute Jacobian A_ij
      jac_Bij(i,j,Pi,ai,bj,c,B[k]); // compute Jacobian B_ij
      jac_Cij(i,j,Pi,ai,bj,c,C[k]); // compute Jacobian C_ij
      if (use_covars_){
        const vnl_matrix<double>& Sij = factored_inv_covars_[k];
        A[k] = Sij*A[k];
        B[k] = Sij*B[k];
        C[k] = Sij*C[k];
      }
    }
  }
}


void
vpgl_bundle_adjust_lsqr::compute_weight_ij(int /*i*/, int /*j*/,
                                           vnl_vector<double> const& /*ai*/,
                                           vnl_vector<double> const& /*bj*/,
                                           vnl_vector<double> const& /*c*/,
                                           vnl_vector<double> const& fij,
                                           double& weight)
{
  double u2 = fij.squared_magnitude()/scale2_;

  // Beaton-Tukey
  weight = (u2 > 1.0) ? 0.0 : 1 - u2;

  // Cauchy
  //weight = std::sqrt(1 / (1 + u2));
}


//: compute the 2x3 Jacobian of camera projection with respect to point location df/dpt where $f(pt) = P*pt$
void vpgl_bundle_adjust_lsqr::
jac_inhomg_3d_point(vnl_double_3x4 const& P,
                    vnl_vector<double> const& pt,
                    vnl_matrix<double>& J)
{
  double denom = P(2,0)*pt[0] + P(2,1)*pt[1] + P(2,2)*pt[2] + P(2,3);
  denom *= denom;

  double txy = P(0,0)*P(2,1) - P(0,1)*P(2,0);
  double txz = P(0,0)*P(2,2) - P(0,2)*P(2,0);
  double tyz = P(0,1)*P(2,2) - P(0,2)*P(2,1);
  double tx  = P(0,0)*P(2,3) - P(0,3)*P(2,0);
  double ty  = P(0,1)*P(2,3) - P(0,3)*P(2,1);
  double tz  = P(0,2)*P(2,3) - P(0,3)*P(2,2);

  J(0,0) = ( txy*pt[1] + txz*pt[2] + tx) / denom;
  J(0,1) = (-txy*pt[0] + tyz*pt[2] + ty) / denom;
  J(0,2) = (-txz*pt[0] - tyz*pt[1] + tz) / denom;

  txy = P(1,0)*P(2,1) - P(1,1)*P(2,0);
  txz = P(1,0)*P(2,2) - P(1,2)*P(2,0);
  tyz = P(1,1)*P(2,2) - P(1,2)*P(2,1);
  tx  = P(1,0)*P(2,3) - P(1,3)*P(2,0);
  ty  = P(1,1)*P(2,3) - P(1,3)*P(2,1);
  tz  = P(1,2)*P(2,3) - P(1,3)*P(2,2);

  J(1,0) = ( txy*pt[1] + txz*pt[2] + tx) / denom;
  J(1,1) = (-txy*pt[0] + tyz*pt[2] + ty) / denom;
  J(1,2) = (-txz*pt[0] - tyz*pt[1] + tz) / denom;
}


//: compute the 2x3 Jacobian of camera projection with respect to camera center df/dC where $f(C) = [M | -M*C]*pt$
void vpgl_bundle_adjust_lsqr::jac_camera_center(vnl_double_3x3 const& M,
                                                vnl_vector<double> const& C,
                                                vnl_vector<double> const& pt,
                                                vnl_matrix<double>& J)
{
  // compute by swapping the role of the camera center and point position
  // then reused the jac_inhomg_3d_point code
  vnl_double_3x4 P;
  P.update(M);
  P.set_column(3,-(M*pt));
  jac_inhomg_3d_point(P,C,J);
}


//: compute the 2x3 Jacobian of camera projection with respect to camera rotation df/dr where $f(r) = K*rod_to_matrix(r)*[I | -C]*pt$
//  Here r is a Rodrigues vector, K is an upper triangular calibration matrix
void vpgl_bundle_adjust_lsqr::jac_camera_rotation(vnl_double_3x3 const& K,
                                                  vnl_vector<double> const& C,
                                                  vnl_vector<double> const& r,
                                                  vnl_vector<double> const& pt,
                                                  vnl_matrix<double>& J)
{
  vnl_double_3 t(pt[0]-C[0], pt[1]-C[1], pt[2]-C[2]);

  const double& x = r[0];
  const double& y = r[1];
  const double& z = r[2];
  double x2 = x*x, y2 = y*y, z2 = z*z;
  double m2 = x2 + y2 + z2;

  // special case for the identity rotation
  if (m2 == 0.0)
  {
    double inv_tz2 = 1.0/(t[2]*t[2]);
    J(0,0) = -t[0]*t[1] * inv_tz2;
    J(1,0) = -1 - t[1]*t[1] * inv_tz2;
    J(0,1) = 1 + t[0]*t[0] * inv_tz2;
    J(1,1) = t[0]*t[1] * inv_tz2;
    J(0,2) = -t[1] / t[2];
    J(1,2) = t[0] / t[2];
  }
  else
  {
    double m = std::sqrt(m2);  // Rodrigues magnitude = rotation angle
    double c = std::cos(m);
    double s = std::sin(m);

    // common trig terms
    double ct = (1-c)/m2;
    double st = s/m;

    // derivative coefficients for common trig terms
    // ds = d/dx_i{st}/x_i
    // dc = d/dx_i{ct}/x_i
    double dct = s/(m*m2);
    double dst = c/m2 - dct;
    dct -=  2*(1-c)/(m2*m2);

    double utc = t[2]*x*z + t[1]*x*y - t[0]*(y2+z2);
    double uts = t[2]*y - t[1]*z;
    double vtc = t[0]*x*y + t[2]*y*z - t[1]*(x2+z2);
    double vts = t[0]*z - t[2]*x;
    double wtc = t[0]*x*z + t[1]*y*z - t[2]*(x2+y2);
    double wts = t[1]*x - t[0]*y;

    // projection of the point into normalized homogeneous coordinates
    // should be equal to inv(K)*P*[pt|1]
    double u = ct*utc + st*uts + t[0];
    double v = ct*vtc + st*vts + t[1];
    double w = ct*wtc + st*wts + t[2];

    double w2 = w*w;

    double dw = dct*x*wtc + ct*(t[0]*z - 2*t[2]*x)
              + dst*x*wts + st*t[1];
    J(0,0) = (w*(dct*x*utc + ct*(t[2]*z + t[1]*y)
                  + dst*x*uts) - u*dw)/w2;
    J(1,0) = (w*(dct*x*vtc + ct*(t[0]*y - 2*t[1]*x)
                  + dst*x*vts - st*t[2]) - v*dw)/w2;

    dw = dct*y*wtc + ct*(t[1]*z - 2*t[2]*y)
        + dst*y*wts - st*t[0];
    J(0,1) = (w*(dct*y*utc + ct*(t[1]*x - 2*t[0]*y)
                  + dst*y*uts + st*t[2]) - u*dw)/w2;
    J(1,1) = (w*(dct*y*vtc + ct*(t[0]*x + t[2]*z)
                  + dst*y*vts) - v*dw)/w2;

    dw = dct*z*wtc + ct*(t[0]*x + t[1]*y)
        + dst*z*wts;
    J(0,2) = (w*(dct*z*utc + ct*(t[2]*x - 2*t[0]*z)
                  + dst*z*uts - st*t[1]) - u*dw)/w2;
    J(1,2) = (w*(dct*z*vtc + ct*(t[2]*y - 2*t[1]*z)
                  + dst*z*vts + st*t[0]) - v*dw)/w2;
  }

  // account for the calibration matrix
  J(0,0) *= K(0,0);
  J(0,0) += J(1,0)*K(0,1);
  J(1,0) *= K(1,1);
  J(0,1) *= K(0,0);
  J(0,1) += J(1,1)*K(0,1);
  J(1,1) *= K(1,1);
  J(0,2) *= K(0,0);
  J(0,2) += J(1,2)*K(0,1);
  J(1,2) *= K(1,1);
}


//: Fast conversion of rotation from Rodrigues vector to matrix
vnl_double_3x3
vpgl_bundle_adjust_lsqr::rod_to_matrix(vnl_vector<double> const& r)
{
  double x2 = r[0]*r[0], y2 = r[1]*r[1], z2 = r[2]*r[2];
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
  R(0,1) = R(1,0) = r[0]*r[1]*c;
  R(0,2) = R(2,0) = r[0]*r[2]*c;
  R(1,2) = R(2,1) = r[1]*r[2]*c;

  double t = r[0]*s;
  R(1,2) -= t;
  R(2,1) += t;
  t = r[1]*s;
  R(0,2) += t;
  R(2,0) -= t;
  t = r[2]*s;
  R(0,1) -= t;
  R(1,0) += t;

  return R;
}
