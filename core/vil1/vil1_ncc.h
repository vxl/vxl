#ifndef vil_ncc_h_
#define vil_ncc_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  capes@robots.ox.ac.uk
*/

// Routines to compute normalized cross-correlation between two images.

#include <vil/vil_memory_image_of.h>

//: normalized cross correlation for in-core images.
// I1 : input pixel type 1
// I2 : input pixel type 2
// O  : accumulator type
template <class I1, class I2, class O> 
O vil_ncc(vil_memory_image_of<I1> const &,
	  vil_memory_image_of<I2> const &,
	  O * /*dummy*/);

//: Normalized cross-correlation on [-n, +n] x [-n, +n] neighbourhood.
// No bounds checking is performed.
template <class T1, class T2, class A>
A vil_ncc(T1 const * const *I1, int x1, int y1,
	  T2 const * const *I2, int x2, int y2,
	  int n, A *);

#endif // vil_ncc_h_
