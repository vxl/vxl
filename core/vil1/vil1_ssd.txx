// This is vxl/vil/vil_ssd.txx
#ifndef vil_ssd_txx_
#define vil_ssd_txx_

/*
  fsm
*/
#include "vil_ssd.h"
#include <vcl_cassert.h>

template <class I1, class I2, class O>
O vil_ssd(vil_memory_image_of<I1> const &a,
          vil_memory_image_of<I2> const &b,
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

#define VIL_SSD_INSTANTIATE(I1, I2, O) \
template O vil_ssd/*<I1, I2, O >*/(vil_memory_image_of<I1 > const &, \
                                   vil_memory_image_of<I2 > const &, \
                                   O *)

#endif // vil_ssd_txx_
