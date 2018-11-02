// This is core/vil/vil_math_sse.hxx
#ifndef vil_math_sse_hxx_
#define vil_math_sse_hxx_

#ifndef vil_math_h_
#error "This header cannot be included directly, only through vil_math_.h"
#endif

#include <iostream>
#include <cstring>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>

#include <emmintrin.h>
#ifdef __SSE3__
#include <pmmintrin.h>
#endif

//:
// \file
// \brief Various mathematical manipulations of 2D images implemented with SSE
// intrinsic functions
// \author Chuck Atkins

//: Compute absolute difference of two 1D images (im_sum = |imA-imB|)
template<>
inline void vil_math_image_abs_difference_1d_sse<vxl_byte,vxl_byte,vxl_byte>(
  const vxl_byte* pxA, const vxl_byte* pxB, vxl_byte* pxD,
  unsigned len)
{
  assert(sizeof(vxl_byte) == 1);

  const unsigned ni_d_16 = len >> 4;
  const unsigned ni_m_16 = len & 0x0F;

  const __m128i* pxAxmm = reinterpret_cast<const __m128i*>(pxA);
  const __m128i* pxBxmm = reinterpret_cast<const __m128i*>(pxB);
        __m128i* pxDxmm = reinterpret_cast<__m128i*>(pxD);

  // Loop through the first set of pxs in groups of 16
  for (unsigned i = 0; i < ni_d_16; ++i, ++pxAxmm, ++pxBxmm, ++pxDxmm)
  {
#ifdef __SSE3__
    __m128i xmmA = _mm_lddqu_si128(pxAxmm);
    __m128i xmmB = _mm_lddqu_si128(pxBxmm);
#else // SSE2
    __m128i xmmA = _mm_loadu_si128(pxAxmm);
    __m128i xmmB = _mm_loadu_si128(pxBxmm);
#endif

    __m128i xmmMax = _mm_max_epu8(xmmA, xmmB);
    __m128i xmmMin = _mm_min_epu8(xmmA, xmmB);
    __m128i xmmD   = _mm_subs_epu8(xmmMax, xmmMin);

    _mm_storeu_si128(pxDxmm, xmmD);
  }

  if (ni_m_16 != 0)
  {
    // Process the remainder < 16
    vxl_byte pxLastA[16];
    vxl_byte pxLastB[16];
    vxl_byte pxLastD[16];
    __m128i* pxLastAxmm = reinterpret_cast<__m128i*>(pxLastA);
    __m128i* pxLastBxmm = reinterpret_cast<__m128i*>(pxLastB);
    __m128i* pxLastDxmm = reinterpret_cast<__m128i*>(pxLastD);

    std::memcpy(pxLastA, pxAxmm, ni_m_16);
    std::memcpy(pxLastB, pxBxmm, ni_m_16);
#ifdef __SSE3__
    __m128i xmmA = _mm_lddqu_si128(pxLastAxmm);
    __m128i xmmB = _mm_lddqu_si128(pxLastBxmm);
#else // SSE2
    __m128i xmmA = _mm_loadu_si128(pxLastAxmm);
    __m128i xmmB = _mm_loadu_si128(pxLastBxmm);
#endif

    __m128i xmmMax = _mm_max_epu8(xmmA, xmmB);
    __m128i xmmMin = _mm_min_epu8(xmmA, xmmB);
    __m128i xmmD   = _mm_subs_epu8(xmmMax, xmmMin);

    _mm_storeu_si128(pxLastDxmm, xmmD);
    std::memcpy(pxDxmm, pxLastD, ni_m_16);
  }
}


//: Compute absolute difference of two images (im_sum = |imA-imB|)
template<>
inline void vil_math_image_abs_difference_1d_sse<float,float,float>(
  const float* pxA, const float* pxB, float* pxD,
  unsigned len)
{
  assert(sizeof(float) == 4);

  const unsigned ni_d_4 = len >> 2;
  const unsigned ni_m_4_bytes = (len & 0x03) << 2;

  // Loop through the first set of pxs in groups of 16
  for (unsigned i = 0; i < ni_d_4; ++i, pxA += 4, pxB += 4, pxD += 4)
  {
    __m128 xmmA = _mm_loadu_ps(pxA);
    __m128 xmmB = _mm_loadu_ps(pxB);

    __m128 xmmMax = _mm_max_ps(xmmA, xmmB);
    __m128 xmmMin = _mm_min_ps(xmmA, xmmB);
    __m128 xmmD = _mm_sub_ps(xmmMax, xmmMin);

    _mm_storeu_ps(pxD, xmmD);
  }

  if (ni_m_4_bytes != 0)
  {
    // Process the remainder < 4
    // Use these for the last set of pxs in each row
    float pxLastA[4];
    float pxLastB[4];
    float pxLastD[4];

    std::memcpy(pxLastA, pxA, ni_m_4_bytes);
    std::memcpy(pxLastB, pxB, ni_m_4_bytes);
    __m128 xmmA = _mm_loadu_ps(pxLastA);
    __m128 xmmB = _mm_loadu_ps(pxLastB);

    __m128 xmmMax = _mm_max_ps(xmmA, xmmB);
    __m128 xmmMin = _mm_min_ps(xmmA, xmmB);
    __m128 xmmD = _mm_sub_ps(xmmMax, xmmMin);

    _mm_storeu_ps(pxLastD, xmmD);

    std::memcpy(pxD, pxLastD, ni_m_4_bytes);
  }
}

#endif // vil_math_sse_hxx_
