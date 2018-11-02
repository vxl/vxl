#ifndef bvxm_voxel_storage_disk_hxx_
#define bvxm_voxel_storage_disk_hxx_
//:
// \file

#include <string>
#include <iostream>
#include <utility>
#include "bvxm_voxel_storage_disk.h"
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

#include "bvxm_voxel_slab.h"

template <class T>
bvxm_voxel_storage_disk<T>::bvxm_voxel_storage_disk(std::string storage_filename)
: bvxm_voxel_storage<T>(), storage_fname_(std::move(storage_filename)), fio_(nullptr), active_slab_start_(-1)
{
  // check if file exsist already or not
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

    vgl_vector_3d<unsigned int> grid_size(header.nx_, header.ny_, header.nz_);

    this->grid_size_ = grid_size;

     // assumes desired slab thickness is 1
    slab_buffer_ = new bvxm_memory_chunk(grid_size.x()*grid_size.y()*sizeof(T));
  }
  else {
    // file does not yet exist.
    std::cerr << "error: grid file " << storage_fname_ << " passed to bvxm_voxel_storage_disk does not exist.\n";
  }
}

template <class T>
bvxm_voxel_storage_disk<T>::bvxm_voxel_storage_disk(std::string storage_filename, vgl_vector_3d<unsigned int> grid_size)
: bvxm_voxel_storage<T>(grid_size), storage_fname_(std::move(storage_filename)), fio_(nullptr), active_slab_start_(-1)
{
  // assumes desired slab thickness is 1
  slab_buffer_ = new bvxm_memory_chunk(grid_size.x()*grid_size.y()*sizeof(T));

  // check if file exsist already or not
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
      std::cerr << "error: file on disk: " << storage_fname_<< " has size " << vgl_vector_3d<unsigned>(header.nx_,header.ny_,header.nz_) << std::endl
               << "       size passed to constructor = " << grid_size << std::endl;
      return;
    }
  }
  else {
    // file does not yet exist.
    // open file for write and write header
#ifdef BVXM_USE_FSTREAM64
    fio_ = new vil_stream_fstream64(storage_fname_.c_str(),"w");
#else
    fio_ = new vil_stream_fstream(storage_fname_.c_str(),"w");
#endif

    if (!fio_->ok()) {
      std::cerr << " error opening file " << storage_fname_ << " for write.\n";
      return;
    }
    // write the header
    bvxm_voxel_storage_header<T> header(this->grid_size_);
    fio_->write(reinterpret_cast<char*>(&header),sizeof(header));

    // no longer have any active slabs
    active_slab_start_ = -1;

    // close output stream
    // this will delete the stream object.
    fio_->ref();
    fio_->unref();
    fio_ = nullptr;

    return;
  }
}


template<class T>
bvxm_voxel_storage_disk<T>::~bvxm_voxel_storage_disk()
{
  // this will delete the stream object
  if (fio_) {
    fio_->ref();
    fio_->unref();
    fio_ = nullptr;
  }
}


template <class T>
bool bvxm_voxel_storage_disk<T>::initialize_data(T const& value)
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

  bvxm_voxel_slab<T> init_slab(this->grid_size_.x(),this->grid_size_.y(),1,slab_buffer_,static_cast<T*>(slab_buffer_->data()));
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
  active_slab_start_ = -1;

  // close output stream
  // this will delete the stream object.
  fio_->ref();
  fio_->unref();
  fio_ = nullptr;

  return true;
}


template <class T>
bvxm_voxel_slab<T> bvxm_voxel_storage_disk<T>::get_slab(unsigned slice_idx, unsigned slab_thickness)
{
  if (slice_idx + slab_thickness > this->grid_size_.z()) {
#ifdef DEBUG
    std::cerr << "error: tried to get slab " << slice_idx
             << " with thickness " << slab_thickness
             << "; grid_size_.z() = " << this->grid_size_.z() << std::endl;
#endif
    bvxm_voxel_slab<T> slab;
    return slab;
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
      bvxm_voxel_slab<T> dummy_slab(0,0,0);
      return dummy_slab;
    }
  }
  vil_streampos slice_pos = this->slab_filepos(slice_idx);
  vil_streampos file_pos = fio_->tell();
  if (slice_pos != file_pos) {
    fio_->seek(slice_pos);
  }

  // check that buffer is the right size, if thickness >1
  if ( slab_buffer_->size() != (this->grid_size_.x()* this->grid_size_.y() * slab_thickness *sizeof(T)))
    slab_buffer_->set_size(this->grid_size_.x()* this->grid_size_.y() * slab_thickness *sizeof(T));

  fio_->read(reinterpret_cast<char*>(slab_buffer_->data()),slab_buffer_->size());
  bvxm_voxel_slab<T> slab(this->grid_size_.x(),this->grid_size_.y(),slab_thickness,slab_buffer_,reinterpret_cast<T*>(slab_buffer_->data()));
  active_slab_start_ = slice_idx;
  return slab;
}

template <class T>
void bvxm_voxel_storage_disk<T>::put_slab()
{
  // check to see if file is already open
  if (!fio_) {
    std::cerr << "error: voxel_storage_disk: put_slab() called, but file is not open.\n";
    return;
  }
  // assume that we are putting slab back from where we got it
  if (active_slab_start_ < 0) {
    std::cerr << "error: attempted to put_slice() with no active slab\n";
    return;
  }
  vil_streampos slice_pos = slab_filepos(active_slab_start_);
  vil_streampos file_pos = fio_->tell();
  if (slice_pos != file_pos) {
    fio_->seek(slice_pos);
    file_pos = fio_->tell();
    if (file_pos != slice_pos) {
      std::cerr << "error seeking to file position " << slice_pos << std::endl;
      return;
    }
  }
  fio_->write(reinterpret_cast<char*>(slab_buffer_->data()),slab_buffer_->size());
  //fio_->flush();

  return;
}

template <class T>
unsigned bvxm_voxel_storage_disk<T>::num_observations() const
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
void bvxm_voxel_storage_disk<T>::increment_observations()
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
void bvxm_voxel_storage_disk<T>::zero_observations()
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
vil_streampos bvxm_voxel_storage_disk<T>::slab_filepos(unsigned slab_index)
{
  vil_streampos pos =
    ((vil_streampos)(this->grid_size_.x()*this->grid_size_.y()*slab_index))*sizeof(T) + sizeof(bvxm_voxel_storage_header<T>);

  return pos;
}

#define BVXM_VOXEL_STORAGE_DISK_INSTANTIATE(T) \
template class bvxm_voxel_storage_disk<T >

#endif // bvxm_voxel_storage_disk_hxx_
