// This is vxl/vbl/vbl_sparse_array_2d.txx
#ifndef vbl_sparse_array_2d_txx_
#define vbl_sparse_array_2d_txx_
//:
// \file

#include "vbl_sparse_array_2d.h"
#include "vbl_sparse_array_base.txx"

#include <vcl_iostream.h>

#if 0 // capes@robots - moved print implementation to header file
//: Print the array to a stream in "(i,j): value" format.
template <class T>
vcl_ostream& vbl_sparse_array_2d<T>::print(vcl_ostream& out) const
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
VBL_SPARSE_ARRAY_BASE_INSTANTIATE(T, vcl_pair<unsigned VCL_COMMA unsigned >); \
template class vbl_sparse_array_2d<T >; \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator<< (vcl_ostream&, const vbl_sparse_array_2d<T > &))

#endif // vbl_sparse_array_2d_txx_
