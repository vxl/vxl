// This is core/vsl/vsl_deque_io.h
#ifndef vsl_deque_io_h_
#define vsl_deque_io_h_
//:
// \file
// \brief binary IO functions for std::deque<T>
// \author K.Y.McGaul

#include <iosfwd>
#include <deque>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write deque to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::deque<T>& v);

//: Read deque from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::deque<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream &os, const std::deque<T> &v);

#endif // vsl_deque_io_h_
