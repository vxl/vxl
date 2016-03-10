#ifndef boxm2_data_hxx_
#define boxm2_data_hxx_
#include "boxm2_data.h"

template <boxm2_data_type T>
boxm2_data<T>::boxm2_data(char * data_buffer,
                          std::size_t length,
                          boxm2_block_id id)
: boxm2_data_base(data_buffer,length,id)
{
  unsigned array_length = buffer_length_/sizeof(datatype);
  data_array_ = boxm2_array_1d<datatype>(array_length, reinterpret_cast<datatype*>(data_buffer));
}

template <boxm2_data_type T>
boxm2_data<T>::~boxm2_data()
{
  //delete data_array_;
}

#undef BOXM2_DATA_INSTANTIATE
#define BOXM2_DATA_INSTANTIATE(T) \
template class boxm2_data<T >

#endif // boxm2_data_hxx_
