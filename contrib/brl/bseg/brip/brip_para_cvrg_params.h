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
// \endverbatim
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class brip_para_cvrg_params : public gevd_param_mixin
{
 public:

  brip_para_cvrg_params(float sigma  = 1, float thresh  = 20,
                     float gauss_tail  = 0.015,
                     int proj_width =7, int proj_height=2,
                     int sup_radius = 1,
                     bool verbose = false);

  brip_para_cvrg_params(const brip_para_cvrg_params& old_params);
  bool SanityCheck();

 protected:
  void InitParams(float sigma, float thresh,
                  float gauss_tail,
                  int proj_width, int proj_height,
                  int sup_radius, bool verbose);
  friend
  vcl_ostream& operator<<(vcl_ostream& os, const brip_para_cvrg_params& pcp);
 public:
  //
  float sigma_;       //!< Standard deviation of the smoothing kernel
  float thresh_;      //!< Low hysteresis threshold
  float gauss_tail_;  //!< Used in determining the convolution kernel
  int proj_width_;    //!< 1/2 the projection region width
  int proj_height_;   //!< 1/2 the projection region height
  int sup_radius_;    //!< The non_maximum suppression kernel width.
  bool verbose_;      //!< output debug messages
};

#endif
