// This is brl/bseg/vpro/vpro_fourier_process.h
#ifndef vpro_fourier_process_h_
#define vpro_fourier_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Compute the Fourier transform of a single image
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 28, 2003    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vpro/vpro_video_process.h>
#include <vpro/vpro_fourier_params.h>

class vpro_fourier_process : public vpro_video_process, public vpro_fourier_params
{
  // default constructor is private
  vpro_fourier_process();
 public:
  vpro_fourier_process(vpro_fourier_params vfp) : vpro_fourier_params(vfp) {}
 ~vpro_fourier_process() {}
  virtual process_data_type get_output_type() { return IMAGE; }
  //: difference sequential frames
  virtual bool execute();
  virtual bool finish() { return true; }
};

#endif // vpro_fourier_process_h_
