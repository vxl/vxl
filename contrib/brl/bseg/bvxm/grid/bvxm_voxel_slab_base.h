#ifndef bvxm_voxel_slab_base_h_
#define bvxm_voxel_slab_base_h_

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#include "bvxm_memory_chunk.h"

class bvxm_voxel_slab_base : public vbl_ref_count
{
 public:
  inline unsigned nx() const {return nx_;}
  inline unsigned ny() const {return ny_;}
  inline unsigned nz() const {return nz_;}
  inline unsigned size() const {return nx_*ny_*nz_;}

 protected:

  // constructors are protected; Users should instantiate a bvxm_voxel_slab<T>
  bvxm_voxel_slab_base() : nx_(0),ny_(0),nz_(0) {}
  bvxm_voxel_slab_base(unsigned nx, unsigned ny, unsigned nz) : nx_(nx), ny_(ny), nz_(nz) {}

  unsigned nx_;
  unsigned ny_;
  unsigned nz_;
};

typedef vbl_smart_ptr<bvxm_voxel_slab_base> bvxm_voxel_slab_base_sptr;

#endif // bvxm_voxel_slab_base_h_
