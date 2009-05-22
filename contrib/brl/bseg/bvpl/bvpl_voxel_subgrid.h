#ifndef bvpl_voxel_subgrid_h
#define bvpl_voxel_subgrid_h

#include <bvxm/bvxm_voxel_grid.h>
#include <bvxm/bvxm_voxel_slab.h>

#include <vgl/vgl_point_3d.h>

class bvpl_subgrid_base
{
  bvpl_subgrid_base(vgl_point_3d<unsigned> center, vgl_vector_3d<double> dimensions)
    : center_(center), dim_(dimensions) {}
  ~bvpl_subgrid_base();
 protected:
   vgl_point_3d<unsigned> center_;
   vgl_vector_3d<double> dim_;
};

template <class T>
class bvpl_voxel_subgrid : public bvpl_subgrid_base
{
public:
  bvpl_voxel_subgrid(bvxm_voxel_grid_base_sptr grid, 
                     vgl_point_3d<unsigned> center, 
                     vgl_vector_3d<double> dimensions);

  bvpl_voxel_subgrid(bvxm_voxel_slab<T>& slab, 
                     vgl_point_3d<unsigned> center, 
                     vgl_vector_3d<double> dimensions);

  ~bvpl_voxel_subgrid(){}

  T& voxel(unsigned x, unsigned y, unsigned z);

  //bvpl_voxel_subgrid& begin();
  //bvpl_voxel_subgrid& end();

protected:
  vgl_box_3d<unsigned> box_;
  bvxm_voxel_slab<T> slab_;
};

#endif