#ifndef bvxm_voxel_slab_iterator_hxx_
#define bvxm_voxel_slab_iterator_hxx_

#include "bvxm_voxel_slab_iterator.h"
#include "bvxm_voxel_storage.h"
#include "bvxm_voxel_slab.h"

#include <vgl/vgl_vector_3d.h>

///////////////////// bvxm_voxel_slab_iterator_base methods //////////////////////////////

template <class T>
bvxm_voxel_slab_iterator_base<T>::bvxm_voxel_slab_iterator_base(bvxm_voxel_storage<T> *storage, vgl_vector_3d<unsigned int> grid_size, unsigned slice_idx, unsigned slab_thickness)
: storage_(storage), slab_thickness_(slab_thickness), slice_idx_(slice_idx), end_slab_(0,0,0,nullptr,nullptr), grid_size_(grid_size)
{
  if ( slice_idx >= grid_size_.z() ) {
    slab_ = this->end_slab_;
  }
  else {
    slab_ = storage_->get_slab(slice_idx, slab_thickness);
  }
}


///////////////////// bvxm_voxel_slab_iterator methods //////////////////////////////


template <class T>
bvxm_voxel_slab_iterator<T>& bvxm_voxel_slab_iterator<T>::operator=(const bvxm_voxel_slab_iterator<T>& that)
{
  this->storage_ = that.storage_;
  this->slab_thickness_ = that.slab_thickness_;
  this->slab_ = that.slab_;
  this->grid_size_ = that.grid_size_;
  this->slice_idx_ = that.slice_idx_;

  return *this;
}

template <class T>
bool bvxm_voxel_slab_iterator<T>::operator==(const bvxm_voxel_slab_iterator<T>& that)
{
  return this->slab_ == that.slab_;
}


template <class T>
bool bvxm_voxel_slab_iterator<T>::operator!=(const bvxm_voxel_slab_iterator<T>& that)
{
  return this->slab_ != that.slab_;
}

template <class T>
bvxm_voxel_slab_iterator<T>& bvxm_voxel_slab_iterator<T>::operator++()
{
  // commit prev slice to memory
  this->storage_->put_slab();

  ++this->slice_idx_;
  if (this->slice_idx_ >= static_cast<int>(this->grid_size_.z())) {
    this->slab_ = this->end_slab_;
  }
  else {
    this->slab_ = this->storage_->get_slab(this->slice_idx_,this->slab_thickness_);
  }
  return *this;
}

template <class T>
bvxm_voxel_slab_iterator<T> bvxm_voxel_slab_iterator<T>::operator++(int) // postfix
{
  // commit prev slice to memory
  this->storage_->put_slab();

  bvxm_voxel_slab_iterator<T> r = *this;

  ++this->slice_idx_;
  if (this->slice_idx_ >= static_cast<int>(this->grid_size_.z())) {
    this->slab_ = this->end_slab_;
  }
  else {
    this->slab_ = this->storage_->get_slab(this->slice_idx_,this->slab_thickness_);
  }
  return r; // previous content of *this
}

template <class T>
bvxm_voxel_slab_iterator<T>& bvxm_voxel_slab_iterator<T>::operator+=(unsigned const &rhs)
{
  // commit prev slice to memory
  this->storage_->put_slab();

  this->slice_idx_ += rhs;
  if (this->slice_idx_ >= static_cast<int>(this->grid_size_.z())) {
    this->slab_ = this->end_slab_;
  }
  else {
    this->slab_ = this->storage_->get_slab(this->slice_idx_,this->slab_thickness_);
  }
  return *this;
}

template <class T>
bvxm_voxel_slab_iterator<T>& bvxm_voxel_slab_iterator<T>::operator-=(unsigned const &rhs)
{
  // commit prev slice to memory
  this->storage_->put_slab();

  this->slice_idx_ -= rhs;
  if (this->slice_idx_ >= static_cast<int>(this->grid_size_.z())) {
    this->slab_ = this->end_slab_;
  }
  else {
    this->slab_ = this->storage_->get_slab(this->slice_idx_,this->slab_thickness_);
  }
  return *this;
}

template <class T>
bvxm_voxel_slab_iterator<T> bvxm_voxel_slab_iterator<T>::operator+(unsigned const &rhs)
{
  // commit prev slice to memory
  this->storage_->put_slab();

  bvxm_voxel_slab_iterator<T> r = *this;

  r.slice_idx_+=rhs;
  if (r.slice_idx_ >= static_cast<int>(r.grid_size_.z())) {
    r.slab_ = r.end_slab_;
  }
  else {
    r.slab_ = r.storage_->get_slab(r.slice_idx_,r.slab_thickness_);
  }
  return r;
}

template <class T>
bvxm_voxel_slab_iterator<T>& bvxm_voxel_slab_iterator<T>::operator--()
{
  // commit old slice to memory
  this->storage_->put_slab();

  --this->slice_idx_;
  if (this->slice_idx_ < 0) {
    this->slab_ = this->end_slab_;
  }
  else {
    this->slab_ = this->storage_->get_slab(this->slice_idx_,this->slab_thickness_);
  }
  return *this;
}
///////////////////// bvxm_voxel_slab_const_iterator methods //////////////////////////////

template <class T>
bvxm_voxel_slab_const_iterator<T>::bvxm_voxel_slab_const_iterator(const bvxm_voxel_slab_iterator<T> &non_const_it)
: bvxm_voxel_slab_iterator_base<T>(non_const_it)
{}


template <class T>
bvxm_voxel_slab_const_iterator<T>& bvxm_voxel_slab_const_iterator<T>::operator=(const bvxm_voxel_slab_const_iterator<T>& that)
{
  this->storage_ = that.storage_;
  this->slab_thickness_ = that.slab_thickness_;
  this->slab_ = that.slab_;
  this->grid_size_ = that.grid_size_;
  this->slice_idx_ = that.slice_idx_;

  return *this;
}

template <class T>
bool bvxm_voxel_slab_const_iterator<T>::operator==(const bvxm_voxel_slab_const_iterator<T>& that)
{
  return this->slab_ == that.slab_;
}


template <class T>
bool bvxm_voxel_slab_const_iterator<T>::operator!=(const bvxm_voxel_slab_const_iterator<T>& that)
{
  return this->slab_ != that.slab_;
}

template <class T>
bvxm_voxel_slab_const_iterator<T>& bvxm_voxel_slab_const_iterator<T>::operator++()
{
  // no need to commit prev slice to memory
  //storage_->put_slab();

  ++this->slice_idx_;
  if (this->slice_idx_ >= static_cast<int>(this->grid_size_.z())) {
    this->slab_ = this->end_slab_;
  }
  else {
    this->slab_ = this->storage_->get_slab(this->slice_idx_,this->slab_thickness_);
  }
  return *this;
}


template <class T>
bvxm_voxel_slab_const_iterator<T>& bvxm_voxel_slab_const_iterator<T>::operator--()
{
  // no need to commit old slice to memory
  //storage_->put_slab();

  --this->slice_idx_;
  if (this->slice_idx_ < 0) {
    this->slab_ = this->end_slab_;
  }
  else {
    this->slab_ = this->storage_->get_slab(this->slice_idx_,this->slab_thickness_);
  }
  return *this;
}

#define BVXM_VOXEL_SLAB_ITERATOR_INSTANTIATE(T) \
template class bvxm_voxel_slab_iterator_base<T >; \
template class bvxm_voxel_slab_iterator<T >; \
template class bvxm_voxel_slab_const_iterator<T >

#endif // bvxm_voxel_slab_iterator_hxx_
