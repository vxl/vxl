// This is brl/bseg/boxm/boxm_simple_grey_processor.h
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

#include "boxm_apm_traits.h"

class  boxm_simple_grey_processor
{
 protected:
  // convienance typedefs
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype apm_datatype;
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype obs_datatype;
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_mathtype obs_mathtype;

  // one_over_sigma now a member of apm_datatype
  //static const float one_over_sigma_;
 public:

  // all methods are static - no constructor needed
  //boxm_simple_grey_processor(){};

  static float prob_density(apm_datatype const& appear, obs_datatype const& obs);

  static float prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max);

  static float total_prob(apm_datatype const& appear);

  static bool update( apm_datatype &appear, obs_datatype const& obs, float const& weight);

  static boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype expected_color(boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype const& appear);

  static boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_mathtype most_probable_color(boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype const& appear);

  static void compute_appearance(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void update_appearance(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void finalize_appearance(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model);

 private:
  static void compute_gaussian_params(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> obs, vcl_vector<float> weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype &mean, float &sigma);

  static float sigma_norm_factor(unsigned int nobs);

  static float sigma_norm_factor(float nobs);
};

//vcl_ostream& operator<<(vcl_ostream &os, boxm_simple_grey const& apm);

#endif // boxm_simple_grey_processor_h_
