// This is core/vbl/vbl_sparse_array_1d.hxx
#ifndef vbl_sparse_array_1d_hxx_
#define vbl_sparse_array_1d_hxx_
//:
// \file

#include <iostream>
#include "vbl_sparse_array_1d.h"
#include "vbl_sparse_array_base.hxx"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if 0 // capes@robots - moved print implementation to header file
//: Print the Array to a stream in "(i): value" format.
template <class T>
std::ostream& vbl_sparse_array_1d<T>::print(std::ostream& out) const
{
  for (const_iterator p = begin(); p != end(); ++p)
    out << "(" << (*p).first << "): " << (*p).second << '\n';
  return out;
}
#endif

#undef VBL_SPARSE_ARRAY_1D_INSTANTIATE
#define VBL_SPARSE_ARRAY_1D_INSTANTIATE(T) \
VBL_SPARSE_ARRAY_BASE_INSTANTIATE(T, unsigned int); \
template class vbl_sparse_array_1d<T >; \
/*template std::ostream& operator << (std::ostream&, const vbl_sparse_array_1d<T > &) */

#endif // vbl_sparse_array_1d_hxx_
