// This is core/vbl/io/vbl_io_quadruple.h
#ifndef vbl_io_quadruple_h_
#define vbl_io_quadruple_h_
//:
// \file
// \brief   binary IO functions for vbl_quadruple<S, T, U>
// \author  Ian Scott

#include <vsl/vsl_fwd.h>
#include <vbl/vbl_quadruple.h>
#include <vcl_iosfwd.h>

//: Write pair to binary stream
template <class S, class T, class U, class V>
void vsl_b_write(vsl_b_ostream& s, const vbl_quadruple<S, T, U, V>& v);

//: Read pair from binary stream
template <class S, class T, class U, class V>
void vsl_b_read(vsl_b_istream& s, vbl_quadruple<S, T, U, V>& v);

//: Print human readable summary of object to a stream
template <class S, class T, class U, class V>
void vsl_print_summary(vcl_ostream & os,const vbl_quadruple<S, T, U, V> &v);

#endif // vbl_io_quadruple_h_
