// This is brl/bseg/sdet/sdet_fit_lines_params.h
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
#include <vbl/vbl_ref_count.h>
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class sdet_fit_lines_params : public gevd_param_mixin, public vbl_ref_count
{
 public:
  sdet_fit_lines_params(int  min_fit_length = 10,
                        double rms_distance = 0.1);

  sdet_fit_lines_params(const sdet_fit_lines_params& old_params);
 ~sdet_fit_lines_params() {}

  bool SanityCheck();
 friend
  vcl_ostream& operator<<(vcl_ostream& os, const sdet_fit_lines_params& flp);
 protected:
  void InitParams(int min_fit_length,
                  double rms_distance);
 public:
  //
  // Parameter blocks and parameters
  //
  int  min_fit_length_; //!< the shortest curve to fit a line
  double rms_distance_; //!< the max rms distance error for the fit
};

#endif // sdet_fit_lines_params_h_
