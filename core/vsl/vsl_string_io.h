// This is vxl/vsl/vsl_string_io.h
#ifndef vsl_string_io_h_
#define vsl_string_io_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file 
// \brief   binary IO functions for vcl_basic_string<T>
// \author  K.Y.McGaul

#include <vcl_string.h>
class vsl_b_ostream;
class vsl_b_istream;

//: Write string to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_basic_string<T>& v);

//: Read string from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_basic_string<T>& v);

#endif // vsl_string_io_h_
