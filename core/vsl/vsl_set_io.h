// This is vxl/vsl/vsl_set_io.h
#ifndef vsl_set_io_h_
#define vsl_set_io_h_
//:
// \file 
// \brief binary IO functions for vcl_set<T>
// \author K.Y.McGaul

#include <vcl_iosfwd.h>
#include <vcl_set.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write set to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_set<T>& v);

//: Read set from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_set<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream & os,const vcl_set<T> &v);

#endif // vsl_set_io_h_
