#include <vcl/vcl_compiler.h>
#include <vil/vil_rgb.h>
#undef VBL_RGB_INSTANTIATE
#undef VBL_RGB_INSTANTIATE_LS

//--------------------------------------------------------------------------------

// instantiation macros for vil_rgb<T>s
#undef VBL_RGB_INSTANTIATE
#define VBL_RGB_INSTANTIATE(T) \
template struct vil_rgb<T>; \
VCL_INSTANTIATE_INLINE(vil_rgb<T> max(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<T> min(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<T> average(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<T> operator+(const vil_rgb<T>& a, const vil_rgb<T>& b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double> operator/(const vil_rgb<T>& a, double b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double> operator*(const vil_rgb<T>& a, double b)); \
VCL_INSTANTIATE_INLINE(vil_rgb<double> operator*(double b, const vil_rgb<T>& a))

// must do << separately for byte specialization
#undef VBL_RGB_INSTANTIATE_LS
#define VBL_RGB_INSTANTIATE_LS(T)\
VCL_INSTANTIATE_INLINE(ostream& operator<<(ostream&, const vil_rgb<T>& ))

