#include <osl/osl_edge_detector_params.h>
//#include <vcl_sstream.h>
//#include <vcl_fstream.h>

//:
//  \file


osl_edge_detector_params::osl_edge_detector_params()
  : sigma_(3.0f)
  , low_(6)
  , gauss_tail_(0.015f)
  , fill_gaps_(true)
  , verbose_(false)
{
}

#if 0 // commented out
// from osl_edge_detector
osl_edge_detector_params::osl_edge_detector_params()
  : sigma_(1.0f)
  , low_(6)
  , gauss_tail_(0.05f)
  , grd_hist(false)
  , res(15)
  , verbose_(true)
{
}
#endif

osl_edge_detector_params::~osl_edge_detector_params() { }

#if 0 // commented out
//-----------------------------------------------------------------------------
//
//: Checks that parameters are within acceptable bounds
//
bool osl_edge_detector_params::SanityCheck()
{
  vcl_strstream msg;
  bool valid = true;

  if (sigma_ <= 0)  // Standard deviation of the smoothing kernel
  {
    msg << "ERROR: Value of gaussian smoothing sigma is too low <=0" << vcl_ends;
    valid = false;
  }

  if (low_ <= 0)    // Noise weighting factor
  {
    msg << "ERROR: Value of noise weight must be >0" << vcl_ends;
    valid = false;
  }

  if (gauss_tail_ <= 0) // Cutoff for gaussian kernel radius
  {
    msg << "ERROR: Value of gauss tail fraction is too low <= 0" << vcl_ends;
    valid = false;
  }
  error_msg_ = msg.str();
  return valid;
}
#endif
