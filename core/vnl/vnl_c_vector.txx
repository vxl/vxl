//-*- c++ -*-------------------------------------------------------------------
//
// Class: vnl_c_vector
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 12 Feb 98
// Modifications:
//   980212 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_complex.h> // as usual, this must come first.
#include "vnl_c_vector.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_complex_traits.h>

template <class T>
T vnl_c_vector<T>::sum(T const* v, int n)
{
  T tot = 0;
  for(int i = 0; i < n; ++i)
    tot += *v++;
  return tot;
}

template <class T>
void vnl_c_vector<T>::normalize(T* v, int n)
{
  vnl_numeric_traits<T>::abs_t tot = 0;
  for(int i = 0; i < n; ++i)
    tot += vnl_math::squared_magnitude(v[i]);
  tot = vnl_numeric_traits<T>::abs_t(1.0 / sqrt(tot));
  for(int i = 0; i < n; ++i)
    v[i] *= tot;
}

template <class T>
void vnl_c_vector<T>::apply(T const* v, int n, T (*f)(T const&), T* v_out)
{
  for(int i = 0; i < n; ++i)
    v_out[i] = f(v[i]);
}

template <class T>
void vnl_c_vector<T>::apply(T const* v, int n, T (*f)(T), T* v_out)
{
  for(int i = 0; i < n; ++i)
    v_out[i] = f(v[i]);
}

template <class T>
void vnl_c_vector<T>::copy(T const *src,T       *dst,int n)
{
  for (int i=0;i<n;i++)
    dst[i] = src[i];
}

// non-conjugating "dot" product.
template<class T> 
T vnl_c_vector<T>::dot_product(T const *a, T const *b, unsigned n) {
  T ip(0);
  for (unsigned i=0; i<n; ++i)
    ip += a[i] * b[i];
  return ip;
}

// conjugating "dot" product.
template<class T> 
T vnl_c_vector<T>::inner_product(T const *a, T const *b, unsigned n) {
  T ip(0);
  for (unsigned i=0; i<n; ++i)
    ip += a[i] * vnl_complex_traits<T>::conjugate(b[i]);
  return ip;
}

// conjugates one block of data into another block.
template<class T> 
void vnl_c_vector<T>::conjugate(T const *src, T *dst, unsigned n) {
  for (unsigned i=0; i<n; ++i)
    dst[i] = vnl_complex_traits<T>::conjugate( src[i] );
}

//--------------------------------------------------------------------------------

//: Returns max value of the vector.
template<class T> 
T vnl_c_vector<T>::max_value(T const *src, unsigned n) {
  T tmp = src[0];
  
  for (unsigned i=1; i<n; ++i)
    if (src[i] > tmp)
      tmp = src[i];
  
  return tmp;
}

//: Returns min value of the vector.
template<class T> 
T vnl_c_vector<T>::min_value(T const *src, unsigned n) {
  T tmp = src[0];
  
  for (unsigned i=1; i<n; ++i)
    if (src[i] > tmp)
      tmp = src[i];
  
  return tmp;
}

//--------------------------------------------------------------------------------

//#define return_t vnl_c_vector<T>::abs_t
//#define return_aux return
#define return_t void
#define return_aux aux_var =

//
template<class T> 
return_t vnl_c_vector<T>::one_norm_aux(T const *src, unsigned n) {
  typedef vnl_numeric_traits<T>::abs_t abs_t;
  abs_t sum(0);
  
  for (unsigned i=0; i<n; ++i)
    sum += vnl_math::abs(src[i]);

  return_aux sum;
}

//
template<class T> 
return_t vnl_c_vector<T>::two_nrm2_aux(T const *src, unsigned n) {
  abs_t tot = 0;
  for(unsigned i=0; i<n; ++i)
    tot += vnl_math::squared_magnitude(src[i]);
  return_aux tot;
}

//
template<class T> 
return_t vnl_c_vector<T>::inf_norm_aux(T const *src, unsigned n) {
  typedef vnl_numeric_traits<T>::abs_t abs_t;
  abs_t max(0);
  
  for (unsigned i=0; i<n; ++i) {
    abs_t v = vnl_math::abs(src[i]);
    if (v > max)
      max = v;
  }
  return_aux max;
}

//--------------------------------------------------------------------------------

#define VNL_C_VECTOR_INSTANTIATE_ordered(T) \
VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var = 0); \
template class vnl_c_vector<T >; \
VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var);


#define VNL_C_VECTOR_INSTANTIATE_unordered(T) \
VCL_DO_NOT_INSTANTIATE(T vnl_c_vector<T >::max_value(T const *, unsigned), T(0)); \
VCL_DO_NOT_INSTANTIATE(T vnl_c_vector<T >::min_value(T const *, unsigned), T(0)); \
VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var = 0); \
template class vnl_c_vector<T >; \
VCL_UNINSTANTIATE_SPECIALIZATION(T vnl_c_vector<T >::max_value(T const *, unsigned)); \
VCL_UNINSTANTIATE_SPECIALIZATION(T vnl_c_vector<T >::min_value(T const *, unsigned)); \
VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var); \
;

#define VNL_C_VECTOR_INSTANTIATE(T) extern "no such macro";
