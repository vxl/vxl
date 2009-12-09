#ifndef bvxm_voxel_storage_disk_cached_h_
#define bvxm_voxel_storage_disk_cached_h_
//:
// \file

#include <vcl_string.h>
#ifdef BVXM_USE_FSTREAM64
#include <vil/vil_stream_fstream64.h>
#else
#include <vil/vil_stream_fstream.h>
#endif
#include <vgl/vgl_vector_3d.h>

#include "bvxm_voxel_storage.h"
#include "bvxm_voxel_storage_disk.h" // for header


//: object for reading and writing voxel data from a file on disk.
template <class T>
class bvxm_voxel_storage_disk_cached : public bvxm_voxel_storage<T>
{
 public:
  bvxm_voxel_storage_disk_cached(vcl_string storage_filename, vgl_vector_3d<unsigned int> grid_size, vxl_int_64 max_cache_size);
  virtual ~bvxm_voxel_storage_disk_cached();

  virtual bool initialize_data(T const& value);
  virtual bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness);
  virtual void put_slab();

  //: return number of observations
  virtual unsigned num_observations() const;
  //: increment the number of observations
  virtual void increment_observations();
  //: zero the number of observations
  virtual void zero_observations();
 protected:

   bool fill_cache(unsigned start_slice_idx);
   bool purge_cache();

   bvxm_memory_chunk_sptr cache_mem_;
   unsigned n_cache_slices_;

   //vcl_map<unsigned, T*> cache_map_;
   int first_cache_slice_;
   int last_cache_slice_;

   vcl_string storage_fname_;

  // input and output file stream
#ifdef BVXM_USE_FSTREAM64
  mutable vil_stream_fstream64 *fio_;
#else //BVXM_USE_FSTREAM64
  mutable vil_stream_fstream *fio_;
#endif //BVXM_USE_FSTREAM64

  //: convert slab start index to file position
  vil_streampos slab_filepos(unsigned slab_index);
};

#endif // bvxm_voxel_storage_disk_cached_h_
