// This is core/vil/vil_math_sse.h
#ifndef vil_math_sse_h_
#define vil_math_sse_h_

#ifndef vil_math_h_
#error "This header cannot be included directly, only through vil_math.h"
#endif

//:
// \file
// \brief Various mathematical manipulations of 2D images implemented with SSE
// intrinsic functions
// \author Chuck Atkins

// To add a new optimized implementation for a different set of types:
//
//  1.  Add a call to VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_DECL for the
//      new combination of types the implementation is defined for.  This will
//      declare the prototype for the specialization.
//  2.  Add a call to VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_IMPL for the
//      new combination of types the implementation is defined for.  This will
//      implement the outer function which calls the SSE optimized function for
//      the right set of types.
//  3.  Add the implementation for vil_math_image_abs_difference_1d_sse
//      specialized for the new type combination in vil_math_sse.hxx
//
// Note:
//  The types need not all three be the same so long as the specialization
//  of vil_math_image_abs_difference_1d_sse in vil_math_sse.hxx is implemented
//  for the defined combination of types


//: Compute absolute difference of two 1D images (imD = |imA-imB|)
template<class aT, class bT, class dT>
void vil_math_image_abs_difference_1d_generic(
  const aT* pxA, std::ptrdiff_t isA,
  const bT* pxB, std::ptrdiff_t isB,
        dT* pxD, std::ptrdiff_t isD,
  unsigned len);

template<class aT, class bT, class dT>
void vil_math_image_abs_difference_1d_sse(
  const aT* pxA, const bT* pxB, dT* pxD,
  unsigned len);

template<class aT, class bT, class dT>
void vil_math_image_abs_difference_1d(
  const aT* pxA, std::ptrdiff_t isA,
  const bT* pxB, std::ptrdiff_t isB,
        dT* pxD, std::ptrdiff_t isD,
  unsigned len);

#define VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_DECL(aT,bT,dT)  \
template<>                                                        \
void vil_math_image_abs_difference_1d_sse<aT,bT,dT>(              \
  const aT* pxA, const bT* pxB, dT* pxD,                          \
  unsigned len);                                                  \
template<>                                                        \
void vil_math_image_abs_difference_1d<aT,bT,dT>(                  \
  const aT* pxA, std::ptrdiff_t isA,                               \
  const bT* pxB, std::ptrdiff_t isB,                               \
        dT* pxD, std::ptrdiff_t isD,                               \
  unsigned len);

VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_DECL(vxl_byte,vxl_byte,vxl_byte)
VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_DECL(float,float,float)

#undef VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_DECL

#define VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_IMPL(aT,bT,dT)        \
template<>                                                              \
inline void vil_math_image_abs_difference_1d<aT,bT,dT>(                 \
  const aT* pxA, std::ptrdiff_t isA,                                     \
  const bT* pxB, std::ptrdiff_t isB,                                     \
        dT* pxD, std::ptrdiff_t isD,                                     \
  unsigned len)                                                         \
{                                                                       \
  if (isA == 1 && isB == 1 && isD == 1)                                 \
  {                                                                     \
    vil_math_image_abs_difference_1d_sse<aT,bT,dT>(pxA, pxB, pxD, len); \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    vil_math_image_abs_difference_1d_generic<aT,bT,dT>(                 \
      pxA, isA, pxB, isB, pxD, isD, len);                               \
  }                                                                     \
}

VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_IMPL(vxl_byte,vxl_byte,vxl_byte)
VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_IMPL(float,float,float)

#undef VIL_MATH_IMAGE_ABS_DIFF_1D_SSE_SPECIALIZE_IMPL

#endif
