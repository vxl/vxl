//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_complex_h_
#define vnl_complex_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vnl_complex
// .HEADER vxl package
// .LIBRARY vnl
// .INCLUDE vnl/vnl_complex.h
// .FILE vnl_complex.cxx
//
// .SECTION Description
//     We don't want everyone to pay for complex when they don't need it, as
//     its ratio of expense to frequency of use is high. So we define those 
//     functions from vnl_math which use complex here instead.
//     In a sense, vnl_math should be a namespace, and this file adds to that
//     namespace.

#include <vcl_cmath.h>
#include <vcl_complex.h>
#include <vnl/vnl_math.h>

// isnan
bool vnl_math_isnan(const vcl_complex<float>&);
bool vnl_math_isnan(const vcl_complex<double>&);

// isinf
bool vnl_math_isinf(const vcl_complex<float>&);
bool vnl_math_isinf(const vcl_complex<double>&);

// isfinite
bool vnl_math_isfinite(const vcl_complex<float>&);
bool vnl_math_isfinite(const vcl_complex<double>&);

// abs
inline float    vnl_math_abs(vcl_complex<float> const& x) { return vcl_abs(x); }
inline double   vnl_math_abs(vcl_complex<double> const& x) { return vcl_abs(x); }

// sqr (square)
inline vcl_complex<float>  vnl_math_sqr(vcl_complex<float> const& x) { return x*x; }
inline vcl_complex<double> vnl_math_sqr(vcl_complex<double> const& x) { return x*x; }

// squared_magnitude
inline float    vnl_math_squared_magnitude(vcl_complex<float> const& x) { return vcl_norm(x); }
inline double   vnl_math_squared_magnitude(vcl_complex<double> const& x) { return vcl_norm(x); }

#endif // vnl_complex_h_
