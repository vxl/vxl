// This is core/vsl/vsl_stack_io.h
#ifndef vsl_stack_io_h_
#define vsl_stack_io_h_
//:
// \file
// \brief binary IO functions for std::stack<T>
// \author K.Y.McGaul

#include <iosfwd>
#include <stack>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write stack to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::stack<T>& v);

//: Read stack from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::stack<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream & os,const std::stack<T> &v);

#endif // vsl_stack_io_h_
