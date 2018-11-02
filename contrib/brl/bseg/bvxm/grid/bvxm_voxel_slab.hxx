#ifndef bvxm_voxel_slab_hxx_
#define bvxm_voxel_slab_hxx_
//:
// \file
// \brief A 3-D slab of data of type T.  Based loosely on vil_image_view.
// \author Daniel Crispell (dec@lems.brown.edu)
// \date Jan 22, 2008

#include <cstring>
#include <iostream>
#include <algorithm>
#include "bvxm_voxel_slab.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

//: Constructor allocating new memory
template<class T>
bvxm_voxel_slab<T>::bvxm_voxel_slab(unsigned nx, unsigned ny, unsigned nz)
: bvxm_voxel_slab_base(nx,ny,nz)
{
  mem_ = new bvxm_memory_chunk(nx*ny*nz*sizeof(T));
  first_voxel_ = static_cast<T*>(mem_->data());
}


//: Fill view with given value
template<class T>
void bvxm_voxel_slab<T>::fill(T const& value)
{
  std::fill(begin(), end(), value);
  return;
}


//: True if they share same memory
template<class T>
bool bvxm_voxel_slab<T>::operator==(const bvxm_voxel_slab &rhs) const
{
  if (this == &rhs) return true;

  return    mem_ == rhs.mem_ &&
    first_voxel_ == rhs.first_voxel_ &&
            nx_  == rhs.nx_ &&
            ny_  == rhs.ny_ &&
            nz_  == rhs.nz_;
}

//: deep copy data in slab
template<class T>
void bvxm_voxel_slab<T>::deep_copy(bvxm_voxel_slab<T> const& src)
{
  set_size(src.nx(),src.ny(),src.nz());

  std::memcpy((void*)(this->first_voxel_),(void*)(src.first_voxel()),src.size()*sizeof(T));
  return;
}

//: change the size of the slab
template<class T>
void bvxm_voxel_slab<T>::set_size(unsigned nx, unsigned ny, unsigned nz)
{
  if ((nx_ == nx) && (ny_ == ny) && (nz_ == nz)) {
    return;
  }
  release_memory();
  mem_ = new bvxm_memory_chunk(nx*ny*nz*sizeof(T));
  first_voxel_ = static_cast<T*>(mem_->data());
  nx_ = nx;
  ny_ = ny;
  nz_ = nz;

  return;
}

#define BVXM_VOXEL_SLAB_INSTANTIATE(T) \
template class bvxm_voxel_slab<T >

#endif // bvxm_voxel_slab_hxx_
