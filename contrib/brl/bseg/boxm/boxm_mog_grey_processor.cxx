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