#ifndef bvxm_voxel_grid_base_h_
#define bvxm_voxel_grid_base_h_

//:
// \file
// \brief  A class for a generic bvxm_voxel_grid.
//         The only purpose of this class is to be able to pass around
//         generic voxel_grid and voxel_grid_sptr
//           
// \author Isabel Restrepo
// \date 02/23/ 08
// \verbatim
//
// \Modifications 

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vgl/vgl_vector_3d.h>

class bvxm_voxel_grid_base : public vbl_ref_count
{
public:
  bvxm_voxel_grid_base(vgl_vector_3d<unsigned> grid_size) : grid_size_(grid_size) {};
  vgl_vector_3d<unsigned> grid_size(){ return grid_size_;}

protected:
    vgl_vector_3d<unsigned> grid_size_;
};

typedef vbl_smart_ptr<bvxm_voxel_grid_base> bvxm_voxel_grid_base_sptr ;

#endif
