#ifndef bnl_quadratic_interpolator_h_
#define bnl_quadratic_interpolator_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Quadratic interpolation of a 2-d point set
//
// \author
//   J.L. Mundy - November 12, 2003
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>

//-----------------------------------------------------------------------------
class bnl_quadratic_interpolator
{
 public:
  bnl_quadratic_interpolator() = default;
  ~bnl_quadratic_interpolator() = default;
  //: px and py are the 2-d parameters, v is the data value
  void add_data_point(const double px, const double py, const double v);
  //: reset data array
  void clear();
  //: number of data points
  int n_points();
  //: compute quadratic linear regression
  bool solve();
  //:Parameters at the peak or valley of quadratic surface
  void extremum(double& px, double& py);
  void print();
 private:
  //private methods
  void fill_scatter_matrix();
  //members
  std::vector<double> px_; //parameter values
  std::vector<double> py_;
  std::vector<double> v_; //data values
  vnl_matrix<double> s_; //scatter matrix
  double px_ext_;//extremum values
  double py_ext_;
};


#endif // bnl_quadratic_interpolator_h_
