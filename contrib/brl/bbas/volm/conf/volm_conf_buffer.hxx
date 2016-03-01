#ifndef volm_conf_buffer_hxx_
#define volm_conf_buffer_hxx_

#include "volm_conf_buffer.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#include <vul/vul_file.h>

template <class T>
volm_conf_buffer<T>::volm_conf_buffer()
 : current_id_(0), buffer_size_(0), active_buffer_(0), m_(NOT_INITIALIZED), file_name_(""), global_current_id_(0), ofs_ptr_(0), ifs_ptr_(0), prev_length_(-1), read_in_(0)
{
  length_vec_.clear();
}

template <class T>
volm_conf_buffer<T>::volm_conf_buffer(float const& buffer_capacity)
 : current_id_(0), m_(NOT_INITIALIZED), file_name_(""), global_current_id_(0), ofs_ptr_(0), ifs_ptr_(0), prev_length_(-1), read_in_(0)
{
  length_vec_.clear();
  buffer_size_ = (unsigned)vcl_floor( (buffer_capacity*1024.0f*1024.0f*1024.0f)/(2.0f*unit_size_));
  active_buffer_ = new T[buffer_size_];
}

template <class T>
volm_conf_buffer<T>::~volm_conf_buffer()
{
  this->finalize();
  if (active_buffer_)
    delete [] active_buffer_;
  // delete file stream pointer
  if (ifs_ptr_) {
    delete ifs_ptr_;  ifs_ptr_ = 0;
  }
  if (ofs_ptr_) {
    delete ofs_ptr_;  ofs_ptr_ = 0;
  }
}

template <class T>
bool volm_conf_buffer<T>::finalize()
{
  if (m_ == WRITE) // write whatever in buffer to disk
  {
    this->write_to_disk();
    (*ofs_ptr_).close();
  }
  if (m_ == READ)
    (*ifs_ptr_).close();
  m_ = NOT_INITIALIZED;
  return true;
}

template <class T>
bool volm_conf_buffer<T>::initialize_write(vcl_string const& filename)
{
  file_name_ = filename;
  m_ = WRITE;
  if (ifs_ptr_) {
    (*ifs_ptr_).close();
    delete ifs_ptr_;
    ifs_ptr_ = 0;
  }
  ofs_ptr_ = new vsl_b_ofstream(filename);
  current_id_ = 0;
  global_current_id_  = 0;
  length_vec_.clear();
  return true;
}

template <class T>
bool volm_conf_buffer<T>::initialize_read(vcl_string const& filename)
{
  //if (!vul_file::exists(filename))  {
  //  vcl_cerr << "In volm_conf_buffered_index: can not find " << filename << " for reading!\n";
  //  return false;
  //}
  file_name_ = filename;
  m_ = READ;
  // clean the output file stream
  if (ofs_ptr_) {
    (*ofs_ptr_).close();
    delete ofs_ptr_;
    ofs_ptr_ = 0;
  }
  ifs_ptr_ = new vsl_b_ifstream(filename);
  current_id_ = 0;
  global_current_id_ = 0;
  prev_length_ = -1;
  read_in_ = 0;
  length_vec_.clear();
  return true;
}

template <class T>
bool volm_conf_buffer<T>::add_to_index(vcl_vector<T> const& values)
{
  if (m_ != WRITE) {
    vcl_cerr << "In volm_conf_buffered_index: index object is in not in WRITE mode! cannot add to index!\n";
    return false;
  }
  if (values.size() > buffer_size_) {
    vcl_cerr << "In volm_conf_buffered_index: index size is larger than buffer size! increase the buffer capacity!\n";
    return false;
  }
  // check whether buffer is filled up
  if ((current_id_+values.size()) >= buffer_size_) {
    this->write_to_disk();
    current_id_ = 0;
    length_vec_.clear();
  }
  for (unsigned k = 0; k < values.size(); k++)
    active_buffer_[current_id_ + k] = values[k];
  current_id_ += (unsigned)values.size();
  length_vec_.push_back((int)values.size());
  return true;
}

template <class T>
bool volm_conf_buffer<T>::add_to_index(T const& value)
{
  vcl_vector<T> values(1);
  values[0] = value;
  return this->add_to_index(values);
}

template <class T>
bool volm_conf_buffer<T>::get_next(vcl_vector<T>& values)
{
  if (m_ != READ) {
    vcl_cerr << "In volm_conf_buffered_index: index object is NOT in READ mode! cannot read from index!\n";
    return false;
  }
  values.clear();
  // check whether cache is exhausted, get a chunk from the disc again
  if (current_id_ == 0 || current_id_ >= global_current_id_) {
    read_in_ = 0;
    if (!this->read_to_buffer())
      return false;
  }
  unsigned s = current_id_;
  unsigned e = current_id_ + length_vec_[read_in_];
  for (unsigned i = s; i < e; i++)
    values.push_back(active_buffer_[i]);
  current_id_ += length_vec_[read_in_++];
  return true;
}

template <class T>
bool volm_conf_buffer<T>::get_next(T& value)
{
  vcl_vector<T> values;
  if (!this->get_next(values))
    return false;
  value = values[0];
  return true;
}

template <class T>
bool volm_conf_buffer<T>::write_to_disk()
{
  if (!ofs_ptr_) {
    vcl_cerr << "In volm_conf_buffered_index: write data into disk failed!\n";
    return false;
  }
  //vcl_cout << "write to disk..." << this->file_name_ << vcl_endl;
  unsigned cnt = 0;
  for (vcl_vector<int>::iterator vit = length_vec_.begin(); vit != length_vec_.end(); ++vit) {
    unsigned s = cnt;
    unsigned e = cnt + (*vit);
    // write into binary file
    vsl_b_write((*ofs_ptr_), (*vit));
    for (unsigned i = s; i < e; i++)
      vsl_b_write((*ofs_ptr_), active_buffer_[i]);
    cnt += (*vit);
  }
  return true;
}

template <class T>
bool volm_conf_buffer<T>::read_to_buffer()
{
  if (!ifs_ptr_) {
    vcl_cerr << "In volm_conf_buffered_index: read data from disk failed!\n";
    return false;
  }
  length_vec_.clear();
  current_id_ = 0;
  global_current_id_ = 0;
  unsigned idx = 0;
  // read from file until the buffer is filled up
  bool filled_up = false;
  if (prev_length_ == -1) {
    vsl_b_read(*ifs_ptr_, prev_length_);
  }
  while (!filled_up)
  {
    if ( (global_current_id_ + prev_length_) >= buffer_size_) {
      filled_up = true;
      continue;
    }
    global_current_id_ += prev_length_;
    // read data into buffer
    for (unsigned i = 0; i < prev_length_; i++) {
      T obj;
      vsl_b_read(*ifs_ptr_, obj);
      active_buffer_[idx++] = obj;
    }
    length_vec_.push_back(prev_length_);
    // check whether the end of file has been reached
    if (ifs_ptr_->is().peek() == EOF)
      return true;
    // read next length
    vsl_b_read(*ifs_ptr_, prev_length_);
  }
  return true;
}


#undef VOLM_CONF_BUFFER_INSTANTIATE
#define VOLM_CONF_BUFFER_INSTANTIATE(T) \
template class volm_conf_buffer<T >

#endif // volm_conf_buffer_hxx_
