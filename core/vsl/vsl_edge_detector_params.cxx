#include <vsl/vsl_edge_detector_params.h>
//#include <vcl/vcl_sstream.h>
//#include <vcl/vcl_fstream.h>


vsl_edge_detector_params::vsl_edge_detector_params()
  : _sigma(3.0)
  , _low(6)
  , _gauss_tail(0.015)
  , _fill_gaps(true)
  , _verbose(false)
{
}

// // from vsl_edge_detector
// vsl_edge_detector_params::vsl_edge_detector_params()
//   : _sigma(1.0)
//   , _low(6)
//   , _gauss_tail(0.05)
//   , grd_hist(false)
//   , res(15)
//   , _verbose(true)
// {
// }

vsl_edge_detector_params::~vsl_edge_detector_params() { }


// //-----------------------------------------------------------------------------
// //
// // -- Checks that parameters are within acceptable bounds
// // 
// bool vsl_edge_detector_params::SanityCheck()
// {
//   strstream msg;
//   bool valid = true;

//   if (_sigma <= 0)  	// Standard deviation of the smoothing kernel
//   {
//     msg << "ERROR: Value of gaussian smoothing sigma is too low <=0" << ends;
//     valid = false;
//   }

//   if (_low <= 0)  	// Noise weighting factor
//   {
//     msg << "ERROR: Value of noise weight must be >0" << ends;
//     valid = false;
//   }

//   if (_gauss_tail <= 0)	// Cutoff for gaussian kernel radius
//   {
//     msg << "ERROR: Value of gauss tail fraction is too low <= 0" << ends;
//     valid = false;
//   }
//   _error_msg = msg.str();
//   return valid;
// }

