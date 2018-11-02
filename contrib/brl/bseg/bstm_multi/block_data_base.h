#ifndef bstm_multi_block_data_base_h_
#define bstm_multi_block_data_base_h_
//:
// \file block_data_base.h
// \brief Generic, untemplated class for data blocks that does not need to know
// block metadata. Essentially just contains a data buffer and its length. The
// underlying buffer is owned by this class and deleted upon destruction.
//
// Based off of boxm2_data_base.h
//
// \author Raphael Kargon
// \date Aug 03, 2017
//

#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

#include <bstm/bstm_data_traits.h>

//: Generic, untemplated base class for data blocks
class block_data_base : public vbl_ref_count {
public:
  //: Constructor - beware that the data_buffer becomes OWNED (and will be
  // deleted) by this class!
  block_data_base(char *data_buffer, std::size_t length, bool read_only = true)
      : read_only_(read_only)
      , buffer_length_(length)
      , data_buffer_(data_buffer) {}

  block_data_base(std::size_t length, bool read_only = true)
      : read_only_(read_only)
      , buffer_length_(length)
      , data_buffer_(new char[length]()) {}

  block_data_base(std::size_t num_elements,
                  const std::string &datatype,
                  bool read_only = true)
      : read_only_(read_only)
      , buffer_length_(num_elements * bstm_data_info::datasize(datatype))
      , data_buffer_(new char[buffer_length_]()) {}

  // //: Initializes this data block with length 0 and a NULLPTR data pointer.
  // block_data_base(bool read_only = true) : read_only_(read_only) {}

  // NOTE unlike in BSTM/BOXM2, block_data_base creates new buffers as
  // empty. Thus there is no need to initialize them.
  // void set_default_value(std::string data_type);

  //: This destructor is correct - by our design the original data_buffer
  // becomes OWNED by the data_base class
  ~block_data_base() override {
    if (data_buffer_)
      delete[] data_buffer_;
  }

  //: accessor for low level byte buffer kept by the data_base
  char *data_buffer() { return data_buffer_; }
  const char *data_buffer() const { return data_buffer_; }
  std::size_t buffer_length() const { return buffer_length_; }

  //: returns copy of portion of buffer corresponding a cell. Returns
  // cell_size bytes starting at index i of data buffer. Returns
  // nullptr if request goes outside of bounds. Caller owns returns data.
  char *cell_buffer(std::size_t i, std::size_t cell_size) const {
    if ((i + cell_size - 1) < buffer_length_) {
      char *out = new char[cell_size];
      for (std::size_t j = 0; j < cell_size; j++) {
        out[j] = data_buffer_[i + j];
      }
      return out;
    } else {
      return nullptr;
    }
  }

  // Replaces buffer with the given new buffer and length and deletes the old
  // data.
  void set_data_buffer(char *new_buffer, std::size_t new_len) {
    delete[] data_buffer_;
    data_buffer_ = new_buffer;
    buffer_length_ = new_len;
  }

  // Replaces buffer with a newly allocated and zero-initialized buffer of the
  // given length. Deletes old data.
  void new_data_buffer(std::size_t new_len) {
    delete[] data_buffer_;
    buffer_length_ = new_len;
    data_buffer_ = new char[new_len]();
  }

  void swap(block_data_base &other) {
    std::swap(buffer_length_, other.buffer_length_);
    std::swap(data_buffer_, other.data_buffer_);
  }

  void swap(block_data_base *other) { this->swap(*other); }

  //: by default data is read-only, i.e. cache doesn't save it before destroying
  // it
  bool read_only_;
  void enable_write() { read_only_ = false; }
  void disable_write() { read_only_ = true; }

protected:
  //: byte buffer and its size
  std::size_t buffer_length_;
  char *data_buffer_;
};

//: Smart_Pointer typedef for bstm_data_base
typedef vbl_smart_ptr<block_data_base> block_data_base_sptr;

#endif // bstm_multi_block_data_base_h_
