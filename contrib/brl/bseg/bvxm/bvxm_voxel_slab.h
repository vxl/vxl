#ifndef bvxm_voxel_slab_h_
#define bvxm_voxel_slab_h_
//:
// \file
// \brief A 3-D slab of data of type T.  Based loosely on vil_image_view.
// \author Daniel Crispell (dec@lems.brown.edu)
// \date 1/22/2008

#include <vcl_cassert.h>

#include "bvxm_voxel_slab_base.h"
#include "bvxm_memory_chunk.h"

template <class T>
class bvxm_voxel_slab : public bvxm_voxel_slab_base
{
 public:
  //: Default Constructor. Create an empty slab.
  bvxm_voxel_slab() : bvxm_voxel_slab_base(0,0,0), first_voxel_(0), mem_(0) {};

  //: Create a voxel slab using its own memory.
  bvxm_voxel_slab(unsigned nx, unsigned ny, unsigned nz);

  //: Create a voxel_slab using an exisiting bvxm_memory_chunk
  bvxm_voxel_slab(unsigned nx, unsigned ny, unsigned nz, bvxm_memory_chunk_sptr mem, T* first_voxel)
    : bvxm_voxel_slab_base(nx,ny,nz), first_voxel_(first_voxel), mem_(mem){};

  //: Copy Constructor
  bvxm_voxel_slab(const bvxm_voxel_slab<T>& that);

  //: Pointer to first voxel in slab (voxel at (0,0,0))
  inline T* first_voxel() const { return first_voxel_;}

  //: Fill view with given value
  void fill(T const& value);

  //: True if they share same memory
  bool operator==(const bvxm_voxel_slab &rhs) const;

  // iterators
  typedef T *iterator;
  //: iterator pointing to first voxel
  inline iterator begin() { return first_voxel_; }
  //: iterator pointing to one past the last voxel
  inline iterator end  () { return first_voxel_ + size(); }

  typedef T const* const_iterator;
  inline const_iterator begin() const { return first_voxel_; }
  inline const_iterator end  () const { return first_voxel_ + size(); }

  //: Return read-only reference to voxel at (x,y,0).
  inline const T& operator()(unsigned x, unsigned y) const {
    assert(x<nx_); assert(y<ny_);
    return first_voxel_[nx_*y + x]; }

  //: Return read/write reference to voxel at (x,y,0).
  inline T&       operator()(unsigned x, unsigned y) {
    assert(x<nx_); assert(y<ny_);
    return first_voxel_[nx_*y + x]; }

  //: Return read-only reference to voxel at (x,y,z)
  inline const T& operator()(unsigned x, unsigned y, unsigned z) const {
    assert(x<nx_); assert(y<ny_); assert(z<nz_);
    return first_voxel_[nx_*ny_*z + nx_*y + x]; }

  //: Return read-only reference to voxel at (x,y,z)
  inline T&       operator()(unsigned x, unsigned y, unsigned z) {
    assert(x<nx_); assert(y<ny_); assert(z<nz_);
    return first_voxel_[nx_*ny_*z + nx_*y + x]; }

  //: not-equals operator
  bool operator != (bvxm_voxel_slab const& rhs)
  {
    return !operator==(rhs);
  }

  //: deep copy data in slab
  void deep_copy(bvxm_voxel_slab<T> const& src);

  void set_size(unsigned nx, unsigned ny, unsigned nz);

 protected:
  T* first_voxel_;

  //: Reference to actual voxel data.
  bvxm_memory_chunk_sptr mem_;

  //: Disconnect this slab from the underlying data.
  void release_memory() { mem_ = 0; }
};

#endif // bvxm_voxel_slab_h_
