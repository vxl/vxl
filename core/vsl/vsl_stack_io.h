// This is vxl/vsl/vsl_stack_io.h
#ifndef vsl_stack_io_h_
#define vsl_stack_io_h_
//:
// \file 
// \brief binary IO functions for vcl_stack<T>
// \author K.Y.McGaul

#include <vcl_iosfwd.h>
#include <vcl_stack.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write stack to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_stack<T>& v);

//: Read stack from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_stack<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream & os,const vcl_stack<T> &v);

#endif // vsl_stack_io_h_
