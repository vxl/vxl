//: \file
//  \brief Specialisations of is_a() function
//  \author Ian Scott

#include "vil2_image_view.h"
#include <vil/vil_byte.h>

// Specialise the is_a() for vil_byte
template<> vcl_string vil2_image_view<vil_byte>::is_a() const
{  return vcl_string("vil2_image_view<vil_byte>"); }

// Specialise the is_a() for int
template<> vcl_string vil2_image_view<int>::is_a() const
{  return vcl_string("vil2_image_view<int>"); }

// Specialise the is_a() for float
template<> vcl_string vil2_image_view<float>::is_a() const
{  return vcl_string("vil2_image_view<float>"); }

// Specialise the is_a() for char
template<> vcl_string vil2_image_view<char>::is_a() const
{  return vcl_string("vil2_image_view<char>"); }

// Specialise the is_a() for short
template<> vcl_string vil2_image_view<short>::is_a() const
{  return vcl_string("vil2_image_view<short>"); }


