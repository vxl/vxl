// This is core/vsl/vsl_array_io.hxx
#ifndef vsl_array_io_hxx_
#define vsl_array_io_hxx_
//:
// \file
// \brief  binary IO functions for std::array<T, N>
// \author Noah Johnson
//
// Implementation

#include "vsl_array_io.h"

#include <array>

#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_block_binary.h>
#include <vsl/vsl_indent.h>


//: Write array to binary stream
template <class T, size_t N>
void
vsl_b_write(vsl_b_ostream & os, const std::array<T, N> & arr)
{

  constexpr short version_no = 1;
  vsl_b_write(os, version_no);

  if (N != 0)
  {
    vsl_block_binary_write(os, &arr.front(), N);
  }
}


//: Read array from binary stream
template <class T, size_t N>
void
vsl_b_read(vsl_b_istream & is, std::array<T, N> & arr)
{
  if (!is)
    return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
    {
      if (N != 0)
      {
        vsl_block_binary_read(is, &arr.front(), N);
      }
      break;
    }
    default:
    {
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::array<T, N>&), "
                << "Unknown version number " << io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
}


//: Print human readable summary of array to a stream
template <class T, size_t N>
void
vsl_print_summary(std::ostream & os, const std::array<T, N> & arr)
{
  os << vsl_indent() << "Array length: " << N << std::endl;
  for (size_t i = 0; i < N && i < 5; ++i)
  {
    os << vsl_indent() << ' ' << i << ": ";
    vsl_indent_inc(os);
    vsl_print_summary(os, arr[i]);
    os << std::endl;
    vsl_indent_dec(os);
  }
  if (N > 5)
    os << vsl_indent() << " ..." << std::endl;
}


#undef VSL_ARRAY_IO_INSTANTIATE
#define VSL_ARRAY_IO_INSTANTIATE(T, N)                                                  \
  template void vsl_print_summary<T, N>(std::ostream & os, const std::array<T, N> & v); \
  template void vsl_b_write<T, N>(vsl_b_ostream & os, const std::array<T, N> & v);      \
  template void vsl_b_read<T, N>(vsl_b_istream & is, std::array<T, N> & v)

#endif // vsl_array_io_hxx_
