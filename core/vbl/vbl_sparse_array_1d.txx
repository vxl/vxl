// This is vxl/vbl/vbl_sparse_array_1d.txx

#include <vbl/vbl_sparse_array_1d.h>
#include "vbl_sparse_array_base.txx"
#include <vcl_iostream.h>

//: Print the Array to a stream in "(i): value" format.
template <class T>
vcl_ostream& vbl_sparse_array_1d<T>::print(vcl_ostream& out) const
{
  for(const_iterator p = begin(); p != end(); ++p)
    out << "(" << (*p).first << "): " << (*p).second << vcl_endl;
  return out;
}

#undef VBL_SPARSE_ARRAY_1D_INSTANTIATE
#define VBL_SPARSE_ARRAY_1D_INSTANTIATE(T)\
template class vbl_sparse_array_1d<T >; \
template class vbl_sparse_array_base<T , unsigned >; \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator << (vcl_ostream&, const vbl_sparse_array_1d<T > &))


