// This is brl/bmvl/brct/brct_plane_sweeper_params.h
#ifndef brct_plane_sweeper_params_h_
#define brct_plane_sweeper_params_h_
//:
// \file
// \brief parameter mixin for brct_plane_sweeper
//
// \author
//    Joseph L. Mundy - February 15, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <sdet/sdet_harris_detector_params.h>
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class brct_plane_sweeper_params : public gevd_param_mixin
{
 public:
  brct_plane_sweeper_params(const float zmin = 0,
                            const float zmax = 0,
                            const int  nz = 2,
                            const float point_radius=1.0,
                            const int corr_radius=2,
                            const float corr_min =-1,
                            const float corr_max =1,
                            const float corr_thresh = 0.25,
                            const float corr_sigma = 1.0,
                            const float  intensity_thresh = 25.0,
                            const sdet_harris_detector_params& hdp =
                                       sdet_harris_detector_params());

  brct_plane_sweeper_params(const brct_plane_sweeper_params& old_params);
 ~brct_plane_sweeper_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&, const brct_plane_sweeper_params& sp);
 protected:
  void InitParams(float zmin,
                  float zmax,
                  int  nz,
                  float point_radius,
                  int corr_radius,
                  float corr_min,
                  float corr_max,
                  float corr_thresh,
                  float corr_sigma,
                  float  intensity_thresh,
                  const sdet_harris_detector_params& hdp);
 public:
  //
  // Parameter blocks and parameters
  //
  float zmin_;
  float zmax_;
  int  nz_;
  float point_radius_;
  int corr_radius_;
  float corr_min_;
  float corr_max_;
  float corr_thresh_;
  float corr_sigma_;
  float  intensity_thresh_;
  sdet_harris_detector_params hdp_;
};

#endif // brct_plane_sweeper_params_h_
