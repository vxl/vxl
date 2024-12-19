// This is core/vsl/vsl_tuple_io.hxx
#ifndef vsl_tuple_io_hxx_
#define vsl_tuple_io_hxx_
//:
// \file
// \brief  binary IO functions for std::tuple<T1, T2, ...>
// \author Noah Johnson
//
// Implementation

#include "vsl_tuple_io.h"
#include "vsl_utils.hxx"

#include <tuple>

#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>


//: Write tuple to binary stream
template <typename... Types>
void
vsl_b_write(vsl_b_ostream & os, const std::tuple<Types...> & v)
{

  constexpr short version_no = 1;
  vsl_b_write(os, version_no);

  _vsl_for_each(v, _vsl_generic_write(os));
}


//: Read tuple from binary stream
template <typename... Types>
void
vsl_b_read(vsl_b_istream & is, std::tuple<Types...> & v)
{
  if (!is)
    return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
    {
      _vsl_for_each(v, _vsl_generic_read(is));
      break;
    }
    default:
    {
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::tuple<Types ...>&), "
                << "Unknown version number " << io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
}


//: Print human readable summary of tuple to a stream
template <typename... Types>
void
vsl_print_summary(std::ostream & os, const std::tuple<Types...> & v)
{

  os << vsl_indent() << "Tuple size: " << std::tuple_size<std::tuple<Types...>>::value << std::endl;
  vsl_indent_inc(os);
  _vsl_for_each(v, _vsl_generic_print(os));
  os << std::endl;
  vsl_indent_dec(os);
}


#undef VSL_TUPLE_IO_INSTANTIATE
#define VSL_TUPLE_IO_INSTANTIATE(...)                                                                 \
  template void vsl_print_summary<__VA_ARGS__>(std::ostream & os, const std::tuple<__VA_ARGS__> & v); \
  template void vsl_b_write<__VA_ARGS__>(vsl_b_ostream & os, const std::tuple<__VA_ARGS__> & v);      \
  template void vsl_b_read<__VA_ARGS__>(vsl_b_istream & is, std::tuple<__VA_ARGS__> & v)

#endif // vsl_tuple_io_hxx_
