#ifndef brip_para_cvrg_params_h_
#define brip_para_cvrg_params_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author  Joe Mundy July 31, 1999
// \brief brip_para_cvrg_params - Parameter block
//
// \verbatim
//  Modifications:
//   Ported to vxl July 01, 2004
//   9 Sept 2004 - Peter Vanroose - removed InitParams
// \endverbatim
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class brip_para_cvrg_params : public gevd_param_mixin
{
 public:
  brip_para_cvrg_params(float sigma = 1, float thresh = 20,
                        float gauss_tail = 0.015,
                        int proj_width =7, int proj_height=2,
                        int sup_radius = 1, bool verbose = false)
  : sigma_(sigma), thresh_(thresh), gauss_tail_(gauss_tail),
    proj_width_(proj_width), proj_height_(proj_height),
    sup_radius_(sup_radius), verbose_(verbose) {}

  brip_para_cvrg_params(brip_para_cvrg_params const& pcp)
  : gevd_param_mixin(), sigma_(pcp.sigma_), thresh_(pcp.thresh_),
    gauss_tail_(pcp.gauss_tail_), proj_width_(pcp.proj_width_),
    proj_height_(pcp.proj_height_), sup_radius_(pcp.sup_radius_),
    verbose_(pcp.verbose_) {}

  bool SanityCheck();

  //
  float sigma_;       //!< Standard deviation of the smoothing kernel
  float thresh_;      //!< Low hysteresis threshold
  float gauss_tail_;  //!< Used in determining the convolution kernel
  int proj_width_;    //!< 1/2 the projection region width
  int proj_height_;   //!< 1/2 the projection region height
  int sup_radius_;    //!< The non_maximum suppression kernel width.
  bool verbose_;      //!< output debug messages
};

vcl_ostream& operator<<(vcl_ostream& os, brip_para_cvrg_params const& pcp);

#endif
