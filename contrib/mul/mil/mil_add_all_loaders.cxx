#include <mil/mil_all.h>

void mil_add_all_loaders()
{
  vsl_add_to_binary_loader(mil_byte_image_2d_io());
  vsl_add_to_binary_loader(mil_gaussian_pyramid_builder_2d<vxl_byte>());
  vsl_add_to_binary_loader(mil_gaussian_pyramid_builder_2d_general<vxl_byte>());

  vsl_add_to_binary_loader(mil_image_2d_of<vxl_byte>());
}
