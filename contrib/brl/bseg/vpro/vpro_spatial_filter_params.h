// This is brl/bseg/vpro/vpro_spatial_filter_params.h
#ifndef vpro_spatial_filter_params_h_
#define vpro_spatial_filter_params_h_
//:
// \file
// \brief parameter mixin for the vpro_spatial_filter_process
//
// \author
//    Joseph L. Mundy - October 28, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class vpro_spatial_filter_params : public gevd_param_mixin
{
 public:
  vpro_spatial_filter_params(const bool show_filtered_fft = false,
                             const float dir_fx = 53.0f,
                             const float dir_fy = 81.0f,
                             const float f0     = 30.0f,
                             const float radius = 20.0f);

  vpro_spatial_filter_params(const vpro_spatial_filter_params& old_params);
 ~vpro_spatial_filter_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&,const vpro_spatial_filter_params& sfp);
 protected:
  void InitParams(bool show_filtered_fft,
                  float dir_fx, float dir_fy, float f0, float radius);

 public:
  //
  // Parameter blocks and parameters
  //
  bool show_filtered_fft_; //!< output the filtered fft magnitude
  float dir_fx_; //!< x component of blocking filter direction
  float dir_fy_; //!< y component of blocking filter direction
  float f0_;     //!< center frequency of blocking filter (+- f0)
  float radius_; //!< radius of blocking filter lobe
};

#endif // vpro_spatial_filter_params_h_
