/*
  capes@robots.ox.ac.uk
*/
#include "vil_ncc.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cmath.h> // vcl_sqrt()

template <class I1, class I2, class O> 
O vil_ncc(vil_memory_image_of<I1> const &a,
	  vil_memory_image_of<I2> const &b,
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
  return acc / (sqrt(w*h*var1)*sqrt(w*h*var2));
}

//--------------------------------------------------------------------------------

#define VIL_NCC_INSTANTIATE(I1, I2, O) \
template O vil_ncc/*<I1, I2, O >*/(vil_memory_image_of<I1 > const &, \
                                   vil_memory_image_of<I2 > const &, \
                                   O *); \
;
