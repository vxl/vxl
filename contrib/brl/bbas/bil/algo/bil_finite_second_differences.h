// This is contrib/brl/bbas/bil/algo/bil_finite_second_differences.h
#ifndef bil_finite_second_differences_h_
#define bil_finite_second_differences_h_
//:
// \file
// \brief Given forward/backward finite differences, calculates second derivatives
// \author  jdfunaro
// \verbatim
//  Modifications <none>
// \endverbatim


#include <vil/vil_image_view.h>
template <class T>
void bil_finite_second_differences(const vil_image_view<T>& src_im,
                    const vil_image_view<T>& _dxp,
                    const vil_image_view<T>& _dxm,
                    const vil_image_view<T>& _dyp,
                    const vil_image_view<T>& _dym,
                    vil_image_view<T>& _dxx,
                    vil_image_view<T>& _dyy,
                    vil_image_view<T>& _dxy);
#endif 
