#ifndef ftraits_h_
#define ftraits_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_cplxfwd.h>

template <class T> struct ftraits; // an empty template class.
// abs_t : type of absolute value
// acc_t : accumulator type

//VCL_DECLARE_SPECIALIZATION 
struct ftraits<bool> {
  typedef unsigned abs_t;
  typedef unsigned acc_t;
};

//VCL_DECLARE_SPECIALIZATION 
struct ftraits<int> {
  typedef unsigned abs_t;
  typedef long acc_t;
};

//VCL_DECLARE_SPECIALIZATION 
struct ftraits<float> {
  typedef float abs_t;
  typedef double acc_t;
};

//VCL_DECLARE_SPECIALIZATION 
struct ftraits<double> {
  typedef double abs_t;
  typedef double acc_t; // or long double
};

//VCL_DECLARE_SPECIALIZATION 
struct ftraits<vcl_float_complex> {
  typedef float abs_t;
  typedef vcl_double_complex acc_t;
};

//VCL_DECLARE_SPECIALIZATION 
struct ftraits<vcl_double_complex> {
  typedef double abs_t;
  typedef vcl_double_complex acc_t; // or vcl_complex<long double>
};

#endif
