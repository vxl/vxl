#include "osl_fit_lines_params.h"
#include <vcl_strstream.h>

osl_fit_lines_params::osl_fit_lines_params(int min_fit_len,
                                           bool use_sqr,
                                           double thresh,
                                           double th,
                                           bool dc,
                                           bool inc,
                                           int ignore_end_edgels)
: _min_fit_length(min_fit_len)
  , _use_square_fit(use_sqr)
  , _threshold(thresh)
  , _theta(th)
  , _dc_only(dc)
  , _incremtl(inc)
  , _ignore_end_edgels(ignore_end_edgels)
{}

// -- Checks that parameters are within acceptable bounds
bool osl_fit_lines_params::SanityCheck() {
#if 0
  strstream msg;
  bool valid = true;

  if (_min_fit_length < 3) {
    msg << "ERROR: Min fit length too short < 3" << ends;
    valid = false;
  }

  if (_threshold > 1.0) {
    msg << "ERROR: Fit error is quit large" << ends;
    valid = false;
  }

  if (_theta > 15) {
    msg << "ERROR: Value of angle tolerance is too large >15" << ends;
    valid = false;
  }
  SetErrorMsg(msg.str());
  //bug delete [] msg.str();
  return valid;
#else
  return true;
#endif
}
