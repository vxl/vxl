#ifndef vbl_sparse_array_2d_txx_
#define vbl_sparse_array_2d_txx_

// This is vxl/vbl/vbl_sparse_array_2d.txx

#include "vbl_sparse_array_2d.h"
#include "vbl_sparse_array_base.txx"

#include <vcl_iostream.h>

//: Print the array to a stream in "(i,j): value" format.
template <class T>
vcl_ostream& vbl_sparse_array_2d<T>::print(vcl_ostream& out) const
{
  for(const_iterator p = storage_.begin(); p != storage_.end(); ++p)
    out << "(" << (*p).first.first
        << "," << (*p).first.second
        << "): " << (*p).second << vcl_endl;
  return out;
}

#undef VBL_SPARSE_ARRAY_2D_INSTANTIATE
#define VBL_SPARSE_ARRAY_2D_INSTANTIATE(T) \
template class vbl_sparse_array_base<T , vcl_pair<unsigned, unsigned> >; \
template class vbl_sparse_array_2d<T >; \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator<< (vcl_ostream&, const vbl_sparse_array_2d<T > &))

#endif
