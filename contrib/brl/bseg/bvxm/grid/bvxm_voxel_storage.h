#ifndef bvxm_voxel_storage_h_
#define bvxm_voxel_storage_h_
//:
// \file

#include <vgl/vgl_vector_3d.h>
#include "bvxm_voxel_slab.h"

template<class T>
class bvxm_voxel_storage
{
 public:
  //: Constructor
   bvxm_voxel_storage()= default;
  bvxm_voxel_storage(vgl_vector_3d<unsigned int> grid_size) : grid_size_(grid_size) {}

  virtual ~bvxm_voxel_storage() = default;

  //: Initialize all data in the grid
  virtual bool initialize_data(T const& value) = 0;
  //: Get a slab of data from the grid.  Only one slab can be "active" at a time.
  virtual bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness) = 0;
  //: Commit currently active slab to memory.
  virtual void put_slab() = 0;

  //: return number of observations
  virtual unsigned num_observations() const = 0;
  //: increment the number of observations
  virtual void increment_observations() = 0;
  //: zero the number of observations
  virtual void zero_observations() = 0;
  //: return grid length in x dimension
  inline unsigned nx() const {return grid_size_.x();}
  //: return grid length in y dimension
  inline unsigned ny() const {return grid_size_.y();}
  //: return grid length in z dimension
  inline unsigned nz() const {return grid_size_.z();}

 protected:
  vgl_vector_3d<unsigned int> grid_size_;
};


#endif
