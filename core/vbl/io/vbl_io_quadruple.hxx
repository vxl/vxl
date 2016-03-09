// This is core/vbl/io/vbl_io_quadruple.hxx
#ifndef vbl_io_quadruple_hxx_
#define vbl_io_quadruple_hxx_
//:
// \file
// \brief  binary IO functions for vbl_quadruple<S, T, U, V>
// \author Ian Scott

#include "vbl_io_quadruple.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write pair to binary stream
template <class S, class T, class U, class V>
void vsl_b_write(vsl_b_ostream& s, const vbl_quadruple<S, T, U, V>& v)
{
  // Do not write a version number here for space efficiency reasons.
  // There is no reason to expect the format to change
  vsl_b_write(s,v.first);
  vsl_b_write(s,v.second);
  vsl_b_write(s,v.third);
  vsl_b_write(s,v.fourth);
}

//====================================================================================
//: Read pair from binary stream
template <class S, class T, class U, class V>
void vsl_b_read(vsl_b_istream& s, vbl_quadruple<S, T, U, V>& v)
{
  vsl_b_read(s,v.first);
  vsl_b_read(s,v.second);
  vsl_b_read(s,v.third);
  vsl_b_read(s,v.fourth);
}


//====================================================================================
//: Output a human readable summary to the stream
template <class S, class T, class U, class V>
void vsl_print_summary(std::ostream& os, const vbl_quadruple<S, T, U, V> &v)
{
  os << "(";
  vsl_print_summary(os, v.first);
  os << ", ";
  vsl_print_summary(os, v.second);
  os << ", ";
  vsl_print_summary(os, v.third);
  os << ", ";
  vsl_print_summary(os, v.fourth);
  os << ")";
}


#undef VBL_QUADRUPLE_INSTANTIATE
#define VBL_QUADRUPLE_INSTANTIATE(S, T, U, V ) \
template void vsl_print_summary(std::ostream& s, const vbl_quadruple<S, T, U, V >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vbl_quadruple<S, T, U, V > & v); \
template void vsl_b_read(vsl_b_istream& s, vbl_quadruple<S, T, U, V > & v)

#endif // vbl_io_quadruple_hxx_
