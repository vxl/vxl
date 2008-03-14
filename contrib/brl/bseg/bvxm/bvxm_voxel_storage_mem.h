#ifndef bvxm_voxel_storage_mem_h_
#define bvxm_voxel_storage_mem_h_

#include "bvxm_voxel_storage.h"

#include <vgl/vgl_vector_3d.h>

template <class T>
class bvxm_voxel_storage_mem : public bvxm_voxel_storage<T>
{
public:
  bvxm_voxel_storage_mem(vgl_vector_3d<unsigned int> grid_size);

  virtual bool initialize_data(T const& value);
  virtual bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness);
  virtual void put_slab();

  //: return number of observations
  virtual unsigned num_observations(){return nobservations_;}
  //: increment the number of observations
  virtual void increment_observations(){++nobservations_;}


private:

  bvxm_memory_chunk_sptr mem_;
  unsigned nobservations_;

};
                          
                         
#endif
