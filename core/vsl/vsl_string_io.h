// This is core/vsl/vsl_string_io.h
#ifndef vsl_string_io_h_
#define vsl_string_io_h_
//:
// \file
// \brief   binary IO functions for std::basic_string<T>
// \author  K.Y.McGaul

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class vsl_b_ostream;
class vsl_b_istream;

//: Write string to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::basic_string<T>& v);

//: Read string from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::basic_string<T>& v);

#endif // vsl_string_io_h_
