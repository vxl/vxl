#ifndef bvpl_subgrid_iterator_txx_
#define bvpl_subgrid_iterator_txx_

#include "bvpl_subgrid_iterator.h"

template <class T>
bvpl_subgrid_iterator<T>::bvpl_subgrid_iterator(bvxm_voxel_grid<T> grid, 
                                                vgl_vector_3d<unsigned> dimensions,
                                                vgl_vector_3d<unsigned int> grid_size)

    : bvpl_subgrid_iterator_base(dimensions), cur_voxel_(vgl_point_3d<unsigned>(0,0,0))
{
  // start from the top go towards the botton slab
  bvxm_voxel_slab_iterator iter_ = grid_.slab_iterator(0, dimensions.z());
}

template <class T>
bvpl_voxel_subgrid<T> bvpl_subgrid_iterator<T>::operator*()
{
  bvpl_voxel_subgrid<T> subgrid(*iter_, cur_voxel_, dim_);
  return subgrid;
}

template <class T>
bvpl_subgrid_iterator<T>& bvpl_subgrid_iterator<T>::operator++()
{
  // move to the next voxel
  unsigned x = cur_voxel_.x();
  unsigned y = cur_voxel_.y();
  unsigned z = cur_voxel_.z();
  if (++x == grid_size_.x()) {
    x=0;
    if (++y == grid_size_.y()) {
      y=0;
      if (++z == dim_.z()) {
        // time to move to the next slab
        z=0;
        ++iter;
      }
    }
  }
  cur_voxel_.set(x, y, z);
  return *this;
}

template <class T>
bvpl_subgrid_iterator<T>& bvpl_subgrid_iterator<T>::operator--()
{
  // move to the next voxel
  unsigned x = cur_voxel_.x();
  unsigned y = cur_voxel_.y();
  unsigned z = cur_voxel_.z();
  if (--x < 0) {
    x=grid_size_.x();
    if (--y < 0) {
      y=grid_size_.y();
      if (--z < 0) {
        // time to move to the next slab
        z=dim_z();
        --iter;
      }
    }
  }
  cur_voxel_.set(x, y, z);
  return *this;
}

template <class T>
bool bvpl_subgrid_iterator<T>::done()
{
  if (iter! == grid_->end())
    return true;
  else
    return false;
}

#endif // bvpl_subgrid_iterator_txx_
