#ifndef bvpl_voxel_subgrid_txx_
#define bvpl_voxel_subgrid_txx_

#include "bvpl_voxel_subgrid.h"

template <class T>
bvpl_voxel_subgrid<T>::bvpl_voxel_subgrid(bvxm_voxel_slab<T>& slab,
                                          vgl_point_3d<int> center,
                                          vgl_vector_3d<int> dimensions)
: bvpl_subgrid_base(center, dimensions), slab_(slab)
{
  box_=vgl_box_3d<int>(center, dim_.x(), dim_.y(), dim_.z(), vgl_box_3d<int>::centre);
}

template <class T>
bool bvpl_voxel_subgrid<T>::voxel(int x, int y, int z, T& v)
{
  vgl_point_3d<int> c = box_.centroid();

  x = c.x()+x;
  y = c.y()+y;
  z = c.z()+z;

  vgl_box_3d<int> slab_box(vgl_point_3d<int>(0,0,0), vgl_point_3d<int>(slab_.nx()-1,slab_.ny()-1,slab_.nz()-1));
  // make sure that the point is inside the box
  if (!box_.contains(x,y,z)){
#ifdef DEBUG
    vcl_cerr << "bvpl_subgrid_voxel_iterator: The index is out of subgrid boundaries!\n";
#endif
    return false;
  }
  else if (!slab_box.contains(x,y,z)) {
#ifdef DEBUG
    vcl_cerr << "bvpl_subgrid_voxel_iterator: The index is out of grid boundaries!\n";
#endif
    return false;
  }
  else {
    v = slab_(x,y,z);
    return true;
  }
}

#define BVPL_VOXEL_SUBGRID_INSTANTIATE(T) \
template class bvpl_voxel_subgrid<T >

#endif // bvpl_voxel_subgrid_txx_
