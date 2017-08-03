#ifndef bstm_multi_block_data_base_h_
#define bstm_multi_block_data_base_h_
//:
// \file block_data_base.h
// \brief Generic, untemplated class for data blocks that does not need to know
// block metadata. Essentially just contains a data buffer and its length. The
// underlying buffer is owned by this class and deleted upon destruction.
//

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>

#include <bstm/bstm_data_traits.h>

//: Generic, untemplated base class for data blocks
class block_data_base : public vbl_ref_count {
public:
  //: Constructor - beware that the data_buffer becomes OWNED (and will be
  // deleted) by this class!
  block_data_base(char *data_buffer, vcl_size_t length, bool read_only = true)
      : read_only_(read_only)
      , buffer_length_(length)
      , data_buffer_(data_buffer) {}

  // Initializes this data block with length 0 and a NULLPTR data pointer.
  block_data_base(const vcl_string data_type, bool read_only)
      : read_only_(read_only), buffer_length_(0), data_buffer_(VXL_NULLPTR) {}

  // void set_default_value(vcl_string data_type); // TODO not sure if we need
  // this

  //: This destructor is correct - by our design the original data_buffer
  // becomes OWNED by the data_base class
  virtual ~block_data_base() {
    if (data_buffer_)
      delete[] data_buffer_;
  }

  //: accessor for low level byte buffer kept by the data_base
  char *data_buffer() { return data_buffer_; }
  vcl_size_t buffer_length() const { return buffer_length_; }

  //: returns copy of portion of buffer corresponding a cell. Returns
  // cell_size bytes starting at index i of data buffer. Returns
  // VXL_NULLPTR if request goes outside of bounds.
  char *cell_buffer(vcl_size_t i, vcl_size_t cell_size);

  //: by default data is read-only, i.e. cache doesn't save it before destroying
  // it
  bool read_only_;
  void enable_write() { read_only_ = false; }
  void disable_write() { read_only_ = true; }

protected:
  //: byte buffer and its size
  vcl_size_t buffer_length_;
  char *data_buffer_;
};

//: Smart_Pointer typedef for bstm_data_base
typedef vbl_smart_ptr<block_data_base> block_data_base_sptr;

void vsl_b_write(vsl_b_ostream &os, block_data_base const &scene) {}
void vsl_b_write(vsl_b_ostream &os, const block_data_base *&p) {}
void vsl_b_write(vsl_b_ostream &os, block_data_base_sptr &sptr) {}
void vsl_b_write(vsl_b_ostream &os, block_data_base_sptr const &sptr) {}

void vsl_b_read(vsl_b_istream &is, block_data_base &scene) {}
void vsl_b_read(vsl_b_istream &is, block_data_base *p) {}
void vsl_b_read(vsl_b_istream &is, block_data_base_sptr &sptr) {}
void vsl_b_read(vsl_b_istream &is, block_data_base_sptr const &sptr) {}

#endif // bstm_multi_block_data_base_h_
