#ifndef mbl_linear_interpolator_h_
#define mbl_linear_interpolator_h_
//:
// \file
// \brief Linear interpolation of tabulated data
// \author Graham Vincent

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Linear interpolation of tabulated data
class mbl_linear_interpolator
{
 public:
  mbl_linear_interpolator() ;

  //: Remove all data
  void clear();

  //: Add a (x,y) data
  bool set(const std::vector<double> &x, const std::vector<double> &y);

  //! estimate y and x using linear interpolation. Returns NaN if there is no data
  double y(double x) const;

 private:

  // sorts paired data so that x is monotonics
  void sort();

  // ordered x values
  std::vector<double> x_;

  // ordered y values
  std::vector<double> y_;
};

#endif
