// This is vxl/vsl/vsl_deque_io.h
#ifndef vsl_deque_io_h_
#define vsl_deque_io_h_
//:
// \file 
// \brief binary IO functions for vcl_deque<T>
// \author K.Y.McGaul

#include <vcl_iosfwd.h>
#include <vcl_deque.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write deque to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_deque<T>& v);

//: Read deque from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_deque<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream &os, const vcl_deque<T> &v);

#endif // vsl_deque_io_h_
