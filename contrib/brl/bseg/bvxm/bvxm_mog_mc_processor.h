// This is brl/bseg/bvxm/bvxm_mog_mc_processor.h
#ifndef bvxm_mog_mc_processor_h_
#define bvxm_mog_mc_processor_h_
//:
// \file
// \brief A mixture of gaussian appearance model processor for multi-channel imagery
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date August 1, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "grid/bvxm_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


//:
// This class is templated over the dimension of gaussian and the number of modes in the mixture.
// The first argument may correspond to the number of planes on an image. The second argument is
// determined by the user based on the data.

template <unsigned int dim, unsigned int modes> class bvxm_mog_mc_processor
{
 protected:

  typedef bsta_num_obs<bsta_gaussian_indep<float,dim> > gauss_type;
  typedef bsta_mixture_fixed<gauss_type, modes> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

 public:

  typedef mix_gauss_type apm_datatype;
  typedef typename gauss_type::vector_type obs_datatype;
  typedef typename gauss_type::math_type obs_mathtype;

  bvxm_mog_mc_processor()= default;

  bvxm_voxel_slab<float>  prob_density(bvxm_voxel_slab<apm_datatype> const& appear,
                                       bvxm_voxel_slab<obs_datatype> const& obs);

  bvxm_voxel_slab<float> prob_range(bvxm_voxel_slab<apm_datatype> const& appear,
                                    bvxm_voxel_slab<obs_datatype> const& obs_min,
                                    bvxm_voxel_slab<obs_datatype> const& obs_max);

  bool update( bvxm_voxel_slab<apm_datatype> &appear,
               bvxm_voxel_slab<obs_datatype> obs,
               bvxm_voxel_slab<float> weight);

   bvxm_voxel_slab<obs_datatype> expected_color( bvxm_voxel_slab<mix_gauss_type> const& appear);
   bvxm_voxel_slab<obs_datatype> most_probable_mode_color(bvxm_voxel_slab<mix_gauss_type > const& appear);
};

#endif // bvxm_mog_mc_processor_h_
