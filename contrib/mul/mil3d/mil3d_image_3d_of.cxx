// This is mul/mil3d/mil3d_image_3d_of.cxx
#include "mil3d_image_3d_of.h"
//:
//  \file
//  \brief Specialisations of is_a() function
//  \author Graham Vincent

#include <vil1/vil1_byte.h>
 
// Specialise the is_a() for vil1_byte
template<> vcl_string mil3d_image_3d_of<vil1_byte>::is_a() const
{  return vcl_string("mil3d_image_3d_of<vil1_byte>"); }

// Specialise the is_a() for int
template<> vcl_string mil3d_image_3d_of<int>::is_a() const
{  return vcl_string("mil3d_image_3d_of<int>"); }

// Specialise the is_a() for float
template<> vcl_string mil3d_image_3d_of<float>::is_a() const
{  return vcl_string("mil3d_image_3d_of<float>"); }

// Specialise the is_a() for float
template<> vcl_string mil3d_image_3d_of<short>::is_a() const
{  return vcl_string("mil3d_image_3d_of<short>"); }

// Specialise the is_a() for long
template<> vcl_string mil3d_image_3d_of<long>::is_a() const
{  return vcl_string("mil3d_image_3d_of<long>"); }
