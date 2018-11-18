// This is mul/vimt/vimt_add_all_binary_loaders.cxx
#include "vimt_add_all_binary_loaders.h"
//:
// \file

#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d_general.h>
#include <vimt/vimt_vil_v2i.h>
#include <vil/vil_file_format.h>

void vimt_add_all_binary_loaders()
{
  vsl_add_to_binary_loader(vimt_gaussian_pyramid_builder_2d<vxl_byte>());
  vsl_add_to_binary_loader(vimt_gaussian_pyramid_builder_2d<vxl_uint_16>());
  vsl_add_to_binary_loader(vimt_gaussian_pyramid_builder_2d<float>());
  vsl_add_to_binary_loader(vimt_gaussian_pyramid_builder_2d_general<vxl_byte>());
  vsl_add_to_binary_loader(vimt_gaussian_pyramid_builder_2d_general<float>());
  vsl_add_to_binary_loader(vimt_image_2d_of<vxl_int_32>());
  vsl_add_to_binary_loader(vimt_image_2d_of<vxl_byte>());
  vsl_add_to_binary_loader(vimt_image_2d_of<float>());
  vsl_add_to_binary_loader(vimt_image_2d_of<vxl_uint_16>());

  vil_file_format::add_file_format(new vimt_vil_v2i_format);
}
