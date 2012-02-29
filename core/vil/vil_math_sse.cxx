// This is core/vil/vil_math_sse.cxx

#include <vcl_cstddef.h>
#include <vcl_cstring.h>
#include <vxl_config.h>
#include <x86intrin.h>

#include "vil_math_sse.h"

#include <vcl_iostream.h>

//:
// \file
// \brief Various mathematical manipulations of 2D images implemented with SSE
// intrinsic functions
// \author Chuck Atkins

//: Compute absolute difference of two images (im_sum = |imA-imB|)
// \relatesalso vil_image_view
template<>
void vil_math_image_abs_difference_sse<vxl_byte>(
  const void* imA, const void* imB, void* imD,
  unsigned ni, unsigned nj, unsigned np,
  vcl_ptrdiff_t jstepA, vcl_ptrdiff_t jstepB, vcl_ptrdiff_t jstepD,
  vcl_ptrdiff_t pstepA, vcl_ptrdiff_t pstepB, vcl_ptrdiff_t pstepD)
{
  const unsigned ni_d_16 = ni / 16;
  const unsigned ni_m_16 = ni % 16 ;

  // Use these for the last set of pixels in each row
  vxl_uint_8 pixelLastA[16];
  vxl_uint_8 pixelLastB[16];
  vxl_uint_8 pixelLastD[16];
  __m128i* pixelLastAxmm = reinterpret_cast<__m128i*>(pixelLastA);
  __m128i* pixelLastBxmm = reinterpret_cast<__m128i*>(pixelLastB);
  __m128i* pixelLastDxmm = reinterpret_cast<__m128i*>(pixelLastD);

  const vxl_byte* planeA = reinterpret_cast<const vxl_byte*>(imA);
  const vxl_byte* planeB = reinterpret_cast<const vxl_byte*>(imB);
        vxl_byte* planeD = reinterpret_cast< vxl_byte*>(imD);
  for (unsigned p = 0; p < np; ++p)
   {
    const vxl_byte* rowA = planeA;
    const vxl_byte* rowB = planeB;
          vxl_byte* rowD = planeD;
    for (unsigned j = 0; j < nj; ++j)
    {
      const __m128i* pixelAxmm = reinterpret_cast<const __m128i*>(rowA);
      const __m128i* pixelBxmm = reinterpret_cast<const __m128i*>(rowB);
            __m128i* pixelDxmm = reinterpret_cast<__m128i*>(rowD);
      
      // Loop through the first set of pixels in groups of 16
      for (unsigned i = 0; i < ni_d_16; ++i)
      {
#ifdef __SSE3__
        __m128i xmmA = _mm_lddqu_si128(pixelAxmm);
        __m128i xmmB = _mm_lddqu_si128(pixelBxmm);
#else // SSE2
        __m128i xmmA = _mm_loadu_si128(pixelAxmm);
        __m128i xmmB = _mm_loadu_si128(pixelBxmm);
#endif
        __m128i xmmMax = _mm_max_epu8(xmmA, xmmB);
        __m128i xmmMin = _mm_min_epu8(xmmA, xmmB);
        __m128i xmmD = _mm_subs_epu8(xmmMax, xmmMin);
        _mm_storeu_si128(pixelDxmm, xmmD);

        ++pixelAxmm;
        ++pixelBxmm;
        ++pixelDxmm;
      }
      // Process the remainder < 16
      vcl_memcpy(pixelLastA, pixelAxmm, ni_m_16);
      vcl_memcpy(pixelLastB, pixelBxmm, ni_m_16);
#ifdef __SSE3__
      __m128i xmmA = _mm_lddqu_si128(pixelLastAxmm);
      __m128i xmmB = _mm_lddqu_si128(pixelLastBxmm);
#else // SSE2
      __m128i xmmA = _mm_loadu_si128(pixelLastAxmm);
      __m128i xmmB = _mm_loadu_si128(pixelLastBxmm);
#endif
      __m128i xmmMax = _mm_max_epu8(xmmA, xmmB);
      __m128i xmmMin = _mm_min_epu8(xmmA, xmmB);
      __m128i xmmD = _mm_subs_epu8(xmmMax, xmmMin);
      _mm_storeu_si128(pixelLastDxmm, xmmD);
      vcl_memcpy(pixelDxmm, pixelLastD, ni_m_16);

      rowA += jstepA;
      rowB += jstepB;
      rowD += jstepD;
    }
    planeA += pstepA;
    planeB += pstepB;
    planeD += pstepD;
  }
}

