#ifndef vil_ssd_h_
#define vil_ssd_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vil_ssd - Routines to compute sum of squared differences between two images
// .INCLUDE vil/vil_ssd.h
// .FILE vil_ssd.txx
// .SECTION Author
//  fsm@robots.ox.ac.uk

#include <vil/vil_memory_image_of.h>

//: sum of squared differences for in-core images.
// I1 : input pixel type 1
// I2 : input pixel type 2
// O  : accumulator type
template <class I1, class I2, class O>
O vil_ssd(vil_memory_image_of<I1> const &,
          vil_memory_image_of<I2> const &,
          O * /*dummy*/);

#endif // vil_ssd_h_
