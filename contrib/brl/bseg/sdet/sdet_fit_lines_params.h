//----*-c++-*----tells emacs to use C++ mode----------
#ifndef sdet_fit_lines_params_h_
#define sdet_fit_lines_params_h_
//:
// \file
// \brief parameter mixin for sdet_fit_lines
//
// \author
//    Joseph L. Mundy - April 10, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class sdet_fit_lines_params : public gevd_param_mixin
{
 public:
  sdet_fit_lines_params(const double min_fit_length = 10.0,
                        const double rms_distance = 0.15);

 sdet_fit_lines_params(const sdet_fit_lines_params& old_params);
 ~sdet_fit_lines_params(){}

  bool SanityCheck();
 friend
   vcl_ostream& operator<<(vcl_ostream& os, const sdet_fit_lines_params& flp);
 protected:
  void InitParams(double min_fit_length,
                  double rms_distance);
 public:
  //
  // Parameter blocks and parameters
  //
  double min_fit_length_; // the shortest curve to fit a line
  double rms_distance_; // the max rms distance error for the fit
};


#endif // sdet_fit_lines_params_h_
