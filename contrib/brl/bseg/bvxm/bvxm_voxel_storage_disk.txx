#ifndef bvxm_voxel_storage_disk_txx_
#define bvxm_voxel_storage_disk_txx_

#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vul/vul_file.h>
#include <vgl/vgl_vector_3d.h>

#include "bvxm_voxel_storage_disk.h"
#include "bvxm_voxel_slab.h"

template <class T>
bvxm_voxel_storage_disk<T>::bvxm_voxel_storage_disk(vcl_string storage_filename, vgl_vector_3d<unsigned int> grid_size)
: bvxm_voxel_storage<T>(grid_size), storage_fname_(storage_filename)
{
  slab_buffer_ = new bvxm_memory_chunk(grid_size.x()*grid_size.y()*sizeof(T));

  // check if file exsist already or not
  if (vul_file::exists(storage_fname_))  {
    // make sure filename is not a directory
    bool is_dir = vul_file::is_directory(storage_fname_);
    if (is_dir) {
      vcl_cerr << "error: directory name " << storage_fname_ << " passed to bvxm_voxel_storage_disk constructor." << vcl_endl;
      return;
    }
    // read header and make sure that it matches given dimensions
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),vcl_ios::binary | vcl_ios::out | vcl_ios::in);
    if (!fio_.is_open()) {
      vcl_cerr << "error opening " << storage_fname_ << " for read/write " << vcl_endl;
      return;
    }
    // seek to beginning of file
    fio_.seekp(0,vcl_ios::beg);
    bvxm_voxel_storage_header<T> header;

    fio_.read(reinterpret_cast<char*>(&header),sizeof(header));

    if ((header.nx_ != grid_size.x()) || (header.ny_ != grid_size.y()) || (header.nz_ != grid_size.z())) {
      vcl_cerr << "error: file on disk has size " << vgl_vector_3d<unsigned>(header.nx_,header.ny_,header.nz_) << vcl_endl;
      vcl_cerr << "       size passed to constructor = " << grid_size << vcl_endl;
      return;
    }
  }
  else {
    // file does not yet exist. do nothing for now.
  }
}

template <class T>
bool bvxm_voxel_storage_disk<T>::initialize_data(T const& value)
{
  // check if file exsist already or not
  if (!(vul_file::exists(storage_fname_)))  {
    // create file
    vcl_string base_dir = vul_file::dirname(storage_fname_);
    bool base_dir_exists = vul_file::is_directory(base_dir);
    if (!base_dir_exists) {
      vcl_cerr << "error: base directory " << base_dir << " does not exist. " << vcl_endl;
      return false;
    }
  }else {
    // make sure filename is not a directory
    bool is_dir = vul_file::is_directory(storage_fname_);
    if (is_dir) {
      vcl_cerr << "error: directory name " << storage_fname_ << " passed to bvxm_voxel_storage_disk constructor." << vcl_endl;
      return false;
    }
  }
  // everything looks ok. open file for write and fill with data
  bvxm_voxel_slab<T> init_slab(grid_size_.x(),grid_size_.y(),1,slab_buffer_,static_cast<T*>(slab_buffer_->data()));
  init_slab.fill(value);

  if (fio_.is_open()) {
    // close the file if it was already open.
    fio_.close();
  }
  fio_.open(storage_fname_.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!fio_.is_open()) {
    vcl_cerr << " error opening file " << storage_fname_ << " for write. " << vcl_endl;
    return false;
  }
  // write the header
  bvxm_voxel_storage_header<T> header(grid_size_);
  fio_.write(reinterpret_cast<char*>(&header),sizeof(header));
  // write each slice
  for (unsigned z=0; z <grid_size_.z(); z++) {
    fio_.write(reinterpret_cast<char*>(init_slab.first_voxel()),init_slab.size()*sizeof(T));
  }

  // close output stream
  fio_.close();

  return true;

}





template <class T>
bvxm_voxel_slab<T> bvxm_voxel_storage_disk<T>::get_slab(unsigned slice_idx, unsigned slab_thickness)
{
  if (slice_idx + slab_thickness > grid_size_.z()) {
    vcl_cerr << "error: tried to get slab " << slice_idx << " with thickness " << slab_thickness << "; grid_size_.z() = " << grid_size_.z() << vcl_endl;
    bvxm_voxel_slab<T> slab;
    return slab;
  }
  // check to see if file is already open
  if (!fio_.is_open()) {
    fio_.open(storage_fname_.c_str(),vcl_ios::binary | vcl_ios::in | vcl_ios::out);
  }
  unsigned long slice_pos = grid_size_.x()*grid_size_.y()*slice_idx*sizeof(T) + sizeof(bvxm_voxel_storage_header<T>);
  unsigned long file_pos = fio_.tellg();
  if (slice_pos != file_pos) {
    fio_.seekg(slice_pos,vcl_ios::beg);
  }
  fio_.read(reinterpret_cast<char*>(slab_buffer_->data()),slab_buffer_->size());
  bvxm_voxel_slab<T> slab(grid_size_.x(),grid_size_.y(),1,slab_buffer_,reinterpret_cast<T*>(slab_buffer_->data()));
  return slab;
}

template <class T>
void bvxm_voxel_storage_disk<T>::put_slab()
{
  // check to see if file is already open
  if (!fio_.is_open()) {
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),vcl_ios::binary | vcl_ios::out | vcl_ios::in);
    if (!fio_.is_open()) {
      vcl_cerr << "error opening " << storage_fname_ << "for read/write " << vcl_endl;
      return;
    }
  }
  unsigned long in_pos = fio_.tellg();
  if (in_pos < grid_size_.x()*grid_size_.y()*sizeof(T) + sizeof(bvxm_voxel_storage_header<T>)) {
    vcl_cerr << "error: attempted to put_slice() when current file position is not past first slice." << vcl_endl;
    return;
  }
  unsigned long slice_pos = in_pos - (grid_size_.x()*grid_size_.y()*sizeof(T));
  unsigned long file_pos = fio_.tellp();
  if (slice_pos != file_pos) {
    fio_.seekp(slice_pos,vcl_ios::beg);
  }
  fio_.write(reinterpret_cast<char*>(slab_buffer_->data()),slab_buffer_->size());
  fio_.flush();
  
  return;

}

template <class T>
unsigned bvxm_voxel_storage_disk<T>::num_observations()
{
  // read header from disk
  // check to see if file is already open
  if (!fio_.is_open()) {
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),vcl_ios::binary | vcl_ios::out | vcl_ios::in);
    if (!fio_.is_open()) {
      vcl_cerr << "error opening " << storage_fname_ << "for read/write " << vcl_endl;
      return 0;
    }
  }
  // seek to beginning of file
  fio_.seekp(0,vcl_ios::beg);
  bvxm_voxel_storage_header<T> header;

  fio_.read(reinterpret_cast<char*>(&header),sizeof(header));

  return header.nobservations_;
}

template <class T>
void bvxm_voxel_storage_disk<T>::increment_observations()
{
  // read header from disk
  // check to see if file is already open
  if (!fio_.is_open()) {
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),vcl_ios::binary | vcl_ios::out | vcl_ios::in);
    if (!fio_.is_open()) {
      vcl_cerr << "error opening " << storage_fname_ << "for read/write " << vcl_endl;
      return;
    }
  }
  // seek to beginning of file
  fio_.seekp(0,vcl_ios::beg);
  bvxm_voxel_storage_header<T> header;
  // read
  fio_.read(reinterpret_cast<char*>(&header),sizeof(header));

  // increment observations
  ++header.nobservations_;

  // write header back to disk
  // seek to beginning of file
  fio_.seekp(0,vcl_ios::beg);
  // write
  fio_.write(reinterpret_cast<char*>(&header),sizeof(header));

}



#define BVXM_VOXEL_STORAGE_DISK_INSTANTIATE(T)\
template class bvxm_voxel_storage_disk<T >; \

#endif
