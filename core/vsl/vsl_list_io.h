// This is vxl/vsl/vsl_list_io.h
#ifndef vsl_list_io_h_
#define vsl_list_io_h_
//:
// \file 
// \brief binary IO functions for vcl_list<T>
// \author K.Y.McGaul

#include <vcl_iosfwd.h>
#include <vcl_list.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write list to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_list<T>& v);

//: Read list from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_list<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream &os, const vcl_list<T> &v);

#endif // vsl_list_io_h_
