// This is brl/bseg/vpro/vpro_fourier_params.h
#ifndef vpro_fourier_params_h_
#define vpro_fourier_params_h_
//:
// \file
// \brief parameter mixin for the vpro_fourier_process
//
// \author
//    Joseph L. Mundy - October 28, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class vpro_fourier_params : public gevd_param_mixin
{
 public:
  vpro_fourier_params(const float thresh = 50.0f,
                      const float level  = 0.0f,
                      const float range  = 0.05f);

  vpro_fourier_params(const vpro_fourier_params& old_params);

 ~vpro_fourier_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream& os, const vpro_fourier_params& vfp);
 protected:
  void InitParams(float thresh, float level, float range);

 public:
  //
  // Parameter blocks and parameters
  //
  float thresh_; //!< threshold on frame difference (not used)
  float level_;  //!< clip level (not used)
  float range_;  //!< dynamic range for conversion to byte pixels
};

#endif // vpro_fourier_params_h_
