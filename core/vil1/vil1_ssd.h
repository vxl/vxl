// This is core/vil1/vil1_ssd.h
#ifndef vil1_ssd_h_
#define vil1_ssd_h_
//:
// \file
// \brief Routines to compute sum of squared differences between two images
// \author  fsm

#include <vil1/vil1_memory_image_of.h>

//: sum of squared differences for in-core images.
// \verbatim
// I1 : input pixel type 1
// I2 : input pixel type 2
// O  : accumulator type
// \endverbatim
template <class I1, class I2, class O>
O vil1_ssd(vil1_memory_image_of<I1> const &,
           vil1_memory_image_of<I2> const &,
           O * /*dummy*/);

#endif // vil1_ssd_h_
