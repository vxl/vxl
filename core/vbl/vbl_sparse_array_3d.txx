// This is core/vbl/vbl_sparse_array_3d.txx
#ifndef vbl_sparse_array_3d_txx_
#define vbl_sparse_array_3d_txx_
//:
// \file

#include "vbl_sparse_array_3d.h"
#include "vbl_sparse_array_base.txx"
#include <vcl_iostream.h>

//: Print the array to a stream in "(i,j,k): value" format.
template <class T>
vcl_ostream& vbl_sparse_array_3d<T>::print(vcl_ostream& out) const
{
  typedef typename vbl_sparse_array_base<T,vbl_triple<unsigned,unsigned,unsigned> >::const_iterator ci;
  for (ci p = this->begin(); p != this->end(); ++p)
    out << '(' << (*p).first.first
        << ',' << (*p).first.second
        << ',' << (*p).first.third
        << "): " << (*p).second << '\n';
  return out;
}

#undef VBL_SPARSE_ARRAY_3D_INSTANTIATE
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE(T) \
VBL_SPARSE_ARRAY_BASE_INSTANTIATE(T, vbl_triple<unsigned VCL_COMMA unsigned VCL_COMMA unsigned >); \
template class vbl_sparse_array_3d<T >; \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator<< (vcl_ostream&, const vbl_sparse_array_3d<T > &))

#endif // vbl_sparse_array_3d_txx_
