// This is core/vil/vil_round.h
#ifndef vil_round_h_
#define vil_round_h_
//:
// \file
// \brief Namespace with standard rounding functions.
//
//  These are just copies of the vnl_math rounding functions.

#include <vxl_config.h>
#include <vil/vil_config.h> // for VIL_CONFIG_ENABLE_SSE2_ROUNDING
#ifdef VNL_CHECK_FPU_ROUNDING_MODE
# include <vcl_cassert.h>
#endif

// Figure out when the fast implementation can be used
#if VIL_CONFIG_ENABLE_SSE2_ROUNDING
# if !VXL_HAS_EMMINTRIN_H
#   error "Required file emmintrin.h for SSE2 not found"
# else
#   include <emmintrin.h> // sse 2 intrinsics
# endif
#endif

// Turn on fast impl when using GCC on Intel-based machines
#if defined(__GNUC__) && ((defined(__i386__) || defined(__i386) || defined(__x86_64__) || defined(__x86_64)))
# define GCC_USE_FAST_IMPL 1
#else
# define GCC_USE_FAST_IMPL 0
#endif
// Turn on fast impl when using msvc on 32 bits windows
#if defined(_MSC_VER) && !defined(_WIN64)
# define VC_USE_FAST_IMPL 1
#else
# define VC_USE_FAST_IMPL 0
#endif




// vil_round_rnd_halfinttoeven  -- round towards nearest integer
//         halfway cases are rounded towards the nearest even integer, e.g.
//         vil_round_rnd_halfinttoeven( 1.5) ==  2
//         vil_round_rnd_halfinttoeven(-1.5) == -2
//         vil_round_rnd_halfinttoeven( 2.5) ==  2
//         vil_round_rnd_halfinttoeven( 3.5) ==  4
//
// We assume that the rounding mode is not changed from the default
// one (or at least that it is always restored to the default one).

#if VIL_CONFIG_ENABLE_SSE2_ROUNDING // Fast sse2 implementation

inline int vil_round_rnd_halfinttoeven(float  x)
{
# if defined(VIL_CHECK_FPU_ROUNDING_MODE) && defined(__GNUC__)
  assert(fegetround()==FE_TONEAREST);
# endif
  return _mm_cvtss_si32(_mm_set_ss(x));
}
inline int vil_round_rnd_halfinttoeven(double  x)
{
# if defined(VIL_CHECK_FPU_ROUNDING_MODE) && defined(__GNUC__)
  assert(fegetround()==FE_TONEAREST);
# endif
  return _mm_cvtsd_si32(_mm_set_sd(x));
}

#elif GCC_USE_FAST_IMPL // Fast gcc asm implementation

inline int vil_round_rnd_halfinttoeven(float  x)
{
# ifdef VIL_CHECK_FPU_ROUNDING_MODE
  assert(fegetround()==FE_TONEAREST);
# endif
  int r;
  __asm__ __volatile__ ("fistpl %0" : "=m"(r) : "t"(x) : "st");
  return r;
}
inline int vil_round_rnd_halfinttoeven(double  x)
{
# ifdef VIL_CHECK_FPU_ROUNDING_MODE
  assert(fegetround()==FE_TONEAREST);
# endif
  int r;
  __asm__ __volatile__ ("fistpl %0" : "=m"(r) : "t"(x) : "st");
  return r;
}

#elif VC_USE_FAST_IMPL // Fast msvc asm implementation

inline int vil_round_rnd_halfinttoeven(float  x)
{
  int r;
  __asm {
    fld x
    fistp r
  }
  return r;
}
inline int vil_round_rnd_halfinttoeven(double  x)
{
  int r;
  __asm {
    fld x
    fistp r
  }
  return r;
}

#else // Vanilla implementation

inline int vil_round_rnd_halfinttoeven(float  x)
{
  if (x>=0.f)
  {
     x+=0.5f;
     const int r = static_cast<int>(x);
     if ( x != static_cast<float>(r) ) return r;
     return 2*(r/2);
  }
  else
  {
     x-=0.5f;
     const int r = static_cast<int>(x);
     if ( x != static_cast<float>(r) ) return r;
     return 2*(r/2);
  }
}
inline int vil_round_rnd_halfinttoeven(double x)
{
  if (x>=0.)
  {
     x+=0.5;
     const int r = static_cast<int>(x);
     if ( x != static_cast<double>(r) ) return r;
     return 2*(r/2);
  }
  else
  {
     x-=0.5;
     const int r = static_cast<int>(x);
     if ( x != static_cast<double>(r) ) return r;
     return 2*(r/2);
  }
}

#endif



// vil_round_rnd_halfintup  -- round towards nearest integer
//         halfway cases are rounded upward, e.g.
//         vil_round_rnd_halfintup( 1.5) ==  2
//         vil_round_rnd_halfintup(-1.5) == -1
//         vil_round_rnd_halfintup( 2.5) ==  3
//
// Be careful: argument absolute value must be less than INT_MAX/2
// for vil_round_rnd_halfintup to be guaranteed to work.
// We also assume that the rounding mode is not changed from the default
// one (or at least that it is always restored to the default one).

#if VIL_CONFIG_ENABLE_SSE2_ROUNDING || GCC_USE_FAST_IMPL || VC_USE_FAST_IMPL

inline int vil_round_rnd_halfintup(float  x) { return vil_round_rnd_halfinttoeven(2*x+0.5f)>>1; }
inline int vil_round_rnd_halfintup(double  x) { return vil_round_rnd_halfinttoeven(2*x+0.5)>>1; }

#else // Vanilla implementation

inline int vil_round_rnd_halfintup(float  x)
{
  x+=0.5f;
  return static_cast<int>(x>=0.f?x:(x==static_cast<int>(x)?x:x-1.f));
}
inline int vil_round_rnd_halfintup(double x)
{
  x+=0.5;
  return static_cast<int>(x>=0.?x:(x==static_cast<int>(x)?x:x-1.));
}

#endif



// vil_round_rnd  -- round towards nearest integer
//         halfway cases such as 0.5 may be rounded either up or down
//         so as to maximize the efficiency, e.g.
//         vil_round_rnd_halfinttoeven( 1.5) ==  1 or  2
//         vil_round_rnd_halfinttoeven(-1.5) == -2 or -1
//         vil_round_rnd_halfinttoeven( 2.5) ==  2 or  3
//         vil_round_rnd_halfinttoeven( 3.5) ==  3 or  4
//
// We assume that the rounding mode is not changed from the default
// one (or at least that it is always restored to the default one).

#if VIL_CONFIG_ENABLE_SSE2_ROUNDING || GCC_USE_FAST_IMPL || VC_USE_FAST_IMPL

inline int vil_round_rnd(float  x) { return vil_round_rnd_halfinttoeven(x); }
inline int vil_round_rnd(double  x) { return vil_round_rnd_halfinttoeven(x); }

#else // Vanilla implementation

inline int vil_round_rnd(float  x) { return x>=0.f?static_cast<int>(x+.5f):static_cast<int>(x-.5f); }
inline int vil_round_rnd(double x) { return x>=0.0?static_cast<int>(x+0.5):static_cast<int>(x-0.5); }


#endif



// vil_round_floor -- round towards minus infinity
//
// Be careful: argument absolute value must be less than INT_MAX/2
// for vil_round_floor to be guaranteed to work.
// We also assume that the rounding mode is not changed from the default
// one (or at least that it is always restored to the default one).

#if VIL_CONFIG_ENABLE_SSE2_ROUNDING // Fast sse2 implementation

inline int vil_round_floor(float  x)
{
# if defined(VIL_CHECK_FPU_ROUNDING_MODE) && defined(__GNUC__)
  assert(fegetround()==FE_TONEAREST);
# endif
   return _mm_cvtss_si32(_mm_set_ss(2*x-.5f))>>1;
}
inline int vil_round_floor(double  x)
{
# if defined(VIL_CHECK_FPU_ROUNDING_MODE) && defined(__GNUC__)
  assert(fegetround()==FE_TONEAREST);
# endif
   return _mm_cvtsd_si32(_mm_set_sd(2*x-.5))>>1;
}

#elif GCC_USE_FAST_IMPL // Fast gcc asm implementation

inline int vil_round_floor(float  x)
{
# ifdef VIL_CHECK_FPU_ROUNDING_MODE
  assert(fegetround()==FE_TONEAREST);
# endif
  int r;
  x = 2*x-.5f;
  __asm__ __volatile__ ("fistpl %0" : "=m"(r) : "t"(x) : "st");
  return r>>1;
}
inline int vil_round_floor(double  x)
{
# ifdef VIL_CHECK_FPU_ROUNDING_MODE
  assert(fegetround()==FE_TONEAREST);
# endif
  int r;
  x = 2*x-.5;
  __asm__ __volatile__ ("fistpl %0" : "=m"(r) : "t"(x) : "st");
  return r>>1;
}

#elif VC_USE_FAST_IMPL // Fast msvc asm implementation

inline int vil_round_floor(float  x)
{
  int r;
  x = 2*x-.5f;
  __asm {
    fld x
    fistp r
  }
  return r>>1;
}
inline int vil_round_floor(double  x)
{
  int r;
  x = 2*x-.5;
  __asm {
    fld x
    fistp r
  }
  return r>>1;
}

#else // Vanilla implementation

inline int vil_round_floor(float  x)
{
  return static_cast<int>(x>=0.f?x:(x==static_cast<int>(x)?x:x-1.f));
}
inline int vil_round_floor(double x)
{
  return static_cast<int>(x>=0.0?x:(x==static_cast<int>(x)?x:x-1.0));
}

#endif



// vil_round_ceil -- round towards plus infinity
//
// Be careful: argument absolute value must be less than INT_MAX/2
// for vil_round_ceil to be guaranteed to work.
// We also assume that the rounding mode is not changed from the default
// one (or at least that it is always restored to the default one).

#if VIL_CONFIG_ENABLE_SSE2_ROUNDING // Fast sse2 implementation

inline int vil_round_ceil(float  x)
{
# if defined(VIL_CHECK_FPU_ROUNDING_MODE) && defined(__GNUC__)
  assert(fegetround()==FE_TONEAREST);
# endif
   return -(_mm_cvtss_si32(_mm_set_ss(-.5f-2*x))>>1);
}
inline int vil_round_ceil(double  x)
{
# if defined(VIL_CHECK_FPU_ROUNDING_MODE) && defined(__GNUC__)
  assert(fegetround()==FE_TONEAREST);
# endif
   return -(_mm_cvtsd_si32(_mm_set_sd(-.5-2*x))>>1);
}

#elif GCC_USE_FAST_IMPL // Fast gcc asm implementation

inline int vil_round_ceil(float  x)
{
# ifdef VIL_CHECK_FPU_ROUNDING_MODE
  assert(fegetround()==FE_TONEAREST);
# endif
  int r;
  x = -.5f-2*x;
  __asm__ __volatile__ ("fistpl %0" : "=m"(r) : "t"(x) : "st");
  return -(r>>1);
}
inline int vil_round_ceil(double  x)
{
# ifdef VIL_CHECK_FPU_ROUNDING_MODE
  assert(fegetround()==FE_TONEAREST);
# endif
  int r;
  x = -.5-2*x;
  __asm__ __volatile__ ("fistpl %0" : "=m"(r) : "t"(x) : "st");
  return -(r>>1);
}

#elif VC_USE_FAST_IMPL // Fast msvc asm implementation

inline int vil_round_ceil(float  x)
{
  int r;
  x = -.5f-2*x;
  __asm {
    fld x
    fistp r
  }
  return -(r>>1);
}
inline int vil_round_ceil(double  x)
{
  int r;
  x = -.5-2*x;
  __asm {
    fld x
    fistp r
  }
  return -(r>>1);
}

#else // Vanilla implementation

inline int vil_round_ceil(float  x)
{
  return static_cast<int>(x<0.f?x:(x==static_cast<int>(x)?x:x+1.f));
}
inline int vil_round_ceil(double x)
{
  return static_cast<int>(x<0.0?x:(x==static_cast<int>(x)?x:x+1.0));
}

#endif


#endif // vil_round_h_
