// This is brl/bbas/bil/algo/bil_finite_second_differences.h
#ifndef bil_finite_second_differences_h_
#define bil_finite_second_differences_h_
//:
// \file
// \brief Given forward/backward finite differences, calculates second derivatives
// \author  jdfunaro
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>

template <class T>
void bil_finite_second_differences(const vil_image_view<T>& src_im,
                                   const vil_image_view<T>& dxp_,
                                   const vil_image_view<T>& dxm_,
                                   const vil_image_view<T>& dyp_,
                                   const vil_image_view<T>& dym_,
                                   vil_image_view<T>& dxx_,
                                   vil_image_view<T>& dyy_,
                                   vil_image_view<T>& dxy_);

#endif // bil_finite_second_differences_h_
