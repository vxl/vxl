#ifndef vepl_add_random_noise_h_
#define vepl_add_random_noise_h_

// .NAME vepl_add_random_noise - add random noise to all pixels
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_add_random_noise.h
// .FILE vepl_add_random_noise.txx
//
// .SECTION Description
//   The function vepl_add_random_noise takes two arguments: the input image
//   and the average `width' of the noise.
//
//   Note that the input image data type must support "operator+(double)",
//   or that it has a constructor taking `double' as argument.
//   Thus when random noise is to be added to colour images, the
//   operator `vil_rgb<ubyte>+double' must be defined.
//   This should be changed in the future.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

vil_image vepl_add_random_noise(vil_image const& , double maxdev);

#endif // vepl_add_random_noise_h_
