// This is core/vsl/vsl_complex_io.hxx
#ifndef vsl_complex_io_hxx_
#define vsl_complex_io_hxx_
//:
// \file
// \brief  binary IO functions for std::complex<T>
// \author K.Y.McGaul
//
// Implementation

#include "vsl_complex_io.h"
#include <vsl/vsl_binary_io.h>


//====================================================================================
//: Write complex to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::complex<T>& v)
{
  // Do not write a version number here for space efficiency reasons.
  // There is no reason to expect the format to change
  vsl_b_write(s, std::real(v));
  vsl_b_write(s, std::imag(v));
}

//====================================================================================
//: Read complex from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::complex<T>& v)
{
  T real_part, imag_part;
  vsl_b_read(s, real_part);
  vsl_b_read(s, imag_part);
  v = std::complex<T>(real_part, imag_part);
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(std::ostream& os, const std::complex<T> &v)
{
  os << std::real(v) << " + " << std::imag(v) << "i ";
}

#define VSL_COMPLEX_IO_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream&, const std::complex<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const std::complex<T >& v); \
template void vsl_b_read(vsl_b_istream& s, std::complex<T >& v)

#endif // vsl_complex_io_hxx_
