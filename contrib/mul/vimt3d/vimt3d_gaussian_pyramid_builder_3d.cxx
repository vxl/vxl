// This is mul/mil3d/vimt3d_gaussian_pyramid_builder_3d.cxx
#include "vimt3d_gaussian_pyramid_builder_3d.h"
//:
//  \file
//  \brief Specialisations of is_a() function
//  \author Tim Cootes

#include <vxl_config.h>

// Specialise the is_a() for vxl_byte
template<> vcl_string vimt3d_gaussian_pyramid_builder_3d<vxl_byte>::is_a() const
{  return vcl_string("vimt3d_gaussian_pyramid_builder_3d<vxl_byte>"); }

// Specialise the is_a() for int
template<> vcl_string vimt3d_gaussian_pyramid_builder_3d<int>::is_a() const
{  return vcl_string("vimt3d_gaussian_pyramid_builder_3d<int>"); }

// Specialise the is_a() for float
template<> vcl_string vimt3d_gaussian_pyramid_builder_3d<float>::is_a() const
{  return vcl_string("vimt3d_gaussian_pyramid_builder_3d<float>"); }

