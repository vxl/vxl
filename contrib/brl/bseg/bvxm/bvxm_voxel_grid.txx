#ifndef bvxm_voxel_grid_txx_
#define bvxm_voxel_grid_txx_

#include "bvxm_voxel_grid.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_voxel_slab_iterator.h"


template <class T>
bvxm_voxel_slab_iterator<T> bvxm_voxel_grid<T>::slab_iterator(unsigned slice_idx, unsigned slab_thickness)
{
  bvxm_voxel_slab_iterator<T> slab_it(storage_ ,grid_size_, slice_idx ,slab_thickness);

  return slab_it;
}

// note: since we usually want to traverse top down, begin() means the top-most slab
template <class T>
bvxm_voxel_slab_iterator<T> bvxm_voxel_grid<T>::begin(unsigned slab_thickness)
{
  bvxm_voxel_slab_iterator<T> slab_it(storage_, grid_size_, 0, slab_thickness);

  return slab_it;
}

// note: traversing top-down, end() is one slab past the bottom.
template <class T>
bvxm_voxel_slab_iterator<T> bvxm_voxel_grid<T>::end(unsigned slab_thickness)
{
  bvxm_voxel_slab_iterator<T> slab_it(storage_, grid_size_, grid_size_.z(), slab_thickness);

  return slab_it;
}

#define BVXM_VOXEL_GRID_INSTANTIATE(T) \
template class bvxm_voxel_grid<T >

#endif // bvxm_voxel_grid_txx_
