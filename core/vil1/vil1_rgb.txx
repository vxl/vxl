#include <vcl/vcl_compiler.h>
#include <vil/vil_rgb.h>
#undef VIL_RGB_INSTANTIATE
#undef VIL_RGB_INSTANTIATE_LS

//--------------------------------------------------------------------------------

#undef SPECIALIZE_CLAMP_PIXEL
#define SPECIALIZE_CLAMP_PIXEL(T) \
VCL_DEFINE_SPECIALIZATION \
vil_rgb<T> vil_clamp_pixel(vil_rgb<T> const& d, double min, double max) \
{ \
  return vil_rgb<T>(vil_clamp_pixel(d.R_, min , max), \
		    vil_clamp_pixel(d.G_, min , max), \
		    vil_clamp_pixel(d.B_, min , max)); \
}

// instantiation macros for vil_rgb<T>s
#undef VIL_RGB_INSTANTIATE
#define VIL_RGB_INSTANTIATE(T) \
template struct vil_rgb<T>; \
VCL_INSTANTIATE_INLINE(vil_rgb<T> max(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<T> min(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<T> average(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<T> operator+(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double> operator/(const vil_rgb<T>& a, double b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double> operator*(const vil_rgb<T>& a, double b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double> operator*(double b, const vil_rgb<T>& a))

// must do << separately for byte specialization
#undef VIL_RGB_INSTANTIATE_LS
#define VIL_RGB_INSTANTIATE_LS(T)\
VCL_INSTANTIATE_INLINE(ostream& operator<<(ostream&, const vil_rgb<T>& ))

