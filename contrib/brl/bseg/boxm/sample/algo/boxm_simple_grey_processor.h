// This is brl/bseg/boxm/sample/algo/boxm_simple_grey_processor.h
#ifndef boxm_simple_grey_processor_h_
#define boxm_simple_grey_processor_h_
//:
// \file
// \brief A class for processing simple, single valued grey-scale appearance models.
//  The represented distribution can be thought of as a delta function centered on the value.
//
// \author Daniel Crispell
// \date Sept 29, 2008
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <boxm/boxm_apm_traits.h>
#if 0 // operator<< in commented-out section
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

class  boxm_simple_grey_processor
{
 protected:
  // convienance typedefs
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype apm_datatype;
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype obs_datatype;
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_mathtype obs_mathtype;

  // all methods are static - no constructor needed
 private:
  boxm_simple_grey_processor() = default;

#if 0  // one_over_sigma is now a member of apm_datatype
  static const float one_over_sigma_;
#endif

 public:

  static float prob_density(apm_datatype const& appear, obs_datatype const& obs);

  static float prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max);

  static float total_prob(apm_datatype const& appear);

  static bool update( apm_datatype &appear, obs_datatype const& obs, float const& weight);

  static boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype expected_color(boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype const& appear);

  static boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_mathtype most_probable_color(boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype const& appear);

  static void compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, std::vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma = 0.01f);
  static void compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, std::vector<float> const& pre, std::vector<float> const& vis, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma=0.01f);

  static void update_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, std::vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void finalize_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, std::vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model);

  //: includes a shadow density component to the single gaussian scalar appearance model, defined by a fixed shadow mean and sigma.
  // The shadow prior defines the probability that an observation is in shadow.
  // The function updates the "model" mean and sigma.
  static void boxm_compute_shadow_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs,
                                             std::vector<float> const& pre,
                                             std::vector<float> const& vis,
                                             boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model,
                                             float min_app_sigma,
                                             float shadow_prior,
                                             float shadow_mean,
                                             float shadow_sigma,
                                             bool verbose = false);
 private:
  static void compute_gaussian_params(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> obs, std::vector<float> weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype &mean, float &sigma);

  static float sigma_norm_factor(unsigned int nobs);

  static float sigma_norm_factor(float nobs);
};

#if 0
std::ostream& operator<<(std::ostream &os, boxm_simple_grey const& apm);
#endif

#endif // boxm_simple_grey_processor_h_
