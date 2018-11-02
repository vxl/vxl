// This is contrib/brl/bseg/bvxm/grid/bvxm_voxel_storage_slab_mem.hxx
#ifndef bvxm_voxel_storage_slab_mem_hxx_
#define bvxm_voxel_storage_slab_mem_hxx_

#include <iostream>
#include <vector>
#include "bvxm_voxel_storage_slab_mem.h"
// \file
#include "bvxm_voxel_storage.h"
#include "bvxm_memory_chunk.h"

#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

template <class T>
bvxm_voxel_storage_slab_mem<T>::bvxm_voxel_storage_slab_mem(vgl_vector_3d<unsigned int> grid_size, unsigned nslabs)
: bvxm_voxel_storage<T>(grid_size), nobservations_(0), nslabs_(nslabs)
{
  // ensure sufficient storage space
  assert(nslabs == grid_size.z());

  mem_.clear();
  mem_.resize(nslabs);
  for (unsigned slice_idx = 0; slice_idx < nslabs; slice_idx++) {
    bvxm_memory_chunk_sptr slab_mem = new bvxm_memory_chunk(grid_size.x()*grid_size.y()*sizeof(T));
    mem_[slice_idx] = slab_mem;
  }
}

template <class T>
bool bvxm_voxel_storage_slab_mem<T>::initialize_data(const T& value)
{
  // interpret entire grid as numerous slabs and fill with data
  for (unsigned slice_idx = 0; slice_idx < this->nslabs_; slice_idx++) {
    bvxm_voxel_slab<T> grid_slab(this->grid_size_.x(), this->grid_size_.y(), 1, mem_[slice_idx], reinterpret_cast<T*>(mem_[slice_idx]->data()));
    grid_slab.fill(value);
  }
  nobservations_ = 0;

  return true;
}

template <class T>
bvxm_voxel_slab<T> bvxm_voxel_storage_slab_mem<T>::get_slab(unsigned slice_idx, unsigned slab_thickness)
{
  if (slice_idx > this->nslabs_) {
    bvxm_voxel_slab<T> dummy_slab(0,0,0);
    return dummy_slab;
  }
  unsigned slab_thickness_actual = slab_thickness;
  if (slab_thickness_actual != 1) {
    slab_thickness_actual = 1;
  }
  T* first_voxel = static_cast<T*>(mem_[slice_idx]->data());
  bvxm_voxel_slab<T> slab(this->grid_size_.x(), this->grid_size_.y(), slab_thickness_actual, mem_[slice_idx], first_voxel);
  return slab;
}

template <class T>
void bvxm_voxel_storage_slab_mem<T>::put_slab()
{
  // nothing to do here since we are operating on the voxel memory itself
  return;
}

#define BVXM_VOXEL_STORAGE_SLAB_MEM_INSTANTIATE(T) \
template class bvxm_voxel_storage_slab_mem<T >

#endif // bvxm_voxel_storage_slab_mem_hxx_
