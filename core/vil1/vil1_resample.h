#ifndef vil_resample_h_
#define vil_resample_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_resample
// .INCLUDE vil/vil_resample.h
// .FILE vil_resample.cxx
// \author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>

//:
vil_image vil_resample(vil_image const &, unsigned new_width, unsigned new_height);

#endif // vil_resample_h_
