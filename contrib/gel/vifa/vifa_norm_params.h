// This is gel/vifa/vifa_norm_params.h
#ifndef VIFA_NORM_PARAMS_H
#define VIFA_NORM_PARAMS_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Parameter mixin for intensity normalization.
//
// These parameters govern a linear normalization of intensity values from
// some arbitrary range to [0,1].  If the raw intensity range is the x-axis,
// and the normalized range is the y-axis, then two points, "high" and "low",
// are given.  A line is fit to these two points, and the two x-coordinates
// where y==0 and y==1 are the intensity minimum and maximum clip points,
// imin_ and imax_.  The context is that the y-coordinates of high and low
// points are found by histogramming the source image, and picking the (say)
// 5% and 95% points.
//
// The defaults are interpreted by the normalize() routine as a no-op: no
// normalization is performed.
//
// \author Roddy Collins, from DDB in TargetJr
//
// \date Oct 2001
//
// \verbatim
//  Modifications:
//   MPP Mar 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vil/vil_image_view_base.h>
#include <vul/vul_timestamp.h>
#include <gevd/gevd_param_mixin.h>


class vifa_norm_params : public gevd_param_mixin,
             public vul_timestamp,
             public vbl_ref_count
{
 public:
  //: x-coordinate of low point (i == intensity)
  float  ilow;

  //: y-coordinate of low point (p == percentage)
  float  plow;

  //: x-coordinate of high point
  float  ihigh;

  //: y-coordinate of high point
  float  phigh;

 private:
  //: Calculated x-coord of y == 0 [low intensity clip point]
  float  imin_;

  //: Calculated x-coord of y == 1 [high intensity clip point]
  float  imax_;

  //: Computed slope of line connecting high & low points
  float  slope_;

  //: Computed y-intercept of line connecting high & low points
  float  b_;

 public:
  //: Default constructor
  vifa_norm_params(float  IntLow = 0.0,
                   float  ProbLow = 0.0,
                   float  IntHigh = 0.0,
                   float  ProbHigh = 0.0
                  );

  //: Copy constructor
  vifa_norm_params(const vifa_norm_params&  old_params);

  //: Force update of clip points & interpolation line
  void recompute(void);

  //: Compute normalized equivalent of given intensity
  float  normalize(float  raw_intensity);

  //: Find an image's low & high intensities for normalization
  static bool  get_norm_bounds(vil_image_view_base*  img,
                               float                 low_bound_pcent,
                               float                 high_bound_pcent,
                               float&                normal_low,
                               float&                normal_high
                              );

  //: Dump the parameters
  void  print_info(void);

 private:
  //: Internal method to update clip points & interpolation line
  void  calculate_clip_points(void);
};

typedef vbl_smart_ptr<vifa_norm_params>  vifa_norm_params_sptr;


#endif  // VIFA_NORM_PARAMS_H
