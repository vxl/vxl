#ifndef bvpl_subgrid_iterator_txx_
#define bvpl_subgrid_iterator_txx_

#include "bvpl_subgrid_iterator.h"

template <class T>
bvpl_subgrid_iterator<T>::bvpl_subgrid_iterator(bvxm_voxel_grid<T>* grid,
                                                vgl_vector_3d<int> dimensions)
  : bvpl_subgrid_iterator_base(dimensions), grid_(grid)
{
  // start from the z radius top go towards the botton slab
  int x_r = dimensions.x()/2;
  int y_r = dimensions.y()/2;
  int z_r = dimensions.z()/2;
  iter_ = grid->slab_iterator(0, dimensions.z());
  cur_voxel_ = vgl_point_3d<int>(x_r, y_r, z_r);
}

template <class T>
bvpl_voxel_subgrid<T> bvpl_subgrid_iterator<T>::operator*()
{
  bvpl_voxel_subgrid<T> subgrid(*iter_, cur_voxel_, dim_);
  return subgrid;
}

template <class T>
bvpl_voxel_subgrid<T> bvpl_subgrid_iterator<T>::operator->()
{
  bvpl_voxel_subgrid<T> subgrid(*iter_, cur_voxel_, dim_);
  return subgrid;
}

template <class T>
bvpl_subgrid_iterator<T>& bvpl_subgrid_iterator<T>::operator++()
{
  int x_r = dim_.x()/2;
  int y_r = dim_.y()/2;
  // move to the next voxel
  unsigned x = cur_voxel_.x();
  unsigned y = cur_voxel_.y();
  unsigned z = cur_voxel_.z();
  if (++x == grid_->grid_size().x()-x_r) {
    x=x_r;
    if (++y == grid_->grid_size().y()-y_r) {
      y=y_r;
      // time to move to the next slab
      vcl_cout << ". " ;
      ++iter_;
    }
  }
  cur_voxel_.set(x, y, z);
  return *this;
}

template <class T>
bvpl_subgrid_iterator<T>& bvpl_subgrid_iterator<T>::operator--()
{
  int x_r = dim_.x()/2;
  int y_r = dim_.y()/2;
  // move to the next voxel
  int x = cur_voxel_.x();
  int y = cur_voxel_.y();
  int z = cur_voxel_.z();
  if (--x < x_r) {
    x=grid_->grid_size().x();
    if (--y < y_r) {
      y=grid_->grid_size().y();
      // time to move to the next slab
      --iter_;
    }
  }
  cur_voxel_.set(x, y, z);
  return *this;
}

template <class T>
bool bvpl_subgrid_iterator<T>::isDone()
{
  if (iter_ == grid_->end())
    return true;
  else
    return false;
}

template <class T>
void bvpl_subgrid_iterator<T>::begin()
{
  iter_ = grid_->begin();
}

#define BVPL_SUBGRID_ITERATOR_INSTANTIATE(T) \
template class bvpl_subgrid_iterator<T >

#endif
