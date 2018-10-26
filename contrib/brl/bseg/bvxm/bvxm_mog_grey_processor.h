// This is brl/bseg/bvxm/bvxm_mog_grey_processor.h
#ifndef bvxm_mog_grey_processor_h_
#define bvxm_mog_grey_processor_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor
//
// \author Isabel Restrepo
// \date Jan. 14, 2008
// \verbatim
//  Modifications
//   02/11/2008  DEC  Changed prob() to prob_density(), added prob_range
//   04/17/2008  Ozge C. Ozcanli added most_probable_mode_color() method
//   12/10/2008  Ozge C. Ozcanli added sample() method
//   Added the get_light_bin() virtual function which is useful in multiple mixture of gaussian case
//   to return the appropriate bin number from the lighting direction
// \endverbatim

#include "grid/bvxm_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


// The mix_gauss_type contains the same data as mix_gauss plus an
// extra attribute that indicate the number of observations

class  bvxm_mog_grey_processor
{
 protected:
  static const unsigned n_gaussian_modes_ = 3;

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, n_gaussian_modes_> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
 public:

  typedef mix_gauss_type apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;

  bvxm_mog_grey_processor() = default;
  virtual ~bvxm_mog_grey_processor() = default;

  bvxm_voxel_slab<float>  prob_density(bvxm_voxel_slab<mix_gauss_type> const& appear,
                                       bvxm_voxel_slab<float> const& obs);

  bool  region_prob_density(bvxm_voxel_slab<float> &probabilities,
                            bvxm_voxel_slab<mix_gauss_type> const& appear,
                            bvxm_voxel_slab<float> const& obs,
                            bvxm_voxel_slab<float> const& mask);

  bvxm_voxel_slab<float> prob_range(bvxm_voxel_slab<mix_gauss_type> const& appear,
                                    bvxm_voxel_slab<float> const& obs_min,
                                    bvxm_voxel_slab<float> const& obs_max);

  bool update( bvxm_voxel_slab<mix_gauss_type> &appear,
               bvxm_voxel_slab<float> const& obs,
               bvxm_voxel_slab<float> const& weight);

  bvxm_voxel_slab<float> expected_color( bvxm_voxel_slab<mix_gauss_type> const& appear);
  bvxm_voxel_slab<float> most_probable_mode_color(bvxm_voxel_slab<mix_gauss_type > const& appear);

  bvxm_voxel_slab<float> sample(bvxm_voxel_slab<mix_gauss_type > const& appear);


  // bin number is always 0 for the simple mixture of gaussian case
  virtual unsigned int get_light_bin(unsigned int /*num_light_bins*/, const vnl_vector<float>& /*light*/) { return 0; }
};

#endif // bvxm_mog_grey_processor_h_
