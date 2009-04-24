#include "boxm_mog_grey_processor.h"
//:
// \file
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>

#include <vcl_cassert.h>

//: Expected value
boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype
boxm_mog_grey_processor::expected_color(boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype const& appear)
{
  float total_weight= 0;
  obs_datatype c = 0;
  obs_datatype expected = 0;

  //should be components used
  for (unsigned i = 0; i< appear.num_components(); ++i)
  {
    total_weight += appear.weight(i);
    c += appear.distribution(i).mean() * appear.weight(i);
  }
  if (total_weight > 0.0f) {
    expected = c/total_weight;
  }

  return expected;
}
//: Return probability density of observing pixel values
float boxm_mog_grey_processor::prob_density(apm_datatype const& appear, obs_datatype const& obs)
{
  if (appear.num_components() ==0) {
    return 1.00f;
  }
  else {
    return appear.prob_density(obs);
  }
}

//: Update with a new sample image
bool boxm_mog_grey_processor::update( apm_datatype &appear, obs_datatype const& obs, float const& weight)
{
  // the model
  float init_variance = 0.008f;
  float min_stddev = 0.030f; //TO UNDERSTAND: changed from 0.02 for dinoRing dataset -dec
  float g_thresh = 2.5; // number of std devs from mean sample must be
  bsta_gauss_f1 this_gauss(0.0f, init_variance);

  const unsigned int nmodes = boxm_apm_traits<BOXM_APM_MOG_GREY>::n_gaussian_modes_;

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, nmodes> mix_gauss;

  // the updater
  bsta_mg_grimson_weighted_updater<mix_gauss> updater(this_gauss, nmodes ,g_thresh,min_stddev);

  if (weight > 0) {
    updater(appear, obs, weight);
  }

  return true;
}
