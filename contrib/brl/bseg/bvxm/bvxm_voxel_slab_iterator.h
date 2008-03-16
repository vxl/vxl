#ifndef bvxm_voxel_slab_iterator_h_
#define bvxm_voxel_slab_iterator_h_
//:
// \file

#include <vcl_iterator.h>

#include <vgl/vgl_vector_3d.h>

#include "bvxm_voxel_storage.h"
#include "bvxm_voxel_slab.h"


//: The base class for the iterators. Not an iterator itself since it does not implement ++, --, etc.
template <class T>
class bvxm_voxel_slab_iterator_base
{
 public:
  bvxm_voxel_slab_iterator_base() : storage_(0), slice_idx_(0), slab_thickness_(0) {};
  bvxm_voxel_slab_iterator_base(bvxm_voxel_storage<T> *storage,
                                vgl_vector_3d<unsigned int> grid_size,
                                unsigned slice_idx, unsigned slab_thickness);

  ~bvxm_voxel_slab_iterator_base(){};

 protected:
  bvxm_voxel_storage<T> *storage_;

  unsigned slab_thickness_;
  int slice_idx_; // signed so we can move one past the beginning

  bvxm_voxel_slab<T> slab_;
  const bvxm_voxel_slab<T> end_slab_;
  vgl_vector_3d<unsigned int> grid_size_;
};

template <class T>

class bvxm_voxel_slab_iterator : public bvxm_voxel_slab_iterator_base<T>,
  public vcl_iterator<vcl_bidirectional_iterator_tag, T>
{
 public:
  bvxm_voxel_slab_iterator()
    : bvxm_voxel_slab_iterator_base<T>() {};

  bvxm_voxel_slab_iterator(bvxm_voxel_storage<T> *storage,
                           vgl_vector_3d<unsigned int> grid_size,
                           unsigned slice_idx, unsigned slab_thickness)
    : bvxm_voxel_slab_iterator_base<T>(storage,grid_size,slice_idx,slab_thickness){};

  ~bvxm_voxel_slab_iterator(){/*storage_->put_slab();*/};

  bvxm_voxel_slab_iterator<T>& operator=(const bvxm_voxel_slab_iterator<T>& that);

  bool operator==(const bvxm_voxel_slab_iterator<T>& that);

  bool operator!=(const bvxm_voxel_slab_iterator<T>& that);

  bvxm_voxel_slab_iterator<T>& operator++();
  //bvxm_voxel_slab_iterator& operator++(int); // postfix version

  bvxm_voxel_slab_iterator<T>& operator--();
  //bvxm_voxel_slab_iterator& operator--(int); // postfix version

  bvxm_voxel_slab<T>& operator*(){return this->slab_;}

  bvxm_voxel_slab<T>* operator->(){return &(this->slab_);}
};


template <class T>
class bvxm_voxel_slab_const_iterator : public bvxm_voxel_slab_iterator_base<T>,
                                       public vcl_iterator<vcl_bidirectional_iterator_tag, T>
{
 public:
  bvxm_voxel_slab_const_iterator() : bvxm_voxel_slab_iterator_base<T>(){};

  bvxm_voxel_slab_const_iterator(bvxm_voxel_storage<T> *storage,
                                 vgl_vector_3d<unsigned int> grid_size,
                                 unsigned slice_idx, unsigned slab_thickness)
    : bvxm_voxel_slab_iterator_base<T>(storage,grid_size,slice_idx,slab_thickness){};

  bvxm_voxel_slab_const_iterator(const bvxm_voxel_slab_iterator<T> &non_const_it);

  ~bvxm_voxel_slab_const_iterator(){};

  bvxm_voxel_slab_const_iterator& operator=(const bvxm_voxel_slab_const_iterator& that);

  bool operator==(const bvxm_voxel_slab_const_iterator& that);

  bool operator!=(const bvxm_voxel_slab_const_iterator& that);

  bvxm_voxel_slab_const_iterator& operator++();
  //bvxm_voxel_slab_iterator& operator++(int); // postfix version

  bvxm_voxel_slab_const_iterator& operator--();
  //bvxm_voxel_slab_iterator& operator--(int); // postfix version

  bvxm_voxel_slab<T> const& operator*(){return this->slab_;}

  bvxm_voxel_slab<T> const* operator->(){return &this->slab_;}
};

#endif
