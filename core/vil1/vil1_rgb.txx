// This is vxl/vil/vil_rgb.txx
#ifndef vil_rgb_txx_
#define vil_rgb_txx_

#include "vil_rgb.h"

//--------------------------------------------------------------------------------

#undef SPECIALIZE_CLAMP_PIXEL
#define SPECIALIZE_CLAMP_PIXEL(T) \
VCL_DEFINE_SPECIALIZATION \
vil_rgb<T> vil_clamp_pixel(vil_rgb<T> const& d, double min, double max) \
{ \
  return vil_rgb<T>(vil_clamp_pixel(d.r, min , max), \
                    vil_clamp_pixel(d.g, min , max), \
                    vil_clamp_pixel(d.b, min , max)); \
}

// instantiation macros for vil_rgb<T>s
#undef VIL_RGB_INSTANTIATE
#define VIL_RGB_INSTANTIATE(T) \
template struct vil_rgb<T >; \
/* VCL_INSTANTIATE_INLINE(vil_rgb<T > max(const vil_rgb<T >& a, const vil_rgb<T >& b)); */\
/* VCL_INSTANTIATE_INLINE(vil_rgb<T > min(const vil_rgb<T >& a, const vil_rgb<T >& b)); */\
VCL_INSTANTIATE_INLINE(vil_rgb<T > average(const vil_rgb<T >& a, const vil_rgb<T >& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<T > operator+(const vil_rgb<T >& a, const vil_rgb<T >& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double > operator/(const vil_rgb<T >& a, double b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double > operator*(const vil_rgb<T >& a, double b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double > operator*(double b, const vil_rgb<T >& a))

// must do << separately for byte specialization
#undef VIL_RGB_INSTANTIATE_LS
#define VIL_RGB_INSTANTIATE_LS(T) \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator<<(vcl_ostream&, const vil_rgb<T >& ))

#endif // vil_rgb_txx_
