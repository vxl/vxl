#ifndef bvxm_voxel_storage_disk_cached_hxx_
#define bvxm_voxel_storage_disk_cached_hxx_
//:
// \file

#include <string>
#include <iostream>
#include <utility>
#include "bvxm_voxel_storage_disk_cached.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#ifdef BVXM_USE_FSTREAM64
#include <vil/vil_stream_fstream64.h>
#else
#include <vil/vil_stream_fstream.h>
#endif
#include <vul/vul_file.h>
#include <vgl/vgl_vector_3d.h>

#include "bvxm_voxel_storage.h" // base class
#include "bvxm_voxel_storage_disk.h" // for header
#include "bvxm_voxel_slab.h"

template <class T>
bvxm_voxel_storage_disk_cached<T>::bvxm_voxel_storage_disk_cached(std::string storage_filename, vgl_vector_3d<unsigned int> grid_size, vxl_int_64 max_cache_size)
:  bvxm_voxel_storage<T>(grid_size), first_cache_slice_(-1), last_cache_slice_(-1), storage_fname_(std::move(storage_filename)), fio_(nullptr)
{
  //set up cache
  vxl_int_64 slice_size = sizeof(T)*grid_size.x()*grid_size.y();
  n_cache_slices_ = (unsigned)(max_cache_size / slice_size);
  if (n_cache_slices_ > this->grid_size_.z()) {
    n_cache_slices_ = this->grid_size_.z();
  }
  vxl_int_64 cache_size = slice_size * n_cache_slices_;

  std::cout << "allocating cache size of " << cache_size << " bytes ( " << n_cache_slices_ << " slices )." << std::endl;

  cache_mem_ = new bvxm_memory_chunk(cache_size);
  if (!cache_mem_) {
    std::cerr << "ERROR allocating cache memory!\n";
  }

  // check if file exists already or not
  if (vul_file::exists(storage_fname_))  {
    // make sure filename is not a directory
    bool is_dir = vul_file::is_directory(storage_fname_);
    if (is_dir) {
      std::cerr << "error: directory name " << storage_fname_ << " passed to bvxm_voxel_storage_disk constructor.\n";
      return;
    }
    // read header and make sure that it matches given dimensions
    // open the file with in and out flags so we don't truncate
#ifdef BVXM_USE_FSTREAM64
    fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"rw");
#else
    fio_ = new vil_stream_fstream(storage_fname_.c_str(),"rw");
#endif

    if (!fio_->ok()) {
      std::cerr << "error opening " << storage_fname_ << " for read/write\n";
      return;
    }
    bvxm_voxel_storage_header<T> header;

    fio_->read(reinterpret_cast<char*>(&header),sizeof(header));

    if ((header.nx_ != grid_size.x()) || (header.ny_ != grid_size.y()) || (header.nz_ != grid_size.z())) {
      std::cerr << "error: file on disk has size " << vgl_vector_3d<unsigned>(header.nx_,header.ny_,header.nz_) << std::endl
               << "       size passed to constructor = " << grid_size << std::endl;
      return;
    }
  }
  else {
    // file does not yet exist. do nothing for now.
  }
}


template<class T>
bvxm_voxel_storage_disk_cached<T>::~bvxm_voxel_storage_disk_cached()
{
  // purge the cache
  std::cout << " ------------ destructor: purging cache --------------" << std::endl;
  purge_cache();

  // this will delete the stream object
  if (fio_) {
    fio_->ref();
    fio_->unref();
    fio_ = nullptr;
  }

  cache_mem_ = nullptr;
}


template <class T>
bool bvxm_voxel_storage_disk_cached<T>::initialize_data(T const& value)
{
  // check if file exists already or not
  if (!(vul_file::exists(storage_fname_)))  {
    // make sure base directory exists
    std::string base_dir = vul_file::dirname(storage_fname_);
    bool base_dir_exists = vul_file::is_directory(base_dir);
    if (!base_dir_exists) {
      std::cerr << "error: base directory " << base_dir << " does not exist.\n";
      return false;
    }
  }
  else {
    // make sure filename is not a directory
    bool is_dir = vul_file::is_directory(storage_fname_);
    if (is_dir) {
      std::cerr << "error: directory name " << storage_fname_ << " passed to bvxm_voxel_storage_disk constructor.\n";
      return false;
    }
  }
  // everything looks ok. open file for write and fill with data
#ifdef BVXM_USE_FSTREAM64
  fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"w");
#else
  fio_ = new vil_stream_fstream(storage_fname_.c_str(),"w");
#endif

  bvxm_voxel_slab<T> init_slab(this->grid_size_.x(),this->grid_size_.y(),1);
  //bvxm_voxel_slab<T> init_slab(this->grid_size_.x(),this->grid_size_.y(),1,slab_buffer_,static_cast<T*>(slab_buffer_->data()));
  init_slab.fill(value);

  if (!fio_->ok()) {
    std::cerr << " error opening file " << storage_fname_ << " for write.\n";
    return false;
  }
  // write the header
  bvxm_voxel_storage_header<T> header(this->grid_size_);
  fio_->write(reinterpret_cast<char*>(&header),sizeof(header));

  // write each slice
  for (unsigned z=0; z <this->grid_size_.z(); z++) {
    fio_->write(reinterpret_cast<char*>(init_slab.first_voxel()),init_slab.size()*sizeof(T));
  }

  // no longer have any active slabs
  first_cache_slice_ = -1;
  last_cache_slice_ = -1;

  // close output stream
  // this will delete the stream object.
  fio_->ref();
  fio_->unref();
  fio_ = nullptr;

  return true;
}


template <class T>
bvxm_voxel_slab<T> bvxm_voxel_storage_disk_cached<T>::get_slab(unsigned slice_idx, unsigned slab_thickness)
{
  if (slice_idx + slab_thickness > this->grid_size_.z()) {
    std::cerr << "error: tried to get slab " << slice_idx
             << " with thickness " << slab_thickness
             << "; grid_size_.z() = " << this->grid_size_.z() << std::endl;
    bvxm_voxel_slab<T> slab;
    return slab;
  }
  if (slab_thickness > n_cache_slices_) {
    std::cerr << "error: tried to get slab with thickness > cache size\n"
             << "  requested slab_thickness = " << slab_thickness << ", cache size = " << n_cache_slices_ << std::endl;
    bvxm_voxel_slab<T> slab;
    return slab;
  }

  unsigned last_slice_idx = slice_idx + slab_thickness - 1;

  // check to see if slab is already in cache
  T* first_voxel = nullptr;
  if ( ((int)slice_idx < first_cache_slice_ ) || ((int)slice_idx > last_cache_slice_) ){
    // slab is not in cache
    purge_cache();
    fill_cache(slice_idx);
    // make sure fill cache was successful
    if ( ((int)slice_idx < first_cache_slice_ ) || ((int)last_slice_idx > last_cache_slice_) ) {
      std::cerr << "error: slices " << slice_idx << "through " << last_slice_idx << " still not in cache after fill.\n";
      bvxm_voxel_slab<T> slab;
      return slab;
    }
    // fill was successful
    first_voxel = reinterpret_cast<T*>(cache_mem_->data());
  }
  else {
    // entire slab is already in cache.
    vxl_uint_64 slice_size = this->grid_size_.x()*this->grid_size_.y();
    first_voxel = reinterpret_cast<T*>(cache_mem_->data()) + ((slice_idx - first_cache_slice_)*slice_size);
  }
  bvxm_voxel_slab<T> slab(this->grid_size_.x(),this->grid_size_.y(), slab_thickness, cache_mem_, first_voxel);
  return slab;
}


template<class T>
bool bvxm_voxel_storage_disk_cached<T>::purge_cache()
{
  if ( (first_cache_slice_ < 0) || (last_cache_slice_ < 0) )
  {
    // nothing to purge
    return true;
  }

  // check to see if file is already open
  if (!fio_) {
#ifdef BVXM_USE_FSTREAM64
    fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"rw");
#else
    fio_ = new vil_stream_fstream(storage_fname_.c_str(),"rw");
#endif
    if (!fio_->ok()) {
      std::cerr << "error opening file " << storage_fname_ << " for read/write!\n";
      return false;
    }
  }
  vil_streampos slice_pos = slab_filepos(first_cache_slice_);
  vil_streampos file_pos = fio_->tell();
  if (slice_pos != file_pos) {
    fio_->seek(slice_pos);
    file_pos = fio_->tell();
    if (file_pos != slice_pos) {
      std::cerr << "error seeking to file position " << slice_pos << std::endl;
      return false;
    }
  }
  vil_streampos write_len = (last_cache_slice_ - first_cache_slice_ + 1)*this->grid_size_.x()*this->grid_size_.y()*sizeof(T);
  fio_->write(reinterpret_cast<char*>(cache_mem_->data()),write_len);

  first_cache_slice_ = -1;
  last_cache_slice_ = -1;

  return true;
}


template<class T>
bool bvxm_voxel_storage_disk_cached<T>::fill_cache(unsigned start_slice_idx)
{
  // check to see if file is already open
  if (!fio_) {
#ifdef BVXM_USE_FSTREAM64
    fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"rw");
#else
    fio_ = new vil_stream_fstream(storage_fname_.c_str(),"rw");
#endif
    if (!fio_->ok()) {
      std::cerr << "error opening file " << storage_fname_ << " for read/write!\n";
      return false;
    }
  }
  vil_streampos slice_pos = this->slab_filepos(start_slice_idx);
  vil_streampos file_pos = fio_->tell();
  if (slice_pos != file_pos) {
    fio_->seek(slice_pos);
  }
  unsigned last_slice_idx = start_slice_idx + n_cache_slices_ - 1;
  if (last_slice_idx >= this->grid_size_.z()) {
    last_slice_idx = this->grid_size_.z() - 1;
  }
  vil_streampos slice_size = this->grid_size_.x()*this->grid_size_.y()*sizeof(T);
  vil_streampos read_size = (last_slice_idx - start_slice_idx + 1)*slice_size;
  fio_->read(reinterpret_cast<char*>(cache_mem_->data()),read_size);

  first_cache_slice_ = start_slice_idx;
  last_cache_slice_ = last_slice_idx;

  return true;
}

template <class T>
void bvxm_voxel_storage_disk_cached<T>::put_slab()
{
  // don't need to do anything here.
  // data gets written to disk only before it is about to be replaced in cache
  return;
}

template <class T>
unsigned bvxm_voxel_storage_disk_cached<T>::num_observations() const
{
  // read header from disk
  // check to see if file is already open
  if (!fio_) {
    // check if file exists or not
    if (!vul_file::exists(storage_fname_.c_str())) {
      return 0;
    }
#ifdef BVXM_USE_FSTREAM64
    fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"rw");
#else
    fio_ = new vil_stream_fstream(storage_fname_.c_str(),"rw");
#endif
    if (!fio_->ok()) {
      std::cerr << "error opening " << storage_fname_ << "for read/write\n";
      return 0;
    }
  }

  // seek to beginning of file
  fio_->seek(0);

  bvxm_voxel_storage_header<T> header;

  fio_->read(reinterpret_cast<char*>(&header),sizeof(header));

  return header.nobservations_;
}

template <class T>
void bvxm_voxel_storage_disk_cached<T>::increment_observations()
{
  // read header from disk
  // check to see if file is already open
  if (!fio_) {
#ifdef BVXM_USE_FSTREAM64
    fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"rw");
#else
    fio_ = new vil_stream_fstream(storage_fname_.c_str(),"rw");
#endif
    if (!fio_->ok()) {
      std::cerr << "error opening " << storage_fname_ << "for read/write\n";
      return;
    }
  }
  // seek to beginning of file
  fio_->seek(0);
  vil_streampos pos = fio_->tell();
  if (pos != 0) {
    std::cerr << "error seeking to beginning of file\n";
    return;
  }
  bvxm_voxel_storage_header<T> header;
  // read
  fio_->read(reinterpret_cast<char*>(&header),sizeof(header));

  // increment observations
  ++header.nobservations_;

  // write header back to disk
  // seek to beginning of file
  fio_->seek(0);
  pos = fio_->tell();
  if (pos != 0) {
    std::cerr << "error seeking to beginning of file\n";
    return;
  }
  // write
  fio_->write(reinterpret_cast<char*>(&header),sizeof(header));

  return;
}

template <class T>
void bvxm_voxel_storage_disk_cached<T>::zero_observations()
{
  // read header from disk
  // check to see if file is already open
  if (!fio_) {
#ifdef BVXM_USE_FSTREAM64
    fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"rw");
#else
    fio_ = new vil_stream_fstream(storage_fname_.c_str(),"rw");
#endif
    if (!fio_->ok()) {
      std::cerr << "error opening " << storage_fname_ << "for read/write\n";
      return;
    }
  }
  // seek to beginning of file
  fio_->seek(0);
  vil_streampos pos = fio_->tell();
  if (pos != 0) {
    std::cerr << "error seeking to beginning of file\n";
    return;
  }
  bvxm_voxel_storage_header<T> header;
  // read
  fio_->read(reinterpret_cast<char*>(&header),sizeof(header));

  // increment observations
  header.nobservations_=0;

  // write header back to disk
  // seek to beginning of file
  fio_->seek(0);
  pos = fio_->tell();
  if (pos != 0) {
    std::cerr << "error seeking to beginning of file\n";
    return;
  }
  // write
  fio_->write(reinterpret_cast<char*>(&header),sizeof(header));

  return;
}

//: convert slab start index to file position
template<class T>
vil_streampos bvxm_voxel_storage_disk_cached<T>::slab_filepos(unsigned slab_index)
{
  vil_streampos pos =
    ((vil_streampos)(this->grid_size_.x()*this->grid_size_.y()*slab_index))*sizeof(T) + sizeof(bvxm_voxel_storage_header<T>);

  return pos;
}

#define BVXM_VOXEL_STORAGE_DISK_CACHED_INSTANTIATE(T) \
  template class bvxm_voxel_storage_disk_cached<T >

#endif // bvxm_voxel_storage_disk_cached_hxx_
