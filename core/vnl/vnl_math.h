//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_math_h
#define vnl_math_h
#ifdef __GNUC__
#pragma interface
#endif
//
// Namespace : vnl_math
//
// .SECTION Description
//    The vnl_math class provides a standard set of the simple mathematical
//    functions (min, max, sqr, sgn, rnd, abs), and some predefined constants
//    such as pi and e, which are not defined by the ANSI C++ standard.
//    That's right, M_PI is nonstandard!  The class is broadly based on the
//    Java Math class.
//
//    The operations, expressed in terms of a generic type T are:
//
//    T vnl_math::max(T, T) - Maximum of pair
//    
//    T vnl_math::min(T, T) - Minimum of pair
//    
//    T vnl_math::sqr(T)    - Square a number
//    
//    T vnl_math::abs(T)    - Absolute value (or modulus, if vnl_complex)
//    
//    int vnl_math::sgn(T)  - Sign(x).
//    
//    int vnl_math::sgn0(T) - Sign ignoring 0, useful for reals as they are ``never'' zero.
//    
//    int vnl_math::rnd(T)  - Nearest integer, rounding 0.5 to larger magnitude.
//
//    The operations are overloaded for int, float and double arguments,
//    which in combination with inlining can make them  more efficient than
//    their counterparts in the standard C library.  Although not currently
//    templated, they will be when namespaces and/or member templates are supported.
//
//    Aside from e, pi and their associates the class also defines eps,
//    the IEEE double machine precision.  This is the smallest number
//    eps such that 1+eps != 1.
//
// .NAME        vnl_math - Provide standard math functions
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_math.h
// .FILE        vnl/vnl_math.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, July 13, 1996
//
// .SECTION Modifications:
//     Peter Vanroose -Aug.97- added abs(vnl_double_complex) and sqr(vnl_double_complex)

//--------------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_function.h>
#include <vcl/vcl_cmath.h>  // <- this is <math.h>
#include <vnl/dll.h>

#ifdef _INT_64BIT_
// Type-accessible infinities for use in templates.
template <class T> T huge_val(T);
inline double   huge_val(double) { return HUGE_VAL; }
inline float    huge_val(float)  { return HUGE_VAL; }
inline long int huge_val(long int) { return 0x7fffffffffffffff; }
inline int      huge_val(int)    { return 0x7fffffffffffffff; }
inline short    huge_val(short)  { return 0x7fff; }
inline char     huge_val(char)   { return 0x7f; }
#else
// -- Type-accessible infinities for use in templates.
template <class T> T huge_val(T);
inline double huge_val(double) { return HUGE_VAL; }
inline float  huge_val(float)  { return (float)HUGE_VAL; }
inline int    huge_val(int)    { return 0x7fffffff; }
inline short  huge_val(short)  { return 0x7fff; }
inline char   huge_val(char)   { return 0x7f; }
#endif

class vnl_math {
public:
  // constants
  static VNL_DLL_DATA double const e               VCL_STATIC_CONST_INIT_FLOAT(2.7182818284590452354);
  static VNL_DLL_DATA double const log2e           VCL_STATIC_CONST_INIT_FLOAT(1.4426950408889634074);
  static VNL_DLL_DATA double const log10e          VCL_STATIC_CONST_INIT_FLOAT(0.43429448190325182765);
  static VNL_DLL_DATA double const ln2             VCL_STATIC_CONST_INIT_FLOAT(0.69314718055994530942);
  static VNL_DLL_DATA double const ln10            VCL_STATIC_CONST_INIT_FLOAT(2.30258509299404568402);
  static VNL_DLL_DATA double const pi              VCL_STATIC_CONST_INIT_FLOAT(3.14159265358979323846);
  static VNL_DLL_DATA double const pi_over_2       VCL_STATIC_CONST_INIT_FLOAT(1.57079632679489661923);
  static VNL_DLL_DATA double const pi_over_4       VCL_STATIC_CONST_INIT_FLOAT(0.78539816339744830962);
  static VNL_DLL_DATA double const one_over_pi     VCL_STATIC_CONST_INIT_FLOAT(0.31830988618379067154);
  static VNL_DLL_DATA double const two_over_pi     VCL_STATIC_CONST_INIT_FLOAT(0.63661977236758134308);
  static VNL_DLL_DATA double const two_over_sqrtpi VCL_STATIC_CONST_INIT_FLOAT(1.12837916709551257390);
  static VNL_DLL_DATA double const sqrt2           VCL_STATIC_CONST_INIT_FLOAT(1.41421356237309504880);
  static VNL_DLL_DATA double const sqrt1_2         VCL_STATIC_CONST_INIT_FLOAT(0.70710678118654752440);

  // IEEE double machine precision
  static VNL_DLL_DATA double const eps             VCL_STATIC_CONST_INIT_FLOAT(2.2204460492503131e-16);
  static VNL_DLL_DATA double const sqrteps         VCL_STATIC_CONST_INIT_FLOAT(1.490116119384766e-08);

  // MAX* constants.
  // Supplied until compilers accept the templated numeric_traits.
  // These are lowercase to avoid conflict with OS-defined macros.
  static VNL_DLL_DATA int const      maxint;
  static VNL_DLL_DATA long int const maxlong;
  static VNL_DLL_DATA double const   maxdouble;
  static VNL_DLL_DATA float const    maxfloat;
  
  // Queries
  static bool isnan(double);
  static bool isinf(double);
  static bool isfinite(double);

// -- Robust cube root.
  static double cbrt(double a) {
    return((a<0)?-exp(log(-a)/3):exp(log(a)/3));
  }

  // Generic inlines
  // T abs(T)       Absolute value
  // T max(T, T)    Maximum
  // T min(T, T)    Minimum
  // int sgn(T)     Sign +ive, 0, -ive -> (+1, 0, -1)
  // int sgn0(T)    Sign ignoring 0, useful for reals as they are "never" 0.
  // int rnd(T)     Nearest integer, rounding 0.5 to larger magnitude
  // 
  static inline double abs(double x) { return x < 0.0 ? -x : x; }
#undef max
  static inline double max(double x,double y) { return (x < y) ? y : x; }
#undef min
  static inline double min(double x,double y) { return (x > y) ? y : x; }
  static inline double sqr(double x) { return x*x; }
  static inline double hypot(double x, double y) { return sqrt(x*x + y*y); }
  static inline int sgn(double x) { return (x != 0)?((x>0)?1:-1):0; }
  static inline int sgn0(double x) { return (x>=0)?1:-1; }
  static inline int rnd(double x) { return (x>=0.0)?(int)(x + 0.5):(int)(x - 0.5); }
  static inline double squared_magnitude(double x) { return x*x; }
  
  static inline float abs(float x) { return x < 0.0 ? -x : x; }
  static inline float max(float x,float y) { return (x < y) ? y : x; }
  static inline float min(float x,float y) { return (x > y) ? y : x; }
  static inline float sqr(float x) { return x*x; }
  static inline int sgn(float x) { return (x != 0)?((x>0)?1:-1):0; }
  static inline int sgn0(float x) { return (x>=0)?1:-1; }
  static inline int rnd(float x) { return (x>=0.0)?(int)(x + 0.5):(int)(x - 0.5); }
  static inline float squared_magnitude(float x) { return x*x; }

  static inline int abs(int x) { return x < 0 ? -x : x; }
  static inline int max(int x,int y) { return (x > y) ? x : y; }
  static inline int min(int x,int y) { return (x < y) ? x : y; }
  static inline int sqr(int x) { return x*x; }
  static inline int sgn(int x) { return x?((x>0)?1:-1):0; }
  static inline int sgn0(int x) { return (x>=0)?1:-1; }
  static inline int squared_magnitude(int x) { return x*x; }

  static inline long int abs(long int x) { return x < 0 ? -x : x; }
  static inline long int max(long int x,long int y) { return (x > y) ? x : y; }
  static inline long int min(long int x,long int y) { return (x < y) ? x : y; }
  static inline long int sqr(long int x) { return x*x; }
  static inline long int sgn(long int x) { return x?((x>0)?1:-1):0; }
  static inline long int sgn0(long int x) { return (x>=0)?1:-1; }
  static inline long int squared_magnitude(long int x) { return x*x; }

  static inline unsigned abs(unsigned x) { return x; } // to avoid SunPro4.2 float/double conflict
  static inline unsigned max(unsigned x,unsigned y) { return (x > y) ? x : y; }
  static inline unsigned min(unsigned x,unsigned y) { return (x < y) ? x : y; }
  static inline unsigned sqr(unsigned x) { return x*x; }
  static inline unsigned squared_magnitude(unsigned x) { return x*x; }

#define VNL_USED_COMPLEX
#ifdef VNL_COMPLEX_AVAILABLE
// See vnl_complex.h
  static inline double              abs(vnl_complex<double> const& x) { return ::abs(x); }
  static inline vnl_complex<double> sqr(vnl_complex<double> const& x) { return x*x; }
  static inline double              squared_magnitude(vnl_complex<double> const& x) { return ::norm(x); }

  static inline float              abs(vnl_complex<float> const& x) { return ::abs(x); }
  static inline vnl_complex<float> sqr(vnl_complex<float> const& x) { return x*x; }
  static inline float              squared_magnitude(vnl_complex<float> const& x) { return ::norm(x); }

  static bool isnan(const vnl_complex<double>&);
  static bool isinf(const vnl_complex<double>&);
  static bool isfinite(const vnl_complex<double>&);

  static bool isnan(const vnl_complex<float>&);
  static bool isinf(const vnl_complex<float>&);
  static bool isfinite(const vnl_complex<float>&);
#endif

private:
  static bool time_fabs(float *, int n);
  static bool time_fabs(double *, int n);
  static bool time_abs(double *, int n);
  static bool time_abs(float *, int n);
public:
  static bool test();
};

#endif 
