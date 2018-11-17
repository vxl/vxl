// This is brl/bseg/bvxm/algo/bvxm_merge_mog.h
#ifndef bvxm_merge_mog_h_
#define bvxm_merge_mog_h_
//:
// \file
// \brief class with methods to merge gaussian mixtures.
// \author Isabel Restrepo
// \date April 20, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bvxm/grid/bvxm_voxel_grid_base_sptr.h>

class bvxm_merge_mog
{
 private:
  //Only floating point are supported for now and mixtures with 3 components.
  //This class can be further templated if necessary
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

 public:

  //Grid operations

  //: Merges the components of the gaussian mixtures at each voxel into a single gaussian, that minimizes KL divergence.
  //  The resulting grid contains unimodal Gaussians.
  static bool kl_merge_grid(const bvxm_voxel_grid_base_sptr& app_grid,  const bvxm_voxel_grid_base_sptr& gauss_app) ;

  //: Converts a mixture of gaussians grid into an unimodal gaussian grid corresponding to the most probable mode
  static bool mpm_grid(const bvxm_voxel_grid_base_sptr& app_grid,  const bvxm_voxel_grid_base_sptr& gauss_app) ;

  //Distribution operations

  //:Merges all the components in the mixture into one component that minimizes the KL divergence between the two distributions
  static void kl_merge(mix_gauss_type const& mixture,bsta_gauss_sf1 &gaussian);
};

#endif
