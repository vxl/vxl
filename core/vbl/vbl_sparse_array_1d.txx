// This is vxl/vbl/vbl_sparse_array_1d.txx
#ifndef vbl_sparse_array_1d_txx_
#define vbl_sparse_array_1d_txx_
//:
// \file

#include "vbl_sparse_array_1d.h"
#include "vbl_sparse_array_base.txx"
#include <vcl_iostream.h>

#if 0 // capes@robots - moved print implementation to header file
//: Print the Array to a stream in "(i): value" format.
template <class T>
vcl_ostream& vbl_sparse_array_1d<T>::print(vcl_ostream& out) const
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
VCL_INSTANTIATE_INLINE(vcl_ostream& operator << (vcl_ostream&, const vbl_sparse_array_1d<T > &))

#endif // vbl_sparse_array_1d_txx_
