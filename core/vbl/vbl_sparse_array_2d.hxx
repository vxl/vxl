// This is core/vbl/vbl_sparse_array_2d.hxx
#ifndef vbl_sparse_array_2d_hxx_
#define vbl_sparse_array_2d_hxx_
//:
// \file

#include <iostream>
#include "vbl_sparse_array_2d.h"
#include "vbl_sparse_array_base.hxx"
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if 0 // capes@robots - moved print implementation to header file
//: Print the array to a stream in "(i,j): value" format.
template <class T>
std::ostream& vbl_sparse_array_2d<T>::print(std::ostream& out) const
{
  for (const_iterator p = storage_.begin(); p != storage_.end(); ++p)
    out << "(" << (*p).first.first
        << "," << (*p).first.second
        << "): " << (*p).second << '\n';
  return out;
}
#endif

#undef VBL_SPARSE_ARRAY_2D_INSTANTIATE
#define VBL_SPARSE_ARRAY_2D_INSTANTIATE(T) \
VBL_SPARSE_ARRAY_BASE_INSTANTIATE(T, std::pair<unsigned VCL_COMMA unsigned >); \
template class vbl_sparse_array_2d<T >; \
/*template std::ostream& operator<< (std::ostream&, const vbl_sparse_array_2d<T > &) */

#endif // vbl_sparse_array_2d_hxx_
