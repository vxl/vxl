#ifndef bvxm_voxel_storage_disk_h_
#define bvxm_voxel_storage_disk_h_
//:
// \file

#include <vcl_string.h>
#ifdef BVXM_USE_FSTREAM64
#include <vil/vil_stream_fstream64.h>
#else
#include <vil/vil_stream_fstream.h>
#endif
//#include <vcl_fstream.h>
#include <vgl/vgl_vector_3d.h>

#include "bvxm_voxel_storage.h"


//: the data structure at the beginning of each voxel grid file.
template<class T>
class bvxm_voxel_storage_header
{
 public:
  bvxm_voxel_storage_header(){};
  bvxm_voxel_storage_header(vgl_vector_3d<unsigned> grid_size)
    : nx_(grid_size.x()), ny_(grid_size.y()), nz_(grid_size.z()), nobservations_(0) {};
  bvxm_voxel_storage_header(unsigned nx, unsigned ny, unsigned nz)
    : nx_(nx),ny_(ny),nz_(nz),nobservations_(0){};
  ~bvxm_voxel_storage_header(){};

  unsigned nx_;
  unsigned ny_;
  unsigned nz_;
  unsigned nobservations_;
};


//: object for reading and writing voxel data from a file on disk.
template <class T>
class bvxm_voxel_storage_disk : public bvxm_voxel_storage<T>
{
 public:
  bvxm_voxel_storage_disk(vcl_string storage_filename, vgl_vector_3d<unsigned int> grid_size);
  virtual ~bvxm_voxel_storage_disk(){};

  virtual bool initialize_data(T const& value);
  virtual bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness);
  virtual void put_slab();

  //: return number of observations
  virtual unsigned num_observations();
  //: increment the number of observations
  virtual void increment_observations();

 private:

  vcl_string storage_fname_;

  // input and output file stream
#ifdef BVXM_USE_FSTREAM64
  vil_stream_fstream64 *fio_;
#else //BVXM_USE_FSTREAM64
  vil_stream_fstream *fio_;
#endif //BVXM_USE_FSTREAM64

  //vcl_fstream fio_;
  
  // currently active slab starting index
  int active_slab_start_;

  //: convert slab start index to file position
  vil_streampos slab_filepos(unsigned slab_index);
  
  // slab-sized buffer
  bvxm_memory_chunk_sptr slab_buffer_;
};


#endif
