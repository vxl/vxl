// This is brl/bseg/boxm/sample/algo/boxm_mob_grey_processor.h
#ifndef boxm_mob_grey_processor_h_
#define boxm_mob_grey_processor_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-beta processor
//
// \author Gamze Tunali
// \date   Nov 24, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <boxm/boxm_apm_traits.h>


class  boxm_mob_grey_processor
{
 protected:
  // convenience typedefs
  typedef boxm_apm_traits<BOXM_APM_MOB_GREY>::apm_datatype apm_datatype;
  typedef boxm_apm_traits<BOXM_APM_MOB_GREY>::obs_datatype obs_datatype;
  typedef boxm_apm_traits<BOXM_APM_MOB_GREY>::obs_mathtype obs_mathtype;

 public:
  static obs_datatype expected_color(apm_datatype const& appear);
  static float prob_density(apm_datatype const& appear, obs_datatype const& obs);
  static bool update( apm_datatype &appear, obs_datatype const& obs, float const& weight);
};

#endif // boxm_mob_grey_processor_h_
