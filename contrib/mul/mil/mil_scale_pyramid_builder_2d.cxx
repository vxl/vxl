// This is mul/mil/mil_scale_pyramid_builder_2d.cxx
#include "mil_scale_pyramid_builder_2d.h"
//: \file
//  \brief Specialisations of is_a() function
//  \author Tim Cootes

#include <vil/vil_byte.h>

// Specialise the is_a() for vil_byte
template<> vcl_string mil_scale_pyramid_builder_2d<vil_byte>::is_a() const
{  return vcl_string("mil_scale_pyramid_builder_2d<vil_byte>"); }

// Specialise the is_a() for int
template<> vcl_string mil_scale_pyramid_builder_2d<int>::is_a() const
{  return vcl_string("mil_scale_pyramid_builder_2d<int>"); }

// Specialise the is_a() for float
template<> vcl_string mil_scale_pyramid_builder_2d<float>::is_a() const
{  return vcl_string("mil_scale_pyramid_builder_2d<float>"); }
