//: \file
//  \brief Specialisations of is_a() function
//  \author Ian Scott

#include <vil2/vil2_image_view.h>
#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>

// Specialise the is_a() for vil_byte
template<> vcl_string vil2_image_view<vil_byte>::is_a() const
{  return vcl_string("vil2_image_view<uchar>"); }

// Specialise the is_a() for int
template<> vcl_string vil2_image_view<int>::is_a() const
{  return vcl_string("vil2_image_view<int>"); }

// Specialise the is_a() for bool
template<> vcl_string vil2_image_view<bool>::is_a() const
{  return vcl_string("vil2_image_view<bool>"); }

// Specialise the is_a() for uint
template<> vcl_string vil2_image_view<unsigned int>::is_a() const
{  return vcl_string("vil2_image_view<uint>"); }

// Specialise the is_a() for float
template<> vcl_string vil2_image_view<float>::is_a() const
{  return vcl_string("vil2_image_view<float>"); }

// Specialise the is_a() for char
template<> vcl_string vil2_image_view<signed char>::is_a() const
{  return vcl_string("vil2_image_view<schar>"); }

// Specialise the is_a() for short
template<> vcl_string vil2_image_view<short>::is_a() const
{  return vcl_string("vil2_image_view<short>"); }

// Specialise the is_a() for ushort
template<> vcl_string vil2_image_view<unsigned short>::is_a() const
{  return vcl_string("vil2_image_view<ushort>"); }

// Specialise the is_a() for vil_rgb<vil_byte>
template<> vcl_string vil2_image_view<vil_rgb<vil_byte> >::is_a() const
{  return vcl_string("vil2_image_view<vil_rgb<vil_byte>>"); }




