// This is contrib/brl/bseg/bvxm/grid/bvxm_voxel_storage_slab_mem.h
#ifndef bvxm_voxel_storage_slab_mem_h_
#define bvxm_voxel_storage_slab_mem_h_
//:
// \file
// \brief A template class for voxel grid storage where the entire grid is interpreted number of slabs memory chunk
//        This class is similar to bvxm_voxel_storage_mem, only the allocated memory space is not contiguous in order to
//        handle large storage requirement
// \author Yi Dong
// \data Nov. 27, 2013
//
// \verbatim
//  Modifications:
// \endverbatim

#include "bvxm_voxel_storage.h"
#include <vgl/vgl_vector_3d.h>
#include <vcl_vector.h>


template <class T>
class bvxm_voxel_storage_slab_mem : public bvxm_voxel_storage<T>
{
 public:
  bvxm_voxel_storage_slab_mem(vgl_vector_3d<unsigned int> grid_size, unsigned nslabs);

  virtual bool initialize_data(T const& value);
  virtual bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness);
  virtual void put_slab();

  //: return number of observations
  virtual unsigned num_observations() const { return nobservations_; }
  //: increment the number of observations
  virtual void increment_observations(){++nobservations_;}
  //: zero the number of observations
  virtual void zero_observations(){nobservations_=0;}

 private:
  vcl_vector<bvxm_memory_chunk_sptr> mem_;
  unsigned nobservations_;
  unsigned nslabs_;
};

#endif // bvxm_voxel_storage_slab_mem_h_