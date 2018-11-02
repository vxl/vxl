// This is core/vsl/vsl_list_io.h
#ifndef vsl_list_io_h_
#define vsl_list_io_h_
//:
// \file
// \brief binary IO functions for std::list<T>
// \author K.Y.McGaul

#include <iosfwd>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write list to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::list<T>& v);

//: Read list from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::list<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream &os, const std::list<T> &v);

#endif // vsl_list_io_h_
