// This is core/vil/algo/vil_fft.h
#ifndef vil_fft_h_
#define vil_fft_h_
//:
//  \file
//  \brief Functions to apply the FFT to an image.
// \author Fred Wheeler

#include <complex>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>

//: Perform in place forward FFT.
// \relatesalso vil_image_view
// \relatesalso vil_fft_2d_bwd
template<class T>
void
vil_fft_2d_fwd (vil_image_view<std::complex<T> > & img);

//: Perform in place backward FFT.
// Unlike vnl_fft_2d, scaling is done properly, so using
// vil_fft_2d_fwd(), then vil_fft_2d_bwd() gets back the original
// image.
// \relatesalso vil_image_view
// \relatesalso vil_fft_2d_fwd
template<class T>
void
vil_fft_2d_bwd (vil_image_view<std::complex<T> > & img);

#endif // vil_fft_h_
