// This is core/vil1/vil1_rgb.txx
#ifndef vil1_rgb_txx_
#define vil1_rgb_txx_

#include "vil1_rgb.h"

//--------------------------------------------------------------------------------

#undef SPECIALIZE_CLAMP_PIXEL
#define SPECIALIZE_CLAMP_PIXEL(T) \
VCL_DEFINE_SPECIALIZATION \
vil1_rgb<T> vil1_clamp_pixel(vil1_rgb<T> const& d, double min, double max) \
{ \
  return vil1_rgb<T>(vil1_clamp_pixel(d.r, min , max), \
                     vil1_clamp_pixel(d.g, min , max), \
                     vil1_clamp_pixel(d.b, min , max)); \
}

// instantiation macros for vil1_rgb<T>s
#undef VIL1_RGB_INSTANTIATE
#define VIL1_RGB_INSTANTIATE(T) \
template struct vil1_rgb<T >; \
/* VCL_INSTANTIATE_INLINE(vil1_rgb<T > max(const vil1_rgb<T >& a, const vil1_rgb<T >& b)); */\
/* VCL_INSTANTIATE_INLINE(vil1_rgb<T > min(const vil1_rgb<T >& a, const vil1_rgb<T >& b)); */\
VCL_INSTANTIATE_INLINE(vil1_rgb<T > average(const vil1_rgb<T >& a, const vil1_rgb<T >& b)); \
VCL_INSTANTIATE_INLINE(vil1_rgb<T > operator+(const vil1_rgb<T >& a, const vil1_rgb<T >& b)); \
VCL_INSTANTIATE_INLINE(vil1_rgb<double > operator/(const vil1_rgb<T >& a, double b)); \
VCL_INSTANTIATE_INLINE(vil1_rgb<double > operator*(const vil1_rgb<T >& a, double b)); \
VCL_INSTANTIATE_INLINE(vil1_rgb<double > operator*(double b, const vil1_rgb<T >& a))

// must do << separately for byte specialization
#undef VIL1_RGB_INSTANTIATE_LS
#define VIL1_RGB_INSTANTIATE_LS(T) \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator<<(vcl_ostream&, const vil1_rgb<T >& ))

#endif // vil1_rgb_txx_
