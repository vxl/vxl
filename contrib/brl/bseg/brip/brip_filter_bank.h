// This is brl/bseg/brip/brip_filter_bank.h
#ifndef brip_filter_bank_h_
#define brip_filter_bank_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A set of filter operations based on Gaussian derivatives
//
// Provides a set of 2nd derivative filter responses at each pixel over
// scale and orientation. The scale values are spaced at harmonic
// intervals across the range. That is, the scale ratio is the nth root
// of the scale range. The input image is downsampled to form the scale
// pyramid. The anisotropic (in general) 2nd derivative filter is applied
// with the same max and min Gaussian standard deviations, lambda0,
// and lambda1, at each level of the scale pyramid. At each level a search
// over orientations is made and the maximum response is retained in the
// filter response output. If the operator is isotropic, i.e.,
// lambda0 == lambda1, no search over orientations is carried out.
// \verbatim
//  Modifications
//  none
// // \endverbatim
//
//-----------------------------------------------------------------------------
#include <vector>
#include <iostream>
#include <vil/vil_image_view.h>
#include <vil/vil_pyramid_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class brip_filter_bank
{
 public:

  brip_filter_bank(): ni_(0), nj_(0),n_levels_(0), scale_ratio_(1.0),
    lambda0_(1.0f), lambda1_(1.0f), theta_interval_(0.0f),
    cutoff_ratio_(0.01f){}

  brip_filter_bank(unsigned n_levels, double scale_range, float lambda0,
                   float lambda1, float theta_interval, float cuttoff_ratio);


  brip_filter_bank(unsigned n_levels, double scale_range, float lambda0,
                   float lambda1, float theta_interval, float cuttoff_ratio,
                   vil_image_view<float> const& image);

  ~brip_filter_bank() = default;
  //: set image after construction
  void set_image(vil_image_view<float> const& image);
  //: accessors
  unsigned ni() const {return ni_;}
  unsigned nj() const {return nj_;}
  unsigned n_levels() const {return n_levels_;}
  double scale_ratio() const {return scale_ratio_;}
  float lambda0() const {return lambda0_;}
  float lambda1() const {return lambda1_;}
  float theta_interval() const {return theta_interval_;}
  //: the strip width around the image with invalid filter values
  unsigned invalid_border() const;
  //: filter response for scale level
  vil_image_view<float>& response(unsigned int scale_level){
    return filter_responses_[scale_level];}
  //: full set of filter responses
  std::vector<vil_image_view<float> > responses() const{
    return filter_responses_;}
  //: save filter responses as individual images
  bool save_filter_responses(std::string const& dir) const;
  bool load_filter_responses(std::string const& dir, unsigned n_levels);

  void set_params(unsigned n_levels, double scale_range, float lambda0, float lambda1, float theta_interval, float cuttoff_ratio);

 protected:
  //: internal methods
  void construct_scale_pyramid();
  void compute_filter_responses();

  //: members
  unsigned ni_, nj_;
  unsigned n_levels_;
  double scale_ratio_;
  float lambda0_;
  float lambda1_;
  float theta_interval_;
  float cutoff_ratio_;
  vil_pyramid_image_view<float> scale_pyramid_;
  //: signed filter response in direction with maximum response
  std::vector<vil_image_view<float> > filter_responses_;
};

std::ostream&  operator<<(std::ostream& s, brip_filter_bank const& r);

#endif // brip_filter_bank_h_
