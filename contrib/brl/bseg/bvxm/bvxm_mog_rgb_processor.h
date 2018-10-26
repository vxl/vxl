// This is brl/bseg/bvxm/bvxm_mog_rgb_processor.h
#ifndef bvxm_mog_rgb_processor_h_
#define bvxm_mog_rgb_processor_h_
//:
// \file
// \brief A class for a rgb-mixture-of-gaussian processor
//
// \author Pradeep
// \date February 22, 2008
// \verbatim
//  Modifications
//   Apr 17, 2008  Ozge C. Ozcanli added most_probable_mode_color() method
// \endverbatim
//

#include "grid/bvxm_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


// The mix_gauss_type contains the same data as mix_gauss plus an
// extra attribute that indicate the number of observations

class  bvxm_mog_rgb_processor
{
 protected:
  static const unsigned n_gaussian_modes_ = 3;

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

 public:

  typedef mix_gauss_type apm_datatype;
  typedef gauss_type::vector_type obs_datatype;
  typedef gauss_type::math_type obs_mathtype;

  bvxm_mog_rgb_processor() = default;

  bvxm_voxel_slab<float>  prob_density(bvxm_voxel_slab<apm_datatype> const& appear,
                                       bvxm_voxel_slab<obs_datatype> const& obs);

  bvxm_voxel_slab<float> prob_range(bvxm_voxel_slab<apm_datatype> const& appear,
                                    bvxm_voxel_slab<obs_datatype> const& obs_min,
                                    bvxm_voxel_slab<obs_datatype> const& obs_max);

  bool update( bvxm_voxel_slab<apm_datatype> &appear,
               bvxm_voxel_slab<obs_datatype> const& obs,
               bvxm_voxel_slab<float> const& weight);

   bvxm_voxel_slab<obs_datatype> expected_color( bvxm_voxel_slab<mix_gauss_type> const& appear);
   bvxm_voxel_slab<obs_datatype> most_probable_mode_color(bvxm_voxel_slab<mix_gauss_type > const& appear);

   bvxm_voxel_slab<obs_datatype> sample(bvxm_voxel_slab<mix_gauss_type > const& appear);
};

#endif // bvxm_mog_rgb_processor_h_
