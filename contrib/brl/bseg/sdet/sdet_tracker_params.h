// This is brl/bseg/sdet/sdet_tracker_params.h
#ifndef sdet_tracker_params_h_
#define sdet_tracker_params_h_
//:
// \file
// \brief parameter mixin for sdet_tracker
//
// \author
//    Joseph L. Mundy - August. 20, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class sdet_tracker_params : public gevd_param_mixin
{
 public:
  sdet_tracker_params(const int n_samples = 10, 
                      const float search_radius = 10.0,
                      const float match_thresh = 10.0
                     );

 sdet_tracker_params(const sdet_tracker_params& old_params);
 ~sdet_tracker_params(){}

  bool SanityCheck();
 friend
  vcl_ostream& operator<<(vcl_ostream& os, const sdet_tracker_params& tp);
 protected:
  void InitParams(int n_samples, 
                  float search_radius = 10.0,
                  float match_thresh = 10.0
                 );
 public:
  //
  // Parameter blocks and parameters
  //
  int n_samples_;          // number of samples generated per iteration
  float search_radius_;    // window size for generating samples
  float match_thresh_;     // threshold for eliminating active matches
};

#endif // sdet_tracker_params_h_
