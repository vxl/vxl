#ifndef bstm_multi_block_data_h
#define bstm_multi_block_data_h

//:
// \file block_data.h
// \brief NON-OWNING wrapper for block_data_base that provides a view into the
// data buffer that is typed appropriately. (i.e. allows indexing by appearance
// data elements and not just bytes)
//
// This differs in implementation from boxm2_data.h, which is a owning class
// that inherits from boxm2_data_base.
//
// NOTE that changing the underlying buffer's data pointer will invalidate this
// data class's data_array_.
//
// \author Raphael Kargon
// \date Aug 14, 2017

#include <boxm2/basic/boxm2_array_1d.h>
#include <bstm/bstm_data_traits.h>
#include <bstm_multi/block_data_base.h>

template <bstm_data_type T> class block_data {
public:
  //: type of data (float for alpha, bytes for mixture model, etc)
  typedef typename bstm_data_traits<T>::datatype datatype;

  //: Wraps a block_data_base object
  block_data(block_data_base &data) : block_data_base_(data) {
    update_data_array();
  }

  //: data array accessor
  boxm2_array_1d<datatype> &data() { return data_array_; }
  const boxm2_array_1d<datatype> &data() const { return data_array_; }

  // bstm_multi data buffer accessor
  block_data_base &get_data_base() { return block_data_base_; }
  const block_data_base &get_data_base() const { return block_data_base_; }

  // const only since changing block_data_base's underlying pointer will
  // invalidate data_array_. Will add non-const if really needed.
  const block_data_base *operator->() const { return &block_data_base_; }

  // typed array access
  datatype &operator[](std::size_t i) { return data_array_[i]; }
  const datatype &operator[](std::size_t i) const { return data_array_[i]; }

  std::size_t size() const { return data_array_.size(); }

  // creates new buffer for underlying base, and updates data array.
  void new_buffer(std::size_t length) {
    block_data_base_.new_data_buffer(length);
    update_data_array();
  }

  static const block_data const_wrapper(const block_data_base &data) {
    return block_data(const_cast<block_data_base &>(data));
  }

private:
  void update_data_array() {
    data_array_ = boxm2_array_1d<datatype>(
        block_data_base_.buffer_length() / sizeof(datatype),
        reinterpret_cast<datatype *>(block_data_base_.data_buffer()));
  }

  block_data_base &block_data_base_;
  boxm2_array_1d<datatype> data_array_;
};

#endif // bstm_multi_multi_block_data_h
