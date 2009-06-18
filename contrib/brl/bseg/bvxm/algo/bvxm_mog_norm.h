// This is contrib/isabel/bseg3d/algo/bvxm_mog_norm.h
#ifndef bvxm_mog_norm_h_
#define bvxm_mog_norm_h_
//:
// \file
// \brief class to calculate norms between mixtures of gaussians and gaussians
//        Methods support univariate Ting point distributions and mixture with 3 componets
//        The reason for these limitations is because that is what bvxm uses up to date. 
// \author Isabel Restrepo
// \date March 25, 2009
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>

template <class T>
class bvxm_mog_norm
{

private:

  typedef bsta_num_obs<bsta_gaussian_sphere<T,1> >gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
  
  typedef bsta_mixture<gauss_type> mix_gauss_undef;

public:
  
  //Grid opertions
  
  //: Computes the l2 distances between the mixtures of gaussians in a bvxm_grid and a reference.
  //  If no mixture is given as a reference, the first mixture in the grid is used
  static bool mog_l2_grid(bvxm_voxel_grid_base_sptr apm_grid_base,
                             bvxm_voxel_grid_base_sptr mask_grid_base,
                             bvxm_voxel_grid_base_sptr dist_grid_base,
                             bool reference_given,
                             mix_gauss_type reference  = mix_gauss_type());
  
  
  // Distribution operations

 //:Computes the l2 norm between two mixtures of gaussians
 static T mog_l2 (mix_gauss_type const& p1, mix_gauss_type const& p2);

 //: Computes the l2 norm between a mixture of gaussians and  a gaussian
 static T l2_gauss2mix(gauss_type const&g, mix_gauss_undef const& f, bool normalize);
 
 //: Computes l2 norm between two gaussians
 static T l2_gauss(gauss_type const& p1, gauss_type const& p2, bool normalize);
 
 //: Computes the symmetric Kullerback-Liebler distance between two gaussians 
 static T kl_symmetric_distance(gauss_type const&g1, gauss_type const& g2);
 
 //: Computes the KL distance between two gaussians
 static T kl_distance(gauss_type const&g1, gauss_type const& g2);

};

#endif

