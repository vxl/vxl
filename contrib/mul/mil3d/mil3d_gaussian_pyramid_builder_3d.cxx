// This is mul/mil3d/mil3d_gaussian_pyramid_builder_3d.cxx
#include "mil3d_gaussian_pyramid_builder_3d.h"
//:
//  \file
//  \brief Specialisations of is_a() function
//  \author Tim Cootes

#include <vil1/vil1_byte.h>

// Specialise the is_a() for vil1_byte
template<> vcl_string mil3d_gaussian_pyramid_builder_3d<vil1_byte>::is_a() const
{  return vcl_string("mil3d_gaussian_pyramid_builder_3d<vil1_byte>"); }

// Specialise the is_a() for int
template<> vcl_string mil3d_gaussian_pyramid_builder_3d<int>::is_a() const
{  return vcl_string("mil3d_gaussian_pyramid_builder_3d<int>"); }

// Specialise the is_a() for float
template<> vcl_string mil3d_gaussian_pyramid_builder_3d<float>::is_a() const
{  return vcl_string("mil3d_gaussian_pyramid_builder_3d<float>"); }

