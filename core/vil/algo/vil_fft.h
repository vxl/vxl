// This is mul/vil2/algo/vil2_fft.h
#ifndef vil2_fft_h_
#define vil2_fft_h_
//:
//  \file
//  \brief Functions to apply the FFT to an image.
// \author Fred Wheeler

#include <vcl_complex.h>
#include <vil2/vil2_image_view.h>

//: Perform in place forward FFT.
// \relates vil2_image_view
// \relates vil2_fft_2d_bwd
template<class T>
void
vil2_fft_2d_fwd (vil2_image_view<vcl_complex<T> > & img);

//: Perform in place backward FFT.
// Unlike vnl_fft_2d, scaling is done properly, so using
// vil2_fft_2d_fwd(), then vil2_fft_2d_bwd() gets back the original
// image.
// \relates vil2_image_view
// \relates vil2_fft_2d_fwd
template<class T>
void
vil2_fft_2d_bwd (vil2_image_view<vcl_complex<T> > & img);

#endif // vil2_fft_h_
