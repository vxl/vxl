#ifndef bvxm_voxel_storage_mem_h_
#define bvxm_voxel_storage_mem_h_
//:
// \file

#include "bvxm_voxel_storage.h"
#include <vgl/vgl_vector_3d.h>

template <class T>
class bvxm_voxel_storage_mem : public bvxm_voxel_storage<T>
{
 public:
  bvxm_voxel_storage_mem(vgl_vector_3d<unsigned int> grid_size);

  bool initialize_data(T const& value) override;
  bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness) override;
  void put_slab() override;

  //: return number of observations
  unsigned num_observations() const override {return nobservations_;}
  //: increment the number of observations
  void increment_observations() override{++nobservations_;}
  //: zero the number of observations
  void zero_observations() override{nobservations_=0;}
 private:

  bvxm_memory_chunk_sptr mem_;
  unsigned nobservations_;
};

#endif
