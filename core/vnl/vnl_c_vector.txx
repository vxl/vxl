//-*- c++ -*-------------------------------------------------------------------
//
// Class: vnl_c_vector
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 12 Feb 98
// Modifications:
//   980212 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vnl_c_vector.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_complex_traits.h>
#include <vnl/vnl_numeric_traits.h>

template <class T>
T vnl_c_vector<T>::sum(T const* v, unsigned n)
{
  T tot = 0;
  for(unsigned i = 0; i < n; ++i)
    tot += *v++;
  return tot;
}

template <class T>
void vnl_c_vector<T>::normalize(T* v, unsigned n)
{
  abs_t tot(0);
  for(unsigned i = 0; i < n; ++i)
    tot += vnl_math_squared_magnitude(v[i]);
  tot = abs_t(1.0 / sqrt((double)tot));
  for(unsigned i = 0; i < n; ++i)
    v[i] *= tot;
}

template <class T>
void vnl_c_vector<T>::apply(T const* v, unsigned n, T (*f)(T const&), T* v_out)
{
  for(unsigned i = 0; i < n; ++i)
    v_out[i] = f(v[i]);
}

template <class T>
void vnl_c_vector<T>::apply(T const* v, unsigned n, T (*f)(T), T* v_out) {
  for(unsigned i = 0; i < n; ++i)
    v_out[i] = f(v[i]);
}

template <class T>
void vnl_c_vector<T>::copy(T const *src, T *dst, unsigned n)
{
  for (unsigned i=0; i<n; ++i)
    dst[i] = src[i];
}

template <class T>
void vnl_c_vector<T>::scale(T const *x, T *y, unsigned n, T const &s_) {
  T s = s_;
  if (x == y)
    for (unsigned i=0; i<n; ++i)
      y[i] *= s;
  else
    for (unsigned i=0; i<n; ++i)
      y[i] = s*x[i];
}

//--------------------------------------------------------------------------------
#define impl_elmt_wise_commutative(op) \
  if (z == x) \
    for (unsigned i=0; i<n; ++i) \
      z[i] op##= y[i]; \
\
  else if (z == y) \
    for (unsigned i=0; i<n; ++i) \
      z[i] op##= x[i]; \
\
  else \
    for (unsigned i=0; i<n; ++i) \
      z[i] = x[i] op y[i];

#define impl_elmt_wise_non_commutative(op) \
  if (z == x) \
    for (unsigned i=0; i<n; ++i) \
      z[i] op##= y[i]; \
\
  else \
    for (unsigned i=0; i<n; ++i) \
      z[i] = x[i] op y[i];

template <class T>
void vnl_c_vector<T>::add(T const *x, T const *y, T *z, unsigned n) {
  impl_elmt_wise_commutative(+);
}

template <class T>
void vnl_c_vector<T>::subtract(T const *x, T const *y, T *z, unsigned n) {
  impl_elmt_wise_non_commutative(-);
}

template <class T>
void vnl_c_vector<T>::multiply(T const *x, T const *y, T *z, unsigned n) {
  impl_elmt_wise_commutative(*);
}

template <class T>
void vnl_c_vector<T>::divide(T const *x, T const *y, T *z, unsigned n) {
  impl_elmt_wise_non_commutative(/);
}

#undef impl_elmt_wise_commutative
#undef impl_elmt_wise_noncommutative
//--------------------------------------------------------------------------------

template <class T>
void vnl_c_vector<T>::negate(T const *x, T *y, unsigned n) {
  if (x == y)
    for (unsigned i=0; i<n; ++i)
      y[i] = -y[i];
  else
    for (unsigned i=0; i<n; ++i)
      y[i] = -x[i];
}

template <class T>
void vnl_c_vector<T>::invert(T const *x, T *y, unsigned n) {
  if (x == y)
    for (unsigned i=0; i<n; ++i)
      y[i] = T(1)/y[i];
  else
    for (unsigned i=0; i<n; ++i)
      y[i] = T(1)/x[i];
}

template <class T>
void vnl_c_vector<T>::saxpy(T const &a_, T const *x, T *y, unsigned n) {
  T a = a_;
  for (unsigned i=0; i<n; ++i)
    y[i] += a*x[i];
}

template <class T>
void vnl_c_vector<T>::fill(T *x, unsigned n, T const &v_) {
  T v = v_;
  for (unsigned i=0; i<n; ++i)
    x[i] = v;
}

template <class T>
void vnl_c_vector<T>::reverse (T *x, unsigned n) {
  for (unsigned i=0; 2*i<n; ++i) {
    T tmp = x[i];
    x[i] = x[n-1-i];
    x[n-1-i] = tmp;
  }
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
    if (src[i] < tmp)
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
  abs_t sum(0);
  
  for (unsigned i=0; i<n; ++i)
    sum += vnl_math_abs(src[i]);

  return_aux sum;
}

//
template<class T> 
return_t vnl_c_vector<T>::two_nrm2_aux(T const *src, unsigned n) {
  abs_t tot(0);
  for(unsigned i=0; i<n; ++i)
    tot += vnl_math_squared_magnitude(src[i]);
  return_aux tot;
}

//
template<class T> 
return_t vnl_c_vector<T>::inf_norm_aux(T const *src, unsigned n) {
  abs_t max(0);
  
  for (unsigned i=0; i<n; ++i) {
    abs_t v = vnl_math_abs(src[i]);
    if (v > max)
      max = v;
  }
  return_aux max;
}

// useless... #define vnl_c_vector_use_win32_native_alloc 0
static const int vnl_c_vector_use_vnl_alloc = 1;

#include <vnl/vnl_alloc.h>

static inline void* alloc(int n, int size)
{
  //#if vnl_c_vector_use_win32_native_alloc
  // native was:  return (T**)std::allocator<T*>().allocate(n, 0);
  // on windows, it just calls malloc, so is useless....
  if (vnl_c_vector_use_vnl_alloc)
    return vnl_alloc::allocate((n == 0) ? 8 : (n * size)); 
  else
    return new char[n * size];
} 

static inline void dealloc(void* v, int n, int size)
{ 
  if (vnl_c_vector_use_vnl_alloc) {
    if (v) 
      vnl_alloc::deallocate(v, (n == 0) ? 8 : (n * size));
  } else
    delete [] static_cast<char*>(v);
}


template<class T> 
T** vnl_c_vector<T>::allocate_Tptr(int n)
{
  return (T**)alloc(n, sizeof (T*));
}

template<class T> 
T* vnl_c_vector<T>::allocate_T(int n)
{
  return (T*)alloc(n, sizeof (T));
}

template<class T> 
void vnl_c_vector<T>::deallocate(T** v, int n)
{
  dealloc(v, n, sizeof (T*));
}

template<class T> 
void vnl_c_vector<T>::deallocate(T* v, int n)
{
  dealloc(v, n, sizeof (T));
}


//--------------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>

#undef VNL_C_VECTOR_INSTANTIATE_ordered
#define VNL_C_VECTOR_INSTANTIATE_ordered(T) \
VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(template <> vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var = 0); \
template class vnl_c_vector<T >; \
VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(template <> vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var)


#undef VNL_C_VECTOR_INSTANTIATE_unordered
#define VNL_C_VECTOR_INSTANTIATE_unordered(T) \
VCL_DO_NOT_INSTANTIATE(T vnl_c_vector<T >::max_value(T const *, unsigned), T(0)); \
VCL_DO_NOT_INSTANTIATE(T vnl_c_vector<T >::min_value(T const *, unsigned), T(0)); \
VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(template<> vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var = 0); \
template class vnl_c_vector<T >; \
VCL_UNINSTANTIATE_SPECIALIZATION(T vnl_c_vector<T >::max_value(T const *, unsigned)); \
VCL_UNINSTANTIATE_SPECIALIZATION(T vnl_c_vector<T >::min_value(T const *, unsigned)); \
VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(vnl_c_vector<T >::abs_t vnl_c_vector<T >::aux_var)

#undef VNL_C_VECTOR_INSTANTIATE
#define VNL_C_VECTOR_INSTANTIATE(T) extern "no such macro"
