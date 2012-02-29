// This is core/vil/vil_math_sse.h
#ifndef vil_math_sse_h_
#define vil_math_sse_h_

#include <x86intrin.h>
#ifndef __SSE2__
#error "At least SSE2 supprot is required for this header"
#endif

#include <vxl_config.h>

//:
// \file
// \brief Various mathematical manipulations of 2D images implemented with SSE
// intrinsic functions
// \author Chuck Atkins

//: Compute absolute difference of two images (im_sum = |imA-imB|)
// \relatesalso vil_image_view
template<class T>
void vil_math_image_abs_difference_sse(
  const void* imA, const void* imB, void* imD,
  unsigned ni, unsigned nj, unsigned np,
  vcl_ptrdiff_t jstepA, vcl_ptrdiff_t jstepB, vcl_ptrdiff_t jstepD,
  vcl_ptrdiff_t pstepA, vcl_ptrdiff_t pstepB, vcl_ptrdiff_t pstepD);

#endif //vil_math_sse_h_

