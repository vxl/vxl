#ifndef mil_all_h_
#define mil_all_h_

#include <mil/mil_byte_image_2d_io.h>
#include <mil/mil_gaussian_pyramid_builder_2d.h>
#include <mil/mil_gaussian_pyramid_builder_2d_general.h>
#include <mil/mil_image_2d_of.h>

static void mil_add_all_loaders()
{
  vsl_add_to_binary_loader(mil_byte_image_2d_io());
  vsl_add_to_binary_loader(mil_gaussian_pyramid_builder_2d<vil_byte>());
  vsl_add_to_binary_loader(mil_gaussian_pyramid_builder_2d_general<vil_byte>());

  vsl_add_to_binary_loader(mil_image_2d_of<vil_byte>());
}

#endif //mil_all_h_
