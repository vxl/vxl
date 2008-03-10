// This is contrib/brl/bbas/bil/algo/bil_finite_differences.h
#ifndef bil_finite_differences_h_
#define bil_finite_differences_h_
//:
// \file
// \brief Calculates forward, backward, and center difference derivatives
// \author  jdfunaro
// \verbatim
//  Modifications <none>
// \endverbatim

#include <vil/vil_image_view.h>

template <class T>
void bil_finite_differences(const vil_image_view<T>& src_im,
                   vil_image_view<T>& _dxp,
                   vil_image_view<T>& _dxm,
                   vil_image_view<T>& _dxc,
                   vil_image_view<T>& _dyp,
                   vil_image_view<T>& _dym,
                   vil_image_view<T>& _dyc);
#endif 
