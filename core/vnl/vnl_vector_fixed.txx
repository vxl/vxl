//-*- c++ -*-
// Class: vnl_vector_fixed
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 05 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_c_vector.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_vector_fixed.h>

//--------------------------------------------------------------------------------

//
// instantiation macros for vnl_vector_fixed<T,int> :
//

#define VCLi_TEMPLATE_FUNCTION_MARKER 
#ifdef VC50
# undef  VCLi_TEMPLATE_FUNCTION_MARKER 
# define VCLi_TEMPLATE_FUNCTION_MARKER <>
#endif

#undef __COMMA__
#define __COMMA__ ,

#define VNL_VECTOR_FIXED_INSTANTIATE(T,n) \
template class vnl_vector_fixed<T, n>;	     \
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T __COMMA__ n> operator+(const T t, const vnl_vector_fixed<T,n>& rhs)); \
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T __COMMA__ n> operator-(const T t, const vnl_vector_fixed<T,n>& rhs)); \
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T __COMMA__ n> operator*(const T t, const vnl_vector_fixed<T,n>& rhs));\
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T __COMMA__ n> element_product  (const vnl_vector_fixed<T,n>& a, const vnl_vector_fixed<T,n>& b)) \
VCL_INSTANTIATE_INLINE(vnl_vector_fixed<T __COMMA__ n> element_quotient (const vnl_vector_fixed<T,n>& a, const vnl_vector_fixed<T,n>& b));
// #undef __COMMA__ gcc 272 doesnt see it at inst time

//--------------------------------------------------------------------------------

#define VCL_INSTANTIATE_NON_TEMPLATE_FIXED_CROSS_3D(T) \
vnl_vector_fixed<T,3> cross_3d \
         (const vnl_vector_fixed<T,3>& v1, const vnl_vector_fixed<T,3>& v2) \
{ \
  vnl_vector_fixed<T,3> result; \
   \
  vnl_vector_fixed<T,3>& vv1 = *((vnl_vector_fixed<T,3>*) &v1); \
  vnl_vector_fixed<T,3>& vv2 = *((vnl_vector_fixed<T,3>*) &v2); \
   \
  result.x() = vv1.y() * vv2.z() - vv1.z() * vv2.y(); \
  result.y() = vv1.z() * vv2.x() - vv1.x() * vv2.z(); \
  result.z() = vv1.x() * vv2.y() - vv1.y() * vv2.x(); \
  return result;				  \
} 

//--------------------------------------------------------------------------------

template<class T, int n>
vnl_vector<T>& vnl_vector_fixed<T,n>::pre_multiply (const vnl_matrix<T>& ) {
    // cannot use pre_multiply on vnl_vector_fixed<T,n>, since it deallocates the
    // data storage
    assert(false);
    return *this;
}

template<class T, int n>
vnl_vector<T>& vnl_vector_fixed<T,n>::post_multiply (const vnl_matrix<T>& ) {
    // cannot use post_multiply on vnl_vector_fixed<T,n>, since it deallocates the
    // data storage
    assert(false);
    return *this;
}

template<class T, int n>
vnl_vector<T>& vnl_vector_fixed<T,n>::operator*= (const vnl_matrix<T>& ) {
    // cannot use pre_multiply on vnl_vector_fixed<T,n>, since it deallocates the
    // data storage
    assert(false);
    return *this;
}

