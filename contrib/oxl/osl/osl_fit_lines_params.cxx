#include "osl_fit_lines_params.h"
#include <vcl_strstream.h>

//:
//  \file

osl_fit_lines_params::osl_fit_lines_params(int min_fit_len,
                                           bool use_sqr,
                                           double thresh,
                                           double th,
                                           bool dc,
                                           bool inc,
                                           int ignore_end_edgels)
: min_fit_length_(min_fit_len)
  , use_square_fit_(use_sqr)
  , threshold_(thresh)
  , theta_(th)
  , dc_only_(dc)
  , incremtl_(inc)
  , ignore_end_edgels_(ignore_end_edgels)
{}

//: Checks that parameters are within acceptable bounds
bool osl_fit_lines_params::SanityCheck() {
#if 0
  vcl_strstream msg;
  bool valid = true;

  if (min_fit_length_ < 3) {
    msg << "ERROR: Min fit length too short < 3" << vcl_ends;
    valid = false;
  }

  if (threshold_ > 1.0) {
    msg << "ERROR: Fit error is quit large" << vcl_ends;
    valid = false;
  }

  if (theta_ > 15) {
    msg << "ERROR: Value of angle tolerance is too large >15" << vcl_ends;
    valid = false;
  }
  SetErrorMsg(msg.str());
  //bug delete [] msg.str();
  return valid;
#else
  return true;
#endif
}
