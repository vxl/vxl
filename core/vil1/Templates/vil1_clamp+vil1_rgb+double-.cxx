#if 0 // this file is no longer needed?
#include <vil/vil_rgb.h>

// the following cpp logic mirrors that in vil_rgb.h :

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
// instantiate the inline partially specializated template from vil_rgb.h :
VCL_INSTANTIATE_INLINE(vil_rgb<unsigned char> vil_clamp(const vil_rgb<double> &, vil_rgb<unsigned char> *));
#else
// instantiate the inline non-partial specialization from vil_rgb.h :
VCL_INSTANTIATE_INLINE(vil_rgb<unsigned char> vil_clamp(const vil_rgb<double> &, vil_rgb<unsigned char> *));
#endif
#endif
