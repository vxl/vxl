// This is / bvma_mog_rgb_processor.h
#ifndef bvxm_mog_rgb_processor_h_
#define bvxm_mog_rgb_processor_h_

//:
// \file
// \brief // A class for a rgb-mixture-of-gaussian processor
//           
// \author Pradeep
// \date 02/22/ 08
// \verbatim
//


#include "bvxm_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_gaussian_indep.h>

typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
typedef bsta_num_obs<mix_gauss> mix_gauss_type;
//typedef gauss_type::vector_type T;


// The mix_gauss_type contains the same data as mix_gauss plus an
// extra attribute that indicate the number of observations

class  bvxm_mog_rgb_processor
{
public: 

  typedef mix_gauss_type apm_datatype;
  typedef gauss_type::vector_type obs_datatype;
  typedef gauss_type::math_type obs_mathtype;

  bvxm_mog_rgb_processor(){};

  bvxm_voxel_slab<float>  prob_density(bvxm_voxel_slab<apm_datatype> const& appear,
                                       bvxm_voxel_slab<obs_datatype> const& obs);

  bvxm_voxel_slab<float> prob_range(bvxm_voxel_slab<apm_datatype> const& appear,
                                    bvxm_voxel_slab<obs_datatype> const& obs_min,
                                    bvxm_voxel_slab<obs_datatype> const& obs_max);

  bool update( bvxm_voxel_slab<apm_datatype> &appear,
    bvxm_voxel_slab<obs_datatype> const& obs,
    bvxm_voxel_slab<float> const& weight);

   bvxm_voxel_slab<obs_datatype> expected_color( bvxm_voxel_slab<mix_gauss_type> const& appear);

  //bin number is always 0 for the simple mixture of gaussian case
  virtual unsigned int get_light_bin(unsigned int num_light_bins, const vnl_vector<float>& light) {return 0;}
   

};

#endif // bvma_mog_rgb_processor_h_


