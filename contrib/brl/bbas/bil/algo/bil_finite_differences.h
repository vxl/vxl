// This is brl/bbas/bil/algo/bil_finite_differences.h
#ifndef bil_finite_differences_h_
#define bil_finite_differences_h_
//:
// \file
// \brief Calculates forward, backward, and center difference derivatives
// \author  jdfunaro
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>

template <class T>
void bil_finite_differences(const vil_image_view<T>& src_im,
                            vil_image_view<T>& dxp_,
                            vil_image_view<T>& dxm_,
                            vil_image_view<T>& dxc_,
                            vil_image_view<T>& dyp_,
                            vil_image_view<T>& dym_,
                            vil_image_view<T>& dyc_);

#endif // bil_finite_differences_h_
