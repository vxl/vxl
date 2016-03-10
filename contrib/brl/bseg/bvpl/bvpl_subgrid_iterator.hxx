#ifndef bvpl_subgrid_iterator_hxx_
#define bvpl_subgrid_iterator_hxx_

#include "bvpl_subgrid_iterator.h"

template <class T>
bvpl_subgrid_iterator<T>::bvpl_subgrid_iterator(bvxm_voxel_grid<T>* grid,
                                                vgl_point_3d<int> min, vgl_point_3d<int> max)
  : bvpl_subgrid_iterator_base(min, max), grid_(grid)
{
  // start from the z radius top, go towards the bottom slab
  iter_ = grid->slab_iterator(0, dim_.z());
  cur_voxel_ = offset_;
  global_cur_voxel_=offset_;
}

template <class T>
bvpl_voxel_subgrid<T> bvpl_subgrid_iterator<T>::operator*()
{
  bvpl_voxel_subgrid<T> subgrid(*iter_, cur_voxel_, min_point_,max_point_);
  return subgrid;
}

template <class T>
bvpl_voxel_subgrid<T> bvpl_subgrid_iterator<T>::operator->()
{
  bvpl_voxel_subgrid<T> subgrid(*iter_, cur_voxel_, min_point_, max_point_);
  return subgrid;
}

template <class T>
bvpl_subgrid_iterator<T>& bvpl_subgrid_iterator<T>::operator++()
{
  int x_r = (max_point_.x()>0)?max_point_.x():0;
  int y_r = (max_point_.y()>0)?max_point_.y():0;
  // move to the next voxel
  int x = cur_voxel_.x();
  int y = cur_voxel_.y();
  int z = cur_voxel_.z();
  int global_z = global_cur_voxel_.z();

  if (++x + x_r == int(grid_->grid_size().x())) {
    x=offset_.x();
    if (++y + y_r == int(grid_->grid_size().y())) {
      y=offset_.y();
      // time to move to the next slab
      std::cout << ". " ;
      ++iter_;
      ++ global_z;
    }
  }
  cur_voxel_.set(x, y, z);
  global_cur_voxel_.set(x,y,global_z);
  return *this;
}

template <class T>
bvpl_subgrid_iterator<T>& bvpl_subgrid_iterator<T>::operator--()
{
  std::cout << "Warning in  bvpl_subgrid_iterator::operator--: this operator has not been tested\n";

  int x_r = (min_point_.x()<0)?min_point_.x():0;
  int y_r = (min_point_.y()<0)?min_point_.y():0;
  int offset_x = (max_point_.x()>0)?max_point_.x():0;
  int offset_y = (max_point_.y()>0)?max_point_.y():0;
  // move to the next voxel
  int x = cur_voxel_.x();
  int y = cur_voxel_.y();
  int z = cur_voxel_.z();
  int global_z = global_cur_voxel_.z();
  if (--x < x_r) {
    x=grid_->grid_size().x()- offset_x;
    if (--y < y_r) {
      y=grid_->grid_size().y()- offset_y;
      // time to move to the next slab
      --iter_;
      --global_z;
    }
  }
  cur_voxel_.set(x, y, z);
  global_cur_voxel_.set(x,y,global_z);

  return *this;
}

template <class T>
bool bvpl_subgrid_iterator<T>::isDone()
{
  return iter_.slice_idx() + dim_.z() > int(grid_->grid_size().z());
}

template <class T>
void bvpl_subgrid_iterator<T>::begin()
{
  iter_ = grid_->begin();
}

#define BVPL_SUBGRID_ITERATOR_INSTANTIATE(T) \
template class bvpl_subgrid_iterator<T >

#endif
