// This is vxl/vsl/vsl_complex_io.txx

//:
// \file   
// \brief  binary IO functions for vcl_complex<T>
// \author K.Y.McGaul
// Implementation

#include <vsl/vsl_complex_io.h>
#include <vsl/vsl_binary_io.h>


//====================================================================================
//: Write complex to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_complex<T>& v)
{
  vsl_b_write(s, v.real());
  vsl_b_write(s, v.imag());
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
  os << v.real() << " + " << v.imag() << "i ";
}

#define VSL_COMPLEX_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream&, const vcl_complex<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_complex<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_complex<T >& v); \
;
