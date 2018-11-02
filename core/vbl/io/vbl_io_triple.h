// This is core/vbl/io/vbl_io_triple.h
#ifndef vbl_io_triple_h_
#define vbl_io_triple_h_
//:
// \file
// \brief   binary IO functions for vbl_triple<S, T, U>
// \author  Ian Scott

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_triple.h>
class vsl_b_ostream;
class vsl_b_istream;

//: Write pair to binary stream
template <class S, class T, class U>
void vsl_b_write(vsl_b_ostream& s, const vbl_triple<S, T, U>& v);

//: Read pair from binary stream
template <class S, class T, class U>
void vsl_b_read(vsl_b_istream& s, vbl_triple<S, T, U>& v);

//: Print human readable summary of object to a stream
template <class S, class T, class U>
void vsl_print_summary(std::ostream & os,const vbl_triple<S, T, U> &v);

#endif // vbl_io_triple_h_
