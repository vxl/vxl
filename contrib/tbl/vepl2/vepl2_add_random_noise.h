#ifndef vepl2_add_random_noise_h_
#define vepl2_add_random_noise_h_
//:
// \file
// \brief add random noise to all pixels
//
//   The function vepl2_add_random_noise takes two arguments: the input image
//   and the average `width' of the noise.
//
//   Note that the input image data type must support "operator+(double)",
//   or that it has a constructor taking `double' as argument.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: add random noise to all pixels
vil2_image_view_base_sptr vepl2_add_random_noise(vil2_image_view_base const& , double maxdev);

#endif // vepl2_add_random_noise_h_
