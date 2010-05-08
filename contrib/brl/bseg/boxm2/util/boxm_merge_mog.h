// This is brl/bseg/boxm2/util/boxm_merge_mog.h
#ifndef boxm2_merge_mog_h_
#define boxm2_merge_mog_h_
//:
// \file
// \brief class with methods to merge gaussian mixtures.
// \author Isabel Restrepo
// \date April 20, 2009
//
// \verbatim
//  Modifications
//   Nov 7 2009 - Isabel Restrepo: Copied from bvxm
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>

#include <boxm2/boxm_scene.h>

//:
// Only floating point are supported for now and mixtures with 3 components.
// This class can be further templated if necessary
class boxm_merge_mog
{
 private:
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
  typedef boct_tree<short, mix_gauss_type> mog_tree_type;
  typedef boct_tree<short, gauss_type> gauss_tree_type;
  typedef boct_tree_cell<short, mix_gauss_type> mog_cell_type;
  typedef boct_tree_cell<short, gauss_type> gauss_cell_type;

 public:

  // === Scene operations ===

  //: Merges the components of the gaussian mixtures at each voxel into a single gaussian, that minimizes KL divergence.
  //  The resulting scene contains unimodal Gaussians.
  bool kl_merge_scene(boxm_scene<boct_tree<short, mix_gauss_type> > &app_scene,  boxm_scene<boct_tree<short, gauss_type> > &gauss_app);

  bool kl_merge_octree(boct_tree<short, mix_gauss_type> *mog_tree, boct_tree<short, gauss_type> *gauss_tree);

#if 0
  //: Converts a mixture of gaussians grid into an unimodal gaussian grid corresponding to the most probable mode
  static bool mpm_merge_scene(boxm_scene<boct_tree<short, mix_gauss_type> app_scene,  boxm_scene<boct_tree<short, gauss_type> gauss_app);
#endif

  // === Distribution operations ===

  //: Merges all the components in the mixture into one component that minimizes the KL divergence between the two distributions
  static void kl_merge(mix_gauss_type const& mixture,bsta_gauss_f1 &gaussian);
};

#endif
