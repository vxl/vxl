//----*-c++-*----tells emacs to use C++ mode----------
#ifndef vpro_frame_diff_params_h_
#define vpro_frame_diff_params_h_
//:
// \file
// \brief parameter mixin for the vpro_frame_diff_process
//
// \author
//    Joseph L. Mundy - October 24, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class vpro_frame_diff_params : public gevd_param_mixin
{
 public:
  vpro_frame_diff_params(const float thresh = 50.0, 
                         const float level = 0.0,
                         const float range=50.0);

 vpro_frame_diff_params(const vpro_frame_diff_params& old_params);
 ~vpro_frame_diff_params(){}

  bool SanityCheck();
 friend
   vcl_ostream& operator<<(vcl_ostream& os, const vpro_frame_diff_params& rpp);
 protected:
 void InitParams(float thresh, float level, float range);

 public:
  //
  // Parameter blocks and parameters
  //
  float thresh_;        // threshold on frame difference (not used)
  float level_;       // clip level (not used)
  float range_;       // dynamic range for conversion to byte pixels
};


#endif // vpro_frame_diff_params_h_
