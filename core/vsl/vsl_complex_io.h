// This is vxl/vsl/vsl_complex_io.h
#ifndef vsl_complex_io_h_
#define vsl_complex_io_h_
//:
// \file 
// \brief  binary IO functions for vcl_complex<T>
// \author K.Y.McGaul

#include <vcl_iosfwd.h>
#include <vcl_complex.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write complex to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_complex<T>& v);

//: Read complex from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_complex<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream &os, const vcl_complex<T> &v);

#endif // vsl_complex_io_h_
