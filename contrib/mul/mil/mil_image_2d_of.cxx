// This is mul/mil/mil_image_2d_of.cxx
#include "mil_image_2d_of.h"
//:
//  \file
//  \brief Specialisations of is_a() function
//  \author Tim Cootes

#include <vil/vil_byte.h>

// Specialise the is_a() for vil_byte
template<> vcl_string mil_image_2d_of<vil_byte>::is_a() const
{  return vcl_string("mil_image_2d_of<vil_byte>"); }

// Specialise the is_a() for int
template<> vcl_string mil_image_2d_of<int>::is_a() const
{  return vcl_string("mil_image_2d_of<int>"); }

// Specialise the is_a() for float
template<> vcl_string mil_image_2d_of<float>::is_a() const
{  return vcl_string("mil_image_2d_of<float>"); }

// Specialise the is_a() for char
template<> vcl_string mil_image_2d_of<char>::is_a() const
{  return vcl_string("mil_image_2d_of<char>"); }

// Specialise the is_a() for short
template<> vcl_string mil_image_2d_of<short>::is_a() const
{  return vcl_string("mil_image_2d_of<short>"); }

