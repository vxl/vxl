// This is core/vil/algo/vil_fft.h
#ifndef vil_fft_h_
#define vil_fft_h_
//:
//  \file
//  \brief Functions to apply the FFT to an image.
// \author Fred Wheeler

#include <vcl_complex.h>
#include <vil/vil_image_view.h>

//: Perform in place forward FFT.
// \relates vil_image_view
// \relates vil_fft_2d_bwd
template<class T>
void
vil_fft_2d_fwd (vil_image_view<vcl_complex<T> > & img);

//: Perform in place backward FFT.
// Unlike vnl_fft_2d, scaling is done properly, so using
// vil_fft_2d_fwd(), then vil_fft_2d_bwd() gets back the original
// image.
// \relates vil_image_view
// \relates vil_fft_2d_fwd
template<class T>
void
vil_fft_2d_bwd (vil_image_view<vcl_complex<T> > & img);

#endif // vil_fft_h_
