#ifndef vbl_sparse_array_3d_txx_
#define vbl_sparse_array_3d_txx_

// This is vxl/vbl/vbl_sparse_array_3d.txx


#include "vbl_sparse_array_3d.h"
#include "vbl_sparse_array_base.txx"
#include <vcl_iostream.h>




template <class T>
vcl_ostream& vbl_sparse_array_3d<T>::print(vcl_ostream& out) const
{
#if defined(VCL_EGCS) || defined(VCL_GCC_295) || defined(VCL_GCC_30)
  typedef vcl_map<vbl_index_3d, T, vcl_less<vbl_index_3d> >::const_iterator I;
#else
  typedef Map::const_iterator I;
#endif
  for(I p = storage_.begin(); p != storage_.end(); ++p)
  {
    out << "(" << (*p).first.i << "," << (*p).first.j << "," << (*p).first.k
        << "): " << (*p).second << vcl_endl;
  }
  return out;
}


#undef VBL_SPARSE_ARRAY_3D_INSTANTIATE
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE(T) \
template class vbl_sparse_array_base<T , vbl_index_3d >; \
template class vbl_sparse_array_3d<T >; \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator << (vcl_ostream&, const vbl_sparse_array_3d<T > &))

#endif
