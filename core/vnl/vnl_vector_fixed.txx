// This is vxl/vnl/vnl_vector_fixed.txx
#ifndef vnl_vector_fixed_txx_
#define vnl_vector_fixed_txx_
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 05 Aug 96

#include "vnl_vector_fixed.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>


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
vnl_vector_fixed<T,3 > cross_3d(vnl_vector_fixed_ref<T,3 > const& v1, \
                                vnl_vector_fixed_ref<T,3 > const& v2) \
{ \
  vnl_vector_fixed<T,3 > result; \
   \
  vnl_vector_fixed_ref<T,3 >& vv1 = *((vnl_vector_fixed_ref<T,3 >*) &v1); \
  vnl_vector_fixed_ref<T,3 >& vv2 = *((vnl_vector_fixed_ref<T,3 >*) &v2); \
   \
  result.x() = vv1.y() * vv2.z() - vv1.z() * vv2.y(); \
  result.y() = vv1.z() * vv2.x() - vv1.x() * vv2.z(); \
  result.z() = vv1.x() * vv2.y() - vv1.y() * vv2.x(); \
  return result; \
}

#endif // vnl_vector_fixed_txx_
