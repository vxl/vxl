//: \file
//  \brief Specialisations of is_a() function
//  \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>
#include <vxl_config.h> // for vxl_byte

// Specialise the is_a() for vxl_byte
template<> vcl_string vimt_image_2d_of<vxl_byte>::is_a() const
{  return vcl_string("vimt_image_2d_of<vxl_byte>"); }

// Specialise the is_a() for int
template<> vcl_string vimt_image_2d_of<int>::is_a() const
{  return vcl_string("vimt_image_2d_of<int>"); }

// Specialise the is_a() for float
template<> vcl_string vimt_image_2d_of<float>::is_a() const
{  return vcl_string("vimt_image_2d_of<float>"); }

// Specialise the is_a() for char
template<> vcl_string vimt_image_2d_of<char>::is_a() const
{  return vcl_string("vimt_image_2d_of<char>"); }

// Specialise the is_a() for short
template<> vcl_string vimt_image_2d_of<short>::is_a() const
{  return vcl_string("vimt_image_2d_of<short>"); }


