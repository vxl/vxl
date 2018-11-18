#ifndef brec_bayesian_propagation_H_
#define brec_bayesian_propagation_H_
//:
// \file
// \brief brec Bayesian propagation algorithm to update probability maps using prior probability distributions on appearance
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>

#include "brec_fg_pair_density.h"
#include "brec_fg_bg_pair_density.h"
#include "brec_bg_pair_density.h"
#include <bsta/bsta_weibull.h>

class brec_bayesian_propagation
{
 public:
  brec_bayesian_propagation(vil_image_view<vxl_byte>& inp, vil_image_view<float>& prob_density) : img_view_(inp), prob_density_(prob_density) { convert_img(); }

  //: constructor for area updates
  brec_bayesian_propagation(vil_image_view<float>& bg_map) : bg_map_(bg_map) {}

  bool initialize_bg_map(float high_prior = 0.9f, float top_percentile = 0.1f);

  bool horizontal_pass(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, brec_bg_pair_density& bgp);
  bool directional_pass(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, brec_bg_pair_density& bgp, int ii, int jj);

  bool horizontal_pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp);
  bool vertical_pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp);
  bool directional_pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, int ii, int jj);

  bool pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, vil_image_view<float>& temp);

  bool run(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, brec_bg_pair_density& bgp, int npasses = 1);
  bool run_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, int npasses = 1);

  bool run_area(vil_image_view<float>& area_map, float lambda1, float k1, float lambda2, float k2);

 protected:
  void convert_img() {
    ni_ = img_view_.ni();
    nj_ = img_view_.nj();
    img_.set_size(ni_, nj_, 1);
    vil_convert_stretch_range_limited(img_view_, img_, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);
  }

 public:

  unsigned ni_;
  unsigned nj_;
  vil_image_view<float> img_;
  vil_image_view<vxl_byte> img_view_;
  vil_image_view<float> prob_density_;

  vil_image_view<float> bg_map_;  // foreground map is always inverse of this map, i.e. fg(i,j) = 1-bg_map(i,j)
};

class brec_area_density
{
 public:
  brec_area_density(float lambda1, float k1, float lambda2, float k2) : back_model_(lambda1, k1), fore_model_(lambda2, k2)  {}

  float back_density(float val);
  float fore_density(float val);
 protected:

  // P(A_d | G_b)  // distribution of Area difference given a foreground glitch on a background
  bsta_weibull<float> back_model_;

  // P(A_d | ~G_b)  // distribution of Area difference given not a foreground glitch on a background
  bsta_weibull<float> fore_model_;          // weibul distribution becomes exponential when k = 1.0f
};


#endif // brec_bayesian_propagation_H_
