// This is core/vsl/vsl_set_io.h
#ifndef vsl_set_io_h_
#define vsl_set_io_h_
//:
// \file
// \brief binary IO functions for std::set<T>
// \author K.Y.McGaul

#include <iosfwd>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write set to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::set<T>& v);

//: Read set from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, std::set<T>& v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream & os,const std::set<T> &v);

#endif // vsl_set_io_h_
