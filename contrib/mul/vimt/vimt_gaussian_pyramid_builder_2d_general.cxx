// This is mul/vimt/vimt_gaussian_pyramid_builder_2d_general.cxx
#include "vimt_gaussian_pyramid_builder_2d_general.h"
//:
//  \file
//  \brief Specialisations of is_a() function
//  \author Tim Cootes

#include <vxl_config.h>

// Specialise the is_a() for vxl_byte
template<> vcl_string vimt_gaussian_pyramid_builder_2d_general<vxl_byte>::is_a() const
{  return vcl_string("vimt_gaussian_pyramid_builder_2d_general<vxl_byte>"); }

// Specialise the is_a() for int
template<> vcl_string vimt_gaussian_pyramid_builder_2d_general<int>::is_a() const
{  return vcl_string("vimt_gaussian_pyramid_builder_2d_general<int>"); }

// Specialise the is_a() for float
template<> vcl_string vimt_gaussian_pyramid_builder_2d_general<float>::is_a() const
{  return vcl_string("vimt_gaussian_pyramid_builder_2d_general<float>"); }
