//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_c_vector_h_
#define vnl_c_vector_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_c_vector
//
// .SECTION Description
//    vnl_c_vector interfaces to lowlevel memory-block operations.
//
// .NAME        vnl_c_vector - Math on blocks of memory
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/c_vector.h
// .FILE        vnl/c_vector.ccxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Feb 98
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h> // for sqrt()

template <class T>
class vnl_c_vector {
public:
  typedef vnl_numeric_traits<T>::abs_t abs_t;

  static T sum(const T* v, int n);
  static inline abs_t squared_magnitude(T const *p, int n) { return two_nrm2(p,n); }
  static void normalize(T* v, int n);
  static void apply(const T* v, int n, T (*f)(T), T* v_out);
  static void apply(const T* v, int n, T (*f)(const T&), T* v_out);
  static void copy(T const *src,T       *dst,int n);
  static T dot_product(T const *, T const *, unsigned n);
  static T inner_product(T const *, T const *, unsigned n); // conjugate second
  static void conjugate(T const *src, T *dst, unsigned n);

  static T max_value(T const *, unsigned n);
  static T min_value(T const *, unsigned n);
  static T mean(T const *p, unsigned n) { return sum(p,n)/n; }

  // one_norm : sum of abs values
  // two_nrm2 : sum of squared abs values
  // two_norm : sqrt of sum of squared abs values
  // rms_norm : sqrt of mean sum of squared abs values
  // inf_norm : max of abs values
  static inline abs_t one_norm(T const *p, unsigned n) 
    { one_norm_aux(p,n); return aux_var; }
  static inline abs_t two_nrm2(T const *p, unsigned n) 
    { two_nrm2_aux(p,n); return aux_var; }
  static inline abs_t two_norm(T const *p, unsigned n) 
    { two_nrm2_aux(p,n); return abs_t(sqrt(aux_var)); }
  static inline abs_t rms_norm(T const *p, unsigned n) 
    { two_nrm2_aux(p,n); return abs_t(sqrt(aux_var)/n); }
  static inline abs_t inf_norm(T const *p, unsigned n) 
    { inf_norm_aux(p,n); return aux_var; }
private:
  static abs_t aux_var;
  static void one_norm_aux(T const *, unsigned );
  static void two_nrm2_aux(T const *, unsigned );
  static void inf_norm_aux(T const *, unsigned );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_c_vector.

