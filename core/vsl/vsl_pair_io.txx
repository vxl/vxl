// This is vxl/vsl/vsl_pair_io.txx
#ifndef vsl_pair_io_txx_
#define vsl_pair_io_txx_
//:
// \file
// \brief  binary IO functions for vcl_pair<T>
// \author Ian Scott
//
// Implementation

#include "vsl_pair_io.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write pair to binary stream
template <class S, class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_pair<S, T>& v)
{
  // Do not write a version number here for space efficiency reasons.
  // There is no reason to expect the format to change
  vsl_b_write(s,v.first);
  vsl_b_write(s,v.second);
}

//====================================================================================
//: Read pair from binary stream
template <class S, class T>
void vsl_b_read(vsl_b_istream& s, vcl_pair<S, T>& v)
{
  vsl_b_read(s,v.first);
  vsl_b_read(s,v.second);
}


//====================================================================================
//: Output a human readable summary to the stream
template <class S, class T>
void vsl_print_summary(vcl_ostream& os, const vcl_pair<S, T> &v)
{
  os << "(";
  vsl_print_summary(os, v.first);
  os << ", ";
  vsl_print_summary(os, v.second);
  os << ")";
}


#undef VSL_PAIR_IO_INSTANTIATE
#define VSL_PAIR_IO_INSTANTIATE(S, T ) \
template void vsl_print_summary(vcl_ostream& s, const vcl_pair<S, T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_pair<S, T > & v); \
template void vsl_b_read(vsl_b_istream& s, vcl_pair<S, T > & v)

#endif // vsl_pair_io_txx_
