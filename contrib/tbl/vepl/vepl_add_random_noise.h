#ifndef vepl_add_random_noise_h_
#define vepl_add_random_noise_h_

//:
// \file
// \brief add random noise to all pixels
//
//   The function vepl_add_random_noise takes two arguments: the input image
//   and the average `width' of the noise.
//
//   Note that the input image data type must support "operator+(double)",
//   or that it has a constructor taking `double' as argument.
//   Thus when random noise is to be added to colour images, the
//   operator `vil_rgb<ubyte>+double' must be defined.
//   This should be changed in the future.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001

#include <vil/vil_image.h>

//: add random noise to all pixels
vil_image vepl_add_random_noise(vil_image const& , double maxdev);

#endif // vepl_add_random_noise_h_
