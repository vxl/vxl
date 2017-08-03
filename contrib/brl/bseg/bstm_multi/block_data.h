#ifndef bstm_multi_block_data_h
#define bstm_multi_block_data_h
//:
// \file
// \brief A class that represents a data buffer for a block, templated on the
// datra type.
// Based on boxm2_data.h
//
// \author Raphael Kargon
// \date Aug 01, 2017

#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <boxm2/basic/boxm2_array_1d.h>
#include <bstm/bstm_data_traits.h>
#include <bstm_multi/block_data_base.h>

//: Specific, templated derived class for data blocks
template <bstm_data_type T> class block_data : public block_data_base {
public:
  //: type of data (float for alpha, bytes for mixture model, etc)
  typedef typename bstm_data_traits<T>::datatype datatype;

  //: creates block_data object from byte buffer and id
  block_data(char *data_buffer, vcl_size_t length)
      : block_data_base(data_buffer, length)
      , data_array_(buffer_length_ / sizeof(datatype),
                    reinterpret_cast<datatype *>(data_buffer)) {}

  //: destructor
  virtual ~block_data() {}

  //: data array accessor
  boxm2_array_1d<datatype> &data() { return data_array_; }
  // A const overload because hey why not
  const boxm2_array_1d<datatype> &data() const { return data_array_; }

protected:
  boxm2_array_1d<datatype> data_array_;
};

#endif // bstm_multi_block_data_h
