// This is core/vil/algo/vil_dog_filter_5tap.h
#ifndef vil_dog_filter_5tap_h_
#define vil_dog_filter_5tap_h_
//:
// \file
// \brief Computes difference of gaussians (using a 5 tap filter).
// \author Tim Cootes

#include <vil/algo/vil_gauss_filter.h>
#include <vil/vil_math.h>

//: Smooth source with gaussian filter and compute difference
//  Uses a 5-tap filter with gaussian width sigma.
//  \param sigma: Width of gaussian
//  \param smooth_im: Result of smoothing the src_im
//  \param dog_im: src_im - smooth_im
template <class T>
void vil_dog_filter_5tap(const vil_image_view<T>& src_im,
                         vil_image_view<T>& smooth_im,
                         vil_image_view<T>& dog_im,
                         double sigma)
{
  vil_gauss_filter_5tap_params smooth_params(sigma);
  vil_gauss_filter_5tap(src_im,smooth_im,smooth_params,dog_im);
  vil_math_image_difference(src_im,smooth_im,dog_im);
}

#endif

