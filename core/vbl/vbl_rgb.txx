#ifdef __GNUC__
#pragma instantiate "vbl_rgb.h"
#endif

#include <vcl/vcl_compiler.h>
#include <vbl/vbl_rgb.h>
#undef VBL_RGB_INSTANTIATE
#undef VBL_RGB_INSTANTIATE_LS

//--------------------------------------------------------------------------------

// instantiation macros for vbl_rgb<T>s
#undef VBL_RGB_INSTANTIATE
#define VBL_RGB_INSTANTIATE(T) \
template struct vbl_rgb<T>; \
VCL_INSTANTIATE_INLINE(vbl_rgb<T> max(const vbl_rgb<T>& a, const vbl_rgb<T>& b)) \
VCL_INSTANTIATE_INLINE(vbl_rgb<T> min(const vbl_rgb<T>& a, const vbl_rgb<T>& b)) \
VCL_INSTANTIATE_INLINE(vbl_rgb<T> average(const vbl_rgb<T>& a, const vbl_rgb<T>& b)) \
VCL_INSTANTIATE_INLINE(vbl_rgb<T> operator+(const vbl_rgb<T>& a, const vbl_rgb<T>& b)) \
VCL_INSTANTIATE_INLINE(vbl_rgb<double> operator/(const vbl_rgb<T>& a, double b)) \
VCL_INSTANTIATE_INLINE(vbl_rgb<double> operator*(const vbl_rgb<T>& a, double b)) \
VCL_INSTANTIATE_INLINE(vbl_rgb<double> operator*(double b, const vbl_rgb<T>& a)) \

// must do << separately for byte specialization
#undef VBL_RGB_INSTANTIATE_LS
#define VBL_RGB_INSTANTIATE_LS(T)\
VCL_INSTANTIATE_INLINE(ostream& operator<<(ostream&, const vbl_rgb<T>& ));
 
#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
// declare partial specialization, for those compilers which can take it :
template <class V, class U>
inline
vbl_rgb<U> vbl_clamp(const vbl_rgb<V>& d, vbl_rgb<U>* dummy)
{
  return vbl_rgb<U>(vbl_clamp(d.R, &dummy->R),
                vbl_clamp(d.G, &dummy->G),
                vbl_clamp(d.B, &dummy->B));
}
#elif defined(VCL_GCC_27)
// this one seems to be particularly broken: even if the vbl_clamp() from double
// to byte is declared inline and defined in this header, the macro
// VCL_INSTANTIATE_INLINE does not cause a function to be emitted to the
// Templates object file.
extern vbl_rgb<unsigned char> vbl_clamp(const vbl_rgb<double>& d, vbl_rgb<unsigned char> *);
#else
// just declare the most commonly needed specialization(s) :
VCL_DECLARE_SPECIALIZATION(vbl_rgb<unsigned char> vbl_clamp(const vbl_rgb<double>& d, vbl_rgb<unsigned char> *));
inline
vbl_rgb<unsigned char> vbl_clamp(const vbl_rgb<double>& d, vbl_rgb<unsigned char>* dummy)
{
  return vbl_rgb<unsigned char>(vbl_clamp(d.R, &dummy->R),
                            vbl_clamp(d.G, &dummy->G),
                            vbl_clamp(d.B, &dummy->B));
}
#endif

