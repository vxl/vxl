// This is contrib/mul/vil2/vil2_rgb.txx
#ifndef vil2_rgb_txx_
#define vil2_rgb_txx_

#include "vil2_rgb.h"

//--------------------------------------------------------------------------------

#undef SPECIALIZE_CLAMP_PIXEL
#define SPECIALIZE_CLAMP_PIXEL(T) \
VCL_DEFINE_SPECIALIZATION \
vil2_rgb<T> vil2_clamp_pixel(vil2_rgb<T> const& d, double min, double max) \
{ \
  return vil2_rgb<T>(vil2_clamp_pixel(d.r, min , max), \
                    vil2_clamp_pixel(d.g, min , max), \
                    vil2_clamp_pixel(d.b, min , max)); \
}

// instantiation macros for vil2_rgb<T>s
#undef VIL2_RGB_INSTANTIATE
#define VIL2_RGB_INSTANTIATE(T) \
template struct vil2_rgb<T >; \
/* VCL_INSTANTIATE_INLINE(vil2_rgb<T > max(const vil2_rgb<T >& a, const vil2_rgb<T >& b)); */\
/* VCL_INSTANTIATE_INLINE(vil2_rgb<T > min(const vil2_rgb<T >& a, const vil2_rgb<T >& b)); */\
VCL_INSTANTIATE_INLINE(vil2_rgb<T > average(const vil2_rgb<T >& a, const vil2_rgb<T >& b)); \
VCL_INSTANTIATE_INLINE(vil2_rgb<T > operator+(const vil2_rgb<T >& a, const vil2_rgb<T >& b)); \
VCL_INSTANTIATE_INLINE(vil2_rgb<double > operator/(const vil2_rgb<T >& a, double b)); \
VCL_INSTANTIATE_INLINE(vil2_rgb<double > operator*(const vil2_rgb<T >& a, double b)); \
VCL_INSTANTIATE_INLINE(vil2_rgb<double > operator*(double b, const vil2_rgb<T >& a))

// must do << separately for byte specialization
#undef VIL2_RGB_INSTANTIATE_LS
#define VIL2_RGB_INSTANTIATE_LS(T) \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator<<(vcl_ostream&, const vil2_rgb<T >& ))

#endif // vil2_rgb_txx_
