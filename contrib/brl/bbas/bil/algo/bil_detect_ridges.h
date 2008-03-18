// This is brl/bbas/bil/algo/bil_detect_ridges.h
#ifndef bil_detect_ridges_h_
#define bil_detect_ridges_h_
//:
// \file
// \brief  Ridge Detection
// \author  jdfunaro, from matlab code by Amir Tamrakar, from Kalitzin etal PAMI 2001
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>

template <class T> 
void bil_detect_ridges( const vil_image_view<T>& Im,
                        float sigma,
                        float epsilon,
                        vil_image_view<int>& rho_int,
                        vil_image_view<float>& ex,
                        vil_image_view<float>& ey,
                        vil_image_view<float>& lambda);

void bil_detect_ridges( const vil_image_view<float>& Ix,
                        const vil_image_view<float>& Iy,
                        const vil_image_view<float>& Ixx,
                        const vil_image_view<float>& Iyy,
                        const vil_image_view<float>& Ixy,
                        float sigma,
                        float epsilon,
                        vil_image_view<int>& rho_int,
                        vil_image_view<float>& ex,
                        vil_image_view<float>& ey,
                        vil_image_view<float>& lambda);

#endif // bil_detect_ridges_h_
