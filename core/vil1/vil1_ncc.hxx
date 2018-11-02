// This is core/vil1/vil1_ncc.hxx
#ifndef vil1_ncc_hxx_
#define vil1_ncc_hxx_

/*
  capes@robots.ox.ac.uk
*/
#include <cmath>
#include "vil1_ncc.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class I1, class I2, class O>
O vil1_ncc(vil1_memory_image_of<I1> const &a,
           vil1_memory_image_of<I2> const &b,
           O *)
{
  assert(a.width() == b.width());
  assert(a.height() == b.height());
  unsigned w = a.width();
  unsigned h = a.height();

  // initialize accumulator
  O acc(0);
  O mean1(0);
  O mean2(0);
  O var1(0);
  O var2(0);

  for (unsigned j=0; j<h; ++j) {
    // get raster pointers. faster on non-optimized builds.
    I1 const *ra = a[j];
    I2 const *rb = b[j];

    // compute means
    for (unsigned i=0; i<w; ++i) {
      mean1 += O(ra[i]);
      mean2 += O(rb[i]);
    }
  }
  mean1 /= O(w*h);
  mean2 /= O(w*h);

  for (unsigned j=0; j<h; ++j) {
    // get raster pointers. faster on non-optimized builds.
    I1 const *ra = a[j];
    I2 const *rb = b[j];

    // accumulate
    for (unsigned i=0; i<w; ++i) {
      O tmp1 = O(ra[i]) - mean1;
      O tmp2 = O(rb[i]) - mean2;
      var1 += tmp1 * tmp1;
      var2 += tmp2 * tmp2;
      acc += tmp1 * tmp2;
    }
  }
  var1 /= O(w*h);
  var2 /= O(w*h);

  // Looks a bit dumb I know, but makes it clear what's going on...
  return acc / (std::sqrt(w*h*var1) * std::sqrt(w*h*var2));
}

//--------------------------------------------------------------------------------

template <class T1, class T2, class A>
A vil1_ncc(T1 const * const *I1, int x1, int y1,
           T2 const * const *I2, int x2, int y2,
           int size, A *)
{
  unsigned N = 0;
  A S1=0, S2 = 0;
  A S11=0, S12=0, S22=0;

  for (int j=-size; j<=size; ++j) {
    T1 const *row1 = I1[y1+j];
    T2 const *row2 = I2[y2+j];
    for (int i=-size; i<=size; ++i) {
      A im1 = row1[x1+i];
      A im2 = row2[x2+i];
      N += 1;
      S1 += im1; S2 += im2;
      S11 += im1*im1; S12 += im1*im2; S22 += im2*im2;
    }
  }

  return (N*S12 - S1*S2) / std::sqrt((N*S11 - S1*S1) * (N*S22 - S2*S2));
}

//--------------------------------------------------------------------------------

#define VIL1_NCC_INSTANTIATE(I1, I2, O) \
template O vil1_ncc(vil1_memory_image_of<I1 > const &, \
                    vil1_memory_image_of<I2 > const &, \
                    O *); \
template O vil1_ncc(I1 const * const *, int, int, \
                    I2 const * const *, int, int, \
                    int, O *)

#endif // vil1_ncc_hxx_
