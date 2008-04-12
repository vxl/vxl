#include "bvxm_lidar_processor.h"
//:
// \file
#include "bvxm_voxel_slab.h"
#include "bvxm_voxel_slab_iterator.h"

//: Return probability density of observing pixel values
bvxm_voxel_slab<float>
bvxm_lidar_processor::prob_density(unsigned z_dim,  bvxm_voxel_slab<float> const& obs)
{
  //the output
  bvxm_voxel_slab<float> probabilities(obs.nx(), obs.ny(), obs.nz());

  //the slab iterators
  bvxm_voxel_slab<float>::const_iterator obs_it;
  bvxm_voxel_slab<float>::iterator prob_it = probabilities.begin();

  for (obs_it = obs.begin(); obs_it!= obs.end(); ++obs_it, ++prob_it)
  {
    float d = *obs_it - z_dim;

    // Calculate threshold such that the given number of pixels corresponds to
    // 2.5 standard deviations away in a standard normal gaussian
    float thresh = 2.5 / float(vox_thresh_);
    d *= thresh;

    *prob_it = gauss_.prob_density(d);
  }

  return probabilities;
}

