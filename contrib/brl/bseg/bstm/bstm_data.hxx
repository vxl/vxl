#ifndef bstm_data_hxx_
#define bstm_data_hxx_
#include "bstm_data.h"

template <bstm_data_type T>
bstm_data<T>::bstm_data(bstm_data_base &data) : bstm_data_base_(data) {
  unsigned array_length = data.buffer_length() / sizeof(datatype);
  data_array_ = boxm2_array_1d<datatype>(
      array_length, reinterpret_cast<datatype *>(data.data_buffer()));
}

#undef BSTM_DATA_INSTANTIATE
#define BSTM_DATA_INSTANTIATE(T) template class bstm_data<T>

#endif // bstm_data_hxx_
