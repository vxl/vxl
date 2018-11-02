// This is core/vil1/vil1_ssd.hxx
#ifndef vil1_ssd_hxx_
#define vil1_ssd_hxx_

/*
  fsm
*/
#include "vil1_ssd.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class I1, class I2, class O>
O vil1_ssd(vil1_memory_image_of<I1> const &a,
           vil1_memory_image_of<I2> const &b,
           O *)
{
  assert(a.width() == b.width());
  assert(a.height() == b.height());
  unsigned w = a.width();
  unsigned h = a.height();

  // initialize accumulator
  O acc(0);

  for (unsigned j=0; j<h; ++j) {
    // get raster pointers. faster on non-optimized builds.
    I1 const *ra = a[j];
    I2 const *rb = b[j];

    // accumulate
    for (unsigned i=0; i<w; ++i) {
      O tmp = O(ra[i]) - O(rb[i]);
      acc += tmp*tmp;
    }
  }

  //
  return acc;
}

//--------------------------------------------------------------------------------

#define VIL1_SSD_INSTANTIATE(I1, I2, O) \
template O vil1_ssd(vil1_memory_image_of<I1 > const &, \
                    vil1_memory_image_of<I2 > const &, \
                    O *)

#endif // vil1_ssd_hxx_
