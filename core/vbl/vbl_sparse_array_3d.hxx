// This is core/vbl/vbl_sparse_array_3d.hxx
#ifndef vbl_sparse_array_3d_hxx_
#define vbl_sparse_array_3d_hxx_
//:
// \file

#include <iostream>
#include "vbl_sparse_array_3d.h"
#include "vbl_sparse_array_base.hxx"
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Print the array to a stream in "(i,j,k): value" format.
template <class T>
std::ostream& vbl_sparse_array_3d<T>::print(std::ostream& out) const
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
/*template std::ostream& operator<< (std::ostream&, const vbl_sparse_array_3d<T > &) */

#endif // vbl_sparse_array_3d_hxx_
