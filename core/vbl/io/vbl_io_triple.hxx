// This is core/vbl/io/vbl_io_triple.hxx
#ifndef vbl_io_triple_hxx_
#define vbl_io_triple_hxx_
//:
// \file
// \brief  binary IO functions for vbl_triple<S, T, U>
// \author Ian Scott

#include "vbl_io_triple.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write pair to binary stream
template <class S, class T, class U>
void vsl_b_write(vsl_b_ostream& s, const vbl_triple<S, T, U>& v)
{
  // Do not write a version number here for space efficiency reasons.
  // There is no reason to expect the format to change
  vsl_b_write(s,v.first);
  vsl_b_write(s,v.second);
  vsl_b_write(s,v.third);
}

//====================================================================================
//: Read pair from binary stream
template <class S, class T, class U>
void vsl_b_read(vsl_b_istream& s, vbl_triple<S, T, U>& v)
{
  vsl_b_read(s,v.first);
  vsl_b_read(s,v.second);
  vsl_b_read(s,v.third);
}


//====================================================================================
//: Output a human readable summary to the stream
template <class S, class T, class U>
void vsl_print_summary(std::ostream& os, const vbl_triple<S, T, U> &v)
{
  os << "(";
  vsl_print_summary(os, v.first);
  os << ", ";
  vsl_print_summary(os, v.second);
  os << ", ";
  vsl_print_summary(os, v.third);
  os << ")";
}


#undef VBL_IO_TRIPLE_INSTANTIATE
#define VBL_IO_TRIPLE_INSTANTIATE(S, T, U ) \
template void vsl_print_summary(std::ostream& s, const vbl_triple<S, T, U >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vbl_triple<S, T, U > & v); \
template void vsl_b_read(vsl_b_istream& s, vbl_triple<S, T, U > & v)

#endif // vbl_io_triple_hxx_
