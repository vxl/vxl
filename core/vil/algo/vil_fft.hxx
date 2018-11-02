// This is core/vil/algo/vil_fft.hxx
#ifndef vil_fft_hxx_
#define vil_fft_hxx_
//:
// \file
// \brief Functions to apply the FFT to an image.
// \author Fred Wheeler

#include <complex>
#include <vector>
#include "vil_fft.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vnl/algo/vnl_fft_1d.h>

//: Perform in place FFT in one dimension.
template<class T>
static void
vil_fft_2d_base(std::complex<T> * data,
                unsigned n0, std::ptrdiff_t step0, // ni, istep
                unsigned n1, std::ptrdiff_t step1, // nj, jstep
                unsigned n2, std::ptrdiff_t step2, // nplanes, planestep
                int dir)
{
  vnl_fft_1d<T> fft_1d(n0);
  T factor = dir<0 ? T(1) : T(1)/static_cast<T>(n0);

  if (1 == step0) // use contiguous data memory directly
  {
    // FFT every pixel row (or column) in every colour band:
    for (unsigned i1=0; i1<n1; i1++)
    for (unsigned i2=0; i2<n2; i2++)
    {
      std::complex<T> * d = data + i1*step1 + i2*step2;
      fft_1d.transform(d, dir);
      if (dir >= 0)
        for (unsigned i0=0; i0<n0; ++i0)
          d[i0] *= factor; // proper scaling for forward FFT
    }
  }
  else // must copy non-contiguous data memory to a std::vector
  {
    std::vector<std::complex<T> > v(n0);
    for (unsigned i1=0; i1<n1; i1++)
    for (unsigned i2=0; i2<n2; i2++)
    {
      std::complex<T> * d = data + i1*step1 + i2*step2;
      for (unsigned i0=0; i0<n0; i0++, d+=step0)
        v[i0] = *d;
      fft_1d.transform(v, dir);
      // copy std::vector back to non-contiguous data memory
      d = data + i1*step1 + i2*step2;
      for (unsigned i0=0; i0<n0; i0++, d+=step0)
        *d = v[i0]*factor; // proper scaling for forward FFT
    }
  }
}

template<class T>
void
vil_fft_2d_fwd(vil_image_view<std::complex<T> >& img)
{
  vil_fft_2d_base(img.top_left_ptr(),
                  img.ni(), img.istep(),
                  img.nj(), img.jstep(),
                  img.nplanes(), img.planestep(),
                  1);
  vil_fft_2d_base(img.top_left_ptr(),
                  img.nj(), img.jstep(),
                  img.ni(), img.istep(),
                  img.nplanes(), img.planestep(),
                  1);
}

template<class T>
void
vil_fft_2d_bwd(vil_image_view<std::complex<T> >& img)
{
  vil_fft_2d_base(img.top_left_ptr(),
                  img.nj(), img.jstep(),
                  img.ni(), img.istep(),
                  img.nplanes(), img.planestep(),
                  -1);
  vil_fft_2d_base(img.top_left_ptr(),
                  img.ni(), img.istep(),
                  img.nj(), img.jstep(),
                  img.nplanes(), img.planestep(),
                  -1);
}

#undef VIL_FFT_INSTANTIATE
#define VIL_FFT_INSTANTIATE(T) \
template void vil_fft_2d_base(std::complex<T >* data, \
                              unsigned n0, std::ptrdiff_t step0, \
                              unsigned n1, std::ptrdiff_t step1, \
                              unsigned n2, std::ptrdiff_t step2, \
                              int dir); \
template void vil_fft_2d_fwd(vil_image_view<std::complex<T > >& img); \
template void vil_fft_2d_bwd(vil_image_view<std::complex<T > >& img)

#endif // vil_fft_hxx_
