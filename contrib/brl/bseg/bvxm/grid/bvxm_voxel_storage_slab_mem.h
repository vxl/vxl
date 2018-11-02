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

#include <iostream>
#include <vector>
#include "bvxm_voxel_storage.h"
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <class T>
class bvxm_voxel_storage_slab_mem : public bvxm_voxel_storage<T>
{
 public:
  bvxm_voxel_storage_slab_mem(vgl_vector_3d<unsigned int> grid_size, unsigned nslabs);

  bool initialize_data(T const& value) override;
  bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness) override;
  void put_slab() override;

  //: return number of observations
  unsigned num_observations() const override { return nobservations_; }
  //: increment the number of observations
  void increment_observations() override{++nobservations_;}
  //: zero the number of observations
  void zero_observations() override{nobservations_=0;}

 private:
  std::vector<bvxm_memory_chunk_sptr> mem_;
  unsigned nobservations_;
  unsigned nslabs_;
};

#endif // bvxm_voxel_storage_slab_mem_h_
