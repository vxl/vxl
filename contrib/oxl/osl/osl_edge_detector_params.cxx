#include <osl/osl_edge_detector_params.h>
//#include <vcl_sstream.h>
//#include <vcl_fstream.h>


osl_edge_detector_params::osl_edge_detector_params()
  : _sigma(3.0f)
  , _low(6)
  , _gauss_tail(0.015f)
  , _fill_gaps(true)
  , _verbose(false)
{
}

#if 0 // commented out
// from osl_edge_detector
osl_edge_detector_params::osl_edge_detector_params()
  : _sigma(1.0f)
  , _low(6)
  , _gauss_tail(0.05f)
  , grd_hist(false)
  , res(15)
  , _verbose(true)
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

  if (_sigma <= 0)  // Standard deviation of the smoothing kernel
  {
    msg << "ERROR: Value of gaussian smoothing sigma is too low <=0" << ends;
    valid = false;
  }

  if (_low <= 0)    // Noise weighting factor
  {
    msg << "ERROR: Value of noise weight must be >0" << ends;
    valid = false;
  }

  if (_gauss_tail <= 0) // Cutoff for gaussian kernel radius
  {
    msg << "ERROR: Value of gauss tail fraction is too low <= 0" << ends;
    valid = false;
  }
  _error_msg = msg.str();
  return valid;
}
#endif
