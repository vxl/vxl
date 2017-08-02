#ifndef bstm_multi_space_time_data_base_h_
#define bstm_multi_space_time_data_base_h_
//:
// \file space_time_data_base.h
// \brief Generic, untemplated class for data blocks that does not need to know
// block metadata. Essentially wraps a data buffer and keeps track of the ID of
// its owning block. The underlying buffer is owned by this class and dleted
// upon destruction.
//

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_data_traits.h>

//: Generic, untemplated base class for data blocks
class space_time_data_base : public vbl_ref_count {
public:
  //: Constructor - beware that the data_buffer becomes OWNED (and will be
  // deleted) by this class!
  space_time_data_base(char *data_buffer,
                       vcl_size_t length,
                       bstm_block_id id,
                       bool read_only = true)
      : read_only_(read_only)
      , id_(id)
      , buffer_length_(length)
      , data_buffer_(data_buffer) {}

  // Initializes this data block with length 0 and a NULLPTR data pointer.
  space_time_data_base(const bstm_block_id &id,
                       const vcl_string data_type,
                       bool read_only)
      : read_only_(read_only)
      , id_(id)
      , buffer_length_(0)
      , data_buffer_(VXL_NULLPTR) {}

  // void set_default_value(vcl_string data_type); // TODO not sure if we need this

  //: This destructor is correct - by our design the original data_buffer
  // becomes OWNED by the data_base class
  virtual ~space_time_data_base() {
    if (data_buffer_)
      delete[] data_buffer_;
  }

  //: accessor for low level byte buffer kept by the data_base
  char *data_buffer() { return data_buffer_; }
  vcl_size_t buffer_length() const { return buffer_length_; }
  bstm_block_id &block_id() { return id_; }
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
  //: id for this particular block
  bstm_block_id id_;

  //: byte buffer and its size
  vcl_size_t buffer_length_;
  char *data_buffer_;
};

//: Smart_Pointer typedef for bstm_data_base
typedef vbl_smart_ptr<space_time_data_base> space_time_data_base_sptr;

void vsl_b_write(vsl_b_ostream &os, space_time_data_base const &scene) {}
void vsl_b_write(vsl_b_ostream &os, const space_time_data_base *&p) {}
void vsl_b_write(vsl_b_ostream &os, space_time_data_base_sptr &sptr) {}
void vsl_b_write(vsl_b_ostream &os, space_time_data_base_sptr const &sptr) {}

void vsl_b_read(vsl_b_istream &is, space_time_data_base &scene) {}
void vsl_b_read(vsl_b_istream &is, space_time_data_base *p) {}
void vsl_b_read(vsl_b_istream &is, space_time_data_base_sptr &sptr) {}
void vsl_b_read(vsl_b_istream &is, space_time_data_base_sptr const &sptr) {}

#endif // bstm_multi_space_time_data_base_h_
