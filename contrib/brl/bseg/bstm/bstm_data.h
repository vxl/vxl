#ifndef bstm_data_h
#define bstm_data_h

//:
// \file bstm_data.h
// \brief NON-OWNING wrapper for bstm_data_base that provides a view into the
// data buffer that is typed appropriately. (i.e. allows indexing by appearance
// data elements and not just bytes)
//
// This differs in implementation from boxm2_data.h and bstm_multi_data.h for
// logistic reasons involving existing code.
//
// \author Raphael Kargon
// \date Aug 14, 2017

#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm2/basic/boxm2_array_1d.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_data_traits.h>

template <bstm_data_type T> class bstm_data {
public:
  //: type of data (float for alpha, bytes for mixture model, etc)
  typedef typename bstm_data_traits<T>::datatype datatype;

  //: Wraps a bstm_data_base object
  bstm_data(bstm_data_base &data) : bstm_data_base_(data) {
    unsigned array_length = data.buffer_length() / sizeof(datatype);
    data_array_ = boxm2_array_1d<datatype>(
        array_length, reinterpret_cast<datatype *>(data.data_buffer()));
  }

  //: data array accessor
  boxm2_array_1d<datatype> &data() { return data_array_; }
  const boxm2_array_1d<datatype> &data() const { return data_array_; }

  // bstm data buffer accessor
  bstm_data_base &get_data_base() { return bstm_data_base_; }
  const bstm_data_base &get_data_base() const { return bstm_data_base_; }

  // Has science gone too far?
  bstm_data_base *operator->() { return &bstm_data_base_; }

  datatype &operator[](std::size_t idx) { return data_array_[idx]; }
  const datatype &operator[](std::size_t idx) const { return data_array_[idx]; }

private:
  bstm_data_base &bstm_data_base_;
  boxm2_array_1d<datatype> data_array_;
};

#endif // bstm_data_h
