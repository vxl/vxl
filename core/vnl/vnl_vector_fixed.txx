// This is vxl/vnl/vnl_vector_fixed.txx
#ifndef vnl_vector_fixed_txx_
#define vnl_vector_fixed_txx_
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 05 Aug 96

#include "vnl_vector_fixed.h"
#include <vcl_iostream.h>


// instantiation macros for vnl_vector_fixed<T,int> :

#define VNL_VECTOR_FIXED_INSTANTIATE(T,n) \
template class vnl_vector_fixed<T, n >; \
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T VCL_COMMA n > operator+(T const, vnl_vector_fixed_ref<T,n > const&));\
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T VCL_COMMA n > operator-(T const, vnl_vector_fixed_ref<T,n > const&));\
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T VCL_COMMA n > operator*(T const, vnl_vector_fixed_ref<T,n > const&));\
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T VCL_COMMA n > \
    element_product (vnl_vector_fixed_ref<T,n > const&, vnl_vector_fixed_ref<T,n > const&));\
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T VCL_COMMA n > \
    element_quotient(vnl_vector_fixed_ref<T,n > const&, vnl_vector_fixed_ref<T,n > const&));\
VCL_INSTANTIATE_INLINE(vcl_ostream &operator<<(vcl_ostream & VCL_COMMA vnl_vector_fixed<T VCL_COMMA n > const &))

//------------------------------------------------------------------------------

#define VNL_NON_TEMPLATE_FIXED_CROSS_3D_INSTANTIATE(T) \
vnl_vector_fixed<T,3 > cross_3d(vnl_vector_fixed<T,3 > const& v1, \
                                vnl_vector_fixed<T,3 > const& v2) \
{ \
  vnl_vector_fixed<T,3 > result; \
  result.x() = v1.y() * v2.z() - v1.z() * v2.y(); \
  result.y() = v1.z() * v2.x() - v1.x() * v2.z(); \
  result.z() = v1.x() * v2.y() - v1.y() * v2.x(); \
  return result; \
}

#endif // vnl_vector_fixed_txx_
