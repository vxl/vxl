#include "osl_fit_lines_params.h"
#include <vcl_iostream.h>
//:
// \file

osl_fit_lines_params::osl_fit_lines_params(unsigned int min_fit_len,
                                           bool use_sqr,
                                           double thresh,
                                           double th,
                                           bool dc,
                                           bool inc,
                                           unsigned int ignore_end_edgels)
: min_fit_length_(min_fit_len)
  , use_square_fit_(use_sqr)
  , threshold_(thresh)
  , theta_(th)
  , dc_only_(dc)
  , incremtl_(inc)
  , ignore_end_edgels_(ignore_end_edgels)
{}

//: Checks that parameters are within acceptable bounds
bool osl_fit_lines_params::SanityCheck()
{
  bool valid = true;

  if (min_fit_length_ < 3) {
    vcl_cerr << "ERROR: Min fit length too short: "<< min_fit_length_<<" < 3\0";
    valid = false;
  }

  if (threshold_ > 1.0) {
    vcl_cerr << "ERROR: Fit error is quite large: " << threshold_ << " > 1\0";
    valid = false;
  }

  if (theta_ > 15) {
    vcl_cerr << "ERROR: Value of angle tolerance is too large: "
             << theta_ << " > 15\0";
    valid = false;
  }
  return valid;
}
