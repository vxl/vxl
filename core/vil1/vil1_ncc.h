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

#endif
