// This is gel/vdgl/vdgl_fit_lines_params.h
#ifndef vdgl_fit_lines_params_h_
#define vdgl_fit_lines_params_h_
//:
// \file
// \brief parameter mixin for vdgl_fit_lines
//
// \author
//    Joseph L. Mundy - April 10, 2003
//    Brown University
//
// \verbatim
//  Modifications
//   May 2004 - Peter Vanroose - ported from sdet
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vcl_iosfwd.h>

class vdgl_fit_lines_params : public vbl_ref_count
{
 public:
  int  min_fit_length_; //!< the shortest curve to fit a line; defaults to 10
  double rms_distance_; //!< the max rms distance error for the fit; default 0.1

  vdgl_fit_lines_params(int  min_fit_length = 10,
                        double rms_distance = 0.1)
    : min_fit_length_(min_fit_length), rms_distance_(rms_distance) { SanityCheck(); }

  vdgl_fit_lines_params(const vdgl_fit_lines_params& flp)
    : vbl_ref_count(), min_fit_length_(flp.min_fit_length_), rms_distance_(flp.rms_distance_) {}

 ~vdgl_fit_lines_params() {}

  //: Check that parameters are within acceptable bounds
  bool SanityCheck();
};

vcl_ostream& operator<<(vcl_ostream& os, const vdgl_fit_lines_params& flp);

#endif // vdgl_fit_lines_params_h_
