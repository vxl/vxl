// This is mul/vil2/algo/vil2_fft.cxx
#ifndef vil2_fft_txx_
#define vil2_fft_txx_
//:
// \file
// \brief Functions to apply the FFT to an image.
// \author Fred Wheeler

#include "vil2_fft.h"
#include <vcl_complex.h>
#include <vil2/vil2_image_view.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_ref.h>
#include <vnl/algo/vnl_fft_1d.h>

//: Perform in place FFT in one dimension.
template<class T>
static void
vil2_fft_2d_base (vcl_complex<T> * data,
                  unsigned n0, vcl_ptrdiff_t step0,
                  unsigned n1, vcl_ptrdiff_t step1,
                  unsigned n2, vcl_ptrdiff_t step2,
                  int dir)
{
  vnl_fft_1d<T> fft_1d (n0);

  if (1 == step0) {
      for (unsigned i1=0; i1<n1; i1++)
      for (unsigned i2=0; i2<n2; i2++) {
          vcl_complex<T> * d = data + i1*step1 + i2*step2;
          // use contiguous data memory directly in vnl_vector
          vnl_vector_ref<vcl_complex<T> > v (n0, d);
          fft_1d.transform (v, dir);
          if (1 == dir) v /= n0; // proper scaling for inverse FFT
      }
  }
  else {
      vnl_vector<vcl_complex<T> > v (n0);
      for (unsigned i1=0; i1<n1; i1++)
      for (unsigned i2=0; i2<n2; i2++) {
          vcl_complex<T> * d = data + i1*step1 + i2*step2;
          // copy non-contiguous data memory to a vnl_vector
          for (unsigned i0=0; i0<n0; i0++, d+=step0)
              v(i0) = *d;
          fft_1d.transform (v, dir);
          if (1 == dir) v /= n0; // proper scaling for inverse FFT
          d = data + i1*step1 + i2*step2;
          // copy vnl_vector back to non-contiguous data memory
          for (unsigned i0=0; i0<n0; i0++, d+=step0)
              *d = v(i0);
      }
  }
}

template<class T>
void
vil2_fft_2d_fwd (vil2_image_view<vcl_complex<T> > & img)
{
  vil2_fft_2d_base (img.top_left_ptr(),
                    img.ni(), img.istep(),
                    img.nj(), img.jstep(),
                    img.nplanes(), img.planestep(),
                    1);
  vil2_fft_2d_base (img.top_left_ptr(),
                    img.nj(), img.jstep(),
                    img.ni(), img.istep(),
                    img.nplanes(), img.planestep(),
                    1);
}

template<class T>
void
vil2_fft_2d_bwd (vil2_image_view<vcl_complex<T> > & img)
{
  vil2_fft_2d_base (img.top_left_ptr(),
                    img.nj(), img.jstep(),
                    img.ni(), img.istep(),
                    img.nplanes(), img.planestep(),
                    -1);
  vil2_fft_2d_base (img.top_left_ptr(),
                    img.ni(), img.istep(),
                    img.nj(), img.jstep(),
                    img.nplanes(), img.planestep(),
                    -1);
}

#undef VIL2_FFT_INSTANTIATE
#define VIL2_FFT_INSTANTIATE(T) \
template void vil2_fft_2d_base (vcl_complex<T >* data, \
                                unsigned n0, vcl_ptrdiff_t step0, \
                                unsigned n1, vcl_ptrdiff_t step1, \
                                unsigned n2, vcl_ptrdiff_t step2, \
                                int dir); \
template void vil2_fft_2d_fwd(vil2_image_view<vcl_complex<T > >& img); \
template void vil2_fft_2d_bwd(vil2_image_view<vcl_complex<T > >& img)

#endif // vil2_fft_txx_
