#ifndef bvxm_voxel_grid_base_h_
#define bvxm_voxel_grid_base_h_
//:
// \file
// \brief  A class for a generic bvxm_voxel_grid.
//  The only purpose of this class is to be able to pass around
//  generic voxel_grid and its smart pointers
//
// \author Isabel Restrepo
// \date February 23, 2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_vector_3d.h>

class bvxm_voxel_grid_base : public vbl_ref_count
{
 public:
  bvxm_voxel_grid_base() = default;
  ~bvxm_voxel_grid_base() override = default;
  bvxm_voxel_grid_base(vgl_vector_3d<unsigned> grid_size) : grid_size_(grid_size) {}
  vgl_vector_3d<unsigned> grid_size() const { return grid_size_; }

 protected:
  vgl_vector_3d<unsigned> grid_size_;
};

#include "bvxm_voxel_grid_base_sptr.h"

#endif
