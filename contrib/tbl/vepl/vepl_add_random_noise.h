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
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter and return types from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <vil/vil_image_resource.h>

//: add random noise to all pixels
vil_image_resource_sptr vepl_add_random_noise(const vil_image_resource_sptr&, double maxdev);

#endif // vepl_add_random_noise_h_
