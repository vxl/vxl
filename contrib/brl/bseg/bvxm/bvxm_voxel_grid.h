#ifndef bvxm_voxel_grid_h_
#define bvxm_voxel_grid_h_

#include <vcl_string.h>
#include <vgl/vgl_vector_3d.h>

#include "bvxm_voxel_grid_base.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_voxel_storage.h"
#include "bvxm_voxel_storage_disk.h"
#include "bvxm_voxel_storage_mem.h"
#include "bvxm_voxel_slab_iterator.h"



template<class T>
class bvxm_voxel_grid : public bvxm_voxel_grid_base
{
public:

  //: Default Constructor
  bvxm_voxel_grid() : storage_(0), bvxm_voxel_grid_base(vgl_vector_3d<unsigned>(0,0,0)) {};

  //: Constructor for disk-based voxel grid.
  bvxm_voxel_grid(vcl_string storage_fname, vgl_vector_3d<unsigned int> grid_size) 
    : bvxm_voxel_grid_base(grid_size)
  {
    storage_ = new bvxm_voxel_storage_disk<T>(storage_fname, grid_size);
  }

  //: Constructor for memory-based voxel grid.
  bvxm_voxel_grid(vgl_vector_3d<unsigned int> grid_size) 
    : bvxm_voxel_grid_base(grid_size)
  {
    storage_ = new bvxm_voxel_storage_mem<T>(grid_size);
  }

  //: Destructor
  ~bvxm_voxel_grid()
  { 
    if(storage_) {
      delete storage_;
      storage_ = 0;
    }
  }

  // fill with data
  bool initialize_data(T const& val) {return storage_->initialize_data(val); }

  //: return number of observations
  unsigned num_observations(){return storage_->num_observations();}
  //: increment the number of observations
  void increment_observations(){storage_->increment_observations();}

  // access to data via iterators
  typedef bvxm_voxel_slab_iterator<T> iterator;
  typedef bvxm_voxel_slab_const_iterator<T> const_iterator;

  // slice_idx = 0 is the top-most slab.  slice_idx = nz - 1 is the bottom slab.
  bvxm_voxel_slab_iterator<T> slab_iterator(unsigned slice_idx, unsigned slab_thickness = 1);
  // note: since we usually want to traverse top down, begin() means the top-most slab
  bvxm_voxel_slab_iterator<T> begin(unsigned slab_thickness = 1);
  // note: traversing top-down, end() is one slab past the bottom.
  bvxm_voxel_slab_iterator<T> end(unsigned slab_thickness = 1);


private:

  bvxm_voxel_storage<T> *storage_;

};

#endif

