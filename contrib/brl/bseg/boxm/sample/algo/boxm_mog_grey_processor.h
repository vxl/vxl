// This is brl/bseg/boxm/sample/algo/boxm_mog_grey_processor.h
#ifndef boxm_mog_grey_processor_h_
#define boxm_mog_grey_processor_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor
//
// \author Gamze Tunali
// \date   Apr 03, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>

#include <vnl/vnl_random.h>

#include <boxm/boxm_apm_traits.h>


class  boxm_mog_grey_processor
{
 protected:
  // convienance typedefs
  typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype apm_datatype;
  typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype obs_datatype;
  typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_mathtype obs_mathtype;

 public:
  static obs_datatype expected_color(apm_datatype const& appear);

  static obs_datatype sample(apm_datatype const& appear, vnl_random &rand_gen);

  static float prob_density(apm_datatype const& appear, obs_datatype const& obs);

  static bool update( apm_datatype &appear, obs_datatype const& obs, float const& weight);

  static void compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs, std::vector<float> const& obs_weights, boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model, float min_sigma = 0.01f);
  static void update_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs, std::vector<float> const& obs_weights, boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model, float min_sigma = 0.01f);
  static void finalize_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs, std::vector<float> const& obs_weights, boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs, std::vector<float> const& pre, std::vector<float> const& vis, boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model, float min_sigma = 0.01f);
};

#endif // boxm_mog_grey_processor_h_
