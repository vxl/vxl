#ifndef vil_image_as_h_
#define vil_image_as_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_image_as
// .INCLUDE vil/vil_image_as.h
// .FILE vil_image_as.cxx
// .FILE vil_memory_image_of_format.txx
// @author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>
#include <vil/vil_byte.h>
#include <vil/vil_rgb_byte.h>

// These forms are used from templates
vil_image vil_image_as(vil_image const &, int* dummy);
vil_image vil_image_as(vil_image const &, vil_byte* dummy);
vil_image vil_image_as(vil_image const &, float* dummy);
vil_image vil_image_as(vil_image const &, double* dummy);
vil_image vil_image_as(vil_image const &, vil_rgb_byte* dummy);

vil_image vil_image_as_int(vil_image const & i);
vil_image vil_image_as_byte(vil_image const & i);
vil_image vil_image_as_float(vil_image const & i);
vil_image vil_image_as_double(vil_image const & i);
vil_image vil_image_as_rgb_byte(vil_image const & i);
vil_image vil_image_as_rgb_float(vil_image const & i);


#endif // vil_image_as_h_
