#ifndef bvxm_voxel_storage_mem_txx_
#define bvxm_voxel_storage_mem_txx_

#include "bvxm_voxel_storage.h"
#include "bvxm_voxel_storage_mem.h"
#include "bvxm_memory_chunk.h"

#include <vgl/vgl_vector_3d.h>

template <class T>
bvxm_voxel_storage_mem<T>::bvxm_voxel_storage_mem(vgl_vector_3d<unsigned int> grid_size)
: bvxm_voxel_storage<T>(grid_size), nobservations_(0)
{
  mem_ = new bvxm_memory_chunk(grid_size.x() * grid_size.y() * grid_size.z() * sizeof(T));
}

template <class T>
bool bvxm_voxel_storage_mem<T>::initialize_data(const T& value)
{
  // interpret entire grid as a slab and fill with data.
  bvxm_voxel_slab<T> grid_slab(this->grid_size_.x(),this->grid_size_.y(),this->grid_size_.z(),mem_,reinterpret_cast<T*>(mem_->data()));
  grid_slab.fill(value);
  nobservations_ = 0;

  return true;
}


template <class T>
bvxm_voxel_slab<T> bvxm_voxel_storage_mem<T>::get_slab(unsigned slice_idx, unsigned slab_thickness)
{
  unsigned long slice_size = this->grid_size_.x() * this->grid_size_.y();
  unsigned long mem_offset = slice_size * slice_idx;

  T* first_voxel = static_cast<T*>(mem_->data()) + mem_offset;

  unsigned slab_thickness_actual = slab_thickness;
  if (slice_idx + slab_thickness > (unsigned)(this->grid_size_.z() - 1)) {
    slab_thickness_actual = this->grid_size_.z() - slice_idx;
  }
  bvxm_voxel_slab<T> slab(this->grid_size_.x(),this->grid_size_.y(),slab_thickness_actual,mem_, first_voxel);

  return slab;
}

template <class T>
void bvxm_voxel_storage_mem<T>::put_slab()
{
  // nothing to do here since we are operating on the voxel memory itself.
  return;
}

#define BVXM_VOXEL_STORAGE_MEM_INSTANTIATE(T) \
template class bvxm_voxel_storage_mem<T >

#endif // bvxm_voxel_storage_mem_txx_
