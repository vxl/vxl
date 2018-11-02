#ifndef bnl_parabolic_interpolator_h_
#define bnl_parabolic_interpolator_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Parabolic interpolation of a 1-d point set
//
// \author
//  J.L. Mundy - November 12, 2003
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>

//-----------------------------------------------------------------------------
class bnl_parabolic_interpolator
{
 public:
  bnl_parabolic_interpolator() = default;
  ~bnl_parabolic_interpolator() = default;
  //:p is the parameter to be interpolated, v is the data value
  void add_data_point(const double p, const double v);
  //: reset data array
  void clear();
  //: current number of data points
  int n_points();
  //: Solve linear regression for the parabola
  bool solve();
  //:the peak/valley of the parabola
  double extremum() const {return p_ext_;}
  //: Print the data
  void print();
 private:
  //private methods
  void fill_scatter_matrix();
  //members
  std::vector<double> p_; //parameter values
  std::vector<double> v_; //data values
  vnl_matrix<double> s_; //scatter matrix
  double p_ext_;//extremum values
};

#endif // bnl_parabolic_interpolator_h_
