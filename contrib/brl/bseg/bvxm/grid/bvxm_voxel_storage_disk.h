#ifndef bvxm_voxel_storage_disk_h_
#define bvxm_voxel_storage_disk_h_
//:
// \file

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#ifdef BVXM_USE_FSTREAM64
#include <vil/vil_stream_fstream64.h>
#else
#include <vil/vil_stream_fstream.h>
#endif
#include <vgl/vgl_vector_3d.h>

#include "bvxm_voxel_storage.h"


//: the data structure at the beginning of each voxel grid file.
template<class T>
class bvxm_voxel_storage_header
{
 public:
  bvxm_voxel_storage_header()= default;
  bvxm_voxel_storage_header(vgl_vector_3d<unsigned> grid_size)
    : nx_(grid_size.x()), ny_(grid_size.y()), nz_(grid_size.z()), nobservations_(0) {}
  bvxm_voxel_storage_header(unsigned nx, unsigned ny, unsigned nz)
    : nx_(nx),ny_(ny),nz_(nz),nobservations_(0) {}
  ~bvxm_voxel_storage_header() = default;

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
  bvxm_voxel_storage_disk(std::string storage_filename, vgl_vector_3d<unsigned int> grid_size);
  bvxm_voxel_storage_disk(std::string storage_filename);

  ~bvxm_voxel_storage_disk() override;

  bool initialize_data(T const& value) override;
  bvxm_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness) override;
  void put_slab() override;

  //: return number of observations
  unsigned num_observations() const override;
  //: increment the number of observations
  void increment_observations() override;
  //: zero the number of observations
  void zero_observations() override;
 private:

  std::string storage_fname_;

  // input and output file stream
#ifdef BVXM_USE_FSTREAM64
  mutable vil_stream_fstream64 *fio_;
#else //BVXM_USE_FSTREAM64
  mutable vil_stream_fstream *fio_;
#endif //BVXM_USE_FSTREAM64

  //std::fstream fio_;

  // currently active slab starting index
  int active_slab_start_;

  //: convert slab start index to file position
  vil_streampos slab_filepos(unsigned slab_index);

  // slab-sized buffer
  bvxm_memory_chunk_sptr slab_buffer_;
};


#endif
