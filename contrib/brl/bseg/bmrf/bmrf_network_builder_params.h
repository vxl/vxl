//--*-c++-*---
// This is brl/bseg/bmrf/bmrf_network_builder_params.h
#ifndef bmrf_network_builder_params_h_
#define bmrf_network_builder_params_h_
//:
// \file
// \brief parameter mixin for bmrf_network_builder
//
// \author
//    Joseph L. Mundy - January 18, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class bmrf_network_builder_params : public gevd_param_mixin
{
 public:
  bmrf_network_builder_params(const float eu = -610.071,
                              const float ev = 237.358,
                              const int  elu = 1000,
                              const int elv_min =200,
                              const int elv_max =600,
                              const int Ns = 100,
                              const float max_delta_recip_s = 5.0e-5
                              );

 bmrf_network_builder_params(const bmrf_network_builder_params& old_params);
 ~bmrf_network_builder_params(){}

  bool SanityCheck();
 friend
  vcl_ostream& operator<<(vcl_ostream& os, const bmrf_network_builder_params& tp);
 protected:
  void InitParams(float eu,
                  float ev,
                  int elu,
                  int elv_min,
                  int elv_max,
                  int Ns,
                  float max_delta_recip_s
                 );
 public:
  //
  // Parameter blocks and parameters
  //
  float eu_; // col position of the epipole
  float ev_;   //row position of the epipole
  int elu_; //col position of the epipolar parameter space
  int elv_min_; // minimum row position of the epipolar space
  int elv_max_; // maximum row position of the epipolar space
  int Ns_;   // number of intensity samples in s of an unbounded region
  float max_delta_recip_s_;  // maximum difference of reciprocals of s for time neighbors
};

#endif // bmrf_network_builder_params_h_
