#ifndef mbl_linear_interpolator_h_
#define mbl_linear_interpolator_h_

//:
// \file>
// \brief Linear interpolation of tabulated data
// \author Graham Vincent

#include <vcl_vector.h>

// Linear interpolation of tabulated data
class mbl_linear_interpolator
{
public:
  mbl_linear_interpolator() ;

  //: Remove all data
  void clear();

  //: Add a (x,y) data 
  bool set(const vcl_vector<double> &x, const vcl_vector<double> &y);

  //! estimate y and x using linear interpolation. Returns NaN if there is no data
  double y(double x) const;

private:

  // sorts paired data so that x is monotonics
  void sort();

  // ordered x values
  vcl_vector<double> x_;

  // ordered y values
  vcl_vector<double> y_;

};

#endif
