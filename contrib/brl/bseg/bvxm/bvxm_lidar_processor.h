// This is brl/bseg/bvxm/bvxm_lidar_processor.h
#ifndef bvxm_lidar_h_
#define bvxm_lidar_h_
//:
// \file
// \brief A class for a lidar processor
//
// \author Isabel Restrepo
// \date 04/03/2008
// \verbatim
//  Modifications
//
// \endverbatim

#include "bvxm_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>

class  bvxm_lidar_processor
{
 public:

  bvxm_lidar_processor(unsigned vox_thresh): gauss_(bsta_gauss_f1(0,1)), vox_thresh_(vox_thresh) {}

  //: A function that assigns a gaussian weight to the lidar observation
  bvxm_voxel_slab<float>  prob_density(float z_dim,
                                       bvxm_voxel_slab<float> const& obs);

 private:
  bsta_gauss_f1 gauss_;
  unsigned vox_thresh_;
};

#endif // bvxm_lidar_h_
