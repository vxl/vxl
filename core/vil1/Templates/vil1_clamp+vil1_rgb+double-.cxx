#if 0 // this file is no longer needed?
#include <vil/vil_rgb.h>

// the following cpp logic mirrors that in vil_rgb.h :

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
// instantiate the inline partially specializated template from vil_rgb.h :
VCL_INSTANTIATE_INLINE(vil_rgb<unsigned char> vil_clamp(const vil_rgb<double> &, vil_rgb<unsigned char> *));
#elif defined(VCL_GCC_27)
// define a plain (non-templated) function. 2.7 can't tell the difference.
vil_rgb<unsigned char> vil_clamp(vil_rgb<double> const &d, vil_rgb<unsigned char>*) {
  unsigned char ttt;
  return vil_rgb<unsigned char>(vil_clamp(d.R_, &ttt),
			    vil_clamp(d.G_, &ttt),
			    vil_clamp(d.B_, &ttt));
}
#else
// instantiate the inline non-partial specialization from vil_rgb.h :
VCL_INSTANTIATE_INLINE(vil_rgb<unsigned char> vil_clamp(const vil_rgb<double> &, vil_rgb<unsigned char> *));
#endif
#endif
