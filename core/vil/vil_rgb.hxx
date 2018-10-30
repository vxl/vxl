// This is core/vil/vil_rgb.hxx
#ifndef vil_rgb_hxx_
#define vil_rgb_hxx_

#include "vil_rgb.h"

//--------------------------------------------------------------------------------

#undef SPECIALIZE_CLAMP_PIXEL
#define SPECIALIZE_CLAMP_PIXEL(T) \
template <> \
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
/*template vil_rgb<T > max(const vil_rgb<T >& a, const vil_rgb<T >& b); */\
/*template vil_rgb<T > min(const vil_rgb<T >& a, const vil_rgb<T >& b); */\
/*template vil_rgb<T > average(const vil_rgb<T >& a, const vil_rgb<T >& b) ; */ \
/*template vil_rgb<T > operator+(const vil_rgb<T >& a, const vil_rgb<T >& b) ; */ \
/*template vil_rgb<double > operator/(const vil_rgb<T >& a, double b) ; */ \
/*template vil_rgb<double > operator*(const vil_rgb<T >& a, double b) ; */ \
/*template vil_rgb<double > operator*(double b, const vil_rgb<T >& a) */

// must do << separately for byte specialization
#undef VIL_RGB_INSTANTIATE_LS
#define VIL_RGB_INSTANTIATE_LS(T) \
template <> \
std::ostream& operator<<(std::ostream& s, vil_rgb<T > const& rgb) \
{ \
return s<< '[' << (int)rgb.r << ' ' << (int)rgb.g << ' ' << (int)rgb.b << ']'; \
}

#endif // vil_rgb_hxx_
