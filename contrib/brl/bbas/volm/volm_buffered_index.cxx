#include <iostream>
#include <algorithm>
#include <iterator>
#include "volm_buffered_index.h"
//:
// \file
#include <bbas/volm/volm_spherical_container.h>
#include <boxm2/volm/boxm2_volm_locations.h>
#include <vgl/vgl_box_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool volm_buffered_index_params::write_params_file(const std::string& index_file_name_pre)
{
  //std::string index_params_file = vul_file::strip_extension(index_file_name) + ".params";
  std::string index_params_file = index_file_name_pre + ".params";
  std::ofstream ofs(index_params_file.c_str());
  if (!ofs.is_open())
    return false;
  ofs << "layer_size " << layer_size << std::endl;
  ofs.close();
  return true;
}

bool volm_buffered_index_params::read_params_file(const std::string& index_file_name_pre)
{
  //std::string index_params_file = vul_file::strip_extension(index_file_name) + ".params";
  std::string index_params_file = index_file_name_pre + ".params";
  std::ifstream ifs(index_params_file.c_str());
  if (!ifs.is_open())
    return false;
  std::string tmp;
  ifs >> tmp >> layer_size;
  ifs.close();
  return true;
}

bool volm_buffered_index_params::write_ex_param_file(const std::string& index_file_name_pre)
{
  //std::string index_params_file = vul_file::strip_extension(index_file_name) + ".params";
  std::string index_params_file = index_file_name_pre + ".params";
  std::ofstream ofs(index_params_file.c_str());
  if (ofs.fail())
    return false;
  ofs << "layer_size " << layer_size << std::endl;
  ofs << "radius_type " << radius.size() << std::endl;
  ofs << "orientation_type " << norients << std::endl;
  ofs << "land_type " << nlands << std::endl;
  ofs << "radius: ";
  for (double radiu : radius)
    ofs << radiu << ' ';
  ofs << std::endl;
  ofs.close();
  return true;
}

bool volm_buffered_index_params::read_ex_param_file(const std::string& index_file_name_pre)
{
  //std::string index_params_file = vul_file::strip_extension(index_file_name) + ".params";
  std::string index_params_file = index_file_name_pre + ".params";
  std::ifstream ifs(index_params_file.c_str());
  if (!ifs.is_open())
    return false;
  std::string tmp;
  unsigned nradius;
  ifs >> tmp >> layer_size;
  ifs >> tmp >> nradius;
  ifs >> tmp >> norients;
  ifs >> tmp >> nlands;
  ifs >> tmp;
  for (unsigned i = 0; i < nradius; i++) {
    double r;
    ifs >> r;
    radius.push_back(r);
  }
  return true;
}

bool volm_buffered_index_params::write_conf_param_file(std::string const& index_file_name_pre)
{
  std::string index_params_file = index_file_name_pre + ".params";
  std::ofstream ofs(index_params_file.c_str());
  if (ofs.fail())
    return false;
  ofs << "radius " << this->conf_radius << std::endl;
  ofs.close();
  return true;
}

bool volm_buffered_index_params::read_conf_param_file(std::string const& index_file_name_pre)
{
  std::string index_params_file = index_file_name_pre + ".params";
  std::ifstream ifs(index_params_file.c_str());
  if (!ifs.is_open())
    return false;
  std::string tmp;
  ifs >> tmp >> conf_radius;
  return true;
}

bool volm_buffered_index_params::write_size_file(const std::string& index_file_name, unsigned long indexed_cnt)
{
  std::string index_size_file = vul_file::strip_extension(index_file_name) + ".txt";
  std::ofstream ofs(index_size_file.c_str());
  if (!ofs.is_open())
    return false;
  ofs << indexed_cnt << '\n';
  ofs.close();
  return true;
}

bool volm_buffered_index_params::read_size_file(const std::string& index_file_name, unsigned long& size)
{
  std::string index_size_file = vul_file::strip_extension(index_file_name) + ".txt";
  std::ifstream ifs(index_size_file.c_str());
  if (!ifs.is_open())
    return false;
  ifs >> size;
  ifs.close();
  return true;
}


volm_buffered_index::volm_buffered_index(unsigned layer_size, float buffer_capacity) :
layer_size_(layer_size), buffer_size_(0), current_id_(0), current_global_id_(0), m_(NOT_INITIALIZED), file_name_(""), active_buffer_(nullptr)
{
  buffer_size_ = (unsigned int)std::floor((buffer_capacity*1024*1024*1024)/(2.0f*layer_size));
  active_buffer_ = new uchar[buffer_size_*layer_size_];
}

volm_buffered_index::~volm_buffered_index()
{
  finalize();

  if (active_buffer_)
    delete [] active_buffer_;
}

bool volm_buffered_index::initialize_write(const std::string& file_name)
{
  if (m_ == READ)
    this->finalize();
  m_ = WRITE;
  file_name_ = file_name;
  //f_obj_.open(file_name.c_str(), std::ios::app | std::ios::binary);
  of_obj_.open(file_name.c_str(), std::ios::binary);
  if (!of_obj_.good())
    return false;
  current_id_ = 0;
  current_global_id_ = 0;
  return true;
}

bool volm_buffered_index::initialize_read(const std::string& file_name)
{
  if (m_ == WRITE)
    this->finalize();
  m_ = READ;
  file_name_ = file_name;
  if_obj_.open(file_name.c_str(), std::ios::in | std::ios::binary);
  if (!if_obj_.good())
    return false;
  current_id_ = 0;
  current_global_id_ = 0;
  file_size_ = vul_file::size(file_name);
  read_so_far_ = 0;
  return true;
}

bool volm_buffered_index::finalize()
{
  if (m_ == WRITE && current_id_ != 0) { // write whatever is on the cache
    of_obj_.write((char*)active_buffer_, (long)(current_id_*layer_size_));
    of_obj_.close();
  }
  if (m_ == READ)
    if_obj_.close();

  m_ = NOT_INITIALIZED;
  return true;
}


//: just appends to the end of the current active buffer, nothing about which location hypothesis these values correspond is known.
//  caller is responsible to keep the ordering consistent with the hypotheses ordering
bool volm_buffered_index::add_to_index(std::vector<uchar>& values)
{
  if (m_ == READ) {
    std::cout << "index object is in READ mode! cannot add to index!\n";
    return false;
  }
  if (values.size() != layer_size_) {
    std::cout << "In boxm2_volm_wr3db_index::add_to_index() -- size of value array is not consistent with layer size of this index object!\n";
    return false;
  }
  if (current_id_ == buffer_size_) {  // write the current cache
    // initiate a write
    of_obj_.write((char*)active_buffer_, (long)(buffer_size_*layer_size_));
    current_id_ = 0;
  }

  for (unsigned i = 0; i < layer_size_; i++)
    active_buffer_[current_id_*layer_size_ + i] = values[i];
  current_id_++;
  current_global_id_++;

  return true;
}

//: caller is responsible to pass a valid array of size layer_size
bool volm_buffered_index::add_to_index(const uchar* values)
{
  if (m_ == READ) {
    std::cout << "index object is in READ mode! cannot add to index!\n";
    return false;
  }
  if (current_id_ == buffer_size_) {  // write the current cache
    // initiate a write
    of_obj_.write((char*)active_buffer_, (long)(buffer_size_*layer_size_));
    current_id_ = 0;
  }

  for (unsigned i = 0; i < layer_size_; i++)
    active_buffer_[current_id_*layer_size_ + i] = values[i];
  current_id_++;
  current_global_id_++;
  return true;
}

unsigned int volm_buffered_index::read_to_buffer(uchar* buf)
{
  long remaining = file_size_-read_so_far_;
  if ((long)(buffer_size_*layer_size_) <= remaining) {
    if_obj_.read((char*)buf, (long)(buffer_size_*layer_size_));
    read_so_far_ += (long)(buffer_size_*layer_size_);
    return buffer_size_;
  }
  else if (remaining > 0) {
    if_obj_.read((char*)buf, remaining);
    read_so_far_ += remaining;
    return remaining/layer_size_;
  }
  else
    return 0;
}

//: retrieve the next index, use the active_cache, if all on the active_cache has been retrieved, read from disc, values array is resized to layer_size
bool volm_buffered_index::get_next(std::vector<uchar>& values)
{
  if (m_ == WRITE) {
    std::cout << "index object is in WRITE mode! cannot read from index!\n";
    return false;
  }

  if (!current_global_id_ || current_id_ == active_buffer_size_) { // never read before or the cache is exhausted, get a chunk from the disc
    active_buffer_size_ = read_to_buffer(active_buffer_);
    if (!active_buffer_size_)
      return false;
    current_id_ = 0;
  }
  values.resize(layer_size_,0);
  for (unsigned i = 0; i < layer_size_; i++)
    values[i] = active_buffer_[current_id_*layer_size_ + i];
  current_global_id_++;
  current_id_++;
  return true;
}

//: caller is responsible to pass a valid array of size at least layer_size, if size>layer_size, fill the rest with zeros
bool volm_buffered_index::get_next(uchar* values, unsigned size)
{
  if (m_ == WRITE) {
    std::cout << "index object is in WRITE mode! cannot read from index!\n";
    return false;
  }

  if (!current_global_id_ || current_id_ == active_buffer_size_) { // never read before or the cache is exhausted, get a chunk from the disc
    active_buffer_size_ = read_to_buffer(active_buffer_);
    if (!active_buffer_size_)
      return false;
    current_id_ = 0;
  }
  for (unsigned i = 0; i < layer_size_; i++)
    values[i] = active_buffer_[current_id_*layer_size_ + i];
  //unsigned char val = (unsigned char)0;
  //for (unsigned i = layer_size_; i < size; i++)
  //  values[i] = val;
  std::fill(values+layer_size_, values+size, (unsigned char)0);
  current_global_id_++;
  current_id_++;
  return true;
}
