// This is oxl/mvl/HMatrix2DComputeMLESAC.cxx
#include "HMatrix2DComputeMLESAC.h"
#include <mvl/HomgOperator2D.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#if 0
#include <vnl/vnl_fastops.h>
#include <vnl/vnl_inverse.h>
#endif

HMatrix2DComputeMLESAC::HMatrix2DComputeMLESAC(double std)
{
  std_ = std;
}

HMatrix2DComputeMLESAC::~HMatrix2DComputeMLESAC() {}

double HMatrix2DComputeMLESAC::calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count)
{
  double inthresh = 5.99*std_*std_;
  double sse = 0.0;
  for (unsigned int i = 0; i < residuals.size(); i++) {
    if (residuals[i] < inthresh) {
      inlier_list[i] = true;
      sse += residuals[i];
      count++;
    } else {
      inlier_list[i] = false;
      sse += inthresh;
    }
  }
  return sse;
}

double HMatrix2DComputeMLESAC::calculate_residual(vgl_homg_point_2d<double>& one,
                                                  vgl_homg_point_2d<double>& two,
                                                  HMatrix2D* H)
{
  vnl_double_2 r;
  r[0] = vgl_homg_operators_2d<double>::distance_squared(H->transform_to_plane2(one), two);
  r[1] = vgl_homg_operators_2d<double>::distance_squared(H->transform_to_plane1(two), one);
  return r[0] + r[1];
}

double HMatrix2DComputeMLESAC::calculate_residual(HomgPoint2D& one, HomgPoint2D& two, HMatrix2D* H)
{
  vnl_double_2 r;
  r[0] = HomgOperator2D::distance_squared(H->transform_to_plane2(one), two);
  r[1] = HomgOperator2D::distance_squared(H->transform_to_plane1(two), one);
#ifdef DEBUG
  vcl_cerr << "r[0] : " << r[0] << " r[1] : " << r[1] << vcl_endl;
#endif
#if 0
  vnl_double_2 p1 = one.get_double2();
  vnl_double_2 p2 = two.get_double2();
  vcl_cerr << H->transform_to_plane2(one) << " : " << p2 << " : " << r[0] << vcl_endl
           << H->transform_to_plane1(two) << " : " << p1 << " : " << r[1] << vcl_endl;
  if (r[0] < 100.0 && r[1] < 100.0)
  {
    vnl_double_3x3 const& mat = H->get_matrix();
    double t13 = - mat(2,0)*p1[0] - mat(2,1)*p1[1] - mat(2,2);
    double t24 = - mat(2,0)*p1[0] - mat(2,1)*p2[1] - mat(2,2);
    vnl_matrix<double> J(2, 4);
    J(0,0) = mat(0,0)-mat(2,0)*p2[0];  J(0,1) = mat(0,1)-mat(2,1)*p2[0];  J(0,2)= t13;  J(0,3)= 0.0;
    J(1,0) = mat(1,0)-mat(2,0)*p2[1];  J(1,1) = mat(1,1)-mat(2,1)*p2[1];  J(1,2)= 0.0;  J(1,3)= t24;

    vnl_matrix<double> res(2,2);
    vnl_fastops::ABt(res, J, J);
    vnl_double_2 g = vnl_inverse(res)*r;
    return r[0]*g[0] + r[1]*g[1];
  }
  else
#endif
  return r[0] + r[1];
}
