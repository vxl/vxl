// This is brl/bseg/vpro/vpro_motion_params.h
#ifndef vpro_motion_params_h_
#define vpro_motion_params_h_
//:
// \file
// \brief parameter mixin for the vpro_motion_process
//
// \author
//    Joseph L. Mundy - Aug. 16, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class vpro_motion_params : public gevd_param_mixin
{
 public:
  vpro_motion_params(const float low_range = 0.0,
                     const float high_range = 10000.0,
                     const float smooth_sigma = 1.0);

  vpro_motion_params(const vpro_motion_params& old_params);
 ~vpro_motion_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream& os, const vpro_motion_params& rpp);
 protected:
  void InitParams(float low_range,
                  float high_range,
                  float smooth_sigma);

 public:
  //
  // Parameter blocks and parameters
  //
  float low_range_;    //!< low motion value for display
  float high_range_;   //!< high motion value for display
  float smooth_sigma_; //!< sigma for smoothing the video frames
};

#endif // vpro_motion_params_h_
