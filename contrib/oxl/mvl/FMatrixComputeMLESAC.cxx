// This is oxl/mvl/FMatrixComputeMLESAC.cxx
#include "FMatrixComputeMLESAC.h"
#include <mvl/FManifoldProject.h>
#include <mvl/HomgOperator2D.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

FMatrixComputeMLESAC::FMatrixComputeMLESAC(bool rank2_truncate, double std)
{
  rank2_truncate_ = rank2_truncate,
  std_ = std;
  inthresh_ = (1.96*std_)*(1.96*std_);
}

FMatrixComputeMLESAC::~FMatrixComputeMLESAC() {}

// The robust Huber cost function
double FMatrixComputeMLESAC::calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count) {
  double sse = 0.0;
  for (unsigned int i = 0; i < residuals.size(); i++) {
    if (residuals[i] < inthresh_) {
      inlier_list[i] = true;
      sse += residuals[i];
      count++;
    } else {
      inlier_list[i] = false;
      sse += inthresh_;
    }
  }
  return sse;
}

// This Sampson Approximation to the full polynomial correction (Hartley).
// (First Order Geometric Correction)!
double FMatrixComputeMLESAC::calculate_residual(vgl_homg_point_2d<double>& one,
                                                vgl_homg_point_2d<double>& two,
                                                FMatrix* F)
{
  vgl_homg_line_2d<double> l1 = F->image2_epipolar_line(one);
  vgl_homg_line_2d<double> l2 = F->image1_epipolar_line(two);
  return vgl_homg_operators_2d<double>::perp_dist_squared(two, l1)
       + vgl_homg_operators_2d<double>::perp_dist_squared(one, l2);
}

// This Sampson Approximation to the full polynomial correction (Hartley).
// (First Order Geometric Correction)!
double FMatrixComputeMLESAC::calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F)
{
  // I've got some Sampson's first order geometric approximation code here
  // that I have experimented with (ie. approximation to the full solution
  // Hartley 'Triangulation')

#if 0
  double r = 0.0;
  vnl_double_3x3 matrix = F->get_matrix();
  double const* const* mat = matrix.data_array();

  vnl_double_2 p1 = one.get_nonhomogeneous();
  vnl_double_2 p2 = two.get_nonhomogeneous();
  double a11 =  mat[0][0]*p1[0] + mat[1][0]*p1[1] + mat[2][0];
  double a12 =  mat[0][1]*p1[0] + mat[1][1]*p1[1] + mat[2][1];
  double b11 =  mat[0][0]*p2[0] + mat[0][1]*p2[1] + mat[0][2];
  double b12 =  mat[1][0]*p2[0] + mat[1][1]*p2[1] + mat[1][2];
#endif
  HomgLine2D l1 = F->image2_epipolar_line(one);
  HomgLine2D l2 = F->image1_epipolar_line(two);
  double ret = HomgOperator2D::perp_dist_squared(two, l1)
             + HomgOperator2D::perp_dist_squared(one, l2);

#if 0
  double factor = a11*a11 + a12*a12 + b11*b11 + b12*b12;
  if (factor < 0.1)
    factor = 0.1;

  ret /= factor;

  r += ret*b11;
  r += ret*b12;
  r += ret*a11;
  r += ret*a12;
#endif
  return ret;
}
