#if 0 // this file is no longer needed?
#include <vil1/vil1_rgb.h>

// the following cpp logic mirrors that in vil1_rgb.h :

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
// instantiate the inline partially specializated template from vil1_rgb.h :
VCL_INSTANTIATE_INLINE(vil1_rgb<unsigned char> vil1_clamp(const vil1_rgb<double> &, vil1_rgb<unsigned char> *));
#else
// instantiate the inline non-partial specialization from vil1_rgb.h :
VCL_INSTANTIATE_INLINE(vil1_rgb<unsigned char> vil1_clamp(const vil1_rgb<double> &, vil1_rgb<unsigned char> *));
#endif
#endif
