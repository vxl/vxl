// This is vxl/vsl/vsl_complex_io.txx
#ifndef vsl_complex_io_txx_
#define vsl_complex_io_txx_
//:
// \file
// \brief  binary IO functions for vcl_complex<T>
// \author K.Y.McGaul
//
// Implementation

#include "vsl_complex_io.h"
#include <vsl/vsl_binary_io.h>


//====================================================================================
//: Write complex to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_complex<T>& v)
{
  // Do not write a version number here for space efficiency reasons.
  // There is no reason to expect the format to change
  vsl_b_write(s, vcl_real(v));
  vsl_b_write(s, vcl_imag(v));
}

//====================================================================================
//: Read complex from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_complex<T>& v)
{
  T real_part, imag_part;
  vsl_b_read(s, real_part);
  vsl_b_read(s, imag_part);
  v = vcl_complex<T>(real_part, imag_part);
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_complex<T> &v)
{
  os << vcl_real(v) << " + " << vcl_imag(v) << "i ";
}

#define VSL_COMPLEX_IO_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream&, const vcl_complex<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_complex<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_complex<T >& v)

#endif // vsl_complex_io_txx_
