// This is oxl/mvl/HMatrix2DComputeMLESAC.cxx
#include "HMatrix2DComputeMLESAC.h"
#include <mvl/HomgOperator2D.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_fastops.h>


HMatrix2DComputeMLESAC::HMatrix2DComputeMLESAC(double std) {
  std_ = std;
}

HMatrix2DComputeMLESAC::~HMatrix2DComputeMLESAC() {}

double HMatrix2DComputeMLESAC::calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count) {
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

double HMatrix2DComputeMLESAC::calculate_residual(HomgPoint2D& one, HomgPoint2D& two, HMatrix2D* H) {
  vnl_double_2 r;
  r[0] = HomgOperator2D::distance_squared(H->transform_to_plane2(one), two);
  r[1] = HomgOperator2D::distance_squared(H->transform_to_plane1(two), one);
#if 0
  vnl_matrix<double> mat; H->get(&mat);
  double const* t = mat.data_block();
  vnl_double_2 p1 = one.get_double2();
  vnl_double_2 p2 = two.get_double2();
  vcl_cerr << H->transform_to_plane2(one).get_double2() << " : " << two.get_double2() << " : " << r[0] << vcl_endl;
  vcl_cerr << H->transform_to_plane1(two).get_double2() << " : " << one.get_double2() << " : " << r[1] << vcl_endl;
  if (r[0] < 100.0 && r[1] < 100.0) {
    double t11 = t[0] - t[6]*p2[0];
    double t12 = t[1] - t[7]*p2[0];
    double t13 = - t[6]*p1[0] - t[7]*p1[1] - t[8];
    double t14 = 0.0;
    double t21 = t[3] - t[6]*p2[1];
    double t22 = t[4] - t[7]*p2[1];
    double t23 = 0.0;
    double t24 = - t[6]*p1[0] - t[7]*p2[1] - t[8];
    vnl_matrix<double> J(2, 4);
    J.put(0, 0, t11);
    J.put(0, 1, t12);
    J.put(0, 2, t13);
    J.put(0, 3, t14);
    J.put(1, 0, t21);
    J.put(1, 1, t22);
    J.put(1, 2, t23);
    J.put(1, 3, t24);

    vnl_matrix<double> res(2, 2, 0.0);
    vnl_fastops::AB(J, J.transpose(), &res);
    vnl_svd<double> svd(res, 1e-12);
    vnl_matrix<double> res1 = svd.inverse();
    vnl_double_2 g = res1*r;
    double d = r[0]*g[0] + r[1]*g[1];
    return d;
  } else
    return 1e+6;
#endif
//vcl_cerr << "r[0] : " << r[0] << " r[1] : " << r[1] << vcl_endl;
  return r[0] + r[1];
}
