// This is brl/bseg/strk/strk_epipolar_grouper_params.h
#ifndef strk_epipolar_grouper_params_h_
#define strk_epipolar_grouper_params_h_
//:
// \file
// \brief parameter mixin for strk_epipolar_grouper
//
// \author
//    Joseph L. Mundy - December 23, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class strk_epipolar_grouper_params : public gevd_param_mixin
{
 public:
  strk_epipolar_grouper_params(const float eu = -610.071,
                               const float ev = 237.358,
                               const int  elu = 1000,
                               const int elv_min =390,
                               const int elv_max =450,
                               const int Ns = 10,
                               const float angle_thresh = 7.0,
                               const bool  dummy4 = false);

  strk_epipolar_grouper_params(const strk_epipolar_grouper_params& old_params);
 ~strk_epipolar_grouper_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&, const strk_epipolar_grouper_params&);
 protected:
  void InitParams(float eu,
                  float ev,
                  int elu,
                  int elv_min,
                  int elv_max,
                  int Ns,
                  float angle_thresh,
                  bool dummy4);
 public:
  //
  // Parameter blocks and parameters
  //
  float eu_;   //!< col position of the epipole
  float ev_;   //!< row position of the epipole
  int elu_;    //!< col position of the epipolar parameter space
  int elv_min_; //!< minimum row position of the epipolar space
  int elv_max_; //!< maximum col position of the epipolar space
  int Ns_;   //!< number of samples in the s dimension of an unbounded region
  float angle_thresh_;   //!< reject epipolar intersections close to parallel
  bool  dummy4_;  //!< dummy param
};

#endif // strk_epipolar_grouper_params_h_
