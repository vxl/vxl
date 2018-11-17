// This is brl/bseg/bvxm/bvxm_lidar_processor.h
#ifndef bvxm_lidar_h_
#define bvxm_lidar_h_
//:
// \file
// \brief A class for a lidar processor
//
// \author Isabel Restrepo
// \date April 03, 2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "grid/bvxm_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>

#include <vgl/vgl_box_2d.h>
#include <vil/vil_image_view_base.h>

class  bvxm_lidar_processor
{
 public:

  bvxm_lidar_processor(unsigned vox_thresh)
    : gauss_(bsta_gauss_sf1(0,1)), vox_thresh_(vox_thresh) {}

  //: A function that assigns a gaussian weight to the lidar observation
  bvxm_voxel_slab<float>  prob_density(float z_dim,
                                       bvxm_voxel_slab<float> const& obs,
                                       float voxel_width = 0.0 );

  float prob_density(const vil_image_view_base_sptr& lidar,
                     float z_dim,
                     vnl_vector_fixed<float,3>& covar,  // sigma vals
                     vgl_box_2d<double> lidar_roi,
                     float voxel_width);

 private:
  bsta_gauss_sf1 gauss_;
  unsigned vox_thresh_;
};

#endif // bvxm_lidar_h_
