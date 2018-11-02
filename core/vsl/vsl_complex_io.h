// This is core/vsl/vsl_complex_io.h
#ifndef vsl_complex_io_h_
#define vsl_complex_io_h_
//:
// \file
// \brief  binary IO functions for std::complex<T>
// \author K.Y.McGaul

#include <iosfwd>
#include <complex>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write complex to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::complex<T>& v);

//: Read complex from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::complex<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream &os, const std::complex<T> &v);

#endif // vsl_complex_io_h_
