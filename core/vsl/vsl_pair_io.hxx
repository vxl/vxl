// This is core/vsl/vsl_pair_io.hxx
#ifndef vsl_pair_io_hxx_
#define vsl_pair_io_hxx_
//:
// \file
// \brief  binary IO functions for std::pair<T>
// \author Ian Scott
//
// Implementation

#include "vsl_pair_io.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write pair to binary stream
template <class S, class T>
void vsl_b_write(vsl_b_ostream& s, const std::pair<S, T>& v)
{
  // Do not write a version number here for space efficiency reasons.
  // There is no reason to expect the format to change
  vsl_b_write(s,v.first);
  vsl_b_write(s,v.second);
}

//====================================================================================
//: Read pair from binary stream
template <class S, class T>
void vsl_b_read(vsl_b_istream& s, std::pair<S, T>& v)
{
  vsl_b_read(s,v.first);
  vsl_b_read(s,v.second);
}


//====================================================================================
//: Output a human readable summary to the stream
template <class S, class T>
void vsl_print_summary(std::ostream& os, const std::pair<S, T> &v)
{
  os << "(";
  vsl_print_summary(os, v.first);
  os << ", ";
  vsl_print_summary(os, v.second);
  os << ")";
}


#undef VSL_PAIR_IO_INSTANTIATE
#define VSL_PAIR_IO_INSTANTIATE(S, T ) \
template void vsl_print_summary(std::ostream& s, const std::pair<S, T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const std::pair<S, T > & v); \
template void vsl_b_read(vsl_b_istream& s, std::pair<S, T > & v)

#endif // vsl_pair_io_hxx_
