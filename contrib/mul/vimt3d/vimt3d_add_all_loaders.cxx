// This is mul/vimt3d/vimt3d_add_all_loaders.cxx
#include "vimt3d_add_all_loaders.h"
//:
// \file

#include <vimt3d/vimt3d_image_3d_of.h>
#include <vimt3d/vimt3d_gaussian_pyramid_builder_3d.h>
#include <vil3d/vil3d_file_format.h>
#include <vimt3d/vimt3d_vil3d_v3i.h>

void vimt3d_add_all_loaders()
{
// Add vsl base class loaders
  vsl_add_to_binary_loader(vimt3d_gaussian_pyramid_builder_3d<vxl_byte>());
  vsl_add_to_binary_loader(vimt3d_gaussian_pyramid_builder_3d<float>());
  vsl_add_to_binary_loader(vimt3d_gaussian_pyramid_builder_3d<int>());
  vsl_add_to_binary_loader(vimt3d_image_3d_of<vxl_byte>());
  vsl_add_to_binary_loader(vimt3d_image_3d_of<float>());
  vsl_add_to_binary_loader(vimt3d_image_3d_of<int>());

// Allow vil3d_load with the vimt3d-specific .v3i format.
  vil3d_file_format::add_format(new vimt3d_vil3d_v3i_format);
  
}
