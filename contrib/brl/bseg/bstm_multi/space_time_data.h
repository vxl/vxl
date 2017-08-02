#ifndef space_time_data_h
#define space_time_data_h
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
#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_data_traits.h>
#include <bstm_multi/space_time_data_base.h>

//: Specific, templated derived class for data blocks
template <bstm_data_type T>
class space_time_data : public space_time_data_base {
public:
  //: type of data (float for alpha, bytes for mixture model, etc)
  typedef typename bstm_data_traits<T>::datatype datatype;

  //: creates space_time_data object from byte buffer and id
  space_time_data(char *data_buffer, vcl_size_t length, bstm_block_id id)
      : space_time_data_base(data_buffer, length, id)
      , data_array_(buffer_length_ / sizeof(datatype),
                    reinterpret_cast<datatype *>(data_buffer)) {}

  //: destructor
  virtual ~space_time_data() {}

  //: data array accessor
  boxm2_array_1d<datatype> &data() { return data_array_; }
  // A const overload because hey why not
  const boxm2_array_1d<datatype> &data() const { return data_array_; }

protected:
  boxm2_array_1d<datatype> data_array_;
};

#endif // space_time_data_h
